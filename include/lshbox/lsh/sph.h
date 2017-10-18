#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
#include <cmath>
#include <lshbox/utils.h>
#include "probing.h"
#include "hasher.h"

using std::vector;
using std::string;
namespace lshbox
{

template<typename DATATYPE = float>
class SpH : public Hasher<DATATYPE>
{
public:

    SpH() : Hasher<DATATYPE>() {};

    vector<float> getHashFloats(unsigned k, const DATATYPE *domin);

    vector<bool> getHashBits(unsigned k, const DATATYPE *domin) override;

    void loadModel(const string& modelFile, const string& baseBitsFile); 

private:
    std::vector<std::vector<std::vector<DATATYPE>>> pivots;  // L hash tabels, c pivots, each with d dimensions
    std::vector<std::vector<float>> thresholds;
};
template<typename DATATYPE>
vector<float> SpH<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin)
{
    std::vector<float> hashFloats;
    hashFloats.resize(pivots[k].size());
    for (unsigned i = 0; i < hashFloats.size(); ++i) {
        // hashFloats[i] equals to two norm distance to pivot[q];
        hashFloats[i] = 0;
        for (unsigned idx = 0; idx < pivots[k][i].size(); ++idx) {
            hashFloats[i] += (pivots[k][i][idx] - domin[idx]) * (pivots[k][i][idx] - domin[idx]);
        }
        // hashFloats[i] = sqrt(hashFloats[i]);
    }

    for (int i = 0; i < hashFloats.size(); ++i) {
        hashFloats[i] = hashFloats[i] -  thresholds[k][i];
    }
    return hashFloats;
}

template<typename DATATYPE>
vector<bool> SpH<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin) {
    std::vector<float> hashFloats = getHashFloats(k, domin);
    std::vector<bool> hashBits = this->quantization(hashFloats);
    return hashBits;
}
template<typename DATATYPE>
void SpH<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
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
    statIss >> numTables >> tableDim >> tableCodelen >> tableNumItems >> tableNumQueries;

    // mean, pcsAll and rotateAll

    this->pivots.resize(numTables);
    this->thresholds.resize(numTables);
    for (int tb = 0; tb < numTables; ++tb) {
        auto& curPvt = this->pivots[tb];
        curPvt.resize(tableCodelen);
        for (auto& v : curPvt) {
            v.resize(tableDim);
        }
        for (int row = 0; row < tableCodelen; ++row) {
            getline(modelFin, line);
            istringstream iss(line);
            for (int cIdx = 0; cIdx < tableDim; ++cIdx) {
                iss >> curPvt[row][cIdx];
            }
        }

        auto& curThres = this->thresholds[tb];
        curThres.resize(tableCodelen);
        for (int row = 0; row < tableCodelen; ++row) {
            getline(modelFin, line);
            istringstream iss(line);
            iss >> curThres[row];
        }
    }
    modelFin.close();

    // initialized numTotalItems and tables
    this->initBaseHasher(baseBitsFile, numTables, tableNumItems, tableCodelen);
}
}
