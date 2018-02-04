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
#include "lshbox/mip/lmip.h"
#include <util/heap_element.h>
#include <lshbox/query/fv.h>
#include "lshbox/mip/query/inverted_multi_index.h"

template <typename ACCESSOR>
class LengthMarkedLookup: public Prober<ACCESSOR> {


public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef typename Prober<ACCESSOR>::BIDTYPE BIDTYPE;
    typedef lshbox::LMIP<DATATYPE> LSHTYPE;


    LengthMarkedLookup(
            const DATATYPE* domin,
            lshbox::Scanner<ACCESSOR>& scanner,
            LSHTYPE& mylsh,
            FV* fvs)
            :
            Prober<ACCESSOR>(domin, scanner, mylsh),
            fvs_(fvs),
            imiProber(
                    mylsh.tables.size(),               // table size
                    mylsh.getNormIntervals().size()-1, // x equals intervals count, first element is the minimum.
                    mylsh.getHashBitsLen()+1,          // y equals number of bit different. 0 1 2 ... bitsLen
                    [&](InvertedMultiIndex& nearestBucket) { // how to determine distance
                        unsigned L = mylsh.getHashBitsLen();
                        int sameBitNum = L/2 - (L - nearestBucket.x_);
                        int normIndex = mylsh.getLengthBitsCount() - nearestBucket.y_ - 1;
                        return (L / 32.0f - sameBitNum) * mylsh.getNormIntervals()[normIndex+1];
                    }) {

        table_ = 0;
        layer_ = 0;
        idxToLayer_ = 0;

        this->R_ = mylsh.getHashBitsLen();

    }


    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        while (!fvs_->existed(layer_, idxToLayer_)) {

            if (!imiProber.hashNext()) {
                assert(false);
            }

            // if current layer is visited, retrieve from heap
            const InvertedMultiIndex& imi = imiProber.getNext();

            table_ = imi.tableIndex;
            layer_ = imi.y_;            // layer = imi.y_ 0,1,2, ..y_len
            idxToLayer_ = 0;

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

        std::pair<unsigned, BIDTYPE> result = std::make_pair(table, newBucket);
        return result;
    };

private:
    const FV* fvs_ ;
    unsigned table_;
    unsigned layer_;
    unsigned idxToLayer_;

    IMIProber imiProber;
};
