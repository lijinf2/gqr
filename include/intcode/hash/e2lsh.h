#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include "gqr/util/gqrhash.h"
#include <base/basehasher.h>
using std::vector;
using std::unordered_map;
using std::string;
using std::ifstream;
using std::istringstream;

namespace lshbox {

template<typename DATATYPE = float>
class E2LSH: public BaseHasher<DATATYPE, vector<int>>{
protected:
    typedef vector<int> BIDTYPE;
    float W;
    vector<float> mean;

    /* for L hash tables*/
    vector<vector<vector<float> > > pcsAll;
    vector<vector<float>> shift;

public:

    E2LSH() : BaseHasher<DATATYPE, BIDTYPE>()  {}

    void loadModel(const string& modelFile, const string& baseBitsFile) override; 

    void initBaseHasher(
        const string &bitsFile, 
        int NumTable,
        int cardinality,
        int codelength);

    virtual vector<float> getHashFloats(unsigned k, const DATATYPE *domin);

    BIDTYPE getBuckets(unsigned k, const DATATYPE *domin);
    // BIDTYPE getHashVal(unsigned k, const DATATYPE *domin);

    // virtual vector<int> quantization(const vector<float>& hashFloats);
    //
    //
    // vector<bool> quantizeByZero(const vector<float>& hashFloats);
};

//--------------------- Implementations ------------------
template<typename DATATYPE>
void E2LSH<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
    string line;
    // initialized statistics and model
    ifstream modelFin(modelFile.c_str());
    if (!modelFin) {
        std::cout << "cannot open file " << modelFile << std::endl;
        assert(false);
    }
    getline(modelFin, line);
    istringstream statIss(line);
    int modelNumTable, modelNumFeature, modelCodelen, modelNumItem, modelNumQuery;
    statIss >> modelNumTable >> modelNumFeature >> modelCodelen >> modelNumItem >> modelNumQuery;

    statIss >> this->W;

    // mean 
    this->loadFloatVector(modelFin, modelNumFeature).swap(mean);


    // hash functions
    this->pcsAll.resize(modelNumTable);
    this->shift.resize(modelNumTable);
    for (int tb = 0; tb < modelNumTable; ++tb) {
        this->loadFloatMatrixTranspose(modelFin, modelNumFeature, modelCodelen).swap(pcsAll[tb]);
        this->loadFloatVector(modelFin, modelCodelen).swap(shift[tb]);
    }

    // initialized numTotalItems and tables
    this->initBaseHasher(baseBitsFile, modelNumTable, modelNumItem, modelCodelen);
}

template<typename DATATYPE>
void E2LSH<DATATYPE>::initBaseHasher(
    const string &bitsFile,
    int NumTable,
    int cardinality,
    int codelength) {

    this->codelength = codelength;
    this->numTotalItems = cardinality;

    ifstream baseFin(bitsFile.c_str());
    if (!baseFin) {
        std::cout << "cannot open file " << bitsFile << std::endl;
        assert(false);
    }
    this->tables.reserve(NumTable);
    string line;
    vector<int> hashVal(codelength);
    int itemIdx = 0;
    unordered_map<BIDTYPE, vector<unsigned>, gqrhash<BIDTYPE>> curTable;
    while (getline(baseFin, line)) {
        istringstream iss(line);
        for (int i = 0; i < codelength; ++i) {
            iss >> hashVal[i];
        }
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
vector<float> E2LSH<DATATYPE>::getHashFloats(unsigned tableIdx, const DATATYPE *data)
{
    // project
    vector<float> projVector = this->getProjection(data, pcsAll[tableIdx], mean);

    // shift and chop
    for (int i = 0; i < projVector.size(); ++i) {
        projVector[i] += shift[tableIdx][i];
        projVector[i] /= W;
    }
    return projVector;
}

template<typename DATATYPE>
vector<int> E2LSH<DATATYPE>::getBuckets(unsigned tableIdx, const DATATYPE *data)
{
    vector<float> hashFloats = getHashFloats(tableIdx, data);

    vector<int> hashVal(hashFloats.size());
    for (int i = 0; i < hashVal.size(); ++i) {
        hashVal[i] = floor(hashFloats[i]);
    }
    return hashVal;
}
};
