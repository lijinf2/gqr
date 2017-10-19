#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
#include <cmath>
#include <lshbox/utils.h>
#include "hasher.h"
#include "pcarr.h"

using std::vector;
using std::string;
namespace lshbox
{

template<typename DATATYPE = float>
class ITQ : public PCARR<DATATYPE>
{
public:
    ITQ() : PCARR<DATATYPE>() {};
};
// template<typename DATATYPE = float>
// class ITQ : public Hasher<DATATYPE>
// {
// public:
//     ITQ() : PCARR<DATATYPE>() {};
//
//     vector<float> getHashFloats(unsigned k, const DATATYPE *domin);
//
//     vector<bool> getHashBits(unsigned k, const DATATYPE *domin) override;
//
//     void loadModel(const string& modelFile, const string& baseBitsFile); 
//
// private:
//     vector<vector<vector<float> > > pcsAll;
//     vector<vector<vector<float> > > rotateAll;
//     vector<float> mean;
// };
// template<typename DATATYPE>
// vector<float> ITQ<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin)
// {
//     vector<float> domin_pc(pcsAll[k].size());
//     for (unsigned i = 0; i != domin_pc.size(); ++i)
//     {
//         for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
//         {
//             domin_pc[i] += (domin[j] - mean[j]) * pcsAll[k][i][j];
//         }
//     }
//
//     vector<float> hashFloats;
//     hashFloats.resize(domin_pc.size());
//     for (unsigned i = 0; i != domin_pc.size(); ++i)
//     {
//         float product = 0;
//         for (unsigned j = 0; j != rotateAll[k][i].size(); ++j)
//         {
//             product += float(domin_pc[j] * rotateAll[k][i][j]);
//         }
//         hashFloats[i] = product;
//     }
//     return hashFloats;
// }
//
// template<typename DATATYPE>
// vector<bool> ITQ<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin) {
//     vector<float> hashFloats = getHashFloats(k, domin);
//     vector<bool> hashBits = this->quantization(hashFloats);
//     return hashBits;
// }
// template<typename DATATYPE>
// void ITQ<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
//     string line;
//     // initialized statistics and model
//     ifstream modelFin(modelFile.c_str());
//     if (!modelFin) {
//         std::cout << "cannot open file " << modelFile << std::endl;
//         assert(false);
//     }
//     getline(modelFin, line);
//     istringstream statIss(line);
//     int numTables, tableDim, tableCodelen, tableNumItems, tableNumQueries;
//     statIss >> numTables >> tableDim >> tableCodelen >> tableNumItems >> tableNumQueries;
//
//     // mean, pcsAll and rotateAll
//     mean.resize(tableDim);
//     getline(modelFin, line);
//     istringstream meanIss(line);
//     for (int i = 0; i < mean.size(); ++i) {
//         meanIss >> mean[i];
//     }
//
//     this->pcsAll.resize(numTables);
//     this->rotateAll.resize(numTables);
//     for (int tb = 0; tb < numTables; ++tb) {
//         auto& curPcs = pcsAll[tb];
//         curPcs.resize(tableCodelen);
//         for (auto& v : curPcs) {
//             v.resize(tableDim);
//         }
//         for (int row = 0; row < tableDim; ++row) {
//             getline(modelFin, line);
//             istringstream iss(line);
//             for (int cIdx = 0; cIdx < tableCodelen; ++cIdx) {
//                 iss >> curPcs[cIdx][row];
//             }
//         }
//
//         auto& curRotate = rotateAll[tb];
//         curRotate.resize(tableCodelen);
//         for (auto& v : curRotate) {
//             v.resize(tableCodelen);
//         }
//         for (int row = 0; row < tableCodelen; ++row) {
//             getline(modelFin, line);
//             istringstream iss(line);
//             for (int cIdx = 0; cIdx < tableCodelen; ++cIdx) {
//                 iss >> curRotate[cIdx][row];
//             }
//         }
//     }
//     modelFin.close();
//
//     // initialized numTotalItems and tables
//     this->initBaseHasher(baseBitsFile, numTables, tableNumItems, tableCodelen);
// }
}
