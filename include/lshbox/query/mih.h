// hashlookup++, i.e. hashlookuppp, hash lookup with shared PTB
// only work in single-thread environment
#include <vector>
#include <map>
#include <lshbox/query/prober.h>
#include <lshbox/query/fv.h>
#include <unordered_map>
#include <unordered_set>
#pragma once
template<typename ACCESSOR>
class MIH : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;

    template<typename LSHTYPE>
    MIH(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        const std::vector<std::unordered_map<BIDTYPE, std::vector<BIDTYPE> > >& subtables,
        unsigned substringNum) :
            Prober<ACCESSOR>(domin, scanner, mylsh),
            substringNum_(substringNum),
            substringLen_(mylsh.getCodeLength() / substringNum_),
            fvs_(substringLen_),
            currentFv_(fvs_.getFlippingVector(layer_, idxToLayer_)),
            queryHashVal_(mylsh.getHashVal(0, domin)),
            subtables_(subtables) {}

    unsigned computeHammingDist(BIDTYPE bucketId) {
        BIDTYPE xorVal = queryHashVal_ ^ bucketId;

        unsigned hamDist = 0;
        // cal Number of 1, algorithm1, faster than 2, three instructions in while loop
        // key idea: always deduct a one from xorVal
        while(xorVal != 0){
            hamDist++;
            xorVal &= xorVal - 1; 
            // property of xorVal - 1: digits from last one will be flipped
            // e.g.   000100 100 - 1 = 000100 011
            // i.e. can stop earlier compare with check bit by bit
        }

        return hamDist;
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        while (true) {
            switch (nextProbeState_) {
                case 0: {
                    while (subtableIter_ < curBucketList_->size()) {
                        BIDTYPE bid = (*curBucketList_)[subtableIter_];
                        ++subtableIter_;
                        if (computeHammingDist(bid) == hammingDist_) {
                            return std::make_pair(0, bid);
                        }
                    }
                    nextProbeState_ = 1;
                    break;
                }
                
                case 1: {
                    subtableIter_ = 0;
                    ++substringId_;

                    while (substringId_ < substringNum_) {
                        currentSubBID_ = 0;
                        unsigned prev_i = substringId_ * substringLen_;
                        unsigned next_i = prev_i + substringLen_;
                        for (unsigned i = prev_i; i < next_i; ++i) {
                            currentSubBID_ <<= 1; 
                            if (currentFv_[i - prev_i] == true) {
                                currentSubBID_ += 1 - this->hashBits_[table_][i];
                            } else {
                                currentSubBID_ += this->hashBits_[table_][i];
                            }
                        }

                        curSubTable_ = &subtables_[substringId_];
                        auto it = curSubTable_->find(currentSubBID_);
                        if (it != curSubTable_->end()) {
                            curBucketList_ = &(it->second);
                            nextProbeState_ = 0;
                            break;
                        }

                        ++substringId_;
                    }

                    if (substringId_ >= substringNum_) {
                        nextProbeState_ = 2;
                    }
                    break;
                }

                case 2: {
                    substringId_ = -1;
                    subtableIter_ = 0;

                    idxToLayer_++;
                    if (!fvs_.existed(layer_, idxToLayer_)) {
                        layer_++;
                        idxToLayer_ = 0;

                        if (layer_ > hammingDistsubstring_) {
                            ++hammingDist_;
                            hammingDistsubstring_ = hammingDist_ / substringNum_;
                            idxToLayer_ = 0;
                            layer_ = 0;
                        }
                    }

                    currentFv_ = fvs_.getFlippingVector(layer_, idxToLayer_);

                    nextProbeState_ = 1;
                    break;
                }
            }
        }
    }

private:
    unsigned substringNum_;
    unsigned substringLen_;
    FV fvs_ = NULL;
    unsigned layer_ = 0;
    unsigned idxToLayer_ = 0;
    const bool* currentFv_;
    const std::vector<std::unordered_map<BIDTYPE, std::vector<BIDTYPE> > >& subtables_;
    const std::unordered_map<BIDTYPE, std::vector<BIDTYPE> >* curSubTable_;
    const std::vector<BIDTYPE>* curBucketList_;
    unsigned table_ = 0;
    unsigned substringId_ = -1;
    unsigned hammingDist_ = 0;
    unsigned hammingDistsubstring_ = 0;
    unsigned subtableIter_ = 0;
    BIDTYPE currentSubBID_;
    unsigned nextProbeState_ = 1;
    BIDTYPE queryHashVal_;
};
