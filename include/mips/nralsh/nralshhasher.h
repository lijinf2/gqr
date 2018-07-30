#pragma once
#include <iostream>
#include <vector>
#include <utility>

// #include "intcode/hash/alsh.h"
#include "mips/alshrank/alshrankhasher.h"
using std::pair;
using std::vector;

namespace lshbox {

template<typename DATATYPE>
class NRALSHHasher : public ALSHRankHasher<DATATYPE> {
public:
    int numIntervals = -1;
    vector<float> scalers;

    const vector<float>& getScalers() const {
        return scalers;
    }
    virtual void loadModel(const string& modelFile, const string& baseBitsFile) override;
};

template<typename DATATYPE>
void NRALSHHasher<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
    std::cout << "load models and initilize hash tables ...";

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


    // load m and U
    getline(modelFin, line);
    istringstream parameterIss(line);
    parameterIss >> this->W;
    parameterIss >> this->m;
    parameterIss >> this->U;
    parameterIss >> this->numIntervals;

    // set mean to 0
    this->mean = vector<float>(modelNumFeature + this->m);

    // scalers
    this->loadFloatVector(modelFin, numIntervals).swap(this->scalers);

    // hash functions
    this->pcsAll.resize(modelNumTable);
    this->shift.resize(modelNumTable);
    for (int tb = 0; tb < modelNumTable; ++tb) {
        this->loadFloatMatrixTranspose(modelFin, modelNumFeature + this->m, modelCodelen).swap(this->pcsAll[tb]);
        this->loadFloatVector(modelFin, modelCodelen).swap(this->shift[tb]);
    }

    // initialized numTotalItems and tables, modelCodelen + 1 (index of scaling factor)
    this->initBaseHasher(baseBitsFile, modelNumTable, modelNumItem, modelCodelen + 1);
    std::cout << " finished " << std::endl;
}

}
