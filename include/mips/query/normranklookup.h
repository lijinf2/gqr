#pragma once
#include <utility>
#include "lshbox/query/fv.h"
#include "base/onetableprober.h"
#include "base/imisequence.h"
#include "lshbox/query/prober.h"
#include "lshbox/query/mip/lmip.h"
using std::pair;

class LMLOneProber: public OneTableProber<unsigned long long> {
public:
    typedef lshbo::LMIP<DATATYPE> LSHTYPE;
    LMLOneProber(
        const vector<bool>& hashBits, 
        const FV* fvs, 
        unsigned codelen, 
        unsigned numInterval, 
        unsigned numBitLength,
        std::function<float(unsigned, unsigned)> func)
        : hashBits_(hashBits), fvs_(fvs), codelength_(codelen), distor(func),
        sequencer_(codelen, numInterval, distor) {
        
        numBitLength_ = numBitLength;
        mask_ = 0;
        for (unsigned i = 0; i < numBitLength; ++i) {
            mask_ <<= 1;
            mask_ &= 1;
        }
    }

    bool hasNext() override;

    const pair<float, BIDTYPE>& next() override {
        if (!fvs_->existed(getCurNumBitDiff(), fvsIdx_)) {
            triplet_ = sequencer_.next();
            fvsIdx_ = 0;
        }

        const bool* fv = fvs_->getFlippingVector(getCurNumBitDiff(), fvsIdx_);
        fvsIdx_++;

        BIDTYPE bucket = genBucket(fv, getCurIntervalIdx());
        return std::make_pair(getCurDist(), bucket);
    }

private:
    const vector<bool>& hashBits_;
    unsigned numBitLength_;
    unsinged mask_;

    const FV* fvs_;
    unsigned fvsIdx_ = 0;
    unsigned codeLength_;
    std::function<float(unsigned, unsigned)> distor;

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
class LengthMarkedLookup: public MTableProber<ACCESSOR, unsigned long long> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
    LengthMarkedLookup(
        const DATATYPE* query,
        lshbox::Scanner<ACCESSOR>& scanner,
        lshbox::LMIP<DATATYPE>& mylsh) : MTableProber<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {

        this->LTable_.reserve(mylsh.tables.size());
        unsigned numBitHash = mylsh.getHashBitsLen();
        unsigned numBitLength = mylsh.getLengthBigsCount();
        const auto& normIntervals = mylsh.getNormIntervals();
        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {
            auto distor = [&numBitHash, &numBitLength, &normIntervals](unsigned numBitDiff, unsigned intervalIdx) {
                unsigned numBitSame = numBitHash - numBitDiff;
                float dist = (numBitHash / 32.0 - numSameBit) * normIntervals[intervalIdx];
                return dist;
            };
            this->LTable_.emplace_back(
                BucketList<BIDTYPE>(mylsh.tables[tb], distor));
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
    vector<LMLOneProber<BIDTYPE>> LTable_;
};
