#pragma once
#include <cmath>
#include "lshbox/utils.h"
#include "base/baseprober.h"
template<typename ACCESSOR>
class Prober : public BaseProber<ACCESSOR, unsigned long long>{
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
    template<typename LSHTYPE>
    Prober(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : BaseProber<ACCESSOR, BIDTYPE>(domin, scanner, mylsh) {

        hashBits_.resize(mylsh.tables.size());
        for (unsigned tb = 0; tb < hashBits_.size(); ++tb) {
            hashBits_[tb] = mylsh.getHashBits(tb, domin);
        }
    }

    float calL2Norm(const DATATYPE* domin) {
        float sum = 0;
        for (int i = 0; i < hashBits_[0].size(); ++i) {
            sum += domin[i] * domin[i];
        }
        return sqrt(sum);
    }

protected:
    std::vector<std::vector<bool>> hashBits_; // L hash tables
};
