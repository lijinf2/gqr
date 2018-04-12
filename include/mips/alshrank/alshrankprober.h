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
using std::priority_queue;
using std::vector;
using std::pair;
using std::unordered_map;
using lshbox::gqrhash;

class ALSHBucketList : public OneTableProber<vector<int>>{
public:
    typedef vector<int> BIDTYPE;
    ALSHBucketList(
        const BIDTYPE& queryHashInts, 
        const unordered_map<BIDTYPE, std::vector<unsigned>, gqrhash<BIDTYPE>>& table) {
        
        unsigned maxDistance = queryHashInts.size();
        dists.resize(maxDistance + 1);
        for (typename unordered_map<BIDTYPE, std::vector<unsigned>, typename lshbox::gqrhash<BIDTYPE>>::const_iterator it = table.begin(); it != table.end(); ++it) {
            const BIDTYPE& signature = it->first;
            unsigned numMatches = 0;
            assert(signature.size() == queryHashInts.size());
            for (int i = 0; i < signature.size(); ++i) {
                if (signature[i] == queryHashInts[i]) {
                    numMatches++;
                }
            }

            const auto& items = it->second;
            numAllItem += items.size();
            for (const auto& itemId : items) {
                dists[maxDistance - numMatches].push_back(itemId);
            }
        }
    }

    bool hasNext() override {
        return numVisitedItem < numAllItem;
    }

    const pair<float, vector<int>>& next() override {
        while (col >= dists[row].size()) {
            row++;
            col = 0;
        }
        float dist = row;
        current.first = dist;
        current.second = vector<int> (1, dists[row][col++]);
        numVisitedItem++;
        return current; 
    } 

protected:
    vector<vector<unsigned>> dists;
    unsigned row = 0;
    unsigned col = 0;
    std::pair<float, vector<int>> current;

    unsigned numAllItem = 0;
    unsigned numVisitedItem = 0;
};

template<typename ACCESSOR>
class ALSHRankProber : public MTableProber<ACCESSOR, vector<int>> {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef vector<int> BIDTYPE;

    template<typename LSHTYPE>
    ALSHRankProber(
        const DATATYPE* query,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : MTableProber<ACCESSOR, BIDTYPE>(query, scanner, mylsh) {

        this->LTable_.reserve(mylsh.tables.size());
        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {
            vector<float> hashFloats = mylsh.getHashFloats(tb, query);
            vector<int> hashInts (hashFloats.size());
            for (int idx = 0; idx < hashFloats.size(); ++idx) {
                hashInts[idx] = floor(hashFloats[idx]);
            }

            this->LTable_.emplace_back(
                ALSHBucketList(hashInts, mylsh.tables[tb]));
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
    vector<ALSHBucketList> LTable_;
};
