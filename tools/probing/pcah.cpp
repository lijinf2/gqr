#include <cstdlib>
#include <fstream>
#include <unordered_map>

#include <lshbox.h>
#include <lshbox/query/fv.h>
#include <lshbox/query/losslookup.h>
#include <lshbox/query/hammingranking.h>
#include <lshbox/query/hashlookupPP.h>
#include <lshbox/query/lossranking.h>
#include <lshbox/query/treelookup.h>
#include <lshbox/utils.h>
#include <lshbox/lsh/pcah.h>

using std::unordered_map;
using std::string;
int main(int argc, const char **argv)
{
    // currently only support fvecs
    typedef float DATATYPE;

    unordered_map<string, string> params = lshbox::parseParams(argc, argv);
    if (params.size() < 11)
    {
        std::cerr << "Usage: "
            << "./pcah "
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

    string baseFormat = params["base_format"];
    if (baseFormat != "fvecs") {
        std::cerr << "Data format is not fvecs. Only fvecs is supported" << std::endl;
        return -1;
    }
    int cardinality = atoi(params["cardinality"].c_str());
    int dimension = atoi(params["dimension"].c_str());
    int numQueries = atoi(params["num_queries"].c_str());
    int topK = atoi(params["topk"].c_str());
    int numTables = atoi(params["num_tables"].c_str());
    string modelFile = params["model_file"];
    string dataFile = params["base_file"];
    string baseBitsFile = params["base_bits_file"];
    string queryFile = params["query_file"];
    string benchFile = params["benchmark_file"];

    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    lshbox::timer timer;

    // load lshbox type data and query
    std::cout << "load data and query..." << std::endl;
    lshbox::Matrix<DATATYPE> data;
    lshbox::Matrix<DATATYPE> query;
    if (baseFormat == "fvecs") {
        lshbox::loadFvecs(data, dataFile, dimension, cardinality);
        lshbox::loadFvecs(query, queryFile, dimension, numQueries);
    }

    // load bench
    // transform ivecs bench to lshbox bench 
    std::string lshboxBenchFile =  lshbox::genBenchFromIvecs(benchFile.c_str(), numQueries, topK);
    lshbox::Benchmark bench;
    std::string benchmark(lshboxBenchFile);
    bench.load(benchmark);
    unsigned K = bench.getK();

    // load model
    std::cout << "load model ..." << std::endl;
    timer.restart();
    lshbox::PCAH<DATATYPE> mylsh;
    mylsh.loadModel(modelFile, baseBitsFile);
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
    int numAllItems = data.getSize();
    for (unsigned numItems = 1; true ; numItems *= 2) { //  # of probed items must be the power of two
        if (numItems > numAllItems) 
            numItems = numAllItems;

        // numItems = 131072;
        // // numItems = 16;
        // numAllItems = numItems;

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
        if (numItems == numAllItems)
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
