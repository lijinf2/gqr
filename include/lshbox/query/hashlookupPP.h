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
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;

    template<typename LSHTYPE>
    HashLookupPP(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        FV* fvs) : Prober<ACCESSOR>(domin, scanner, mylsh), fvs_(fvs) {

        layer_ = 0;
        idxToLayer_ = 0;
        table_ = 0;
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        if (table_ == this->hashBits_.size()) {
            table_ = 0;
            idxToLayer_++;
            if (!fvs_->existed(layer_, idxToLayer_)) {
                layer_++;
                idxToLayer_ = 0;
            }
        }

        const bool* fv = fvs_->getFlippingVector(layer_, idxToLayer_);

        BIDTYPE newBucket = 0;
        for (unsigned i = 0; i < this->R_; ++i) {
            newBucket <<= 1; 
            if (fv[i] == true) {
                newBucket += 1 - this->hashBits_[table_][i];
            } else {
                newBucket += this->hashBits_[table_][i];
            }
        }
        
        std::pair<unsigned, BIDTYPE> result = std::make_pair(table_, newBucket);
        table_++;

        return result;
    }

private:
    const FV* fvs_ = NULL;
    unsigned layer_;
    unsigned idxToLayer_;
    unsigned table_ = 0;
};
