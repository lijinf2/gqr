#pragma once
#include <vector>
#include <queue>
#include <set>
#include <utility>
#include <lshbox/query/prober.h>
#include "hooker.h"
#include "heap_element.h"
using std::vector;
using std::priority_queue;
using std::pair;
using std::set;

class HookHeap {
public:
    typedef unsigned long long BIDTYPE;
    typedef DistDataMin<pair<unsigned, BIDTYPE>> ElementT;

    HookHeap() {
    }

    void push(float distance, const vector<pair<unsigned, BIDTYPE>>& bucketList) {
        for (auto& tableBucket : bucketList) {
            if (pushed_.find(tableBucket) == pushed_.end()) {
                minHeap_.push(ElementT(distance, tableBucket));
                pushed_.insert(tableBucket);
            }
        }
    }

    pair<unsigned, BIDTYPE> pop() {
        auto tableBucket = minHeap_.top().data();
        minHeap_.pop();
        return tableBucket;
    }

    bool empty() {
        return minHeap_.empty();
    }
private:
    priority_queue<ElementT> minHeap_;
    set<pair<unsigned, BIDTYPE>> pushed_;
};

template<typename ACCESSOR>
class HookSearch: public Prober<ACCESSOR>{
private:
    Hooker* hookerP_;
    HookHeap hookMinHeap_;

public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    HookSearch(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh,
        Hooker* hooker) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        hookerP_ = hooker;

        vector<pair<unsigned, BIDTYPE>> buckets;
        buckets.resize(this->hashBits_.size());
        for (int i = 0; i < this->hashBits_.size(); ++i) {
            buckets[i] = std::make_pair(i, mylsh.bitsToBucket(this->hashBits_[i]));
        }
        hookMinHeap_.push(0, buckets);
    };

    pair<unsigned, BIDTYPE> getNextBID() {
        return hookMinHeap_.pop();
    }; 

    bool nextBucketExisted() {
        return !hookMinHeap_.empty();
    }

    void operator()(unsigned key) {
        // buckets should rank by nearest neighbors
        // update buckets
        auto p = this->evaluate(key);
        // if not visited
        if (p.first) {
            hookMinHeap_.push(p.second, hookerP_->getBucketList(key));
        }
    }
};

