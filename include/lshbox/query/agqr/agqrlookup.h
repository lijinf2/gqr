#pragma once
#include "../treelookup.h"
/*
 * AGQR represents angular distance based GQR 
 * */
using std::priority_queue;
template<typename ACCESSOR>
class AGQRLookup: public TreeLookup<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;

    template<typename LSHTYPE>
    AGQRLookup(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        Tree* tree) : TreeLookup<ACCESSOR>(domin, scanner, mylsh, tree) {

        // useless
        float l2norm = this->calL2Norm(domin);
        float halfPI = 3.1415927 / 2;
        
        this->handlers_.clear();
        this->heap_ = std::priority_queue<ScoreIdxPair>();
        int numTables = mylsh.getNumTables();
        this->handlers_.reserve(numTables);
        for (unsigned t = 0; t < numTables; ++t) {
            std::vector<float> hashFloats = mylsh.getHashFloats(t, domin);
            for (auto& e : hashFloats) {
                float cosValue = fabs(e) / l2norm;
                if(cosValue > 1) cosValue = 1;
                e = halfPI - acos(cosValue);
            }
            this->handlers_.emplace_back(TSTable(this->hashBits_[t], hashFloats, tree));
            this->heap_.push(ScoreIdxPair(this->handlers_[t].getCurScore(), t)); 
        }
    }
};
