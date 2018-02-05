#pragma once
#include <util/heap_element.h>
#include <queue>
#include <vector>
#include <functional>

#include <map>
#include <unordered_map>
#include <cmath>
#include <iostream>


using namespace std;


class InvertedMultiIndex {
public:
    InvertedMultiIndex(unsigned t, unsigned x, unsigned y): tableIndex(t), x_(x), y_(y) {
    }


    unsigned tableIndex;
    unsigned x_;           // codeword U
    unsigned y_;           // codeword V

};


/**
 * Querying the multi-index. Given a query q = [q1 q2]  and a desired candidate list length T , an inverted
 * multi-index allows to generate a list of T (or slightly more)
 * points from D that tend to be close to q with respect to
 * the distance d.
 *
 *
 * First, the two halves of the query q1 and q2 are matched w.r.t. sub-codebooks X and Y to produce the
 * two sequences of codewords ordered by the distance (denoted r and s) from the respective query half.
 * Then, those sequences are traversed with the multi-sequence algorithm that outputs the pairs of codewords
 * ordered by the distance from the query. The lists associated with those pairs are concatenated to
 * produce the answer to the query
 */
class IMIProber {

private:
    typedef DistDataMin<InvertedMultiIndex> PairT;

    priority_queue<PairT> heap_ ;

    vector<vector<bool> > visited_;

    function<float(InvertedMultiIndex& ) > distance_;

    unsigned x_len;
    unsigned y_len;

public:
    IMIProber(unsigned table_size,
              unsigned x,
              unsigned y,
              function<float(InvertedMultiIndex& ) > distance) : distance_(std::move(distance)), x_len(x), y_len(y) {

        for (unsigned i = 0; i != table_size; ++i) {
            InvertedMultiIndex nearestBucket(i, 0, 0) ;
            float dist = distance_(nearestBucket);
            heap_.push(PairT(dist, nearestBucket));
        }
        visited_.resize(x_len);
        for (int i = 0; i < x_len; ++i) {
            visited_[i].resize(y_len, false);
        }
    }

    virtual unsigned getXlen() {
        return x_len;
    }

    virtual unsigned getYlen() {
        return y_len;
    }

    virtual bool hashNext() {
        return heap_.size()>0;
    }

    virtual unsigned long size() {
        return heap_.size();
    }

    virtual const InvertedMultiIndex getNext() {

        const InvertedMultiIndex imi = heap_.top().data();
        unsigned tb = imi.tableIndex;

        heap_.pop();

        visited_[imi.x_][imi.y_] = true;

        if (imi.x_+1 < x_len && !visited_[imi.x_+1][imi.y_]) {
            if (imi.y_==0 || visited_[imi.x_+1][imi.y_-1]) {
                InvertedMultiIndex nearestBucket(tb, imi.x_+1, imi.y_) ;
                float dist = distance_(nearestBucket);
                heap_.push(PairT(dist, nearestBucket));
            }

        }

        if (imi.y_+1 < y_len && !visited_[imi.x_][imi.y_+1]) {
            if (imi.x_==0 ||visited_[imi.x_-1][imi.y_+1]) {
                InvertedMultiIndex nearestBucket(tb, imi.x_, imi.y_+1) ;
                float dist = distance_(nearestBucket);
                heap_.push(PairT(dist, nearestBucket));
            }

        }

        return imi;
    }

};