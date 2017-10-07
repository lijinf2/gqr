#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
using std::vector;
using std::unordered_map;
using std::string;
using std::ifstream;
using std::istringstream;

namespace lshbox {

template<typename DATATYPE = float>
class Hasher {
public:
    typedef unsigned long long BIDTYPE;
    // struct Parameter
    // {
    //     /// Number of hash tables
    //     unsigned L = 0;
    //     /// Dimension of the vector
    //     unsigned D = 0;
    //     /// Binary code bytes
    //     unsigned N = 0;
    //     /// Size of vectors in train
    //     unsigned S = 0;
    // };

    int numTotalItems;
    int codelength;
    vector<unordered_map<BIDTYPE, vector<unsigned>>> tables;

    Hasher() {}

    virtual vector<bool> getHashBits(unsigned k, const DATATYPE *domin) = 0;

    void initBaseHasher(
        const string &bitsFile, 
        int numTables,
        int cardinality,
        int codelength);

    BIDTYPE getHashVal(unsigned k, const DATATYPE *domin);

    BIDTYPE bitsToBucket(const vector<bool>& hashbits); 

    template<typename PROBER>
    int probe(unsigned t, BIDTYPE bucketId, PROBER &prober);

    template<typename PROBER>
    void KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems);

    int getTableSize();

    int getMaxBucketSize();

    int getBaseSize();

    int getCodeLength();

    int getNumTables();
};

//--------------------- Implementations ------------------
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
    unordered_map<BIDTYPE, vector<unsigned>> curTable;
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
typename Hasher<DATATYPE>::BIDTYPE Hasher<DATATYPE>::getHashVal(unsigned k, const DATATYPE *domin) {
    vector<bool> hashbits = getHashBits(k, domin);
    return bitsToBucket(hashbits);
}

template<typename DATATYPE>
typename Hasher<DATATYPE>::BIDTYPE Hasher<DATATYPE>::bitsToBucket(const vector<bool>& hashbits) {
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
template<typename PROBER>
int Hasher<DATATYPE>::probe(unsigned t, BIDTYPE bucketId, PROBER& prober) {
    int numProbed = 0;
    if (this->tables[t].find(bucketId) != this->tables[t].end())
    {
        numProbed = this->tables[t][bucketId].size();
        for (std::vector<unsigned>::iterator iter = this->tables[t][bucketId].begin(); iter != this->tables[t][bucketId].end(); ++iter)
        {
            prober(*iter);
        }
    }
    return numProbed;
}

template<typename DATATYPE>
template<typename PROBER>
void Hasher<DATATYPE>::KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems) {

    while(prober.getNumItemsProbed() < numItems && prober.nextBucketExisted()) {
        // <table, bucketId>
        const std::pair<unsigned, BIDTYPE>& probePair = prober.getNextBID();
        probe(probePair.first, probePair.second, prober); 
    }
}

template<typename DATATYPE>
int Hasher<DATATYPE>::getTableSize() {
    return tables[0].size();
}

template<typename DATATYPE>
int Hasher<DATATYPE>::getMaxBucketSize() {
    int max = 0;
    std::unordered_map<BIDTYPE, std::vector<unsigned> >::const_iterator it;
    for (it = tables[0].begin(); it != tables[0].end(); ++it) {
        if (it->second.size() > max) {
            max = it->second.size();
        }
    }
    return max;
}

template<typename DATATYPE>
int Hasher<DATATYPE>::getBaseSize() {
    return this->numTotalItems;
}

template<typename DATATYPE>
int Hasher<DATATYPE>::getCodeLength() {
    return this->codelength;
}

template<typename DATATYPE>
int Hasher<DATATYPE>::getNumTables() {
    return this->tables.size();
}
};
