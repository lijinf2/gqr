#include <lshbox.h>
#include <lshbox/query/treelookup.h>
#include <lshbox/query/losslookup.h>
#include <string>
using std::string;
template<typename DATATYPE, typename LSHTYPE, typename PROBERTYPE>
void annQuery(const lshbox::Matrix<DATATYPE>& data, const lshbox::Matrix<DATATYPE>& query, LSHTYPE& mylsh, const lshbox::Benchmark& bench, PROBERTYPE* probers) {
    std::cout << "probed items, " << "overall query time, " 
        << "avg recall, " << "avg precision" <<"\n";

    lshbox::timer timer;
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
    SCANNER initScanner) {

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
    annQuery(data, query, mylsh, bench, probers);
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_hr(
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    SCANNER initScanner) {

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
    annQuery(data, query, mylsh, bench, probers);
}
template<typename DATATYPE, typename LSHTYPE>
void search(
    string method,
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench) {

    // initialize scanner
    typename lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), L2_DIST);
    lshbox::Scanner<typename lshbox::Matrix<DATATYPE>::Accessor> initScanner(
        accessor,
        metric,
        bench.getK()
    );

    if (method == "GQR") {
        search_gqr(data, query, mylsh, bench, initScanner);
    } else if (method == "HR") {
        search_hr(data, query, mylsh, bench, initScanner);
    } else {
        std::cerr << "does not exist method " << method << std::endl;
        assert(false);
    }
}
