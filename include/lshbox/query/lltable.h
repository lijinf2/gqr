#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <queue>
#include <lshbox/query/fv.h>
#include <lshbox/query/scoreidxpair.h>
#pragma once
class LLTable{
public:
    typedef unsigned long long BIDTYPE;
    typedef std::unordered_map<BIDTYPE, std::vector<unsigned> > TableT;
    LLTable(
        const std::vector<bool>& queryBits,    
        const std::vector<float>& queryloss,
        const TableT* table,
        const FV* fvs) {

        // initialize table_
        queryBits_ = queryBits;
        table_ = table;
        fvs_ = fvs;

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
        
        // initialize iterators_ and then minHeap
        iterators_.resize(queryloss.size() + 1);
        for (unsigned R = 0; R < iterators_.size(); ++R) {
            iterators_[R].first = -1;
            // initialize iterators_[R].bucket
            enheap(R);
        }
    }

    BIDTYPE getCurBucket() {
        unsigned R = minHeap_.top().index_;
        return iterators_[R].second;
    }

    float getCurScore() {
        return minHeap_.top().score_;
    }

    bool moveForward() {
        unsigned R = minHeap_.top().index_;
        minHeap_.pop();
        enheap(R);
        return !minHeap_.empty();
    }

private:
    // example: 
    // if queryBits = 101, queryFloats = 0.1, -0.05, 0.9 
    // posLossPairs_ = (1, 0.05), (0, 0.1), (2, 0.9)
    const TableT * table_ = NULL;
    std::vector<bool> queryBits_;
    std::vector<std::pair<unsigned int, float>> posLossPairs_;

    const FV* fvs_ = NULL;
    // FVSIdx: FVSIdx[i] remembers the idx of hamming distance i of flopping vectors
    // first -> pointer to R + 1 layers, second -> buckets
    std::vector<std::pair<unsigned int, BIDTYPE>> iterators_;

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

    BIDTYPE calBucket(const bool* fv) {
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

    void enheap(int R) {  // move ahead iterator of R flipping vector sequence 
        // ignore empty bucket

        while (true) {
            iterators_[R].first++;
            if (!fvs_->existed(R, iterators_[R].first)) {
                break;
            }
            const bool* fv = fvs_->getFlippingVector(R, iterators_[R].first);
            //debug
            std::vector<bool> copyFV;
            for (int i = 0; i < queryBits_.size(); ++i) {
                copyFV.push_back(fv[i]);
            }
            //end debug
            auto bucketID = calBucket(fv);
            // assert (bucketID != 59474);
            if((*table_).find(bucketID) != (*table_).end()) {
                // insert into heap
                iterators_[R].second = bucketID;
                float score = calScore(fv);
                minHeap_.push(ScoreIdxPair(score, R));
                break;
            }
        }
    }
};
