#include <vector>
#include <string>
#include <fstream>
using namespace std;
#include "gqr/util/cal_groundtruth.h"
using namespace std;
int main(int argc, char** argv) {
    if (argc != 7 && argc != 8) {
        cout << "usage: program base_file.fvecs query_file.fvecs K groundtruth_file.lshbox groundtruth_file.ivecs num_threads=4" << endl;
        return 0;
    }

    const char* baseFileName = argv[1];
    const char* queryFileName = argv[2];
    int K = std::atoi(argv[3]);
    const char* lshboxBenchFileName = argv[4];
    const char* ivecsBenchFileName = argv[5];
    string metric = argv[6];
    int numThreads = 4;
    if (argc >= 8)
        numThreads = stoi(argv[7]);

    int itemBatchSize = 200000;

    ifstream queryFin(queryFileName, ios::binary);
    if (!queryFin) {
        cout << "query File "  << queryFileName << " does not exist "<< endl;
        return 0;
    }
    vector<vector<float>> queryVecs;
    int dimension;
    while(queryFin.read((char*)&dimension, sizeof(int))) {
        vector<float> query;
        query.resize(dimension);
        queryFin.read((char*)&query[0], sizeof(float) * dimension);
        queryVecs.push_back(query);
    }
    queryFin.close();

    ifstream baseFin(baseFileName, ios::binary);
    if (!baseFin) {
        cout << "base File " << baseFileName << " does not exist" << endl;
        return 0;
    }

    typedef Query<float> Query;
    vector<Query> queryObjs;
    for (int i = 0; i < queryVecs.size(); ++i) {
        if (metric == "euclidean") {
            queryObjs.push_back(Query(queryVecs[i], K, calEuclideanDist));
        } else if (metric == "angular") {
            queryObjs.push_back(Query(queryVecs[i], K, calAngularDist));
        } else if (metric == "product") {
            queryObjs.push_back(Query(queryVecs[i], K, calInnerProductDist));
        } else {
            assert(false);
        }
    }

    int itemStartIdx = 0;
    vector<vector<float>> items;
    items.reserve(itemBatchSize);
    int numBatched = 0;
    while (baseFin.read((char*)&dimension, sizeof(int))) {
        vector<float> vec;
        vec.resize(dimension);
        baseFin.read((char*)&vec[0], sizeof(float) * dimension);
        items.push_back(vec);

        if (items.size() == itemBatchSize) {
            updateAll(queryObjs, items, itemStartIdx, numThreads);
            numBatched++;
            cout << numBatched * itemBatchSize << " items have been evaluated" << endl;

            itemStartIdx += items.size();
            items.clear();
        }
    }
    if (items.size() != 0) {
        updateAll(queryObjs, items, itemStartIdx, numThreads);
        itemStartIdx += items.size();
        items.clear();
    }

    baseFin.close();

    GroundWriter writer;
    writer.writeLSHBOX(lshboxBenchFileName, queryObjs, K);

    writer.writeIVECS(ivecsBenchFileName, queryObjs, K);
    return 0;
}
