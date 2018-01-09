#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>
#include <cmath>
#include <thread>
#include <math.h>
using namespace std;
class IdAndDstPair {
    public:
        float distance;
        int id;

        IdAndDstPair(int id, float dst) {
            this->id = id;
            this->distance = dst;
        }
};

class MaxHeapCMP{
    public:
        bool operator()(const IdAndDstPair& a, const IdAndDstPair& b) {
            if (fabs(a.distance - b.distance) > 0.000001)
                return a.distance < b.distance;
            else 
                return a.id < b.id;
        }
};

class TopK {
    public:
        TopK(int K) {
            K_ = K; 
        }
        void insert(const IdAndDstPair& pair) {
            if (maxHeap_.size() < K_)
                maxHeap_.push(pair);
            else {
                if (pair.distance < maxHeap_.top().distance) {
                    maxHeap_.pop();
                    maxHeap_.push(pair);
                }
            }
        }

        vector<IdAndDstPair> getTopK() {
            priority_queue<IdAndDstPair, vector<IdAndDstPair>, MaxHeapCMP> heap = maxHeap_;
            vector<IdAndDstPair> results;
            while (!heap.empty()) {
                results.push_back(heap.top());
                heap.pop();
            }
            std::sort(results.begin(), results.end(), MaxHeapCMP());
            return results;
        }
    private:
        int K_;
        priority_queue<IdAndDstPair, vector<IdAndDstPair>, MaxHeapCMP> maxHeap_;
};


void test() {
    TopK toker(3);
    toker.insert(IdAndDstPair(0, 1));
    toker.insert(IdAndDstPair(1, 1.2));
    toker.insert(IdAndDstPair(2, 1.1));
}

float calEuclideanDist(const vector<float>& query, const vector<float>& item) {
    float l2Dst = 0;
    assert(query.size() == item.size());
    for (int i = 0; i < query.size(); ++i) {
        l2Dst += (query[i] - item[i]) * (query[i] - item[i]);
    }
    return sqrt(l2Dst);
}

static float calAngularDist(const vector<float>& query, const vector<float>& item) {
    float cosDst = 0;
    assert(query.size() == item.size());
    float qNorm = 0;
    float iNorm = 0;
    for (int i = 0; i < query.size(); ++i) {
        qNorm += query[i] * query[i];
        iNorm += item[i] * item[i];
    }
    qNorm = sqrt(qNorm);
    iNorm = sqrt(iNorm);
    for (int i = 0; i < query.size(); ++i) {
        cosDst += query[i] * item[i];
    } 
    cosDst /= (qNorm * iNorm); 
    return acos(cosDst);
}

static float calInnerProductDist(const vector<float >& query, const vector<float >& item) {
    float ipDist = 0;
    assert(query.size() == item.size());
    for (int i = 0; i < query.size(); ++i) {
        ipDist -= (query[i] * item[i]);
    }
    return ipDist;
}

class Query {
    public: 
        vector<float> content;
        TopK topk;
        std::function<float(const vector<float>&, const vector<float>&)> distor;

        Query(const vector<float>& cont, int K, std::function<float(const vector<float>&, const vector<float>&)> functor) : topk(K){
            this->content = cont;
            this->distor = functor;
        }

        void evaluate(const vector<float>& item, int itemId) {
            float distance;
            distance = distor(this->content, item);
            topk.insert(IdAndDstPair(itemId, distance));
        }

        float calCosDst(const vector<float>& item) {
        }

        vector<IdAndDstPair> getTopK() {
            return this->topk.getTopK();
        }
};

void updateQueries(vector<Query*> queries, const vector<vector<float>>* itemsPtr, int itemStartIdx) {
    for (auto& query: queries) {
        for (int i = 0; i < itemsPtr->size(); ++i) {
            query->evaluate((*itemsPtr)[i], itemStartIdx + i);
        }
    }
}

void updateAll(vector<Query>& queries, const vector<vector<float>>& items, int itemStartIdx, int numThreads = 4) {
    vector<thread> threads;
    int numQueriesPerThread = queries.size() / numThreads + 1;

    int queryIdx = 0;
    vector<Query*> queryLinks;
    while(queryIdx < queries.size()) {
        queryLinks.push_back(&queries[queryIdx++]);
        if (queryLinks.size() == numQueriesPerThread) {
            threads.push_back(thread(updateQueries, queryLinks, &items, itemStartIdx));
            queryLinks.clear();
        }
    }
    threads.push_back(thread(updateQueries, queryLinks, &items, itemStartIdx));
    for (int i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
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

    // vector<Query> queryObjs(queryVecs.size());
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

    // lshbox file
    ofstream lshboxFout(lshboxBenchFileName);
    if (!lshboxFout) {
        cout << "cannot create output file " << lshboxBenchFileName << endl;
    }
    lshboxFout << queryObjs.size() << "\t" << K << endl;
    for (int i = 0; i < queryObjs.size(); ++i) {
        lshboxFout << i << "\t";
        vector<IdAndDstPair> topker = queryObjs[i].getTopK();
        for (int idx = 0; idx < topker.size(); ++idx) {
            lshboxFout << topker[idx].id << "\t" << topker[idx].distance << "\t";
        }
        lshboxFout << endl;
    }
    lshboxFout.close();

    // ivecs file
    ofstream fout(ivecsBenchFileName, ios::binary);
    if (!fout) {
        cout << "cannot create output file " << ivecsBenchFileName << endl;
    }
    for (int i = 0; i < queryObjs.size(); ++i) {
        fout.write((char*)&K, sizeof(int));
        vector<IdAndDstPair> topker = queryObjs[i].getTopK();
        for (int idx = 0; idx < topker.size(); ++idx) {
            fout.write((char*)&topker[idx].id, sizeof(int));
        }
    }
    fout.close();
    return 0;
}
