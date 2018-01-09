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
#include <lshbox/lsh/simpleLSH.h>
#include <bits/unordered_map.h>

#include "search.h"
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
            << "./search"
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
    string hashMethod = params["hash_method"];
    string queryMethod = params["query_method"];
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

    unsigned TYPE_DIST = L2_DIST;
    if (params.find("TYPE_DIST")!=params.end()) {
        string type_dist_str = params["TYPE_DIST"];
        if(type_dist_str=="AG") {
            TYPE_DIST = AG_DIST;
        } else if (type_dist_str=="L2") {
            TYPE_DIST = L2_DIST;
        } else if (type_dist_str=="L1") {
            TYPE_DIST = L1_DIST;
        }
    }

    int invalid_dim = 0;
    if (params.find("invalid_dim")!=params.end()) {
        invalid_dim = atoi(params["invalid_dim"].c_str());
    }

    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << " ";
    }
    std::cout<<"dist type "<<TYPE_DIST;
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
    
    lshbox::Benchmark bench;
    // std::string lshboxBenchFile =  lshbox::genBenchFromIvecs(benchFile.c_str(), numQueries, topK);
    // bench.load(lshboxBenchFile);
    if (benchFile.find("lshbox") == string::npos) {
        std::cout << "only lshbox benchmark file is supported" << std::endl;
        assert(false);
    } else {
        bench.load(benchFile.c_str());
    }
    
    // load model
    if (hashMethod == "PCAH") {
        lshbox::PCAH<DATATYPE> pcah;
        pcah.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, pcah, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "ITQ") {
        lshbox::ITQ<DATATYPE> itq;
        itq.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, itq, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "PCARR") {
        lshbox::PCARR<DATATYPE> pcarr;
        pcarr.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, pcarr, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "SpH") {
        lshbox::SpH<DATATYPE> sph;
        sph.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, sph, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "IsoH") {
        lshbox::IsoH<DATATYPE> isoh;
        isoh.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, isoh, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "KMH") {
        lshbox::KMH<DATATYPE> mylsh;
        mylsh.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, mylsh, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "SH") {
        lshbox::spectral<DATATYPE > spectralHashing;
        spectralHashing.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, spectralHashing, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "SIM") {
        lshbox::SIMH<DATATYPE > sim;
        sim.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, sim, bench, params, TYPE_DIST, invalid_dim);
    } else if (hashMethod == "SimpleLSH") {
        lshbox::SimpleLSH<DATATYPE > mip;
        mip.loadModel(modelFile, baseBitsFile);
        search(queryMethod, data, query, mip, bench, params, TYPE_DIST, invalid_dim);
    }
    else {
        cout << "parameters are not corrected, please double check and give correct parameters" << endl;
        return -1;
    }
}
