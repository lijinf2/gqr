#include <lshbox.h>
#include <lshbox/query/treelookup.h>
#include <lshbox/query/losslookup.h>
#include <string>
#include <unordered_map>
#include "apps/opq_evaluate.cpp"
#include "lshbox/bench/bencher.h"

using std::string;
using std::unordered_map;
template<typename DATATYPE, typename LSHTYPE, typename PROBERTYPE>
void annQuery(const lshbox::Matrix<DATATYPE>& data, const lshbox::Matrix<DATATYPE>& query, LSHTYPE& mylsh, const lshbox::Benchmark& bench, PROBERTYPE* probers, const unordered_map<string, string>& params) {
    string benchFile = params.find("benchmark_file")->second; 
    Bencher opqBencher(benchFile.c_str());

    int numQueries = bench.getQ();
    auto it =  params.find("num_queries");
    if (it != params.end()) {
        numQueries = atoi((it->second).c_str());
    }

    std::cout << "probed items, " << "overall query time, " 
        << "avg recall, " << "avg error ratio" <<"\n";

    double runtime = 0;
    lshbox::timer timer;
    int numAllItems = data.getSize();
    for (unsigned numItems = 1; true ; numItems *= 2) { //  # of probed items must be the power of two
        if (numItems > numAllItems) 
            numItems = numAllItems;

        // numItems = 65536;
        // // numItems = 16;
        // numAllItems = numItems;

        for (unsigned i = 0; i != numQueries; ++i) {
            probers[i].getScanner().opqReserve(2* numItems);
        }

        timer.restart();
        // queries are applied incrementally, i.e. the result of this round depends on the last round
        for (unsigned i = 0; i != numQueries; ++i) {
            mylsh.KItemByProber(query[bench.getQuery(i)], probers[i], numItems);
        }
        double roundTime= timer.elapsed();
        runtime += roundTime;

        // // collect accuracy information
        // lshbox::Stat recall, error;
        // for (unsigned i = 0; i != numQueries; ++i) {
        //     setStat(probers[i].getScanner(), bench.getAnswer(i), recall, error);
        // }
        // std::cout << numItems << ", " << runtime / numQueries <<", "
        //     << recall.getAvg() << ", " << error.getAvg() << "\n";
        
        vector<vector<pair<float, int>>> results;
        results.reserve(numQueries);
        for (unsigned i = 0; i != numQueries; ++i) {
            results.push_back(probers[i].getScanner().getOpqResult());
        }
        std::cout << numItems << ", " << runtime / numQueries <<", "
            << cal_opq_avg_recall(opqBencher, results) << ", " << cal_opq_avg_error(opqBencher, results) << "\n";


        if (numItems == numAllItems)
            break;
    }

    // release memory of prober;
    std::cout << "bench.getQ: " << bench.getQ() << std::endl;
    for (unsigned i = bench.getQ() - 1; i !=0; --i) {
        probers[i].~PROBERTYPE();
    }

    std::cout << "HASH TABLE SIZE    : " << mylsh.getTableSize() << std::endl;
    std::cout << "LARGEST BUCKET SIZE    : " << mylsh.getMaxBucketSize() << std::endl;
    std::cout << "end of program" << std::endl;
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_gqr(
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    SCANNER initScanner,
    const unordered_map<string, string>& params) {

    // initialized tree lookup
    typedef TreeLookup<typename lshbox::Matrix<DATATYPE>::Accessor> GQRT;
    Tree fvs(mylsh.getCodeLength());

    void* raw_memory = operator new[]( 
        sizeof(GQRT) * bench.getQ());
    GQRT* probers = static_cast<GQRT*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) GQRT(
            query[bench.getQuery(i)],
            initScanner,
            mylsh,
            &fvs);// for non losslookup probers
    }
    annQuery(data, query, mylsh, bench, probers, params);
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_hr(
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    SCANNER initScanner,
    const unordered_map<string, string>& params) {

    typedef HammingRanking<typename lshbox::Matrix<DATATYPE>::Accessor> HRT;

    void* raw_memory = operator new[]( 
        sizeof(HRT) * bench.getQ());
    HRT* probers = static_cast<HRT*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) HRT(
            query[bench.getQuery(i)],
            initScanner,
            mylsh);// for non losslookup probers
    }
    annQuery(data, query, mylsh, bench, probers, params);
}
template<typename DATATYPE, typename LSHTYPE>
void search(
    string method,
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    const unordered_map<string, string>& params) {

    // initialize scanner
    typename lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), L2_DIST);
    lshbox::Scanner<typename lshbox::Matrix<DATATYPE>::Accessor> initScanner(
        accessor,
        metric,
        bench.getK()
    );

    if (method == "GQR") {
        search_gqr(data, query, mylsh, bench, initScanner, params);
    } else if (method == "HR") {
        search_hr(data, query, mylsh, bench, initScanner, params);
    } else {
        std::cerr << "does not exist method " << method << std::endl;
        assert(false);
    }
}
