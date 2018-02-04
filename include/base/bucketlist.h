#pragma once
#include <algorithm>
#include <vector>
#include <utility>
using std::vector;
using std::pair;
template<typename BIDTYPE>
class BucketList {
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

    bool hasNext() const {
        return index < sortedBucket_.size();
    }

    const pair<float, BIDTYPE>& next() {
        return sortedBucket_[index++];
    } 

    int getNumBuckets() {
        return sortedBucket_.size();
    }

protected:
    vector<pair<float, BIDTYPE>> sortedBucket_;
    unsigned index = 0;
};

