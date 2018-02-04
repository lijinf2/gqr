#pragma once
#include "bucketlist.h"
#include "base/baseprober.h"
#include <vector>
#include <utility>
#include <queue>
using std::vector;
using std::pair;
using std::priority_queue;

template<typename ACCESSOR, typename BIDTYPE>
class Ranking : public BaseProber<ACCESSOR, BIDTYPE> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;

    template<typename LSHTYPE>
    Ranking(
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

protected:
    vector<BucketList<BIDTYPE>> LTable_;

    priority_queue<DistDataMin<unsigned>> minHeap_;
    vector<BIDTYPE> nextBucket_;

    void tbNextEnheap(unsigned tb) {
        if (LTable_[tb].hasNext()) {
            const pair<float, BIDTYPE>& p = LTable_[tb].next();
            nextBucket_[tb] = p.second;
            minHeap_.emplace(DistDataMin<unsigned>(p.first, tb));
        }
    }
};

