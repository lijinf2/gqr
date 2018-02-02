#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
#include <cmath>
#include <lshbox/matrix.h>
#include <lshbox/eval.h>
#include <lshbox/utils.h>
#include "lshbox/lsh/hasher.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::ifstream;
using std::istringstream;
namespace lshbox
{

template<typename DATATYPE = float>
class LMIP: public Hasher<DATATYPE>
{

public:

    typedef typename Hasher<DATATYPE>::BIDTYPE BIDTYPE;

    LMIP() : Hasher<DATATYPE>() {};

    vector<bool> getHashBits(unsigned k, const DATATYPE *domin) override;

    vector<float> getHashFloats(unsigned k, const DATATYPE *domin);

    void loadModel(const string& modelFile, const string& baseBitsFile);

private:
    vector<vector<vector<float> > > pcsAll;
    vector<float> mean;
    vector<float> normPrctile;
    unsigned lengthBitsCount;
    unsigned normIntervalCount;
    unsigned tableCodelen;
};
}

template<typename DATATYPE>
vector<float> lshbox::LMIP<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin) {

    vector<float> domin_pc(tableCodelen + lengthBitsCount, 0);

    // zero-centered first
    for (unsigned i = 0; i != pcsAll[k].size(); ++i) {
        for (unsigned j = 0; j != pcsAll[k][i].size(); ++j) {
            domin_pc[i] += (domin[j] - mean[j] )* pcsAll[k][i][j];
        }
    }

    return domin_pc;
}

template<typename DATATYPE>
vector<bool> lshbox::LMIP<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin)
{
    vector<float> hashFloats = getHashFloats(k, domin);
    vector<bool> hashBits = this->quantization(hashFloats);
    return hashBits;
}

template<typename DATATYPE>
void lshbox::LMIP<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
    string line;
    // initialized statistics and model
    ifstream modelFin(modelFile.c_str());
    if (!modelFin) {
        std::cout << "cannot open file " << modelFile << std::endl;
        assert(false);
    }
    getline(modelFin, line);
    istringstream statIss(line);
    int numTables, tableDim, tableNumItems, tableNumQueries;
    statIss >> numTables >> tableDim >> tableCodelen >> tableNumItems >> tableNumQueries;

    getline(modelFin, line);
    istringstream paramIss(line);
    paramIss >> this->lengthBitsCount >> this->normIntervalCount;

    // mean and pcsAll
    mean.resize(tableDim);
    getline(modelFin, line);
    istringstream meanIss(line);
    for (int i = 0; i < mean.size(); ++i) {
        meanIss >> mean[i];
    }

    normPrctile.resize(this->normIntervalCount+1);;
    getline(modelFin, line);
    istringstream prctileIss(line);
    for (int i = 0; i < normPrctile.size(); ++i)
    {
        prctileIss >> normPrctile[i];
    }

    this->pcsAll.resize(numTables);
    for (auto& curPcs : pcsAll) {
        curPcs.resize(tableCodelen);
        for (auto& v : curPcs) {
            v.resize(tableDim);
        }
        for (int row = 0; row < tableDim; ++row) {
            getline(modelFin, line);
            istringstream iss(line);
            for (int cIdx = 0; cIdx < tableCodelen; ++cIdx) {
                iss >> curPcs[cIdx][row];
            }
        }
    }
    modelFin.close();

    assert(normPrctile.size()-1 == normIntervalCount);
    assert(tableCodelen >= normIntervalCount );

    // initialized numTotalItems and tables
    this->initBaseHasher(baseBitsFile, numTables, tableNumItems, tableCodelen+lengthBitsCount);
    // should be avoid
    this->codelength = tableCodelen;
}
