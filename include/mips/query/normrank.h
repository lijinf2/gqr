#include <vector>
#include <queue>
#include "lshbox/query/scoreidxpair.h"
#include <vector>
#include <unordered_map>
#include <lshbox/query/prober.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <lshbox.h>
#include "lshbox/mip/lmip.h"
#include "base/bucketlist.h"
#include "lshbox/utils.h"

template<typename ACCESSOR>
class NormRank : public Ranking<ACCESSOR, unsigned long long>{
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
    typedef ScoreIdxPair PairT;

    typedef lshbox::LMIP<DATATYPE> LSHTYPE;
    NormRank(
            const DATATYPE* domin,
            lshbox::Scanner<ACCESSOR>& scanner,
            LSHTYPE& mylsh) : Ranking<ACCESSOR, BIDTYPE>(domin, scanner, mylsh) {

        this->LTable_.reserve(mylsh.tables.size());

        const auto& normIntervals = mylsh.getNormIntervals();
        int numBitHash = mylsh.getHashBitsLen(); // number of bits taken by hash bits
        int numBitLength = mylsh.getLengthBitsCount(); // number of bits to present number of intervals

        // if numBitLength = 4, lenthMask will be 00...001111 
        BIDTYPE lengthMask = 0; 
        for (int i = 0; i < numBitLength; ++i) {
            lengthMask <<= 1;
            lengthMask ^= 1;
        }

        for (int tb = 0; tb < mylsh.tables.size(); ++tb) {

            BIDTYPE qHashValue = mylsh.getHashVal(tb, domin);

            auto distor = [&qHashValue, &numBitHash, &numBitLength, &lengthMask, &normIntervals](const BIDTYPE& bucket) {
                unsigned numSameBit = numBitHash - lshbox::countOnes((qHashValue ^ bucket) & (~lengthMask));
                unsigned intervalIdx = (bucket & lengthMask) + 1;
                float dist = (numBitHash / 32.0 - numSameBit) * normIntervals[intervalIdx];
                return dist;
            };

            this->LTable_.emplace_back(
                BucketList<BIDTYPE>(mylsh.tables[tb], distor));
        }

        // initialize minHeap
        this->nextBucket_.resize(this->LTable_.size());
        for (int tb = 0; tb < this->LTable_.size(); ++tb) {
            this->tbNextEnheap(tb);
        }
    }
};
