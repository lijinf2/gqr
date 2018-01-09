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
#include <lshbox/lsh/benchhasher.h>

#include "search.h"
using std::unordered_map;
using std::string;
int main(int argc, const char **argv)
{
    // currently only support fvecs
    typedef float DATATYPE;

    unordered_map<string, string> params = lshbox::parseParams(argc, argv);
    if (params.size() < 12)
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
    string queryBitsFile = params["query_bits_file"];
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
    lshbox::Benchmark bench;
    if (benchFile.find("lshbox") == string::npos) {
        std::cout << "only lshbox benchmark file is supported" << std::endl;
        assert(false);
    } else {
        bench.load(benchFile.c_str());
    }

    // load model
    lshbox::BenchHasher<DATATYPE> mylsh;
    mylsh.loadModel(modelFile, baseBitsFile, queryBitsFile, query, bench);

    search("HR", data, query, mylsh, bench, params);
}
