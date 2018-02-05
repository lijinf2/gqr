#pragma once
#include <algorithm>
#include <vector>
#include <utility>
#include <string>

#include "base/onetableprober.h"
using std::vector;
using std::pair;
using std::string;
template<typename BIDTYPE>
class BucketList : public OneTableProber<BIDTYPE> {
public:
    BucketList(
        const unordered_map<BIDTYPE, std::vector<unsigned> >& table,
        const std::function<float (const BIDTYPE&)>& distor){
        
        sortedBucket_.reserve(table.size());
        // ranking by linear sorting
        for (typename unordered_map<BIDTYPE, std::vector<unsigned> >::const_iterator it = table.begin(); it != table.end(); ++it) {
            const BIDTYPE& signature = it->first;
            float dist = distor(signature);
            sortedBucket_.emplace_back(make_pair(distor(signature), signature));
        }

        std::sort(
            sortedBucket_.begin(), 
            sortedBucket_.end(), 
            [](const pair<float, BIDTYPE>& a, const pair<float, BIDTYPE>& b) {
                return a.first < b.first;
            });
    }

    bool hasNext() override {
        return index < sortedBucket_.size();
    }

    const pair<float, BIDTYPE>& next() override {
        return sortedBucket_[index++];
    } 

    int getNumBuckets() {
        return sortedBucket_.size();
    }

    string toString() {
        string str = "dist, bucket\n";
        
        for (int i = 0; i < sortedBucket_.size(); ++i) {
            str = str 
                + std::to_string(sortedBucket_[i].first)
                + ", " + std::to_string(sortedBucket_[i].second) + "\n";
        }
        return str;
    }
protected:
    vector<pair<float, BIDTYPE>> sortedBucket_;
    unsigned index = 0;
};

