#pragma once
struct ScoreIdxPair{
    float score_;
    unsigned index_;
    ScoreIdxPair(float s, unsigned i) {
        score_  = s;
        index_ = i;
    }
    // smaller better for heap
    bool operator<(const ScoreIdxPair& other) const  {
        return score_ > other.score_;
    }
};

