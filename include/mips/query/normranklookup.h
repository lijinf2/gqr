#pragma once
#include <utility>
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
        : hashBits_(hashBits), fvs_(fvs), 
        sequencer_(codelen, numInterval, func) {
        
        numBitLength_ = numBitLength;
        mask_ = 0;
        for (unsigned i = 0; i < numBitLength; ++i) {
            mask_ <<= 1;
            mask_ &= 1;
        }
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
    unsigned numBitLength_;
    unsigned mask_;

    const FV* fvs_;
    unsigned fvsIdx_ = 0;

    pair<float, BIDTYPE> next_;
    IMISequence sequencer_;

    // triplet (dist, numBitDiff, intervalIdx)
    pair<float, pair<unsigned, unsigned>> triplet_;

    unsigned getCurDist() {
        return triplet_.first;
    }

    unsigned getCurNumBitDiff() {
        return triplet_.second.first;
    }

    unsigned getCurIntervalIdx() {
        return triplet_.second.second + 1;
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
        newBucket <<= numBitLength_;
        newBucket |= intervalIdx;
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
        // const auto& normIntervals = mylsh.getNormIntervals();
        auto normIntervals = mylsh.getNormIntervals();
        for (auto& e : normIntervals) {
                e = 1 / e;
        }

        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {
            auto distor = [numBitHash, numBitLength, normIntervals](unsigned numBitDiff, unsigned intervalIdx) {
                unsigned numBitSame = numBitHash - numBitDiff;
                float dist = (1 / numBitSame) * normIntervals[intervalIdx + 1];
                return dist;
            };
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
