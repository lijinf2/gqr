#include "search.h"
#include <lshbox/mip/query/lengthmarked.h>
#include <lshbox/mip/query/normalizedrank.h>
#include <mips/query/normrank.h>

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_lm(
        const lshbox::Matrix<DATATYPE>& data,
        const lshbox::Matrix<DATATYPE>& query,
        LSHTYPE& mylsh,
        const lshbox::Benchmark& bench,
        SCANNER initScanner,
        const unordered_map<string, string>& params) {

    typedef LengthMarked<typename lshbox::Matrix<DATATYPE>::Accessor> LMR;

    void* raw_memory = operator new[](
            sizeof(LMR) * bench.getQ());
    LMR* probers = static_cast<LMR*>(raw_memory);

    double construct_time = 0;
    lshbox::timer timer;
    timer.restart();
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) LMR(
                query[bench.getQuery(i)],
                initScanner,
                mylsh);// for non losslookup probers
    }
    construct_time= timer.elapsed();
    std::cout << "LM constructing time : " << construct_time << "." << std::endl;
    annQuery(data, query, mylsh, bench, probers, params);
}


template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_nr(
        const lshbox::Matrix<DATATYPE>& data,
        const lshbox::Matrix<DATATYPE>& query,
        LSHTYPE& mylsh,
        const lshbox::Benchmark& bench,
        SCANNER initScanner,
        const unordered_map<string, string>& params) {

    typedef NormRank<typename lshbox::Matrix<DATATYPE>::Accessor> LMR;

    void* raw_memory = operator new[](
            sizeof(LMR) * bench.getQ());
    LMR* probers = static_cast<LMR*>(raw_memory);

    double construct_time = 0;
    lshbox::timer timer;
    timer.restart();
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) LMR(
                query[bench.getQuery(i)],
                initScanner,
                mylsh);// for non losslookup probers
    }
    construct_time= timer.elapsed();
    std::cout << "NR constructing time : " << construct_time << "." << std::endl;
    annQuery(data, query, mylsh, bench, probers, params);
}


template<typename DATATYPE, typename LSHTYPE>
void search_mip(
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

    if (method == "LM") {
        search_lm(data, query, mylsh, bench, initScanner, params);
    }
    else if (method == "NR") {
        search_nr(data, query, mylsh, bench, initScanner, params);
    }
    else {
        std::cerr << "does not exist method " << method << std::endl;
        assert(false);
    }
}
