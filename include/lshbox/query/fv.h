#include <vector>
#include <string>
#include <cassert>
#pragma once
// flipping vector
class FV {
    public:
        // R: total number of bits
        FV(int R) {
            R_ = R;
            FVS_.resize(R_ + 1);
            numFVS_.resize(R_ + 1);

            numFVS_[0] = 1;
            FVS_[0] = new bool[R_];
            for (int i = 0; i < R_; ++i) {
                FVS_[0][i] = false;
            }


            // reserve space for FVS_, C(R, i) = C(R, i - 1) * (R - i + 1) / i
            unsigned long long combinations = 1;  // for C(R, 0)
            unsigned int parameterN = R_;
            for (int i = 1; i <= R_; ++i) {

                // obtain C(R, i)
                combinations *= (R + 1 - i);
                combinations /= i;

                // the i-th level contains C(R, i) fvs, each fv contains R bits
                numFVS_[i] = combinations;
                bool* cur = new bool[combinations * R_];
                FVS_[i] = (cur);
            }
            // initialize FVS_

            bool* curfv = new bool[R_];
            for (int layer = 1; layer < FVS_.size(); ++layer) {
                int lastLayer = layer - 1;
                int currentCount = 0;

                // for every fv in the FVS_[lastLayer]
                for (int lastCount = 0; lastCount < numFVS_[lastLayer]; lastCount += 1) {
                    int start = lastCount * R_;

                    // get current fv to tmp
                    for (int idx = 0; idx < R_; ++idx) {
                        curfv[idx] = FVS_[lastLayer][start + idx];
                    }

                    // find largest idx of tmp that is 1
                    int minIdx = -1;
                    for (int i = R_ - 1; i >= 0; --i) {
                        if (curfv[i] == true) {
                            minIdx = i;
                            break;
                        }
                    }

                    // set new fv in current by curfv: i.e. 1. copy: copy curfv, 2. set bit to true
                    for (int i = minIdx + 1; i < R_; ++i) {


                        // copy tmpfv to new pos
                        for (int idxToCurFV = 0; idxToCurFV < R_; ++idxToCurFV) {
                            FVS_[layer][currentCount * R_ + idxToCurFV] = curfv[idxToCurFV];
                        }

                        // set bit to true
                        FVS_[layer][currentCount* R_ + i] = true;
                        currentCount++;
                    }
                }
            }
            delete[] curfv;
        }

        // check whether the idx-th flipping set of FVS_[hamDist] exists or not
        bool existed(unsigned int hamDist, unsigned int idx) const {
            if (hamDist >= FVS_.size()) return false;
            if (idx >= numFVS_[hamDist]) return false;
            return true;
        }

        // FVS_[hammingdist], return the idx-th flipping vector
        const bool* getFlippingVector(unsigned int hamDist, unsigned int idx) const {
            assert(existed(hamDist, idx));
            return FVS_[hamDist] + idx * R_;
        }

        std::string fvtoString(const bool* p) const {
            std::string str = "";
            for (int i = 0; i < R_; ++i) {
                str += std::to_string(p[i]);
            }
            return str;
        }

        std::string toString() const {
            std::string str = "";
            for (int layer = 0; layer < numFVS_.size(); ++layer) {
                for (int idx = 0; idx < numFVS_[layer]; ++idx) {
                    if (existed(layer, idx)) {
                        str += fvtoString(getFlippingVector(layer, idx));
                        str += " in layer " + std::to_string(layer);
                        str += " idx " + std::to_string(idx);
                        str += "\n";
                    }
                }
            }
            return str;
        }

        ~FV() {
            for (int i = 0; i < FVS_.size(); ++i) {
                delete[] (FVS_[i]);
            }
        }
    private:
        // FVS_[i] remembers the sequenes of flipping vectors with R bits is 1;
        // i.e. distance to query code is R
        // if R is 2, will generate <00>, <1001>, <11>
        int R_; // # of bits per flipping vector
        std::vector<int> numFVS_; // # of flipping vector of hamming distance r
        std::vector<bool*> FVS_; // hamming distance r from 0 to R_
};
