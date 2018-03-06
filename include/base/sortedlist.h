#pragma once
#include <algorithm>
#include <vector>
#include <utility>
#include <string>
#include <functional>

using std::vector;
using std::pair;
using std::string;

typedef std::pair<unsigned, unsigned > Coord;



class SortedNormRange {

public:

    SortedNormRange(unsigned numHashBits, const std::vector<float >& normIntervals) {

        sortedBucket_.reserve((numHashBits+1) * (normIntervals.size()-1));

        auto distor = [&](Coord& numBitIntervalPair){
            return 1.0 /  (numHashBits-numBitIntervalPair.first) / normIntervals[numBitIntervalPair.second+1];
        };

        for (unsigned diffBitNum = 0; diffBitNum < numHashBits + 1; ++diffBitNum) {
            for (unsigned intervalIndex = 0; intervalIndex < normIntervals.size()-1; ++intervalIndex) {

                Coord data = std::make_pair(diffBitNum, intervalIndex);
                sortedBucket_.emplace_back(std::make_pair(distor(data), data));
            }
        }


        std::sort(
                sortedBucket_.begin(),
                sortedBucket_.end(),
                [](const pair<float, Coord>& a, const pair<float, Coord>& b) {
                    return a.first < b.first;
                });
    }
    const pair<float, Coord>& get(unsigned index) {
        return sortedBucket_[index];
    }

    const bool hasNext(unsigned index) {
        return sortedBucket_.size()>index;
    }

    unsigned long int size() {
        return sortedBucket_.size();
    }

protected:
    vector<pair<float, Coord>> sortedBucket_;

};


class SortedNormRangeSequence {
public:
    SortedNormRangeSequence(SortedNormRange* sorted_list): sorted_list_(sorted_list) {}

    bool hasNext() const {
        return sorted_list_->hasNext(index);
    }

    pair<float, Coord > next() {
        return sorted_list_->get(index++);
    };

private:
    SortedNormRange* sorted_list_;
    unsigned index = 0;
};