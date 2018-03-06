#pragma once
#include <utility>
#include <iostream>
#include "lshbox/query/fv.h"
#include "base/onetableprober.h"
#include "base/sortedlist.h"
#include "base/mtableprober.h"
#include "lshbox/query/prober.h"

#include <mips/normrange/normrangehasher.h>

using std::pair;

class PreSortOneProber: public OneTableProber<unsigned long long> {
public:
    typedef unsigned long long BIDTYPE;
    PreSortOneProber(
            const vector<bool>& hashBits,
            const FV* fvs,
            unsigned codelen,
            unsigned numInterval,
            unsigned numBitLength,
            SortedNormRange* sortedNormRange)
            : fvs_(fvs),
              inforInterval_(numBitLength, numInterval),
              sequencer_(sortedNormRange) {

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
    SortedNormRangeSequence sequencer_;

    // triplet (dist, numBitDiff, intervalIdx)
    pair<float, pair<unsigned, unsigned>> triplet_;

    float getCurDist() {
        return triplet_.first;
    }

    unsigned getCurNumBitDiff() {
        return triplet_.second.first;
    }

    unsigned getCurIntervalIdx() {
        return triplet_.second.second;
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
class NormRankPreSort: public MTableProber<ACCESSOR, unsigned long long> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
NormRankPreSort(
            const DATATYPE* query,
            lshbox::Scanner<ACCESSOR>& scanner,
            lshbox::NormRangeHasher<DATATYPE>& mylsh,
            const FV* fvs,
            SortedNormRange* sortedNormRange) : MTableProber<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {

        this->LTable_.reserve(mylsh.tables.size());
        unsigned numBitHash = mylsh.getHashBitsLen();
        unsigned numBitLength = mylsh.getLengthBitsCount();
        const auto& normIntervals = mylsh.getNormIntervals();


        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {

            this->LTable_.emplace_back(
                    PreSortOneProber(
                            mylsh.getHashBits(tb, query),
                            fvs,
                            numBitHash,
                            normIntervals.size() - 1,
                            numBitLength, sortedNormRange));
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
    vector<PreSortOneProber> LTable_;
};
