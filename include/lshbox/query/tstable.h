#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <lshbox/query/tree.h>
#include <lshbox/query/scoreidxpair.h>
#pragma once
// will ignore the first bucket, i.e. 00000
class TSTable{
public:
    typedef unsigned long long BIDTYPE;
    typedef std::unordered_map<BIDTYPE, std::vector<unsigned> > TableT;
    TSTable(
        const std::vector<bool>& queryBits,    
        const std::vector<float>& queryloss,
        const TableT* table,
        const Tree* tree) {

        // initialize table_
        queryBits_ = queryBits;
        table_ = table;
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
        
        minHeap_.push(ScoreIdxPair(posLossPairs_[0].second, 0));
    }

    BIDTYPE getCurBucket() const  {
        unsigned idx = minHeap_.top().index_;

        // get flipping vector
        const bool* fv = tree_->getFV(idx);
        return calBucket(fv);
    }

    float getCurScore() {
        return minHeap_.top().score_;
    }

    // here we use relation to apply score computation, save some overhead, 
    // but result has very little difference from lossranking since accuracy of float number computation
    // if want to exact the same result, use calScore instead
    bool moveForward() {
        const unsigned& idx = minHeap_.top().index_;
        const float& score = minHeap_.top().score_;
        const unsigned& lastOnePos = tree_->getLastOne(idx);

        if (idx <= upperIdx) {
            // shift
            unsigned shiftIdx = 2 * idx + 1; 
            const bool* shiftFV = tree_->getFV(shiftIdx);
            // float shiftScore = calScore(shiftFV);
            float shiftScore = 
                score - posLossPairs_[lastOnePos].second + posLossPairs_[lastOnePos + 1].second;
            minHeap_.push(ScoreIdxPair(shiftScore, shiftIdx));

            // expand
            unsigned expandIdx = 2 * idx + 2;

            const bool* expandFV = tree_->getFV(expandIdx);
            // float expandScore = calScore(expandFV);
            float expandScore = 
                score + posLossPairs_[lastOnePos + 1].second;
            minHeap_.push(ScoreIdxPair(expandScore, expandIdx));
        }
        minHeap_.pop();
        
        return !minHeap_.empty();
    }

private:
    // example: 
    // if queryBits = 101, queryFloats = 0.1, -0.05, 0.9 
    // posLossPairs_ = (1, 0.05), (0, 0.1), (2, 0.9)
    const TableT * table_ = NULL;
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
                bucketID += 1;
            }
        }
        return bucketID;
    }
};
