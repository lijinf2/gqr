//perturbation set in Multi-Probe LSH
#ifndef _PTBSet_H_
#define _PTBSet_H_

#pragma once

//two valid conditions: 
//  1. each value is smaller than N (number of bits)

#include <queue>
#include <vector>
//for perturbation set
class PTBSet{
public:
    std::vector<int> pSet;
    float score;

    PTBSet(const std::vector<int>& vec, float sco){
        this->pSet = vec;
        this->score = sco;
    }

    unsigned getNewBucket(
        const std::vector<bool>& bits,
        const std::vector<std::pair<float, int>>& dstWithPos){

        // map perturbation set to exact position
        std::vector<int> pPos;
        for (int i = 0; i < pSet.size(); ++i) {
            pPos.push_back(dstWithPos[pSet[i]].second);
        }
        std::sort(pPos.begin(), pPos.end());

        // calculate new bucketId
        unsigned bucketId = 0;

        int pPosIdx = 0;
        for (int i = 0; i < bits.size(); ++i) {
            bucketId <<= 1;
            if (pPosIdx < pPos.size() && i == pPos[pPosIdx]) {
                pPosIdx++;
                bucketId += 1 - bits[i];
            } else {
                bucketId += bits[i];
            }
        }
        return bucketId;
    }

    void shift( 
        std::priority_queue<PTBSet, std::vector<PTBSet>>& minHeap, 
        const std::vector<std::pair<float, int>>& dstWithPos){

        // if last number is the maximum pos, cannot shift
        if (this->pSet.back() >= dstWithPos.size() - 1) return;

        std::vector<int> newPSet = this->pSet;
        float score = this->score;
        int lastValue = newPSet.back();
        score -= dstWithPos[lastValue].first;
        score += dstWithPos[lastValue+1].first;
        newPSet.back()++;
        minHeap.push(PTBSet(newPSet, score));
    }

    void expand(
        std::priority_queue<PTBSet, std::vector<PTBSet>>& minHeap, 
        const std::vector<std::pair<float, int>>& dstWithPos){

        // if last number is the maximum pos, cannot shift
        if (this->pSet.back() >= dstWithPos.size() - 1) return;

        std::vector<int> newPSet = this->pSet;
        newPSet.push_back(newPSet.back() + 1);
        float score = this->score;
        score += dstWithPos[newPSet.back()].first;
        minHeap.push(PTBSet(newPSet, score));
    }

    // std::string toString() const {
    //     std::string str = "(";
    //     str += "score: " + std::to_string(this->score) + "\t";
    //     str += "table: " + std::to_string(this->table) + "\t";
    //     str += "pertbSet: " +std::to_string(this->pSet);
    //     str += ")";
    //     return str;
    // }
    bool operator<(const PTBSet &b) const {
        return score > b.score;
    }

};

#endif
