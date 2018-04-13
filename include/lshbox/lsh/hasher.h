#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "gqr/util/gqrhash.h"
#include "base/basehasher.h"
using std::vector;
using std::unordered_map;
using std::string;
using std::ifstream;
using std::istringstream;
using lshbox::gqrhash;

namespace lshbox {

template<typename DATATYPE = float>
class Hasher : public BaseHasher<DATATYPE, unsigned long long>{
public:
    typedef unsigned long long BIDTYPE;

    Hasher() : BaseHasher<DATATYPE, BIDTYPE>() {}

    BIDTYPE getBuckets(unsigned tb, const DATATYPE *data) const override {
        return getHashVal(tb, data);
    }
    virtual vector<float>  getHashFloats(unsigned k, const DATATYPE* domin) {
        std::cout << "invoke getHashFloats but not override" << std::endl;
        assert(false);
        return vector<float>();
    }; 

    virtual vector<bool> getHashBits(unsigned k, const DATATYPE *domin) const = 0;

    virtual vector<bool> quantization(const vector<float>& hashFloats) const;

    void initBaseHasher(
        const string &bitsFile, 
        int numTables,
        int cardinality,
        int codelength);

    BIDTYPE getHashVal(unsigned k, const DATATYPE *domin) const;

    BIDTYPE bitsToBucket(const vector<bool>& hashbits) const; 

    vector<bool> quantizeByZero(const vector<float>& hashFloats) const;

};

//--------------------- Implementations ------------------
template<typename DATATYPE>
vector<bool> Hasher<DATATYPE>::quantization(const vector<float>& hashFloats) const
{
    return  this->quantizeByZero(hashFloats);
}

template<typename DATATYPE>
void Hasher<DATATYPE>::initBaseHasher(
    const string &bitsFile,
    int numTables,
    int cardinality,
    int codelength) {

    this->codelength = codelength;
    this->numTotalItems = cardinality;

    ifstream baseFin(bitsFile.c_str());
    if (!baseFin) {
        std::cout << "cannot open file " << bitsFile << std::endl;
        assert(false);
    }
    this->tables.reserve(numTables);
    string line;
    int tmp;
    vector<bool> record(codelength);
    int itemIdx = 0;
    unordered_map<BIDTYPE, vector<unsigned>, gqrhash<BIDTYPE>> curTable;
    while (getline(baseFin, line)) {
        istringstream iss(line);
        for (int i = 0; i < codelength; ++i) {
            iss >> tmp;
            if (tmp == 1) record[i] = 1;
            else if(tmp == 0 || tmp == -1) record[i] = 0;
            else assert(false);
        }
        BIDTYPE hashVal = this->bitsToBucket(record);
        if (curTable.find(hashVal) == curTable.end())
            curTable[hashVal] = vector<unsigned>();
        curTable[hashVal].push_back(itemIdx);
        itemIdx++;
        if (itemIdx == cardinality) {
            itemIdx = 0;
            this->tables.emplace_back(curTable);
            curTable.clear();
        }
    }
    baseFin.close();
}

template<typename DATATYPE>
typename Hasher<DATATYPE>::BIDTYPE Hasher<DATATYPE>::getHashVal(unsigned k, const DATATYPE *domin) const {
    vector<bool> hashbits = getHashBits(k, domin);
    return bitsToBucket(hashbits);
}

template<typename DATATYPE>
typename Hasher<DATATYPE>::BIDTYPE Hasher<DATATYPE>::bitsToBucket(const vector<bool>& hashbits) const {
     BIDTYPE hashVal = 0;
     for (unsigned i = 0; i != hashbits.size(); ++i)
     {
         hashVal <<= 1; // hashVal *= 2
         if (hashbits[i])
         {
             hashVal += 1;
         }
     }
    return hashVal;
}

template<typename DATATYPE>
vector<bool> Hasher<DATATYPE>::quantizeByZero(const vector<float>& hashFloats) const
{
    vector<bool> hashBits;
    hashBits.resize(hashFloats.size());
    for (int i = 0; i < hashFloats.size(); ++i) {
        if (hashFloats[i] >= 0) {
            hashBits[i] = 1;
        } else {
            hashBits[i] = 0;
        }
    }
    return  hashBits;
}
};
