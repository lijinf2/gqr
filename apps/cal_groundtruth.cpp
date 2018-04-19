#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include "gqr/util/cal_groundtruth.h"
using namespace std;
using namespace lshbox;

unsigned readFvecs(vector<vector<float>>& fvecs, ifstream& fin, unsigned maxNumRecords = UINT_MAX) {
    int readNumRecords = 0;
    int dimension;
    while(readNumRecords < maxNumRecords && fin.read((char*)&dimension, sizeof(int))) {
        readNumRecords++;
        vector<float> vec;
        vec.resize(dimension);
        fin.read((char*)&vec[0], sizeof(float) * dimension);
        fvecs.emplace_back(vec);
    }
    return readNumRecords;
}

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
    readFvecs(queryVecs, queryFin);
    queryFin.close();

    ifstream baseFin(baseFileName, ios::binary);
    if (!baseFin) {
        cout << "base File " << baseFileName << " does not exist" << endl;
        return 0;
    }

    typedef GTQuery<float> GTQuery;
    vector<GTQuery> queryObjs;
    for (int i = 0; i < queryVecs.size(); ++i) {
        if (metric == "euclidean") {
            queryObjs.push_back(GTQuery(queryVecs[i], K, calEuclideanDist));
        } else if (metric == "angular") {
            queryObjs.push_back(GTQuery(queryVecs[i], K, calAngularDist));
        } else if (metric == "product") {
            queryObjs.push_back(GTQuery(queryVecs[i], K, calInnerProductDist));
        } else {
            assert(false);
        }
    }

    int itemStartIdx = 0;
    vector<vector<float>> items;
    items.reserve(itemBatchSize);
    while (true) {
        readFvecs(items, baseFin, itemBatchSize);
        if (items.size() == 0) {
            break;
        }

        updateAll(queryObjs, items, itemStartIdx, numThreads);
        itemStartIdx += items.size();
        cout << itemStartIdx << " items have been evaluated" << endl;
        items.clear();
    }

    baseFin.close();

    GroundWriter writer;
    writer.writeLSHBOX(lshboxBenchFileName, queryObjs);

    writer.writeIVECS(ivecsBenchFileName, queryObjs);
    return 0;
}
