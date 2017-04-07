// hashlookup++, i.e. hashlookuppp, hash lookup with shared PTB
// only work in single-thread environment
#include <vector>
#include <map>
#include "hashlookup.h"
#pragma once
template<typename ACCESSOR>
class HashLookupPP: public HashLookup<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    HashLookupPP(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : HashLookup<ACCESSOR>(domin, scanner, mylsh) {
    }

    bool nextBucketExisted() {
        if (globalR_ == this->getNumBits() && this->idxToPTB_ >= currentPTB_.size() ) {
            return false;
        } else {
            return true;
        }
    }

    void printCurrentPTB() {
        for (auto& vec : currentPTB_) {

            for (int i = 0; i < vec.size(); ++i) {
                std::cout << vec[i];
            }

            std::cout << std::endl;
        }
    }
private:

    static std::vector<std::vector<bool>> lastPTB_;  
    static std::vector<std::vector<bool>> currentPTB_;
    static int globalR_;

    // each query maintains its PTB_
    std::vector<bool> getNextPTB_() override {
        if (currentPTB_.size() == 0) {
            std::vector<bool> ptb(this->getNumBits(), 0);
            currentPTB_.push_back(ptb);
            lastPTB_.push_back(ptb);
        }

        if (this->idxToPTB_ < currentPTB_.size()) {
            return currentPTB_[this->idxToPTB_++];

        } else {
            // should expand currentPTB_, only one query will change currentPTB_;
            // generate possible buckets whose distance to q is globalR_
            assert(globalR_ < this->getNumBits());

            // lastPTB_.swap(currentPTB_);
            // currentPTB_.clear();
            std::vector<std::vector<bool>> newPTB_; 
            for (auto& ptb : lastPTB_) {
                // find largest idx that is 1
                int idx = -1;
                for (int i = ptb.size() - 1; i >=0; --i) { 
                    if (ptb[i] == true) {
                        idx = i;
                        break;
                    }
                }
                for (int i = idx + 1; i < ptb.size(); ++i) {
                    std::vector<bool> newptb = ptb;
                    newptb[i] = true;
                    currentPTB_.push_back(newptb);
                    newPTB_.push_back(newptb);
                }
            }
            lastPTB_.swap(newPTB_);
            globalR_++;
            
            return currentPTB_[this->idxToPTB_++];
        }

    }

};
template<typename ACCESSOR>
std::vector<std::vector<bool>> HashLookupPP<ACCESSOR>::lastPTB_ = std::vector<std::vector<bool>>();
template<typename ACCESSOR>
std::vector<std::vector<bool>> HashLookupPP<ACCESSOR>::currentPTB_ = std::vector<std::vector<bool>>();
template<typename ACCESSOR>
int HashLookupPP<ACCESSOR>::globalR_ = 0;
