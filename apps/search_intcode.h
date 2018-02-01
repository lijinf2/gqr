#pragma once
#include "search.h"
#include "intcode/query/intranking.h"

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_intrank(
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    SCANNER initScanner,
    const unordered_map<string, string>& params) {

    typedef IntRanking<typename lshbox::Matrix<DATATYPE>::Accessor> IR;

    void* raw_memory = operator new[](
        sizeof(IR) * bench.getQ());
    IR* probers = static_cast<IR*>(raw_memory);

    double construct_time = 0;
    lshbox::timer timer;
    timer.restart();
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) IR(
            query[bench.getQuery(i)],
            initScanner,
            mylsh);// for non losslookup probers
    }
    construct_time= timer.elapsed();
    std::cout << "IntRank constructing time : " << construct_time << "." << std::endl;
    annQuery(data, query, mylsh, bench, probers, params);
}

template<typename DATATYPE, typename LSHTYPE>
void search_intcode(
    string method,
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    const unordered_map<string, string>& params,
    const unsigned TYPE_DIST) {

    // initialize scanner
    typename lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), TYPE_DIST);
    lshbox::Scanner<typename lshbox::Matrix<DATATYPE>::Accessor> initScanner(
        accessor,
        metric,
        bench.getK()
    );

    if (method == "IntRank") {
        search_intrank(data, query, mylsh, bench, initScanner, params);
    } else {
        std::cerr << "does not exist method " << method << std::endl;
        assert(false);
    }
}
