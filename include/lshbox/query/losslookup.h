#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <queue>
#include <lshbox/query/fv.h>
#include <lshbox/query/prober.h>
#pragma once

// layer equals to hamDist
struct HeapUnit{
    HeapUnit(unsigned t, unsigned l, float s) {
        table = t;
        layer = l;
        score = s;
    }
    bool operator<(const HeapUnit& other) const  {
        return score > other.score;
    }
    unsigned table;
    unsigned layer;
    float score;
};

class TableHandler{
public:
    // example: 
    // if queryBits = 101, queryFloats = 0.1, -0.05, 0.9 
    // posLossPairs_ = (1, 0.05), (0, 0.1), (2, 0.9)
    std::vector<std::pair<unsigned int, float>> posLossPairs;

    // FVSIdx: FVSIdx[i] remembers the idx of hamming distance i of flopping vectors
    // priority queue: (layer, score) pairs
    std::vector<unsigned int> idxToLayer;
};

template<typename ACCESSOR>
class LossLookup : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    // typedef unsigned long long BIDTYPE;
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;

    template<typename LSHTYPE>
    LossLookup(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        FV* fvs) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        // initialize fvs_, handlers_ , meanHeap_
        fvs_ = fvs;

        handlers_.resize(mylsh.param.L);
        for (unsigned t = 0; t < mylsh.param.L; ++t) {
            std::vector<float> hashFloats = mylsh.getHashFloats(t, domin);
            handlers_[t].posLossPairs.resize(hashFloats.size());
            for (unsigned int idx = 0; idx < hashFloats.size(); ++idx) {
                handlers_[t].posLossPairs[idx].first = idx;
                handlers_[t].posLossPairs[idx].second = fabs(hashFloats[idx]);
            }

            std::sort(handlers_[t].posLossPairs.begin(), 
                handlers_[t].posLossPairs.end(), 
                [] (const std::pair<unsigned, float>& a, const std::pair<unsigned, float>& b ) {
                    return a.second < b.second;
            });
            
            // initialize idxToLayer_ and then minHeap
            handlers_[t].idxToLayer.resize(this->R_ + 1);
            for (unsigned int layer = 0; layer < handlers_[t].idxToLayer.size(); ++layer) {
                handlers_[t].idxToLayer[layer] = 0;
                insertHeap_(t, layer); 
            }

        }

    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        unsigned int table = minHeap_.top().table;
        unsigned int layer = minHeap_.top().layer;
        minHeap_.pop();
        const bool* fv = fvs_->getFlippingVector(layer, handlers_[table].idxToLayer[layer]);

        // apply flippinif while g
        std::vector<bool> newHashBits = this->hashBits_[table];
        for (unsigned int i = 0; i < this->R_; ++i) {
            if (fv[i] == true) {
                newHashBits[handlers_[table].posLossPairs[i].first] = 
                    1 - newHashBits[handlers_[table].posLossPairs[i].first];
            }
        }

        // cal new Bucket
        BIDTYPE newBucket = 0;
        for (unsigned i = 0; i < newHashBits.size() ; ++i) {
            newBucket <<= 1;
            if (newHashBits[i] == true) {
                newBucket += 1;
            }
        }

        // insert next element to minHeap
        handlers_[table].idxToLayer[layer]++;
        insertHeap_(table, layer);

        // return value
        return std::make_pair(table, newBucket);
        
    }

private:
    const FV* fvs_ = NULL;
    std::vector<TableHandler> handlers_;
    std::priority_queue<HeapUnit> minHeap_; 

    // insert the current element in layer $layer to minHeap_
    void insertHeap_(unsigned t, unsigned layer) {

        unsigned& idxToLayer = handlers_[t].idxToLayer[layer];

        // extract fv
        if (!fvs_->existed(layer, idxToLayer)) {
            return;
        }

        const bool* fv = fvs_->getFlippingVector(layer, idxToLayer);

        // calculate score 
        float score = 0;
        for (int idxToFV = 0; idxToFV < this->R_; ++idxToFV) {
            if (fv[idxToFV] == true) {
                score += handlers_[t].posLossPairs[idxToFV].second;
            }
        }

        // push to minHeapk
        minHeap_.push(HeapUnit(t, layer, score));
    }

};
