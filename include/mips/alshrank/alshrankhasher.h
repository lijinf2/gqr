#pragma once
#include "intcode/hash/alsh.h"
#include <utility>
using std::pair;
namespace lshbox {
template<typename DATATYPE>
class ALSHRankHasher : public ALSH<DATATYPE> {
public:
    template<typename PROBER>
    void KItemByProber(
        const DATATYPE *domin, PROBER &prober, int numItems) {
        while(prober.getNumItemsProbed() < numItems && prober.nextBucketExisted()) {
            // <table, nextItemId>
            const auto& p = prober.getNextBID();
            unsigned itemId = p.second.front();
            prober(itemId);
        }
    }
};
}
