#include <lshbox/query/prober.h>
#include "gqr/util/heap_element.h"
#include <queue>
#include <utility>
using std::pair;
using std::priority_queue;
template<typename ACCESSOR>
class KGraphSearch: public Prober<ACCESSOR>{
private:
    typedef DistDataMin<unsigned> ElementT;
    priority_queue<ElementT> minHeap_;

public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    KGraphSearch(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        unsigned root = mylsh.bitsToBucket(mylsh.getHashBits(0, domin)); 
        float dist = this->getScanner().calDist(root);
        minHeap_.push(ElementT(dist, root));
    }

    pair<unsigned, BIDTYPE> getNextBID() {
        BIDTYPE bid = minHeap_.top().data();
        minHeap_.pop();
        return std::make_pair(0, bid);
    }; 

    bool nextBucketExisted() {
        return !minHeap_.empty();
    }

    void operator()(unsigned key) {
        // buckets should rank by nearest neighbors
        // update buckets
        auto p = this->evaluate(key);
        // if unvisited, enheap
        if (p.first) {
            minHeap_.push(ElementT(p.second, key));
        }
    }
};
