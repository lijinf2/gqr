#include <map>
#include <cstdlib>
#include <fstream>

#include <lshbox.h>
#include <lshbox/query/fv.h>
#include <lshbox/query/losslookup.h>
#include <lshbox/query/hammingranking.h>
#include <lshbox/query/hashlookupPP.h>
#include <lshbox/query/lossranking.h>
#include <lshbox/query/treelookup.h>
#include <lshbox/utils.h>

#include <lshbox/lsh/benchhasher.h>

int main(int argc, char const *argv[])
{
    // currently only support fvecs
    typedef float DATATYPE;
    if (argc < 11)
    {
        std::cerr << "Usage: "
            << "./benhasher_topkquery "
            << "base_format "
            << "base_file "
            << "cardinality "
            << "dimension "
            << "base_bits_file "
            << "query_file "
            << "num_query"
            << "query_bits_file "
            << "benchmark_ivecs_file" 
            << "topk" 
            << std::endl;
        return -1;
    }

    const char* baseFormat = argv[1];
    if (strcmp(baseFormat, "fvecs") != 0) {
        std::cerr << "Data format is not fvecs. Only fvecs is supported" << std::endl;
        return -1;
    }
    const char* dataFile = argv[2];
    int cardinality = atoi(argv[3]);
    int dimension = atoi(argv[4]);
    const char* baseBitsFile = argv[5];
    const char* queryFile = argv[6];
    int numQueries = atoi(argv[7]);
    const char* queryBitsFile = argv[8];
    const char* benchFile = argv[9];
    int topK = atoi(argv[10]);

    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    lshbox::timer timer;

    // load lshbox type data
    std::cout << "load data ..." << std::endl;
    lshbox::Matrix<DATATYPE> data;
    if (strcmp(baseFormat, "fvecs") == 0) {
        lshbox::loadFvecs(data, dataFile, dimension, cardinality);
    }
    std::cout << "load data time: " << timer.elapsed() << "s." << std::endl;

    // load queries
    lshbox::Matrix<DATATYPE> query;
    if (strcmp(baseFormat, "fvecs") == 0) {
        lshbox::loadFvecs(query, queryFile, dimension, numQueries);
    }

    // load bench
    // transform ivecs bench to lshbox bench 
    std::string lshboxBenchFile =  lshbox::genBenchFromIvecs(benchFile, numQueries, topK);
    lshbox::Benchmark bench;
    std::string benchmark(lshboxBenchFile);
    bench.load(benchmark);
    unsigned K = bench.getK();

    // load model
    std::cout << "load model ..." << std::endl;
    timer.restart();
    lshbox::BenchHasher<DATATYPE> mylsh;
    mylsh.loadModel(baseBitsFile, queryBitsFile, query, bench);
    std::cout << "load model time: " << timer.elapsed() << "s." << std::endl;

    // initialize scanner
    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), L2_DIST);
    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> initScanner(
        accessor,
        metric,
        K
    );
    std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "RUNING QUERY ..." << std::endl;
    timer.restart();

    int maxProbedBK = 16;
    if (argc >= 6)
        maxProbedBK = std::atoi(argv[5]);

    // initialize prober
    // typedef HashLookup<lshbox::Matrix<DATATYPE>::Accessor> PROBER;
    typedef HammingRanking<lshbox::Matrix<DATATYPE>::Accessor> PROBER;

    void* raw_memory = operator new[]( 
        sizeof(PROBER) * bench.getQ());
    PROBER* probers = static_cast<PROBER*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) PROBER(
            query[bench.getQuery(i)],
            initScanner,
            mylsh);
    }

    double initTime = timer.elapsed();
    std::cout << "init time: " << initTime << std::endl;

    std::cout << "probed items, " << "overall query time, " 
        << "avg recall, " << "avg precision" <<"\n";

    timer.restart();
    // int maxProbedItem = data.getSize();
    for (unsigned numItems = 1; true ; numItems *= 2) { //  # of probed items must be the power of two
        if (numItems > data.getSize()) {
            numItems = data.getSize();
        }
        lshbox::Stat recall, precision;
        for (unsigned i = 0; i != bench.getQ(); ++i)
        {
            // queries are applied incrementally, i.e. the result of this round depends on the last round
            mylsh.KItemByProber(query[bench.getQuery(i)], probers[i], numItems);

            // collect accuracy information
            setStat(probers[i].getScanner(), bench.getAnswer(i), recall, precision);
        }
        double retTime = timer.elapsed();
        std::cout << numItems << ", " << retTime <<", "
            << recall.getAvg() << ", " << precision.getAvg() << "\n";
        if (numItems == data.getSize())
            break;
    }

    // release memory of prober;
    std::cout << "bench.getQ: " << bench.getQ() << std::endl;
    for (unsigned i = bench.getQ() - 1; i !=0; --i) {
        probers[i].~PROBER();
    }

    std::cout << "HASH TABLE SIZE    : " << mylsh.getTableSize() << std::endl;
    std::cout << "LARGEST BUCKET SIZE    : " << mylsh.getMaxBucketSize() << std::endl;
    std::cout << "end of program" << std::endl;
}
