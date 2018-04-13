#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
#include <cmath>
#include <lshbox/utils.h>
#include "hasher.h"

using std::vector;
using std::string;
namespace lshbox
{

template<typename DATATYPE = float>
class KMH : public Hasher<DATATYPE>
{
public:

    KMH() : Hasher<DATATYPE>() {};

    vector<float> project(const DATATYPE *domin) const ;

    vector<float> getHashFloats(unsigned k, const DATATYPE *domin);

    vector<bool> getHashBits(unsigned k, const DATATYPE *domin) const override;

    virtual vector<bool> quantization(const vector<float>& hashFloats) const override;

    void loadModel(const string& modelFile, const string& baseBitsFile); 

private:
    int d, d_subspace, num_bits, num_bits_subspace, num_subspace, num_center;
    vector<vector<float> > R;
    vector<vector<vector<vector<float> > > > center_tables;
    vector<float> mean;
};

template<typename DATATYPE>
vector<float> KMH<DATATYPE>::project(const DATATYPE *domin) const {
    vector<float> projection(d);
    for (unsigned i = 0; i < d; ++i) {
        float tmp = domin[i] - mean[i];
        for (unsigned j = 0; j < d; ++j) {
            projection[j] += tmp * R[i][j];
        }
    }
    return projection;
}

// flipping cost of each bit: d(q, c_q_f) - d(q, c_q)
template<typename DATATYPE>
vector<float> KMH<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin) {
    vector<float> projection = project(domin);
    vector<bool> hashBits = getHashBits(k, projection.data());

    unsigned idx = 0;

    float d_q_cq = 0;
    for (int m = 0; m < num_subspace; ++m) {
        idx = 0;
        for (int i = num_bits_subspace - 1; i >= 0; --i) {
            idx <<= 1;
            idx += hashBits[m * num_bits_subspace + i];
        }
        vector<float>& center = center_tables[k][m][idx];
        for (int i = 0; i < d_subspace; ++i) {
            float diff = projection[m * d_subspace + i] - center[i];
            d_q_cq += diff * diff;
        }
    }
    d_q_cq = std::sqrt(d_q_cq);

    vector<float> hashFloats(num_bits);
    for (unsigned b = 0; b < num_bits; ++b) {
        hashBits[b] = (hashBits[b] == 0 ? 1 : 0);
        for (int m = 0; m < num_subspace; ++m) {
            idx = 0;
            for (int i = num_bits_subspace - 1; i >= 0; --i) {
                idx <<= 1;
                idx += hashBits[m * num_bits_subspace + i];
            }
            vector<float>& center = center_tables[k][m][idx];
            for (int i = 0; i < d_subspace; ++i) {
                float diff = projection[m * d_subspace + i] - center[i];
                hashFloats[b] += diff * diff;
            }
        }
        hashBits[b] = (hashBits[b] == 0 ? 1 : 0);
        hashFloats[b] = std::sqrt(hashFloats[b]) - d_q_cq;
    }
    return hashFloats;
}

template<typename DATATYPE>
vector<bool> KMH<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin) const {
    vector<float> projection = project(domin);
    vector<bool> hashBits(num_bits);

    const vector<vector<vector<float> > >& cur_center_tables = center_tables[k];

    for (int m = 0; m < num_subspace; ++m) {
        float min_dist_sqr = -1;
        int min_idx = -1;
        for (int i = 0; i < num_center; ++i) {
            float dist_sqr = 0;
            for (int j = 0; j < d_subspace; ++j) {
                float diff = projection[m * d_subspace + j] - cur_center_tables[m][i][j];
                dist_sqr += diff * diff;
            }
            if (min_idx == -1 || dist_sqr < min_dist_sqr) {
                min_dist_sqr = dist_sqr;
                min_idx = i;
            }
        }
        for (int i = 0; i < num_bits_subspace; ++i) {
            hashBits[m * num_bits_subspace + i] = (min_idx % 2);
            min_idx /= 2;
        }
    }
    return hashBits;
}

template<typename DATATYPE>
vector<bool> KMH<DATATYPE>::quantization(const vector<float>& hashFloats) const {
    std::cout << "quantization not implemented for KMH";
    assert(false);
}

template<typename DATATYPE>
void KMH<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile) {
    ifstream modelFin(modelFile);
    if (!modelFin) {
        std::cout << "cannot open file " << modelFile << std::endl;
        assert(false);
    }

    int n;
    modelFin >> n >> d >> num_bits >> num_bits_subspace;
    num_subspace = num_bits / num_bits_subspace;
    int num_table = 1;
    num_center = (1 << num_bits_subspace);
    d_subspace = d / num_subspace;

    mean.resize(d);
    for (int i = 0; i < d; ++i) {
        modelFin >> mean[i];
    }

    center_tables.resize(1);

    for (int tb = 0; tb < num_table; ++tb) {
        vector<vector<vector<float> > >& cur_center_tables = center_tables[tb];

        cur_center_tables.resize(num_subspace);
        for (int m = 0; m < num_subspace; ++m) {
            cur_center_tables[m].resize(num_center);
            for (int i = 0; i < num_center; ++i) {
                cur_center_tables[m][i].resize(d_subspace);
                for (int j = 0; j < d_subspace; ++j) {
                    modelFin >> cur_center_tables[m][i][j];
                }
            }
        }
    }

    R.resize(d);
    for (int i = 0; i < d; ++i) {
        R[i].resize(d);
        for (int j = 0; j < d; ++j) {
            modelFin >> R[i][j];
        }
    }

    modelFin.close();

    // initialized numTotalItems and tables
    this->initBaseHasher(baseBitsFile, num_table, n, num_bits);
}
}
