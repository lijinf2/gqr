#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include "gqr/util/gqrhash.h"
#include <lshbox/query/tree.h>
#include <lshbox/query/scoreidxpair.h>
#pragma once
using lshbox::gqrhash;
// will ignore the first bucket, i.e. 00000
class TSTable{
public:
    typedef unsigned long long BIDTYPE;
    typedef std::unordered_map<BIDTYPE, std::vector<unsigned>, gqrhash<BIDTYPE>> TableT;
    TSTable(
        const std::vector<bool>& queryBits,    
        const std::vector<float>& queryloss,
        const Tree* tree) {

        queryBits_ = queryBits;
        tree_ = tree;
        upperIdx = tree_->getSize() / 2 - 1;

        // initialize posLossPairs_
        posLossPairs_.resize(queryloss.size());
        for (unsigned int idx = 0; idx < posLossPairs_.size(); ++idx) {
            posLossPairs_[idx].first = idx;
            posLossPairs_[idx].second = queryloss[idx];
        }

        std::sort(posLossPairs_.begin(), posLossPairs_.end(), 
            [] (const std::pair<unsigned, float>& a, const std::pair<unsigned, float>& b ) {
                return a.second < b.second;
        });
        
        minHeap_.emplace(ScoreIdxPair(posLossPairs_[0].second, 0));
    }

    BIDTYPE getCurBucket() {
        unsigned idx = minHeap_.top().index_;
        float score = minHeap_.top().score_;
        shiftAndExpand(idx, score);
        minHeap_.pop();
        // get flipping vector
        const bool* fv = tree_->getFV(idx);
        return calBucket(fv);
    }

    float getCurScore() {
        return minHeap_.top().score_;
    }

    // equals to next bucket exists
    bool moveForward() {
        return !minHeap_.empty();
        const unsigned& idx = minHeap_.top().index_;

        minHeap_.pop();
    }

private:
    // example: 
    // if queryBits = 101, queryFloats = 0.1, -0.05, 0.9 
    // posLossPairs_ = (1, 0.05), (0, 0.1), (2, 0.9)
    std::vector<bool> queryBits_;
    std::vector<std::pair<unsigned int, float>> posLossPairs_;

    unsigned upperIdx = -1; // maximum idx that can be shifed and expanded

    const Tree* tree_ = NULL;
    std::priority_queue<ScoreIdxPair> minHeap_; // <score, r> pairs

    float calScore(const bool* fv) {
        float score = 0;
        for (unsigned idx = 0; idx < posLossPairs_.size(); ++idx) {
            if (fv[idx]) {
                score += posLossPairs_[idx].second;
            }
        }
        return score;
    }

    BIDTYPE calBucket(const bool* fv) const {
        std::vector<bool> newHashBits = queryBits_;

        // apply flipping
        for (unsigned int i = 0; i < newHashBits.size(); ++i) {
            if (fv[i]) {
                newHashBits[posLossPairs_[i].first] = 
                    1 - newHashBits[posLossPairs_[i].first];
            }
        }

        // calculate bucketID
        BIDTYPE bucketID = 0;
        for (unsigned i = 0; i < newHashBits.size() ; ++i) {
            bucketID <<= 1;
            if (newHashBits[i] == true) {
                bucketID |= 1;
            }
        }
        return bucketID;
    }

    void shiftAndExpand(unsigned idx, float score) {
        // update minHeap_
        if (idx <= upperIdx) {
            const unsigned& lastOnePos = tree_->getLastOne(idx);
            // shift
            unsigned shiftIdx = 2 * idx + 1; 
            const bool* shiftFV = tree_->getFV(shiftIdx);
            // float shiftScore = calScore(shiftFV);
            float shiftScore = 
                score - posLossPairs_[lastOnePos].second + posLossPairs_[lastOnePos + 1].second;
            minHeap_.emplace(ScoreIdxPair(shiftScore, shiftIdx));

            // expand
            unsigned expandIdx = 2 * idx + 2;

            const bool* expandFV = tree_->getFV(expandIdx);
            // float expandScore = calScore(expandFV);
            float expandScore = 
                score + posLossPairs_[lastOnePos + 1].second;
            minHeap_.emplace(ScoreIdxPair(expandScore, expandIdx));
        }
    }
};
