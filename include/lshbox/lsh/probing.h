#pragma once
#include <algorithm>
#include <queue>
#include <utility>
#include <vector>
#include "PTBSet.hpp"
/*
 * Multi-probe LSH*/
class Probing {
public:
    Probing(
        const std::vector<bool>& bits, 
        const std::vector<float>& floats,
        bool withMeanAndSTD = false){

        hashBits_ = bits;
        hashFloats_ = floats;
        assert(hashBits_.size() == hashFloats_.size());
        dstWithPos_.resize(hashBits_.size());
        float dst = -1;
        float dst1 = -1;
        float dst2 = -1;
        for (int i = 0; i < dstWithPos_.size(); ++i) {

            if (!withMeanAndSTD) {
                // without mean and std
                if (hashFloats_[i] >= 0) {
                    dst = hashFloats_[i];
                } else {
                    dst = -hashFloats_[i];
                }

                // using euclidean distance will be worse
                    // dst = hashFloats_[i] * hashFloats_[i];
                    // dst = sqrt(dst);
            } else {
                // with mean and std
                //
                // dst = (meanAndSTD[1][i] - hashFloats[i]) / meanAndSTD[3][i];
                // dst += (hashFloats[i] - meanAndSTD[0][i] ) / meanAndSTD[2][i];
                // if (hashFloats[i] >= 0) {
                //     dst1 = (hashFloats[i] - meanAndSTD[1][i]) / meanAndSTD[3][i];
                //     dst2 = (hashFloats[i] - meanAndSTD[0][i]) / meanAndSTD[2][i];
                //     dst = fabs(dst1) - fabs(dst2);   
                // } else {
                //     dst1 = (hashFloats[i] - meanAndSTD[1][i]) / meanAndSTD[3][i];
                //     dst2 = (hashFloats[i] - meanAndSTD[0][i]) / meanAndSTD[2][i];
                //     dst = fabs(dst2) - fabs(dst1);   
                // }
            }
            
            dstWithPos_[i] = std::pair<float, int>(dst, i);
        }
        std::sort(dstWithPos_.begin(), dstWithPos_.end(),
            [] (const std::pair<float, int>& a, const std::pair<float, int>& b) {
                return a.first < b.first;
            });
        minHeap_.push( PTBSet(std::vector<int>({0}), dstWithPos_[0].first));
    }

    unsigned pop(){
        assert(minHeap_.size() != 0);
        PTBSet ptb = minHeap_.top();
        minHeap_.pop();

        // generate 
        unsigned probedBId = ptb.getNewBucket(hashBits_, dstWithPos_);

        ptb.shift(minHeap_, dstWithPos_); 
        ptb.expand(minHeap_, dstWithPos_); 
        return probedBId;
    }
    void report() {
        // print out hash Bits
        std::cout << std::endl;
        std::cout << "hashBits: ";
        for (int i = 0; i < hashBits_.size(); ++i) {
            std::cout << hashBits_[i] << ", ";
        }
        // print out hash Floats
        std::cout << std::endl;
        std::cout << "hashFloats: ";
        for (int i = 0; i < hashFloats_.size(); ++i) {
            std::cout << hashFloats_[i] << ", ";
        }
        // print out hash dstWithPos 
        std::cout << std::endl;
        std::cout << "dstWithPos: ";
        for (int i = 0; i < dstWithPos_.size(); ++i) {
            std::cout << "<" << dstWithPos_[i].first << " "
                << dstWithPos_[i].second << ", ";
        }
        std::cout << std::endl;
    }
private:
    std::vector<bool> hashBits_;
    std::vector<float> hashFloats_;
    std::vector<std::pair<float, int> > dstWithPos_;
    std::priority_queue<PTBSet> minHeap_;
    bool withMeanAndSTD_ = false;
};
