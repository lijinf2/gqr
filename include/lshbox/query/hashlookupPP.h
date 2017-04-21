// hashlookup++, i.e. hashlookuppp, hash lookup with shared PTB
// only work in single-thread environment
#include <vector>
#include <map>
#include <lshbox/query/prober.h>
#include <lshbox/query/fv.h>
#pragma once
template<typename ACCESSOR>
class HashLookupPP : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    HashLookupPP(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        FV* fvs) : Prober<ACCESSOR>(domin, scanner), fvs_(fvs) {

        hashBits_ = mylsh.getHashBits(0, domin);
        R_ = hashBits_.size();
        layer_ = 0;
        idxToLayer_ = 0;
    }

    BIDTYPE getNextBID(){
        this->numBucketsProbed_++;

        const bool* fv = fvs_->getFlippingVector(layer_, idxToLayer_);

        BIDTYPE newBucket = 0;
        for (int i = 0; i < R_; ++i) {
            newBucket <<= 1; 
            if (fv[i] == true) {
                newBucket += 1 - hashBits_[i];
            } else {
                newBucket += hashBits_[i];
            }
        }
        
        idxToLayer_++;
        if (!fvs_->existed(layer_, idxToLayer_)) {
            layer_++;
            idxToLayer_ = 0;
        }

        return newBucket;
    }

    bool nextBucketExisted() {
        if (fvs_->existed(layer_, idxToLayer_)) return true;
        else return false;
    }

private:
    const FV* fvs_ = NULL;
    std::vector<bool> hashBits_;
    unsigned R_;
    unsigned layer_;
    unsigned idxToLayer_;
};
