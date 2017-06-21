#include <vector>
#include <string>
#include <cassert>
#include <queue>
#include <cmath>
#pragma once
// good cache locality, can be cached, computed offline and shared by all queries
// flipping vector tree
class Tree {
public:
    // R: total number of bits
    //
    Tree(unsigned R) {
        assert(R <= 27);  // maximum of unsigned : 2^27 * 2^5
        R_ = R;

        // cal number of flipping vectors
        unsigned numFV = 1;
        for (int i = 1; i <= R_; ++i) {
            numFV <<= 1; 
        }
        numFV -= 1;

        lastOne_.reserve(numFV);
        bits_ = new bool[numFV * R_];  // largest is 2^27 * 27 < 2^27 * 32 = 2^32 - 1


        // build bits_ as well as lastone_
        lastOne_.push_back(0);
        bits_[0] = (1);
        for (unsigned i = 1; i < R_; ++i) {
            bits_[i] = 0;
        }
        count_++;


        for (unsigned idx = 0; idx < numFV / 2; ++idx) {
            const bool* p = getFV(idx);
            const int& one = lastOne_[idx];

            std::vector<bool> fv;
            // copy from fv and shift
            fv = copy(p);
            fv[one] = 0;
            fv[one + 1] = 1;
            insertToBits_(fv, count_ * R_);
            lastOne_.push_back(one + 1);
            count_++;

            // copy from fv and expand 
            fv = copy(p);
            fv[one + 1] = 1;
            insertToBits_(fv, count_ * R_);
            lastOne_.push_back(one + 1);
            count_++;
        }


        assert(lastOne_.size() == numFV);
        assert(count_ == numFV);
    }

    const bool* getFV(unsigned idx) {
        return &(bits_[idx * R_]);
    }

    std::vector<bool> copy(const bool* p) {
        std::vector<bool> result(R_);
        for (int i = 0; i < R_; ++i) {
            result[i] = p[i];
        }
        return result;
    }

    std::string toString() {
        std::string log = "";
        for (unsigned idx = 0; idx < lastOne_.size(); ++idx) {
            auto p = getFV(idx);
            for (unsigned i = 0; i < R_; ++i) {
                log += std::to_string(p[i]);
            }
            log += " ";
            log += std::to_string(lastOne_[idx]);
            log += "\n";
        }
        return log;
    }

    ~Tree(){
        delete bits_;
    }
private:
    unsigned R_ = 0; // step is R_

    // lastOne_ and bits_ should be updated as a whole
    bool* bits_ = NULL; 
    std::vector<unsigned> lastOne_;
    unsigned count_ = 0;

    void insertToBits_(const std::vector<bool>& fv, unsigned start) {
        assert (fv.size() == R_);
        for (int i = 0; i < fv.size(); ++i) {
            bits_[start + i] = fv[i];
        }
    }
};
