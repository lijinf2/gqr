#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <queue>
#include <lshbox/query/tree.h>
#include <lshbox/query/prober.h>
#include <lshbox/query/tstable.h>
#pragma once

template<typename ACCESSOR>
class TreeLookup : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    // typedef unsigned long long BIDTYPE;
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;
    typedef std::pair<float, unsigned > PairT; // <score, tableIdx> 

    template<typename LSHTYPE>
    TreeLookup(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        Tree* tree) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        handlers_.reserve(mylsh.param.L);
        for (unsigned t = 0; t < mylsh.param.L; ++t) {
            std::vector<float> hashFloats = mylsh.getHashFloats(t, domin);
            for (auto& e : hashFloats) {
                e = fabs(e);
            }
            handlers_.emplace_back(TSTable(this->hashBits_[t], hashFloats, &mylsh.tables[t], tree));
            heap_.push(ScoreIdxPair(handlers_[t].getCurScore(), t)); 
        }

        // initialize firstBKs_
        firstBK_.reserve(mylsh.param.L);
        for (unsigned t = 0; t < mylsh.param.L; ++t) {
            firstBK_.push_back(mylsh.getHashVal(t, domin));
        }

    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        if (this->numBucketsProbed_++ < handlers_.size()) {
            const unsigned int tb = this->numBucketsProbed_  - 1;
            return std::make_pair(
                    tb,
                    firstBK_[tb]);
        }
        // always return the first bucket of every table

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
    std::vector<TSTable> handlers_;
    std::vector<BIDTYPE> firstBK_;

    std::priority_queue<ScoreIdxPair> heap_; // <score, r> pairs
};