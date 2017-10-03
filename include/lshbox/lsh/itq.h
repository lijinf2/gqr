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
class ITQ : public Hasher <DATATYPE>
{
public:

    ITQ() : Hasher<DATATYPE>() {};

    vector<float> getHashFloats(unsigned k, const DATATYPE *domin);

    vector<bool> quantization(const vector<float>& hashFloats);

    vector<bool> getHashBits(unsigned k, const DATATYPE *domin) override;

    void loadModel(const string& fileName) override; 

private:
    vector<vector<vector<float> > > pcsAll;
    vector<vector<vector<float> > > omegasAll;
};
template<typename DATATYPE>
vector<float> ITQ<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin)
{
    vector<float> domin_pc(pcsAll[k].size());
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
        {
            domin_pc[i] += domin[j] * pcsAll[k][i][j];
        }
    }

    vector<float> hashFloats;
    hashFloats.resize(domin_pc.size());
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        float product = 0;
        for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
        {
            product += float(domin_pc[j] * omegasAll[k][i][j]);
        }
        hashFloats[i] = product;
    }
    return hashFloats;
}
template<typename DATATYPE>
vector<bool> ITQ<DATATYPE>::quantization(const vector<float>& hashFloats)
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
template<typename DATATYPE>
vector<bool> ITQ<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin) {
    vector<float> hashFloats = getHashFloats(k, domin);
    vector<bool> hashBits = quantization(hashFloats);
    return hashBits;
}
template<typename DATATYPE>
void ITQ<DATATYPE>::loadModel(const string& fileName) {
}
}
