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

#define MATH_PI (3.1415926535897)

using std::vector;
using std::string;
using std::unordered_map;
using std::ifstream;
using std::istringstream;
namespace lshbox
{

    template<typename DATATYPE = float>
    class spectral: public Hasher<DATATYPE>
    {
    public:

        typedef typename Hasher<DATATYPE>::BIDTYPE BIDTYPE;

        spectral() : Hasher<DATATYPE>() {};

        vector<bool> getHashBits(unsigned k, const DATATYPE *domin) override;

        vector<float> getHashFloats(unsigned k, const DATATYPE *domin);

        void loadModel(const string& modelFile, const string& baseBitsFile);

    private:
        vector<vector<vector<float> > > pcsAll;
        vector<float > mean;
        vector<vector<float > > mn;
        vector<vector<float > > mx;
        vector<vector<vector<float > > > omegas;
        vector<vector<vector<float> > > modes;
        int nbits;

        void initOmegas();
    };
}

template<typename DATATYPE>
vector<float> lshbox::spectral<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin)
{
    // zero-centered first
    vector<float> domin_pc(pcsAll[k].size());
    // X = X*SHparam.pc;
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
        {
		domin_pc[i] += (domin[j] - mean[j] )* pcsAll[k][i][j];
            //	domin_pc[i] += (domin[j])* pcsAll[k][i][j];
        }
    }
    // X = X-repmat(SHparam.mn, [Nsamples 1]);
    for (int i = 0; i < domin_pc.size(); ++i) {
        domin_pc[i] -= this->mn[k][i];
    }

    vector<float > u(this->nbits);
    for (int i = 0; i < this->nbits; ++i) {
        vector<float >& omegai = this->omegas[k][i];
        float yi = 1.0f;
        float  ys = 1;
        for (int j = 0; j < omegai.size(); ++j) {
            ys *= sin(omegai[j]*domin_pc[j]+MATH_PI/2);
        }
        u[i] = ys;
    }

    return u;
}

template<typename DATATYPE>
void lshbox::spectral<DATATYPE>::initOmegas() {
    this->omegas = this->modes;

    for(size_t tableIndex = 0; tableIndex<this->omegas.size(); tableIndex++) {

        vector<vector<float> >& curPcs = this->pcsAll[tableIndex];
        vector<vector<float> >& curModes = this->modes[tableIndex];
        vector<vector<float> >& curOmegas = this->omegas[tableIndex];
        vector<float >& curMN = this->mn[tableIndex];
        vector<float >& curMX = this->mx[tableIndex];

        vector<float > deviation(this->nbits);

        for (int i = 0; i < this->nbits; ++i) {
            deviation[i] = MATH_PI / (curMX[i] - curMN[i]);
        }

        for (int row = 0; row < this->nbits; ++row) {
            for (int col = 0; col < this->nbits; ++col) {
                curOmegas[row][col] = curModes[row][col] * deviation[col];
            }
        }
    }
}

template<typename DATATYPE>
vector<bool> lshbox::spectral<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin)
{

    vector<float> hashFloats = getHashFloats(k, domin);
    vector<bool> hashBits = this->quantization(hashFloats);
    return hashBits;
}

template<typename DATATYPE>
void lshbox::spectral<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
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

    this->nbits = tableCodelen;

    // mean and pcsAll
    mean.resize(tableDim);
    getline(modelFin, line);
    istringstream meanIss(line);
    for (int i = 0; i < mean.size(); ++i) {
        meanIss >> mean[i];
    }

    this->pcsAll.resize(numTables);
    this->modes.resize(numTables);
    this->mn.resize(numTables);
    this->mx.resize(numTables);
    this->omegas.resize(numTables);

    for (size_t tableIndex=0; tableIndex<this->pcsAll.size(); ++tableIndex) {

        vector<vector<float> >& curPcs = this->pcsAll[tableIndex];
        vector<vector<float> >& curModes = this->modes[tableIndex];
        vector<float >& curMN = this->mn[tableIndex];
        vector<float >& curMX = this->mx[tableIndex];

        curPcs.resize(tableCodelen);
        curModes.resize(tableCodelen);
        curMN.resize(tableCodelen);
        curMX.resize(tableCodelen);

        for (auto& v : curPcs) {
            v.resize(tableDim);
        }
        for (int row = 0; row < tableDim; ++row) {
            getline(modelFin, line);
            istringstream iss(line);
            for (int cIndex = 0; cIndex < tableCodelen; ++cIndex) {
                iss >> curPcs[cIndex][row];
            }
        }

        for (auto& v : curModes) {
            v.resize(tableCodelen);
        }
        for (int modes = 0; modes < tableCodelen; ++modes) {
            getline(modelFin, line);
            istringstream iss(line);
            for (int cIndex = 0; cIndex < tableCodelen; ++cIndex) {
                iss >> curModes[modes][cIndex];
            }
        }

        {
            getline(modelFin, line);
            istringstream iss(line);
            for (int cIndex = 0; cIndex < tableCodelen; ++cIndex) {
                iss >> curMN[cIndex];
            }
        }

        {
            getline(modelFin, line);
            istringstream iss(line);
            for (int cIndex = 0; cIndex < tableCodelen; ++cIndex) {
                iss >> curMX[cIndex];
            }
        }

    }
    modelFin.close();

    this->initOmegas();

    // initialized numTotalItems and tables
    this->initBaseHasher(baseBitsFile, numTables, tableNumItems, tableCodelen);
}
