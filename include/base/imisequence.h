#pragma once
#include <vector>
#include <algorithm>
#include <utility>
#include <queue>
#include <functional>
#include "util/heap_element.h"
using std::vector;
using std::pair;
using std::function;
using std::priority_queue;

// stands for inverted multi index
class IMISequence{
public:
    // first stores row coordinate, second stores col coordinate
    typedef pair<unsigned, unsigned> Coord;

    IMISequence(
        unsigned rowLen,
        unsigned colLen,
        const function<float (unsigned, unsigned)>& func)
            : rowLen_(rowLen), colLen_(colLen) {

        distor_ = func;

        enHeap(0, 0);
        colIdxVisited_ = vector<int>(rowLen, -1);
        colIdxVisited_[0] = 0; // first row, only the first element visited
    }

    bool hasNext() const {
        return !minHeap_.empty();
    }

    pair<float, Coord> next() {
        Coord p = minHeap_.top().data();
        float dist = minHeap_.top().dist();
        minHeap_.pop();

        // check the right element
        int newRow = p.first;
        int newCol = p.second + 1;
        if (newCol < colLen_) { // right element exists
            if(newRow - 1 < 0 || colIdxVisited_[newRow - 1] >= newCol) {
                enHeap(newRow, newCol);
                colIdxVisited_[newRow]++;
            }
        }

        // check the down element
        newRow = p.first + 1;
        newCol = p.second;
        if (newRow < rowLen_) { // down element exists
            if(colIdxVisited_[newRow] == newCol - 1) {
                enHeap(newRow, newCol);
                colIdxVisited_[newRow]++;
            }
        }
        return std::make_pair(dist, p);
    } 
private:
    // dist stores float value, data stores the index in the given leftVec or rightVec;

    unsigned rowLen_;
    unsigned colLen_;
    vector<int> colIdxVisited_;
    function<float (unsigned, unsigned)> distor_;
    priority_queue<DistDataMin<Coord>> minHeap_;

    void enHeap(unsigned row, unsigned col) {
        float dist = distor_(row, col);
        minHeap_.emplace(DistDataMin<Coord>(dist, Coord(row, col)));
    }
};

// class InvertedMultiIndex {
// public:
//     struct Coord {
//         Coord(unsigned x, unsigned y): x_(x), y_(y) {}
//         unsigned x_;
//         unsigned y_;
//     }
//
//     InvertedMultiIndex(
//         const vector<float>& leftVec,
//         const vector<float>& rightVec,
//         const function<float (float, float)>& func) {
//
//         left_.reserve(leftVec.size());
//         for (unsigned i = 0; i < leftVec.size() ++i) {
//             left_.emplace_back(DistDataSmaller(leftVec[i],i));
//         }
//         std::sort(left_.begin(), left_.end());
//
//         right_.reserve(rightVec.size());
//         for (unsigned i = 0; i < rightVec.size(); ++i) {
//             right_.emplace_back(DistDataSamller(rightVec[i], i));
//         }
//         std::sort(right_.begin(), right_.end());
//
//         distor_ = func;
//     }
//
//     bool hasNext() const {
//         return !minHeap_.empty()
//     }
//
//     pair<unsigned, unsigned> next() {
//     } 
// private:
//     // dist stores float value, data stores the index in the given leftVec or rightVec;
//     vector<DistDataSmaller<unsigned>> left_;
//     vector<DistDataSmaller<unsigned>> right_;
//
//     function<float (float, float)> distor_;
//     priority_queue<DistDataMin<Coord>> minHeap_;
//
//     void enHeap(unsigned x, unsigned y) {
//         float dist = distor_(left_[x].dist(), right_[y].dist());
//         minHeap.emplace(DistDataMin(dist, Coord(x, y)));
//     }
// };
