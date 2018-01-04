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
#include "hasher.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::ifstream;
using std::istringstream;
namespace lshbox
{

template<typename DATATYPE = float>
class MIPH: public Hasher<DATATYPE>
{
public:

    typedef typename Hasher<DATATYPE>::BIDTYPE BIDTYPE;

    MIPH() : Hasher<DATATYPE>() {};

    vector<bool> getHashBits(unsigned k, const DATATYPE *domin) override;

    vector<float> getHashFloats(unsigned k, const DATATYPE *domin);

    void loadModel(const string& modelFile, const string& baseBitsFile); 

private:
    vector<vector<vector<float> > > pcsAll;
    vector<float> mean;
    float max_norm;
};
}

template<typename DATATYPE>
vector<float> lshbox::MIPH<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin)
{
    // zero-centered first
    vector<float> domin_pc(pcsAll[k].size());
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        float norm = 0.0;
        for (unsigned j = 0; j != pcsAll[k][i].size()-1; ++j)
        {
            domin_pc[i] += (domin[j] - mean[j] )* pcsAll[k][i][j];
            norm += ( domin[j] - mean[j] ) * ( domin[j] - mean[j] );
        }
        if (max_norm>norm)
            domin_pc[i] += sqrt(max_norm-norm);

    }
    return domin_pc;
}

template<typename DATATYPE>
vector<bool> lshbox::MIPH<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin)
{
    vector<float> hashFloats = getHashFloats(k, domin);
    vector<bool> hashBits = this->quantization(hashFloats);
    return hashBits;
}

template<typename DATATYPE>
void lshbox::MIPH<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
    string line;
    // initialized statistics and model
    ifstream modelFin(modelFile.c_str());
    if (!modelFin) {
        std::cout << "cannot open file " << modelFile << std::endl;
        assert(false);
    }
    getline(modelFin, line);
    istringstream statIss(line);
    int numTables, tableDim, tableCodelen, tableNumItems, tableNumQueries;
    statIss >> numTables >> tableDim >> tableCodelen >> tableNumItems >> tableNumQueries >> max_norm;

    // mean and pcsAll
    mean.resize(tableDim);
    getline(modelFin, line);
    istringstream meanIss(line);
    for (int i = 0; i < mean.size(); ++i) {
        meanIss >> mean[i];
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

    // initialized numTotalItems and tables
    this->initBaseHasher(baseBitsFile, numTables, tableNumItems, tableCodelen);
}
