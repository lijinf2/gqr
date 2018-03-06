#include <cstdlib> 
#include <fstream>
#include <unordered_map>

#include <lshbox.h>
#include <lshbox/query/fv.h>
#include <lshbox/query/hammingranking.h>
#include <lshbox/query/hashlookupPP.h>
#include <lshbox/query/lossranking.h>
#include <lshbox/utils.h>
#include <lshbox/lsh/pcah.h>
#include <lshbox/lsh/itq.h>
#include <lshbox/lsh/pcarr.h>
#include <lshbox/lsh/sph.h>
#include <lshbox/lsh/isoh.h>
#include <lshbox/lsh/kmh.h>
#include <lshbox/lsh/spectral.h>
#include <lshbox/lsh/sim.h>
#include <intcode/hash/e2lsh.h>
#include <intcode/hash/alsh.h>

#include <lshbox/graph/knngraphh.h>

#include <mips/alshrank/alshrankhasher.h>
#include <mips/normrange/normrangehasher.h>

#include "search.h"
#include "search_graph.h"
#include "search_intcode.h"
#include "search_mip.cpp"

using std::unordered_map;
using std::string;
int main(int argc, const char **argv)
{
    // currently only support fvecs
    typedef float DATATYPE;

    unordered_map<string, string> params = lshbox::parseParams(argc, argv);
    if (params.size() < 8)
    {
        std::cerr << "Usage: "
            << "./search   "
            << "--hash_method=xxx "
            << "--query_method=xxx "
            << "--base_format=xxx "
            << "--modle_file=xxx " 
            << "--base_file=xxx "
            << "--base_bits_file=xxx "
            << "--query_file=xxx "
            << "--benchmark_file=xxx" 
            << std::endl;
        return -1;
    }

    string baseFormat = params["base_format"];
    if (baseFormat != "fvecs") {
        std::cerr << "Data format is not fvecs. Only fvecs is supported" << std::endl;
        return -1;
    }
    string hashMethod = params["hash_method"];
    string queryMethod = params["query_method"];
    string modelFile = params["model_file"];
    string dataFile = params["base_file"];
    string baseBitsFile = params["base_bits_file"];
    string queryFile = params["query_file"];
    string benchFile = params["benchmark_file"];

    unsigned metric = L2_DIST;

    if (params.find("metric")!=params.end()) {
        string type_dist_str = params["metric"];
        if(type_dist_str=="angular") {
            metric = AG_DIST;
        } else if (type_dist_str=="euclidean") {
            metric = L2_DIST;
        } else if (type_dist_str=="L1") {
            metric = L1_DIST;
        } else if (type_dist_str=="product") {
            metric = IP_DIST;
        }
    }



    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << " ";
    }
    std::cout<<"dist type "<<metric;
    std::cout << std::endl;
    std::cout << std::endl;
    lshbox::timer timer;


    // load bench
    // transform ivecs bench to lshbox bench 
    lshbox::Benchmark bench;
    // std::string lshboxBenchFile =  lshbox::genBenchFromIvecs(benchFile.c_str(), numQueries, topK);
    // bench.load(lshboxBenchFile);
    if (benchFile.find("lshbox") == string::npos) {
        std::cout << "only lshbox benchmark file is supported" << std::endl;
        assert(false);
    } else {
        bench.load(benchFile.c_str());
    }

    // load lshbox type data and query
    std::cout << "load data and query..." << std::endl;
    lshbox::Matrix<DATATYPE> data;
    lshbox::Matrix<DATATYPE> query;
    if (baseFormat == "fvecs") {
        lshbox::loadFvecs(data, dataFile);
        lshbox::loadFvecs(query, queryFile);
    }
    
    // load model
    if (hashMethod == "PCAH") {
        lshbox::PCAH<DATATYPE> pcah;
        pcah.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, pcah, bench, params, metric);
    } else if (hashMethod == "ITQ") {
        lshbox::ITQ<DATATYPE> itq;
        itq.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, itq, bench, params, metric);
    } else if (hashMethod == "PCARR") {
        lshbox::PCARR<DATATYPE> pcarr;
        pcarr.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, pcarr, bench, params, metric);
    } else if (hashMethod == "SpH") {
        lshbox::SpH<DATATYPE> sph;
        sph.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, sph, bench, params, metric);
    } else if (hashMethod == "IsoH") {
        lshbox::IsoH<DATATYPE> isoh;
        isoh.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, isoh, bench, params, metric);
    } else if (hashMethod == "KMH") {
        lshbox::KMH<DATATYPE> mylsh;
        mylsh.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, mylsh, bench, params, metric);
    } else if (hashMethod == "SH") {
        lshbox::spectral<DATATYPE > spectralHashing;
        spectralHashing.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, spectralHashing, bench, params, metric);
    } else if (hashMethod == "SIM") {
        lshbox::SIMH<DATATYPE> sim;
        sim.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, sim, bench, params, metric);
    } else if (hashMethod == "LMIP") {
        lshbox::NormRangeHasher<DATATYPE> lmip;
        metric = IP_DIST;
        lmip.loadModel(modelFile, baseBitsFile);
        search_mip(queryMethod, data, query, lmip, bench, params, metric);
    } else if (hashMethod == "NLMIP") {
        lshbox::NormRangeHasher<DATATYPE> lmip;
        metric = IP_DIST;
        lmip.loadModel(modelFile, baseBitsFile);
        search_mip(queryMethod, data, query, lmip, bench, params, metric);

    } else if (hashMethod == "IntRankALSH") {
        lshbox::ALSH<DATATYPE> alsh;
        alsh.loadModel(modelFile, baseBitsFile);
        search_intrankalsh(queryMethod, data, query, alsh, bench, params, IP_DIST);

    } else if (hashMethod == "ALSHRank") {
        lshbox::ALSHRankHasher<DATATYPE > alshrank;
        alshrank.loadModel(modelFile, baseBitsFile);
        search_alshmatchrank(queryMethod, data, query, alshrank, bench, params, IP_DIST);

    } else if (hashMethod == "KNNGraph") { // graph method
        lshbox::KNNGraphH<DATATYPE> kgraphhasher; 
        kgraphhasher.loadModel(modelFile);
        search_graph(queryMethod, data, query, kgraphhasher, bench, params, metric);

    } else if (hashMethod == "E2LSH") {
        lshbox::E2LSH<DATATYPE> e2lsh; 
        e2lsh.loadModel(modelFile, baseBitsFile);
        search_intcode(queryMethod, data, query, e2lsh, bench, params, metric);
    } else {
        cout << "do not support hashMethod: " << hashMethod << endl;
        return -1;
    }
}
