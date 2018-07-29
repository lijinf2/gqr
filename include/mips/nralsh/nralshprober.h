#pragma once
#include <vector>
#include <utility>
#include <functional>
#include <cmath>
#include <unordered_map>
#include <queue>
#include "gqr/util/heap_element.h"
#include "gqr/util/gqrhash.h"
#include <base/baseprober.h>
#include <base/bucketlist.h>
#include <base/mtableprober.h>
#include <base/onetableprober.h>
#include "mips/nralsh/nralshhasher.h"
#include "mips/nralsh/nritemlist.h"

using std::priority_queue;
using std::vector;
using std::pair;
using std::unordered_map;
using lshbox::gqrhash;
using lshbox::NRALSHHasher;

namespace lshbox{

template<typename ACCESSOR>
class NRALSHProber : public MTableProber<ACCESSOR, vector<int>> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef vector<int> BIDTYPE;

    NRALSHProber(
        const DATATYPE* query,
        lshbox::Scanner<ACCESSOR>& scanner,
        NRALSHHasher<DATATYPE>& mylsh) : MTableProber<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {

        this->LTable_.reserve(mylsh.tables.size());
        const auto& scalers = mylsh.getScalers();

        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {
            BIDTYPE hashInts = mylsh.getBuckets(tb, query);

            auto distor = [&hashInts, &scalers] (const BIDTYPE& bucket) {
                assert(bucket.size() - hashInts.size() == 1);
                unsigned numMatched = 0;
                for (int i = 0; i < hashInts.size(); ++i) {
                    if (hashInts[i] == bucket[i])
                        numMatched++;
                }
                assert(scalers.size() > bucket.back());
                float scaler = scalers[bucket.back()];
                float dist = numeric_limits<float>::max();
                if (numMatched != 0 && scaler != 0)
                    dist = 1 / (scaler * numMatched);
                return dist;
            };

            this->LTable_.emplace_back(
                NRItemList(mylsh.tables[tb], distor));
        }

        // initialize minHeap
        this->nextBucket_.resize(this->LTable_.size());
        for (int tb = 0; tb < this->LTable_.size(); ++tb) {
            this->tbNextEnheap(tb);
        }
    }

    OneTableProber<BIDTYPE>* getTableProber (unsigned tb) override {
        return &LTable_[tb];
    }
private:
    vector<NRItemList> LTable_;
};

}
