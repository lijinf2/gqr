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
class LMIP: public Hasher<DATATYPE>
{

private:

    inline float calculateNorm(const DATATYPE *domin) {
        float normSquare = 0.0;
        for (int i = 0; i < mean.size(); ++i) {
            normSquare += domin[i] * domin[i];
        }
        return normSquare;
    }

    /**
     *    find the index of the first item bigger than item's norm
     *    if no bigger item find, then item = normPrctile.size()
     *    so item length = 1,2,normPrctile.size()-1=normIntevalCount, 
     *    then shift length to 0,1,2,normPrctile.size()-2=normIntevalCount-1
     *    start with index=1, cause normPrctile[0] is the minist norm of train data
     */
    inline unsigned findPrctile(const DATATYPE *domin) {
        float norm = calculateNorm(domin);
        int normPrctileIndex;
        for (normPrctileIndex = 1; normPrctileIndex < normPrctile.size()-1; ++normPrctileIndex) {
            if (normPrctile[normPrctileIndex] >= norm) {
                break;
            }
        }
        // shift length to 0,1,2,normPrctile.size()-2=normIntevalCount-1
        normPrctileIndex--;

        assert(normPrctile.size()-2==normIntevalCount-1);
        assert(normPrctileIndex<=normIntervalCount-1 && normPrctileIndex>=0);
        
        return normPrctileIndex;
    }

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

    vector<float> domin_pc(mean.size() + lengthBitsCount);

    // zero-centered first
    for (unsigned i = 0; i != pcsAll[k].size(); ++i) {
        for (unsigned j = 0; j != pcsAll[k][i].size(); ++j) {
            domin_pc[i] += (domin[j] - mean[j] )* pcsAll[k][i][j];
        }
    }

    // determine the prctile 
    unsigned normPrctileIndex = findPrctile(domin);
    // shift length to 0,1,2,..tableCodelen-1>=normIntervalCount-1
    int currentLength = normPrctileIndex + tableCodelen - normIntervalCount;

    assert(currentLength<=tableCodelen-1 && currentLength>=tableCodelen-normIntervalCount);

    for (int i = 0; i < lengthBitsCount; ++i) {
        domin_pc[ mean.size() - i -1 ] = ( currentLength & (1<<i) ) ? 1 : 0;
    }

//     std::cout << "-------------------------" << std::endl;
//     float normDomin = calculateNorm(domin);
//     std::cout << "  -- norm : " << normDomin << std::endl;
//     std::cout << "  -- indx : " << normPrctileIndex << std::endl;
//     std::cout << "  -- leng : " ;
//     for (int i = lengthBitsCount-1; i != -1; --i) {
//         std::cout << " " << domin_pc[ mean.size() - i -1 ];
//     }
//     std::cout << std::endl << "-------------------------" << std::endl;

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
    this->initBaseHasher(baseBitsFile, numTables, tableNumItems, tableCodelen);
}
