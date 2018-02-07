#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <base/basehasher.h>
#include "intcode/hash/e2lsh.h"
#include <assert.h>

using std::vector;
using std::unordered_map;
using std::string;
using std::ifstream;
using std::istringstream;

namespace lshbox {

    template<typename DATATYPE = float>
    class ALSH: public lshbox::E2LSH<DATATYPE>{
    protected:
        typedef vector<int> BIDTYPE;
        /**
         * U = 0.83 default.
         * for train data, we scale the train data to make their max-norm no greater than U (implement in train module).
         *
         */
        float U;
        /**
         * m = 3 default.
         * for train data, we add m term into origin data, for i=1,..m , the I'th added term equals: ||x||^(2^i)
         * for query data, we add m term (each term equals 0.5) into origin test data.
         * (notice that x is already scale to {@link this->U} )
         * x  ---scale to U--> x[1] x[2] ... x[dim-1]
         *    ---add m term--> x[1] x[2] ... x[dim-1] ||x||^(2) ||x||^(4) ||x||^(8) ... ||x||^(2^m)
         * q  ---scale to 1--> q[1] q[2] ... q[dim-1]
         *    ---add m term--> q[1] q[2] ... q[dim-1] 0.5       0.5       0.5       ... 0.5
         *
         * implement in train module, eg: MATLAB.
         */
        int m;

        DATATYPE calculateNormSquare(const DATATYPE* data, unsigned long dimension);


        DATATYPE calculateNorm(const DATATYPE* data, unsigned long dimension);

        /**
         * data[i] = data[i] / norm(data) * targetNorm;
         * which make sure that: norm(data) == targetNorm^2
         * @return a new vector stored normalized data, without change origin data.
         */
        vector<DATATYPE > scale(const DATATYPE* data, unsigned long dimension, DATATYPE targetNorm);
    public:
        void loadModel(const string& modelFile, const string& baseBitsFile) override;
        vector<float> getHashFloats(unsigned k, const DATATYPE *domin) override ;
    };


    template<typename DATATYPE>
    void ALSH<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
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

        // load m and U
        getline(modelFin, line);
        istringstream parameterIss(line);
        parameterIss >> this->m;
        parameterIss >> this->U;

        // mean
        this->loadFloatVector(modelFin, modelNumFeature).swap(this->mean);
        for (unsigned i = 0; i < this->m; ++i) {
            this->mean.push_back(0);
        }

        // hash functions
        this->pcsAll.resize(modelNumTable);
        this->shift.resize(modelNumTable);
        for (int tb = 0; tb < modelNumTable; ++tb) {
            this->loadFloatMatrixTranspose(modelFin, modelNumFeature+m, modelCodelen).swap(this->pcsAll[tb]);
            this->loadFloatVector(modelFin, modelCodelen).swap(this->shift[tb]);
        }

        // initialized numTotalItems and tables
        this->initBaseHasher(baseBitsFile, modelNumTable, modelNumItem, modelCodelen);
    }


    template<typename DATATYPE>
    inline DATATYPE ALSH<DATATYPE>::calculateNormSquare(const DATATYPE* data, unsigned long dimension) {
        DATATYPE normSquare = 0.0;
        for (int i = 0; i < dimension; ++i) {
            normSquare += data[i] * data[i];
        }
        return normSquare;
    }

    template<typename DATATYPE>
    inline DATATYPE ALSH<DATATYPE>::calculateNorm(const DATATYPE* data, unsigned long dimension) {
        return std::sqrt(calculateNormSquare(data, dimension));
    }

    template<typename DATATYPE>
    vector<DATATYPE > ALSH<DATATYPE>::scale(const DATATYPE* data, unsigned long dimension,  DATATYPE targetNorm) {

        vector<DATATYPE > result(dimension);
        DATATYPE norm = this->calculateNorm(data, dimension);
        for (int i = 0; i < dimension; ++i) {
            result[i] = data[i] / norm * targetNorm;
        }
        return result;
    }

    template<typename DATATYPE>
    vector<float> ALSH<DATATYPE>::getHashFloats(unsigned tableIdx, const DATATYPE *data)
    {

        //scale to U
        vector<DATATYPE> normalizedData = this->scale(data, this->mean.size() - this->m, 1.0f);
        // transform
        for (unsigned i = 0; i < this->m; ++i) {
            normalizedData.push_back(0.5);
        }

        // project
        vector<float> projVector = this->getProjection(normalizedData.data(), this->pcsAll[tableIdx], this->mean);

        // shift and chop
        for (int i = 0; i < projVector.size(); ++i) {
            projVector[i] += this->shift[tableIdx][i];
            projVector[i] /= this->W;
        }
        return projVector;
    }

};
