#pragma once
#include <vector>
#include <utility>
#include <functional>
#include <unordered_map>
#include <queue>
#include <util/heap_element.h>
#include <base/baseprober.h>
using std::priority_queue;
using std::vector;
using std::pair;
using std::unordered_map;

template<typename BIDTYPE>
class BucketList {
public:
    BucketList(
        const unordered_map<BIDTYPE, std::vector<unsigned> >& table,
        std::function<float (const BIDTYPE&)> distor){
        
        // ranking by linear sorting
        for ( typename unordered_map<BIDTYPE, std::vector<unsigned> >::const_iterator it = table.begin(); it != table.end(); ++it) {
            const BIDTYPE& signature = it->first;
            float dist = distor(signature);
            sortedBucket_.push_back(make_pair(distor(signature), signature));
        }

        std::sort(
            sortedBucket_.begin(), 
            sortedBucket_.end(), 
            [](const pair<float, BIDTYPE>& a, const pair<float, BIDTYPE>& b) {
                return a.first < b.first;
            });
    }

    bool hasNext() const {
        return index < sortedBucket_.size();
    }

    const pair<float, BIDTYPE>& next() {
        return sortedBucket_[index++];
    } 

    int getNumBuckets(int hamDist) {
        return sortedBucket_.size();
    }

private:
    vector<pair<float, BIDTYPE>> sortedBucket_;
    unsigned index = 0;
};

template<typename ACCESSOR>
class IntRanking : public BaseProber<ACCESSOR, vector<int>> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef vector<int> BIDTYPE;

    template<typename LSHTYPE>
    IntRanking(
        const DATATYPE* query,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : BaseProber<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {

        LTable_.reserve(mylsh.tables.size());
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
            LTable_.emplace_back(
                BucketList<BIDTYPE>(mylsh.tables[tb], distor));
        }

        // initialize minHeap
        nextBucket_.resize(LTable_.size());
        for (int tb = 0; tb < LTable_.size(); ++tb) {
            tbNextEnheap(tb);
        }
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        unsigned tb = minHeap_.top().data();
        BIDTYPE nextBk = nextBucket_[tb];
        tbNextEnheap(tb);
        minHeap_.pop();

        return std::make_pair(tb, nextBk);
    }

private:
    vector<BucketList<BIDTYPE>> LTable_;

    priority_queue<DistDataMin<unsigned>> minHeap_;
    vector<BIDTYPE> nextBucket_;

    void tbNextEnheap(unsigned tb) {
        if (LTable_[tb].hasNext()) {
            const pair<float, BIDTYPE>& p = LTable_[tb].next();
            nextBucket_[tb] = p.second;
            minHeap_.push(DistDataMin<unsigned>(p.first, tb));
        }
    }
};
