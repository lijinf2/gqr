#include <vector>
#include <unordered_map>
#include <lshbox/query/prober.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <bitset>




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

    inline unsigned calculateDist(
        const BIDTYPE& queryVal, 
        const BIDTYPE& bucketVal, 
        const unsigned lengthBitNum, 
        const BIDTYPE& validLengthMask,
        const unsigned paramN) {

        BIDTYPE validLength  = bucketVal & validLengthMask;
        BIDTYPE validBitsMask = getValidBitsMask(validLength, lengthBitNum);

        BIDTYPE xorVal = (queryVal ^ bucketVal) &  validBitsMask;
        unsigned hamDist = countOnes(xorVal);
        // std::cout 
        //     << "----------------------" << std::endl
        //     << "queryVal  : " << std::bitset<32>(queryVal)          << std::endl
        //     << "bucketVal : " << std::bitset<32>(bucketVal)      << std::endl
        //     << "bitsMask  : " << std::bitset<32>(validBitsMask)  << std::endl
        //     << "xorVal    : " << std::bitset<32>(xorVal)         << std::endl
        //     << "validLen  : " << validLength                     << std::endl
        //     << "hamDist   : " << hamDist                         << std::endl
        //     << "hamDist   : " << hamDist + paramN - validLength  << std::endl;

        hamDist += paramN - validLength;
        
        return hamDist;
    }

public:
    
    LengthMarkedTable(
            BIDTYPE hashVal, // hash value of query q
            unsigned paramN, // number of bits per binary code
            const std::unordered_map<BIDTYPE, std::vector<unsigned> >& table
           ){

        // ranking by linear sorting with weight(valid length is represented by last lengthBitNum bit)
        const unsigned lengthBitNum = this->getLengthBit(paramN);
        const BIDTYPE  validLengthMask = this->getValidLengthMask(lengthBitNum);

        dstToBks_.resize(paramN + 1); // maximum hamming dist is paramN

        for ( std::unordered_map<BIDTYPE, std::vector<unsigned> >::const_iterator it = table.begin(); it != table.end(); ++it) {

            const BIDTYPE& bucketVal = it->first;
            unsigned hamDist = calculateDist(hashVal, bucketVal, lengthBitNum, validLengthMask, paramN);

            assert(hamDist < dstToBks_.size());
            dstToBks_[hamDist].push_back(bucketVal);

        }

        // setNextRowCol_();
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
class LengthMarked : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    LengthMarked(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        allTables_.reserve(mylsh.tables.size());
        for (int i = 0; i < mylsh.tables.size(); ++i) {
            BIDTYPE hashValue = mylsh.getHashVal(i, domin);
            allTables_.emplace_back(LengthMarkedTable(hashValue, this->R_, mylsh.tables[i]));
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
