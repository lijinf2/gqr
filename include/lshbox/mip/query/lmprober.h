#pragma once
#include <map>
#include <queue>
#include <vector>
#include "lshbox/query/scoreidxpair.h"
#include <vector>
#include <unordered_map>
#include <lshbox/query/prober.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <bitset>
#include <lshbox.h>

#include <util/heap_element.h>
#include <lshbox/query/fv.h>

#include <mips/normrange/normrangehasher.h>
#include "lshbox/mip/query/inverted_multi_index.h"

template <typename ACCESSOR>
class LengthMarkedLookup: public Prober<ACCESSOR> {


public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;
    typedef lshbox::NormRangeHasher<DATATYPE> LSHTYPE;


    LengthMarkedLookup(
            const DATATYPE* domin,
            lshbox::Scanner<ACCESSOR>& scanner,
            LSHTYPE& mylsh,
            FV* fvs)
            :
            Prober<ACCESSOR>(domin, scanner, mylsh),
            fvs_(fvs),
            imiProber(
                    (unsigned)mylsh.tables.size(),               // table size
                    (unsigned)mylsh.getNormIntervals().size()-1, // x equals intervals count, first element is the minimum.
                    mylsh.getHashBitsLen()+1,          // y equals number of bit different. 0 1 2 ... bitsLen

                    [&](InvertedMultiIndex& nearestBucket) { // how to determine distance

                        float CONSTANT = 32.0f;
                        unsigned L = mylsh.getHashBitsLen();

                        // belongs to interval  [L, L-1, ... 1, 0]
                        unsigned sameBitNum = L - nearestBucket.y_;
                        assert(sameBitNum>=0 && sameBitNum<=L);
                        // when y = 0,  normIndex = U.size-1 where U(normIndex) is the maximum value
                        int normIndex = (unsigned)mylsh.getNormIntervals().size()-1 - nearestBucket.x_;
                        assert(normIndex>=1 && normIndex<=mylsh.getNormIntervals().size()-1);

                        float u_max = mylsh.getNormIntervals()[normIndex];
                        return (L / CONSTANT - sameBitNum) * u_max;
                    }) {

        table_ = 0;
        layer_ = 0;
        idxToLayer_ = 0;

        this->R_ = mylsh.getHashBitsLen();
        this->lengthBitCount = mylsh.getLengthBitsCount();
    }


    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;
        static long  count = 0;

        while (!fvs_->existed(layer_, idxToLayer_)) {

            if (!imiProber.hashNext()) {
                return std::make_pair(-1, -1);
            }

            // if current layer is visited, retrieve from heap
            const InvertedMultiIndex imi = imiProber.getNext();

            table_ = imi.tableIndex;
            layer_ = imi.y_;            // layer = imi.y_ 0,1,2, ..y_len
            curent_interval_ = imiProber.getXlen() - imi.x_ ;
            idxToLayer_ = 0;

//            std::cout << " pop: " << count++ << " imi probe: " << imiProber.size() << std::endl;

        }

        return getCurrentLayerNextBID(table_);
    }


    std::pair<unsigned, BIDTYPE > getCurrentLayerNextBID(unsigned table) {


        const bool* fv = fvs_->getFlippingVector(layer_, idxToLayer_++);


        BIDTYPE newBucket = 0;
        for (unsigned i = 0; i < this->R_; ++i) {
            newBucket <<= 1;
            if (fv[i] == true) {
                newBucket += 1 - this->hashBits_[table][i];
            } else {
                newBucket += this->hashBits_[table][i];
            }
        }

        // last several bits will not used to flip
        // for example: 0 1 0 1 .. 1 1 1 1 1 1
        // flip vector: 1 1 1 1 .. 1 1 0 0 0 0 (hamming dist = R)
        newBucket <<= lengthBitCount;
        for (int i = 0; i < lengthBitCount; ++i) {
            newBucket |= (curent_interval_& (1<<i)==0?0:1);
        }


//        for (int i = 0; i < this->R_; ++i) {
//            std::cout << (fv[i]? 0 : 1) ;
//        }
//        std::cout  << "  " ;
//        for (int i = 0; i < lengthBitCount; ++i) {
//            std::cout << (curent_interval_& (1<<i));
//        }
//
//        std::cout  << " layer_: " << layer_ << " idxToLayer_: " << idxToLayer_;
//        std::cout  << std::endl;

        std::pair<unsigned, BIDTYPE> result = std::make_pair(table, newBucket);

        return result;
    };

private:
    const FV* fvs_ ;
    unsigned table_;
    unsigned layer_;
    unsigned idxToLayer_;
    unsigned lengthBitCount;

    unsigned curent_interval_;

    IMIProber imiProber;
};
