#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <queue>
#include <lshbox/query/tree.h>
#include <lshbox/query/prober.h>
#pragma once

template<typename ACCESSOR>
class TreeSearch : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    // typedef unsigned long long BIDTYPE;
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;
    typedef std::pair<float, unsigned > PairT; // <score, tableIdx> 

    template<typename LSHTYPE>
    TreeSearch(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        FV* fvs) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        handlers_.reserve(mylsh.param.L);
        for (unsigned t = 0; t < mylsh.param.L; ++t) {
            std::vector<float> hashFloats = mylsh.getHashFloats(t, domin);
            for (auto& e : hashFloats) {
                e = fabs(e);
            }
            handlers_.emplace_back(LLTable(this->hashBits_[t], hashFloats, &mylsh.tables[t], fvs));
            heap_.push(ScoreIdxPair(handlers_[t].getCurScore(), t)); 
        }
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        const unsigned int& table = heap_.top().index_;
        BIDTYPE newBucket = handlers_[table].getCurBucket();
        heap_.pop();
        bool success = handlers_[table].moveForward();
        if (success){
            heap_.push(ScoreIdxPair(handlers_[table].getCurScore(), table)); 
        }

        // return value
        return std::make_pair(table, newBucket);
    }

private:
    std::vector<LLTable> handlers_;

    std::priority_queue<ScoreIdxPair> heap_; // <score, r> pairs
};
