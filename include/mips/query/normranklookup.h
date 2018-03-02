#pragma once
#include <utility>
#include <iostream>
#include "lshbox/query/fv.h"
#include "base/onetableprober.h"
#include "base/imisequence.h"
#include "lshbox/query/prober.h"
using std::pair;

class LMLOneProber: public OneTableProber<unsigned long long> {
public:
    typedef unsigned long long BIDTYPE;
    LMLOneProber(
        const vector<bool>& hashBits, 
        const FV* fvs, 
        unsigned codelen, 
        unsigned numInterval, 
        unsigned numBitLength,
        const std::function<float(unsigned, unsigned)>& func)
        : fvs_(fvs), 
        inforInterval_(numBitLength, numInterval),
        sequencer_(codelen, numInterval, func) {

        hashBits_.resize(codelen);    
        for (int i = 0; i < codelen; ++i) {
            hashBits_[i] = hashBits[i];
        } 

        triplet_ = sequencer_.next();
    }

    bool hasNext() override {
        return (fvs_->existed(getCurNumBitDiff(), fvsIdx_) || sequencer_.hasNext());
    };

    const pair<float, BIDTYPE>& next() override {
        if (!fvs_->existed(getCurNumBitDiff(), fvsIdx_)) {
            triplet_ = sequencer_.next();
            fvsIdx_ = 0;
        }

        const bool* fv = fvs_->getFlippingVector(getCurNumBitDiff(), fvsIdx_);
        fvsIdx_++;

        BIDTYPE bucket = genBucket(fv, getCurIntervalIdx());
        next_ = std::make_pair(getCurDist(), bucket);
        return next_;
    }

private:
    vector<bool> hashBits_;

    struct InforInterval {
        InforInterval(unsigned numBitLength, unsigned numInterval) {
            numBitLength_ = numBitLength;
            largestIdx_ = numInterval - 1;
        }
        unsigned numBitLength_;
        unsigned largestIdx_;
    } inforInterval_;


    const FV* fvs_;
    unsigned fvsIdx_ = 0;

    pair<float, BIDTYPE> next_;
    IMISequence sequencer_;

    // triplet (dist, numBitDiff, intervalIdx)
    pair<float, pair<unsigned, unsigned>> triplet_;

    float getCurDist() {
        return triplet_.first;
    }

    unsigned getCurNumBitDiff() {
        return triplet_.second.first;
    }

    unsigned getCurIntervalIdx() {
        return inforInterval_.largestIdx_ - triplet_.second.second;
    }

    unsigned long long genBucket(const bool* fv, unsigned intervalIdx) {
        BIDTYPE newBucket = 0;
        for (unsigned i = 0; i < hashBits_.size(); ++i) {
            newBucket <<= 1;
            if (fv[i] == true) {
                newBucket += 1 - hashBits_[i];
            } else {
                newBucket += hashBits_[i];
            }
        }

        // append interval
        // last several bits will not used to flip
        newBucket <<= inforInterval_.numBitLength_;
        newBucket |= intervalIdx;
        return newBucket;
    }
};

template <typename ACCESSOR>
class NormRankLookup: public MTableProber<ACCESSOR, unsigned long long> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
    NormRankLookup(
        const DATATYPE* query,
        lshbox::Scanner<ACCESSOR>& scanner,
        lshbox::LMIP<DATATYPE>& mylsh,
        const FV* fvs) : MTableProber<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {

        this->LTable_.reserve(mylsh.tables.size());
        unsigned numBitHash = mylsh.getHashBitsLen();
        unsigned numBitLength = mylsh.getLengthBitsCount();
        const auto& normIntervals = mylsh.getNormIntervals();
        vector<float> scoreLength(normIntervals.size() - 1);
        for (unsigned i = 0; i < scoreLength.size(); ++i) {
            scoreLength[i] = 1 / normIntervals[i + 1];
        }
        std::sort(scoreLength.begin(), scoreLength.end());

        vector<float> scoreBit(numBitHash + 1);
        scoreBit[0] = 2.0;
        for (unsigned i = 1; i < scoreBit.size(); ++i) {
            scoreBit[i] = 1.0 / i;
        }
        std::sort(scoreBit.begin(), scoreBit.end());

        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {
            auto distor = [numBitHash, numBitLength, scoreBit, scoreLength](unsigned numBitDiff, unsigned intervalIdx) {
                // unsigned numBitSame = numBitHash - numBitDiff;
                // float dist = (1.0 / numBitSame) * normIntervals[normIntervals.size() - intervalIdx + 1];
                float dist = scoreBit[numBitDiff] * scoreLength[intervalIdx];
                return dist;
            };

            float tmp = distor(0, 0);
            this->LTable_.emplace_back(
                LMLOneProber(
                    mylsh.getHashBits(tb, query) 
                    , fvs, numBitHash, normIntervals.size() - 1, numBitLength, distor));
        }

        // initialize minHeap
        this->nextBucket_.resize(this->LTable_.size());
        for (int tb = 0; tb < this->LTable_.size(); ++tb) {
            this->tbNextEnheap(tb);
        }
    }

    OneTableProber<BIDTYPE>* getTableProber (unsigned tb) override {
        return &LTable_[tb];
    }
private:
    vector<LMLOneProber> LTable_;
};
