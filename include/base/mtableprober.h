#pragma once
#include <vector>
#include <utility>
#include <queue>

#include "base/baseprober.h"
#include "base/onetableprober.h"
using std::vector;
using std::pair;
using std::priority_queue;

template<typename ACCESSOR, typename BIDTYPE>
class MTableProber: public BaseProber<ACCESSOR, BIDTYPE> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;

    template<typename LSHTYPE>
    MTableProber(
        const DATATYPE* query,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : BaseProber<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {}

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        unsigned tb = minHeap_.top().data();
        BIDTYPE nextBk = nextBucket_[tb];
        tbNextEnheap(tb);
        minHeap_.pop();

        return std::make_pair(tb, nextBk);
    }

    virtual OneTableProber<BIDTYPE>* getTableProber(unsigned tbIdx) = 0;
protected:
    // vector<OneTableProber<BIDTYPE>> LTable_;

    priority_queue<DistDataMin<unsigned>> minHeap_;
    vector<BIDTYPE> nextBucket_;

    void tbNextEnheap(unsigned tb) {
        if (getTableProber(tb)->hasNext()) {
            const pair<float, BIDTYPE>& p = getTableProber(tb)->next();
            nextBucket_[tb] = p.second;
            minHeap_.emplace(DistDataMin<unsigned>(p.first, tb));
        }
    }
};
