#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <util/io.h>
using std::vector;
using std::unordered_map;
using std::string;
using std::ifstream;
using std::istringstream;

namespace std {
template<typename T>
class hash<std::vector<T>> {
public:
    size_t operator()(const std::vector<T>& vec) const {
        size_t seed = 0;
        std::hash<T> hasher;
        for (auto& v : vec) {
            seed ^= hasher(v);
        }
        return seed;
    }
};
}

namespace lshbox {

template<typename DATATYPE = float, typename BIDTYPE = unsigned long long>
class BaseHasher {
public:

    unsigned numTotalItems;
    unsigned codelength;
    vector<unordered_map<BIDTYPE, vector<unsigned>>> tables;

    BaseHasher() {}

    /* variables must be initialized in loadModel*/
    virtual void loadModel(const string& modelFile, const string& baseBitsFile) = 0; 

    virtual BIDTYPE getBuckets(unsigned tb, const DATATYPE *domin) = 0;

    vector<unsigned> getAllTableSize();

    vector<unsigned> getAllMaxBucketSize();

    // when there is only one hash table
    unsigned getTableSize();

    // when there is only one hash table
    unsigned getMaxBucketSize();

    unsigned getBaseSize();

    unsigned getCodeLength();

    unsigned getNumTables();

    template<typename PROBER>
    int probe(unsigned t, BIDTYPE bucketId, PROBER &prober);

    template<typename PROBER>
    void KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems);

protected:
    vector<vector<float>> loadFloatMatrixTranspose(ifstream& fin, unsigned numLine, unsigned dimension) const ;

    vector<float> loadFloatVector(ifstream& fin, unsigned dimension) const;

    float getProjection(
        const DATATYPE* data, 
        const vector<float>& function, 
        const vector<float>& mean = vector<float>()) const ;

    vector<float> getProjection(
        const DATATYPE* data, 
        const vector<vector<float>>& matrix, 
        const vector<float>& mean = vector<float>()) const ;
};

//--------------------- Implementations ------------------
template<typename DATATYPE, typename BIDTYPE>
vector<unsigned> BaseHasher<DATATYPE, BIDTYPE>::getAllTableSize() {
    vector<unsigned> vec;
    vec.resize(tables.size());
    for (int i = 0; i < tables.size(); ++i) {
        vec[i] = tables[i].size();
    }
    return vec;
}

template<typename DATATYPE, typename BIDTYPE>
vector<unsigned> BaseHasher<DATATYPE, BIDTYPE>::getAllMaxBucketSize() {
    vector<unsigned> vec(tables.size());
    for (int tb = 0; tb < tables.size(); ++tb) {
        unsigned max = 0;
        typename unordered_map<BIDTYPE, vector<unsigned> >::const_iterator it;
        for (it = tables[tb].begin(); it != tables[tb].end(); ++it) {
            if (it->second.size() > max) {
                max = it->second.size();
            }
        }
        vec[tb] = max;
    }
    return vec;
}

template<typename DATATYPE, typename BIDTYPE>
unsigned BaseHasher<DATATYPE, BIDTYPE>::getTableSize() {
    return tables[0].size();
}

template<typename DATATYPE, typename BIDTYPE>
unsigned BaseHasher<DATATYPE, BIDTYPE>::getMaxBucketSize() {
    unsigned max = 0;
    typename unordered_map<BIDTYPE, std::vector<unsigned> >::const_iterator it;
    for (it = tables[0].begin(); it != tables[0].end(); ++it) {
        if (it->second.size() > max) {
            max = it->second.size();
        }
    }
    return max;
}

template<typename DATATYPE, typename BIDTYPE>
unsigned BaseHasher<DATATYPE, BIDTYPE>::getBaseSize() {
    return this->numTotalItems;
}

template<typename DATATYPE, typename BIDTYPE>
unsigned BaseHasher<DATATYPE, BIDTYPE>::getCodeLength() {
    return this->codelength;
}

template<typename DATATYPE, typename BIDTYPE>
unsigned BaseHasher<DATATYPE, BIDTYPE>::getNumTables() {
    return this->tables.size();
}


template<typename DATATYPE, typename BIDTYPE>
template<typename PROBER>
int BaseHasher<DATATYPE, BIDTYPE>::probe(unsigned t, BIDTYPE bucketId, PROBER& prober) {
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

template<typename DATATYPE, typename BIDTYPE>
template<typename PROBER>
void BaseHasher<DATATYPE, BIDTYPE>::KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems) {

    while(prober.getNumItemsProbed() < numItems && prober.nextBucketExisted()) {
        // <table, bucketId>
        const std::pair<unsigned, BIDTYPE>& probePair = prober.getNextBID();
        probe(probePair.first, probePair.second, prober); 
    }
}

/*
 * protected field*/
template<typename DATATYPE, typename BIDTYPE>
vector<vector<float>> BaseHasher<DATATYPE, BIDTYPE>::loadFloatMatrixTranspose(ifstream& fin, unsigned numLine, unsigned dimension) const {
    vector<vector<float>> transpose;
    transpose.resize(dimension);
    for (auto& vec: transpose) {
        vec.resize(numLine);
    }
    string line;
    for (int row = 0; row < numLine; ++row) {
        getline(fin, line);
        istringstream iss(line);
        for (int cIdx = 0; cIdx < dimension; ++cIdx) {
            iss >> transpose[cIdx][row];
        }
    }
    return transpose;
}

template<typename DATATYPE, typename BIDTYPE>
vector<float> BaseHasher<DATATYPE, BIDTYPE>::loadFloatVector(ifstream& fin, unsigned dimension) const {
    string line;
    getline(fin, line);
    return lshbox::readFloatVectorFromLine(line, dimension);
}

template<typename DATATYPE, typename BIDTYPE>
float BaseHasher<DATATYPE, BIDTYPE>::getProjection(
    const DATATYPE* data, const vector<float>& function, const vector<float>& mean) const { 
    assert(mean.size() == function.size());

    float result = 0;
    for (int i = 0; i < function.size(); ++i) {
        result += (data[i] - mean[i]) * function[i];
    }
    return result;
}

template<typename DATATYPE, typename BIDTYPE>
vector<float> BaseHasher<DATATYPE, BIDTYPE>::getProjection(
    const DATATYPE* data, const vector<vector<float>>& matrix, const vector<float>& mean) const { 

    if(matrix.size() == 0) return vector<float>();
    assert(matrix[0].size() == mean.size());

    vector<float> result(matrix.size());
    for (int i = 0; i < result.size(); ++i) {
        result[i] = getProjection(data, matrix[i], mean);
    }
    return result;
}

};
