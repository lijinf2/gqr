#include <lshbox.h>
#include <lshbox/query/treelookup.h>
#include <lshbox/query/agqr/agqrlookup.h>
#include <lshbox/query/losslookup.h>
#include <lshbox/query/hashlookupPP.h>
#include <lshbox/query/hook/hooksearch.h>
#include <string>
#include <unordered_map>
#include "apps/opq_evaluate.cpp"
#include "lshbox/bench/bencher.h"
#include <lshbox/query/mih.h>

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
        << "avg recall, " << "avg precision, " << "avg error ratio" <<"\n";

    double runtime = 0;
    lshbox::timer timer;
    int numAllItems = data.getSize();
    for (unsigned numItems = 1; true ; numItems *= 2) { //  # of probed items must be the power of two
        if (numItems > numAllItems) 
            numItems = numAllItems;

        // numItems = 65536;
        // // numItems = 16;
        // numAllItems = numItems;

        // for (unsigned i = 0; i != numQueries; ++i) {
        //     probers[i].getScanner().opqReserve(numItems); 
        // }
        timer.restart();
        // queries are applied incrementally, i.e. the result of this round depends on the last round
        for (unsigned i = 0; i != numQueries; ++i) {
            mylsh.KItemByProber(query[bench.getQuery(i)], probers[i], numItems);
        }
        double roundTime= timer.elapsed();
        runtime += roundTime;
        
        vector<unsigned> numItemProbed;
        numItemProbed.reserve(numQueries);
        vector<vector<pair<unsigned, float>>> benchResult;
        benchResult.reserve(numQueries);
        for (unsigned i = 0; i != numQueries; ++i) {
            numItemProbed.push_back(probers[i].getNumItemsProbed());

            // const vector<pair<float, unsigned>>& src = probers[i].getScanner().getOpqResult(); 
            const vector<pair<float, unsigned>>& src = probers[i].getScanner().getMutableTopk().genTopk(); 
            vector<pair<unsigned, float>> dst(src.size()); 
            for (int j = 0; j < src.size(); ++j) {
                dst[j].first = src[j].second;
                dst[j].second = src[j].first;
            }
            benchResult.emplace_back(dst);
        }
        std::cout << numItems << ", " << runtime <<", "
            << cal_avg_recall(opqBencher, benchResult, true) << ", "
            << cal_avg_precision(opqBencher, benchResult, numItemProbed, true) << ", " 
            << cal_avg_error(opqBencher, benchResult, true) << "\n";


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

    double construct_time = 0;
    lshbox::timer timer;
    timer.restart();
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) HRT(
            query[bench.getQuery(i)],
            initScanner,
            mylsh);// for non losslookup probers
    }
    construct_time= timer.elapsed();
    std::cout << "HR constructing time : " << construct_time << "." << std::endl;
    annQuery(data, query, mylsh, bench, probers, params);
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_ghr(
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    SCANNER initScanner,
    const unordered_map<string, string>& params) {

    typedef HashLookupPP<typename lshbox::Matrix<DATATYPE>::Accessor> GHRT;
    FV fvs(mylsh.getCodeLength());

    void* raw_memory = operator new[]( 
        sizeof(GHRT) * bench.getQ());
    GHRT * probers = static_cast<GHRT*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) GHRT(
            query[bench.getQuery(i)],
            initScanner,
            mylsh,
            &fvs);// for non losslookup probers
    }
    annQuery(data, query, mylsh, bench, probers, params);
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_qr(
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    SCANNER initScanner,
    const unordered_map<string, string>& params) {

    typedef LossRanking<typename lshbox::Matrix<DATATYPE>::Accessor> QR;

    void* raw_memory = operator new[]( 
        sizeof(QR) * bench.getQ());
    QR* probers = static_cast<QR*>(raw_memory);

    double construct_time = 0;
    lshbox::timer timer;
    timer.restart();
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) QR(
            query[bench.getQuery(i)],
            initScanner,
            mylsh);// for non losslookup probers
    }
    construct_time= timer.elapsed();
    std::cout << "QR constructing time : " << construct_time << "." << std::endl;
    annQuery(data, query, mylsh, bench, probers, params);
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_mih(
    const lshbox::Matrix<DATATYPE>& data,
    const lshbox::Matrix<DATATYPE>& query,
    LSHTYPE& mylsh,
    const lshbox::Benchmark& bench,
    SCANNER initScanner,
    const unordered_map<string, string>& params) {

    typedef unsigned long long BIDTYPE;
    typedef MIH<typename lshbox::Matrix<DATATYPE>::Accessor> MIH_;

    // Currently only work with single hash table, although it can be extended to support multiple hash tables
    assert(mylsh.tables.size() == 1);

    // Number of substrings is hardcoded here
    const unsigned substringNum = 2;
    unsigned substringLen = mylsh.codelength / substringNum;
    BIDTYPE mask = (1 << substringLen) - 1;

    std::vector<std::unordered_map<BIDTYPE, std::vector<BIDTYPE> > > subtables(substringNum);

    for (auto item : mylsh.tables[0]) {
        BIDTYPE bid = item.first;
        for (int i = substringNum - 1; i >= 0; --i) {
            BIDTYPE subBID = bid & mask;
            subtables[i][subBID].push_back(item.first);
            bid >>= substringLen;
        }
    }

    void* raw_memory = operator new[](
        sizeof(MIH_) * bench.getQ());
    MIH_* probers = static_cast<MIH_*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) MIH_(
            query[bench.getQuery(i)],
            initScanner,
            mylsh,
            subtables,
            substringNum);
    }
    annQuery(data, query, mylsh, bench, probers, params);
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_agqr(
        const lshbox::Matrix<DATATYPE>& data,
        const lshbox::Matrix<DATATYPE>& query,
        LSHTYPE& mylsh,
        const lshbox::Benchmark& bench,
        SCANNER initScanner,
        const unordered_map<string, string>& params) {

    // initialized tree lookup
    typedef AGQRLookup<typename lshbox::Matrix<DATATYPE>::Accessor> AGQRT;
    Tree fvs(mylsh.getCodeLength());

    void* raw_memory = operator new[]( 
            sizeof(AGQRT) * bench.getQ());
    AGQRT* probers = static_cast<AGQRT*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) AGQRT(
                query[bench.getQuery(i)],
                initScanner,
                mylsh,
                &fvs);// for non losslookup probers
    }
    annQuery(data, query, mylsh, bench, probers, params);
}

template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
void search_hook(
        const lshbox::Matrix<DATATYPE>& data,
        const lshbox::Matrix<DATATYPE>& query,
        LSHTYPE& mylsh,
        const lshbox::Benchmark& bench,
        SCANNER initScanner,
        const unordered_map<string, string>& params) {

    int hookDegree = 8;
    auto it =  params.find("hook_degree");
    if (it != params.end()) {
        hookDegree = atoi((it->second).c_str());
    }

    // initialized hook search
    Hooker hooker(hookDegree, data, initScanner, mylsh);
    typedef HookSearch<typename lshbox::Matrix<DATATYPE>::Accessor> HOOKSEARCHT;

    void* raw_memory = operator new[]( 
            sizeof(HOOKSEARCHT) * bench.getQ());
    HOOKSEARCHT* probers = static_cast<HOOKSEARCHT*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) HOOKSEARCHT(
                query[bench.getQuery(i)],
                initScanner,
                mylsh,
                &hooker);// for non losslookup probers
    }
    annQuery(data, query, mylsh, bench, probers, params);
    delete[] probers;
}

template<typename DATATYPE, typename LSHTYPE>
void search(
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

    if (method == "GQR") {
        search_gqr(data, query, mylsh, bench, initScanner, params);
    } else if (method == "HR") {
        search_hr(data, query, mylsh, bench, initScanner, params);
    } else if (method == "GHR" || method == "HL") {
        search_ghr(data, query, mylsh, bench, initScanner, params);
    } else if (method == "QR") {
        search_qr(data, query, mylsh, bench, initScanner, params);
    } else if (method == "MIH") {
        search_mih(data, query, mylsh, bench, initScanner, params);
    } else if (method == "AGQR") {
        search_agqr(data, query, mylsh, bench, initScanner, params);
    } else if (method == "HOOK") {
        search_hook(data, query, mylsh, bench, initScanner, params);
    } else {
        std::cerr << "does not exist method " << method << std::endl;
        assert(false);
    }
}
