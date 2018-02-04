#pragma once
#include <vector>
#include <utility>
#include <functional>
#include <unordered_map>
#include <queue>
#include <util/heap_element.h>
#include <base/baseprober.h>
#include <base/bucketlist.h>
#include <base/ranking.h>
using std::priority_queue;
using std::vector;
using std::pair;
using std::unordered_map;

template<typename ACCESSOR>
class IntRanking : public Ranking<ACCESSOR, vector<int>> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef vector<int> BIDTYPE;

    template<typename LSHTYPE>
    IntRanking(
        const DATATYPE* query,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : Ranking<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {

        this->LTable_.reserve(mylsh.tables.size());
        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {
            vector<float> hashFloats = mylsh.getHashFloats(tb, query);

            auto distor = [&hashFloats](const BIDTYPE& bucket) {
                float distance = 0;
                float tmp;
                for (int i = 0; i < bucket.size(); ++i) {
                    if (bucket[i] > hashFloats[i]) {
                        tmp = bucket[i] - hashFloats[i];
                        distance += tmp * tmp;
                    } else {
                        tmp = hashFloats[i] - bucket[i];
                        if (tmp > 1) {
                            tmp -= 1;
                            distance += tmp * tmp;
                        }
                    }
                }
                return distance;
            };
            this->LTable_.emplace_back(
                BucketList<BIDTYPE>(mylsh.tables[tb], distor));
        }

        // initialize minHeap
        this->nextBucket_.resize(this->LTable_.size());
        for (int tb = 0; tb < this->LTable_.size(); ++tb) {
            this->tbNextEnheap(tb);
        }
    }
};
