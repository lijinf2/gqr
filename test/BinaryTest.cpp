//
// Created by darxan on 2018/1/23.
//


#include <iostream>
#include <vector>
#include <cmath>
#include <iostream>
#include <limits>
#include <bitset>
#include <assert.h>


class LengthMarkedTable {
public:
    typedef unsigned long long BIDTYPE;

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

        BIDTYPE validLength  = (bucketVal & validLengthMask) + 1 ;
        assert(validLength <= paramN);
        BIDTYPE validBitsMask = getValidBitsMask(validLength, lengthBitNum);

        BIDTYPE xorVal = (queryVal ^ bucketVal) &  validBitsMask;
        unsigned hamDist = countOnes(xorVal);
        std::cout
                << "----------------------" << std::endl
                << "queryVal  : " << std::bitset<32>(queryVal)          << std::endl
                << "bucketVal : " << std::bitset<32>(bucketVal)      << std::endl
                << "bitsMask  : " << std::bitset<32>(validBitsMask)  << std::endl
                << "xorVal    : " << std::bitset<32>(xorVal)         << std::endl
                << "validLen  : " << validLength                     << std::endl
                << "hamDist   : " << hamDist                         << std::endl
                << "hamDist   : " << hamDist + paramN - validLength  << std::endl;

        hamDist += paramN - validLength;

        return hamDist;
    }

};

int main()
{
    LengthMarkedTable table;
    unsigned dist = table.calculateDist(
            0b1011010110,
            0b1000010110,
            3,
            0b0000000111,
            7
    );
    std::cout<<"dist : "<<dist<<std::endl;
    return 0;
}



