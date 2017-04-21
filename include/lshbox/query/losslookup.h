#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <queue>
#include <lshbox/query/fv.h>
#pragma once

// layer equals to hamDist
class LayerScoreComparator {
public: 
    bool operator() (
        const std::pair<unsigned int, float>& a,
        const std::pair<unsigned int, float>& b) const {
        return a.second > b.second;
    }
};

template<typename ACCESSOR>
class LossLookup{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    LossLookup(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        FV* fvs) : scanner_(scanner) {

        // initialize scanner_, hashBits_, and R_
        scanner_.reset(domin);
        hashBits_ = mylsh.getHashBits(0, domin);
        R_ = hashBits_.size();

        // initialize posLossPairs_
        std::vector<float> hashFloats = mylsh.getHashFloats(0, domin);
        posLossPairs_.resize(hashFloats.size());
        for (int i = 0; i < hashFloats.size(); ++i) {
            posLossPairs_[i].first = i;
            posLossPairs_[i].second = fabs(hashFloats[i]);
        }

        std::sort(posLossPairs_.begin(), 
            posLossPairs_.end(), 
            [] (const std::pair<int, float>& a, const std::pair<int , float>& b ) {
                return a.second < b.second;
            });

        // initialize fvs_
        
        fvs_ = fvs;
        // initialize idxToLayer_ and then minHeap
        idxToLayer_.resize(R_ + 1);
        for (int layer = 0; layer < idxToLayer_.size(); ++layer) {
            idxToLayer_[layer] = 0;
            insertHeap_(layer, 0); 
        }

    }

    BIDTYPE getNextBID(){
        numBucketsProbed_++;

        unsigned int layer = popHeap_();
        const bool* fv = fvs_->getFlippingVector(layer, idxToLayer_[layer]);

        // apply flipping
        std::vector<bool> newHashBits = hashBits_;
        for (int i = 0; i < R_; ++i) {
            if (fv[i] == true) {
                newHashBits[posLossPairs_[i].first] = 
                    1 - newHashBits[posLossPairs_[i].first];
            }
        }

        // cal new Bucket
        BIDTYPE newBucket = 0;
        for (int i = 0; i < newHashBits.size() ; ++i) {
            newBucket <<= 1;
            if (newHashBits[i] == true) {
                newBucket += 1;
            }
        }

        // insert next element to minHeap
        idxToLayer_[layer]++;
        insertHeap_(layer, idxToLayer_[layer]);

        // return value
        return newBucket;
        
    }

    bool nextBucketExisted() {
        if (minHeap_.empty()) return false;
        else return true;
    }
    void operator()(unsigned key){
        numItemsProbed_++;
        // scanner_(key);
    }

    int getNumItemsProbed() { // get number of items probed;
        return numItemsProbed_;
    }

private:
    std::vector<bool> hashBits_;
    unsigned R_;
    // example: 
    // if queryBits = 101, queryFloats = 0.1, -0.05, 0.9 
    // posLossPairs_ = (1, 0.05), (0, 0.1), (2, 0.9)
    std::vector<std::pair<unsigned int, float>> posLossPairs_;

    // FVSIdx: FVSIdx[i] remembers the idx of hamming distance i of flopping vectors
    // priority queue: (layer, score) pairs
    std::vector<unsigned int> idxToLayer_;

    std::priority_queue<
        std::pair<unsigned int, float>,
        std::vector<std::pair<unsigned int, float> >,
        LayerScoreComparator> minHeap_; 

    const FV* fvs_ = NULL;

    lshbox::Scanner<ACCESSOR> scanner_;
    unsigned int numItemsProbed_ = 0;
    unsigned int numBucketsProbed_ = 0;

    // insert the next element in layer $layer to minHeap_
    void insertHeap_(int layer, int idxToLayer) {

        // extract fv
        if (!fvs_->existed(layer, idxToLayer)) {
            return ;
        }
        const bool* fv = fvs_->getFlippingVector(layer, idxToLayer);

        // calculare score 
        float score = 0;
        for (int idxToFV = 0; idxToFV < R_; ++idxToFV) {
            if (fv[idxToFV] == true) {
                score += posLossPairs_[idxToFV].second;
            }
        }

        // push to minHeap 
        minHeap_.push(std::make_pair(layer, score));
    }

    // return layer
    unsigned int popHeap_() {
        unsigned int layer = minHeap_.top().first;
        minHeap_.pop();
        return layer;
    }
};
