#include <vector>
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

long count_fjdslfjdkfjdklj = 0;

class NRTable {
private:
    typedef unsigned long long BIDTYPE;


    inline unsigned getLengthBit(unsigned paramN) {
        if (paramN<=1) { assert(false); }
        return paramN / 2;
    }

    inline BIDTYPE getValidLengthMask(unsigned lengthBitNum) {

        BIDTYPE lengthMask = 0;
        for (unsigned i = 0; i < lengthBitNum; ++i) {
            // assign the i'th bit 1
            lengthMask |= (1ULL << i);
        }

        return lengthMask;
    }


    inline BIDTYPE getValidBitsMask(unsigned validLength, unsigned lengthBitNum) {
        BIDTYPE bitsMask = 0;

        for (unsigned i = 0; i < validLength; ++i)
        {
            // assign the (lengthBitNum + i)'th bit 1
            bitsMask |= (1ULL << (lengthBitNum + i));
        }
        return bitsMask;
    }


    inline unsigned countOnes(BIDTYPE xorVal) {
        unsigned hamDist = 0;
        while(xorVal != 0){
            hamDist++;
            xorVal &= xorVal - 1;
        }
        return hamDist;
    }


    inline float calculateDist(
            const BIDTYPE& queryVal,
            const BIDTYPE& bucketVal,
            const unsigned lengthBitNum,
            const BIDTYPE& validLengthMask,
            const unsigned paramN) {

        BIDTYPE validLength  = (bucketVal & validLengthMask) ;

        BIDTYPE validBitsMask = getValidBitsMask(paramN, lengthBitNum);
        BIDTYPE xorVal = (queryVal ^ bucketVal) &  validBitsMask;

        unsigned diffBitNum = countOnes(xorVal);
        unsigned sameBitNum = paramN - diffBitNum;
        float hammingDist = (paramN / 32.0f - sameBitNum) * (float)validLength ;
//        float hammingDist = ( - sameBitNum) * (float)validLength ;

        return hammingDist;
    }

public:
    typedef std::unordered_map<BIDTYPE, std::vector<unsigned> > TableT;

    NRTable(
            BIDTYPE hashVal,
            unsigned paramN,
            const TableT& table){

        const unsigned lengthBitNum = this->getLengthBit(paramN);
        const BIDTYPE  validLengthMask = this->getValidLengthMask(lengthBitNum);

        dstToBks_.reserve(table.size());
        BIDTYPE xorVal;
        float dst;
        for ( TableT::const_iterator it = table.begin(); it != table.end(); ++it) {

            // should be improved by xor operations

            const BIDTYPE& bucketVal = it->first;
            dst  = calculateDist(hashVal, bucketVal, lengthBitNum, validLengthMask, paramN);

            dstToBks_.emplace_back(std::pair<float, BIDTYPE>(dst, it->first));
        }
        assert(dstToBks_.size() == table.size());

        std::cout << "start ranking :" << dstToBks_.size() << " count: " << count_fjdslfjdkfjdklj++ << std::endl;

        std::sort(dstToBks_.begin(),
                  dstToBks_.end(),
                  [] (const std::pair<float, BIDTYPE>& a, const std::pair<float, BIDTYPE>& b ) {
                      return a.first < b.first;
                  });

        iterator = 0;
    }

    bool reset() {
        iterator = 0;
    }

    float getCurScore() {
        return dstToBks_[iterator].first;
    }

    BIDTYPE getCurBucket() {
        return dstToBks_[iterator].second;
    }

    // move to next, if exist return true and otherwise false
    bool moveForward() {
        iterator++;
        return iterator < dstToBks_.size();
    }

private:
    std::vector<std::pair<float, BIDTYPE> > dstToBks_;
    unsigned iterator = 0;
};

template<typename ACCESSOR>
class NormalizedRank : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
    // typedef std::pair<float, unsigned > PairT; // <score, tableIdx>
    typedef ScoreIdxPair PairT;

    template<typename LSHTYPE>
    NormalizedRank(
            const DATATYPE* domin,
            lshbox::Scanner<ACCESSOR>& scanner,
            LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        allTables_.reserve(mylsh.tables.size());

        for (int i = 0; i < mylsh.tables.size(); ++i) {

            BIDTYPE hashValue = mylsh.getHashVal(i, domin);
            allTables_.emplace_back(
                    NRTable(hashValue, mylsh.getCodeLength(), mylsh.tables[i]));
        }

        for (unsigned i = 0; i != allTables_.size(); ++i) {
            float score = allTables_[i].getCurScore();
            heap_.push(PairT(score , i));
        }
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;
        unsigned tb = heap_.top().index_;
        heap_.pop();

        BIDTYPE nextBucket = allTables_[tb].getCurBucket();
        if (allTables_[tb].moveForward()) {
            float score = allTables_[tb].getCurScore();
            heap_.push(PairT(score, tb));
        }
        return std::make_pair(tb, nextBucket);
    }

private:
    std::vector<NRTable> allTables_;

    std::priority_queue<PairT> heap_;
};
