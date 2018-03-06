#pragma once
#include <vector>
#include <unordered_map>
#include <lshbox/query/prober.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <bitset>
#include <lshbox.h>

#include <mips/normrange/normrangehasher.h>


/**
 * a total bits array represents a unsigned long long:
 * eg:
 *       total bits num     : 10
 *       paramN             : 7 (7 hash value bit )
 *       length bit num     : 3 (cause 2^3 >= 7, 3 is big enough),
 *       queryVal           : 1 0 1 1 0 1 0 [append with] 1 1 0 (6+1=7 valid bits)
 *       bucketVal          : 0 0 1 1 1 1 0 [append with] 0 1 1 (3+1=4 valid bits)
 *       xor                : 1 0 0 0 1 0 0 ------------  1 0 1
 *
 *       bucketLengthMask   : 0 0 0 0 0 0 0 ------------  1 1 1 (extract 011=>3=>4 valid bits)
 *       bucketValidBitMask : 0 0 0 1 1 1 1 ------------  0 0 0 (extract 4 bits)
 *       valid xor          : 0 0 0 0 1 0 0 ------------  0 0 0 (1 '1')
 *       distance           : numberOfOne(1) + paramN(7) - validBitsNumber(4) = 4
 */
class LengthMarkedTable {
private:
    typedef unsigned long long BIDTYPE;
    /*
     * with 0 bits, we can represent at most 1;
     * with 1 bits, we can represent at most 2;
     * with 2 bits, we can represent at most 4;
     * with i bits, we can represent at most 2^i;
     * return std::ceil(std::log2(paramN));
     */
    inline unsigned getLengthBit(unsigned paramN) {

        // precision error may exist with double calculation.
        // return std::ceil(std::log2(paramN));

        if (paramN<=1) {
            assert(false);
        }

        for (unsigned i = 1; i < sizeof(BIDTYPE)*8; ++i) {
            
            BIDTYPE maxLength = 1ULL << i; // maxLength = 2^i 
            if ( maxLength >= paramN ) {
                return i;
            }
        }

        assert(false);
    }

    /**
     *  Suppose we have 12 hash bit, then 4 extra bit is need to represent validated length.
     *  
     *  Eg: Totally binary: 1 0 1 0 1 1 0 0 1 0 1 0  : 1 0 0 0
     *  means 8 (1000) bits ( 1 0 1 0 1 1 0 0 ) is valid
     *  lengthBitNum = 8
     *  
     *  function: return a "0000 0000 0000 1111" to extract valid length from whole binary squence. 
     *  Eg: lengthBitNum = 4
     *      Loop: i = 0,1,2,3 
     *            mask += 0001, 0010, 0100, 1000
     */
    inline BIDTYPE getValidLengthMask(unsigned lengthBitNum) {

        BIDTYPE lengthMask = 0;
        for (unsigned i = 0; i < lengthBitNum; ++i) {
            // assign the i'th bit 1
            lengthMask |= (1ULL << i); 
        }

        return lengthMask;
    }

    /**
     *
     * @param validLength
     * @param lengthBitNum  how many bits used to represented valid bits
     * @return bitMask used to extract valid bits
     */
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


    inline unsigned calculateDistByLength(
        const BIDTYPE& queryVal, 
        const BIDTYPE& bucketVal, 
        const unsigned lengthBitNum, 
        const BIDTYPE& validLengthMask,
        const unsigned paramN) 
    {

        unsigned validLength  = (unsigned)(bucketVal & validLengthMask) + 1;
        assert(validLength <= paramN);
        BIDTYPE validBitsMask = getValidBitsMask(validLength, lengthBitNum);

        BIDTYPE xorVal = (queryVal ^ bucketVal) &  validBitsMask;
        unsigned hammingDist = countOnes(xorVal);
        hammingDist += paramN - validLength;
        
        return hammingDist;
    }

    inline unsigned calculateDistByNormBias( const BIDTYPE& queryVal,
        const BIDTYPE& bucketVal, 
        const unsigned lengthBitNum, 
        const BIDTYPE& validLengthMask,
        const unsigned paramN) 
    {

        unsigned validLength  = (unsigned)(bucketVal & validLengthMask) + 1;
        assert(validLength <= paramN);

        BIDTYPE validBitsMask = getValidBitsMask(paramN, lengthBitNum);
        BIDTYPE xorVal = (queryVal ^ bucketVal) &  validBitsMask;

        unsigned diffBitNum = countOnes(xorVal);
        unsigned hammingDist = diffBitNum + paramN - validLength;

        return hammingDist;
    }

    inline unsigned calculateDistByWeight( const BIDTYPE& queryVal,
        const BIDTYPE& bucketVal,
        const unsigned lengthBitNum,
        const BIDTYPE& validLengthMask,
        const unsigned paramN)
    {

        unsigned validLength = (unsigned)(bucketVal & validLengthMask) + 1;

        BIDTYPE validBitsMask = getValidBitsMask(paramN, lengthBitNum);
        BIDTYPE xorVal = (queryVal ^ bucketVal) &  validBitsMask;

        unsigned diffBitNum = countOnes(xorVal);
        unsigned sameBitNum = paramN - diffBitNum;
        unsigned hammingDist = paramN * ((unsigned)validLengthMask+1) - sameBitNum * validLength;


        return hammingDist;

    }
    /**
     *
     * cost only o(1) time to call
     * @param queryVal
     * @param bucketVal
     * @param lengthBitNum how many bits used to represented valid bits                           EG: 5
     * @param validLengthMask mask used to extract valid length(@validLength) of whole bits array EG: 0...0 11111 = 63
     * @param paramN totally bit number                                                           EG: 27
     * @return
     */
    inline unsigned calculateDist(
        const BIDTYPE& queryVal, 
        const BIDTYPE& bucketVal, 
        const unsigned lengthBitNum, 
        const BIDTYPE& validLengthMask,
        const unsigned paramN)
    {

        return calculateDistByLength(
            queryVal, 
            bucketVal,
            lengthBitNum, 
            validLengthMask, 
            paramN
        );
    }



    void lengthMarkedRanking(
            BIDTYPE hashVal, // hash value of query q
            const unsigned paramN, // number of bits per binary code
            const unsigned lengthBitNum,
            const std::unordered_map<BIDTYPE, std::vector<unsigned> >& table
           )
    {

        // ranking by linear sorting with weight(valid length is represented by last lengthBitNum bit)
        const BIDTYPE  validLengthMask = this->getValidLengthMask(lengthBitNum);
        dstToBks_.resize(1 + paramN * (1 + (unsigned)validLengthMask)); // maximum hamming dist is paramN*2

        for ( auto it = table.begin(); it != table.end(); ++it) {

            const BIDTYPE& bucketVal = it->first;
            unsigned hamDist = calculateDist(hashVal, bucketVal, lengthBitNum, validLengthMask, paramN);

            assert(hamDist < dstToBks_.size());
            dstToBks_[hamDist].push_back(bucketVal);

        }
    }

public:
    
    LengthMarkedTable(
            BIDTYPE hashVal, // hash value of query q
            const unsigned paramN, // number of bits per binary code
            const unsigned lengthBitNum,
            const std::unordered_map<BIDTYPE, std::vector<unsigned> >& table
           ) {

        lengthMarkedRanking(hashVal, paramN, lengthBitNum, table);
    }

    int getNumBuckets(int hamDist) {
        return dstToBks_[hamDist].size();
    }

    const std::vector<BIDTYPE>& getBuckets(int hamDist) {
        return dstToBks_[hamDist];
    } 
private:
    std::vector<std::vector<BIDTYPE>> dstToBks_;

};

template<typename ACCESSOR>
class LengthMarkedRank : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    typedef lshbox::NormRangeHasher<DATATYPE> LSHTYPE;
    LengthMarkedRank(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        this->R_ = mylsh.getHashBitsLen();
        allTables_.reserve(mylsh.tables.size());

        for (int i = 0; i < mylsh.tables.size(); ++i) {
            BIDTYPE hashValue = mylsh.getHashVal(i, domin);
            allTables_.emplace_back(LengthMarkedTable(hashValue, mylsh.getHashBitsLen(), mylsh.getLengthBitsCount(), mylsh.tables[i]));
        }
        table_ = 0;
        iterator_ = 0;
        dist_ = 0;
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        if (iterator_ < allTables_[table_].getBuckets(dist_).size()) {
            BIDTYPE nextBucketID = allTables_[table_].getBuckets(dist_)[iterator_++];
            return std::make_pair(table_, nextBucketID);
        }

        // the following is the else logic
        
        // find next valid bucketID
        iterator_ = 0;
        table_++;
        while (true) {
            if (table_ == allTables_.size()) {
                dist_++;
                table_ = 0;
            }

            if (allTables_[table_].getBuckets(dist_).size() > 0)
                break;
            else 
                table_++;
        }
        
        BIDTYPE nextBucketID = allTables_[table_].getBuckets(dist_)[iterator_++];
        return std::make_pair(table_, nextBucketID);
    }

private:
    std::vector<LengthMarkedTable> allTables_;
    unsigned table_ = 0;
    unsigned iterator_ = 0; // iterator to allTables[table_].getBuckets(dist_);
    
    unsigned dist_ = 0; // current probing hamming distance
    // std::vector<BIDTYPE>* currentTable_ = NULL;
};


