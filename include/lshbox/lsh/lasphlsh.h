//////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2014 Gefu Tang <tanggefu@gmail.com>. All Rights Reserved.
///
/// This file is part of LSHBOX.
///
/// LSHBOX is free software: you can redistribute it and/or modify it under
/// the terms of the GNU General Public License as published by the Free
/// Software Foundation, either version 3 of the License, or(at your option)
/// any later version.
///
/// LSHBOX is distributed in the hope that it will be useful, but WITHOUT
/// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
/// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
/// more details.
///
/// You should have received a copy of the GNU General Public License along
/// with LSHBOX. If not, see <http://www.gnu.org/licenses/>.
///
/// @version 0.1
/// @author Gefu Tang & Zhifeng Xiao
/// @date 2014.6.30
//////////////////////////////////////////////////////////////////////////////

/**
 * @file newitqlsh.h
 *
 * @brief Locality-Sensitive Hashing Scheme Based on Iterative Quantization.
 */
#pragma once
#include <map>
#include <vector>
#include <random>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <eigen/Eigen/Dense>
#include <cmath>
#include "probing.h"
#include <lshbox/utils.h>

namespace lshbox
{

// select num items uniformaly from source to destination
float calSquareDist(const std::vector<float>& a, const std::vector<float>& b) {
    assert(a.size() == b.size());
    float dist = 0;
    for (unsigned i = 0; i < a.size(); ++i) {
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return dist;
}
void selectData( 
    const Matrix<float> &source,
    std::vector<std::vector<float>>& destination,
    unsigned num) {

    assert(num <= source.getSize());

    std::vector<bool> selected = selection(source.getSize(), num);
    std::vector<unsigned> seqs;
    seqs.reserve(num);
    for (unsigned idx = 0; idx < selected.size(); ++idx) {
        if (selected[idx]) {
            seqs.push_back(idx);
        }
    }
    assert(seqs.size() == num);

    destination.resize(num);

    for (unsigned row = 0; row != destination.size(); ++row)
    {
        destination[row].resize(source.getDim());
        for (unsigned col = 0; col != destination[row].size(); ++col)
        {
            destination[row][col] = source[seqs[row]][col];
        }
    }
}

template<typename T>
T findKthLargest(std::vector<T>& nums, unsigned k) {
    assert(nums.size() >= k);
    std::vector<T> subarray1;
    std::vector<T> subarray2;
    T pivot = nums[nums.size() / 2];

    for (int i = 0; i < nums.size(); ++i) {
        if (nums[i] > pivot) subarray1.push_back(nums[i]);
        else if (nums[i] < pivot) subarray2.push_back(nums[i]);
    }

    int numPivots = nums.size() - subarray1.size() - subarray2.size();

    if (subarray1.size() >= k) {
        return findKthLargest(subarray1, k);
    }
    else if (subarray1.size() + numPivots >= k) {
        return pivot;
    }
    else {
        return findKthLargest(subarray2, k - subarray1.size() - numPivots);
    }
}

template<typename DATATYPE = float>
class laSphLsh 
{
public:
    typedef unsigned long long BIDTYPE;
    struct Parameter
    {
        /// Hash table size
        unsigned M = 0;
        /// Number of hash tables
        unsigned L = 0;
        /// Dimension of the vector, it can be obtained from the instance of Matrix
        unsigned D = 0;
        /// Binary code bytes
        unsigned N = 0;
        /// Size of vectors in train
        unsigned S = 0;
        /// Training iterations
        unsigned I = 0;
    };
    laSphLsh() {}
    laSphLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~laSphLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of laSphLsh<DATATYPE>::Parametor, which contains
     * the necessary parameters
     */
    void reset(const Parameter &param_);

    static void updateThresholds(
        const std::vector<std::vector<float>> &data,
        std::vector<std::vector<float> >* pivotPointer,
        std::vector<float>* thresPointer);

    static std::vector<std::vector<unsigned>> calOIJ(
        const std::vector<std::vector<float>> &data,
        std::vector<std::vector<float> >* pivotPointer,
        std::vector<float>* thresPointer);

    static bool trainSingleTable( 
        const Matrix<DATATYPE> &data,
        std::vector<std::vector<float> >* pivotPointer,
        std::vector<float>* thresPointer,
        Parameter param);

    void trainAll(const Matrix<DATATYPE> &data, unsigned batchSize);
    /**
     * Hash the dataset.
     *
     * @param data A instance of Matrix<DATATYPE>, it is the search dataset.
     */
    void hash(Matrix<DATATYPE> &data);
    /**
     * Insert a vector to the index.
     *
     * @param key   The sequence number of vector
     * @param domin The pointer to the vector
     */
    void insert(unsigned key, const DATATYPE *domin);
    /**
     * probe bucket
     * @param t table to probe
     * @param bucketId bucket to probe
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     * @return number of item probed
     */
    template<typename PROBER>
    int probe(unsigned t, BIDTYPE bucketId, PROBER &prober);
    /**
     * get the hash value of a vector.
     *
     * @param k     The idx of the table
     * @param domin The pointer to the vector
     * @return      The hash value
     */
    BIDTYPE getHashVal(unsigned k, const DATATYPE *domin);
    /**
     * Load the index from binary file.
     *
     * @param file The path of binary file.
     */
    void load(const std::string &file);
    /**
     * Save the index as binary file.
     *
     * @param file The path of binary file.
     */
    void save(const std::string &file);
    /**
     * Added by Jinfeng
     * getHashFloats for of a vector, i.e. projection
     *
     * @param k     The idx of the table
     * @param domin The pointer to the vector
     * @return      The hash value
     */
    std::vector<float> getHashFloats(unsigned k, const DATATYPE *domin);
    /**
     * Added by Jinfeng
     * get quantization of a vector, i.e. quantization
     *
     * @param k     The idx of the table
     * @param domin The pointer to the vector
     * @return      The hash value
     */
    std::vector<bool> quantization(const std::vector<float>& hashFloats);
    /**
     * Added by Jinfeng
     * getHashBits for of a vector, i.e. projection + quantization
     *
     * @param k     The idx of the table
     * @param domin The pointer to the vector
     * @return      The hash value
     */
    std::vector<bool> getHashBits(unsigned k, const DATATYPE *domin);
    /**
     * get all the buckets.
     */
    int getTableSize();
    int getMaxBucketSize();

    /**
     * ranking hash code to query the approximate nearest neighborholds.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     * @param numItem number of buckets to return
     * */
    template<typename PROBER>
    void KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems);

    Parameter param;
    std::vector<std::unordered_map<BIDTYPE, std::vector<unsigned> > > tables;

private:
    std::vector<std::vector<std::vector<DATATYPE>>> pivots;  // L hash tabels, c pivots, each with d dimensions
    std::vector<std::vector<float>> thresholds;
};
}

// ------------------------- implementation -------------------------
template<typename DATATYPE>
void lshbox::laSphLsh<DATATYPE>::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    pivots.resize(param.L);
    for (auto& pivot : pivots) {
        pivot.resize(param.N);
        for (auto& e : pivot) {
            e.resize(param.D);
        }
    }

    thresholds.resize(param.L);
    for (auto& thres : thresholds) {
        thres.resize(param.N);
    }
}

template<typename DATATYPE>
void lshbox::laSphLsh<DATATYPE>::updateThresholds(
    const std::vector<std::vector<float>> &data,
    std::vector<std::vector<float> >* pivotPointer,
    std::vector<float>* thresPointer) {

    assert(pivotPointer->size() != 0);
    assert(pivotPointer->size() == thresPointer->size());

    // update thresPointer
    for (unsigned idx = 0; idx < thresPointer->size(); ++idx) {
        const std::vector<float>& pivot = (*pivotPointer)[idx];

        // scan data and cal distances
        std::vector<float> dists; // this is square distance
        dists.reserve(data.size());
        for (auto record : data) {
            dists.push_back(calSquareDist(record, pivot));
        }
        // find middle value of dists and set to thresPointer
        (*thresPointer)[idx] = findKthLargest(dists, data.size() / 2);
    }
}

template<typename DATATYPE>
std::vector<std::vector<unsigned>> lshbox::laSphLsh<DATATYPE>::calOIJ(
    const std::vector<std::vector<float>> &data,
    std::vector<std::vector<float> >* pivotPointer,
    std::vector<float>* thresPointer) {

    assert(pivotPointer->size() != 0);
    assert(pivotPointer->size() == thresPointer->size());

    unsigned numBits = pivotPointer->size();

    std::vector<std::vector<unsigned>> result;
    result.resize(numBits);
    for (unsigned i = 0; i < numBits; ++i) {
        result[i].resize(numBits);
        for (unsigned j = 0; j < result[i].size(); ++j) {
            result[i][j] = 0;
        }
    }

    // for each record
    for (const auto& record : data) {
        std::vector<bool> hashBits(pivotPointer->size());

        // cal hashBits
        for (unsigned bIdx = 0; bIdx < hashBits.size(); ++bIdx) {
            const auto& pivot = (*pivotPointer)[bIdx];
            float dist = 0;
            for (unsigned index = 0; index < record.size(); ++index) {
                dist += (record[index] - pivot[index]) * (record[index] - pivot[index]);
            }
            if (dist > (*thresPointer)[bIdx]) 
                hashBits[bIdx] = 1;
            else
                hashBits[bIdx] = 0;
        }

        // analyze hashBits
        for (unsigned i = 0; i < hashBits.size() - 1; ++i) {
            for (unsigned j = i + 1; j < hashBits.size(); ++j) {
                if (hashBits[i] && hashBits[j]) {
                    result[i][j]++;
                }
            }
        }
    }
    
    return result;
}

template<typename DATATYPE>
bool lshbox::laSphLsh<DATATYPE>::trainSingleTable(
    const Matrix<DATATYPE> &data,
    std::vector<std::vector<float> >* pivotPointer,
    std::vector<float>* thresPointer,
    Parameter param)
{
    // select training set;
    std::vector<std::vector<float>> trainSet;
    selectData(data, trainSet, param.S);

    // select pivots;
    selectData(data, (*pivotPointer), param.N);

    // update Threshold
    updateThresholds(trainSet, pivotPointer, thresPointer);

    std::vector<std::vector<unsigned>> OIJ = calOIJ(trainSet, pivotPointer, thresPointer);
    unsigned iteration = 0;
    while (iteration < param.I) {
        std::cout << "iteration: " << iteration << std::endl;
        iteration++;
        std::vector<std::vector<std::vector<float>>> FVecs;

        // initilize fij
        FVecs.resize(param.N);
        for (auto& row : FVecs) {
            row.resize(param.N);
            for (auto& col : row) {
                col.resize(param.D);
                for (auto& e : col) {
                    e = 0;
                }
            }
        }

        // cal fij
        for (unsigned i = 0; i < param.N - 1; ++i) {
            std::vector<float>& pivotI = (*pivotPointer)[i];

            for (unsigned j = i + 1; j < param.N; ++j) {
                FVecs[i][j] = pivotI;

                for (unsigned idx = 0; idx < FVecs[i][j].size(); ++idx) {
                    FVecs[i][j][idx] -= (*pivotPointer)[j][idx];
                }
                float coefficient = (OIJ[i][j] - param.S / 4.0) / (param.S / 4.0) / 2;
                for (unsigned idx = 0; idx < FVecs[i][j].size(); ++idx) {
                    FVecs[i][j][idx] *= coefficient;
                    FVecs[j][i][idx] = -FVecs[i][j][idx];
                }

            }
        }

        // update pivots
        for (unsigned i = 0; i < param.N; ++i) {
            std::vector<float>& pivot = (*pivotPointer)[i];
            // cal sum
            for (unsigned j = 0; j < param.N; ++j) {
                for (unsigned index = 0; index < pivot.size(); ++index) {
                    pivot[index] += FVecs[i][j][index];
                }
            }
        }
        updateThresholds(trainSet, pivotPointer, thresPointer);
        OIJ = calOIJ(trainSet, pivotPointer, thresPointer);

    }
        return true;
}

template<typename DATATYPE>
void lshbox::laSphLsh<DATATYPE>::trainAll(const Matrix<DATATYPE> &data, unsigned batchSize){

    // use loop
    unsigned numBatches = param.L / batchSize;
    for (unsigned batch = 0; batch < numBatches; ++batch) {
        std::vector<std::thread> threads;
        threads.resize(batchSize);

        unsigned startk = batch * batchSize;
        for (unsigned i = 0; i < threads.size(); ++i) {
            unsigned tableId = startk + i;
            threads[i] = std::thread(
                    trainSingleTable, 
                    data, &pivots[tableId], &thresholds[tableId], param);
            std::cout << "table " + std::to_string(tableId) + " added!\n" << std::endl;
        }

        for (unsigned i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }
        std::cout << "batch " + std::to_string(batch) + " finished!\n" << std::endl;
    }

    unsigned remaining = param.L % batchSize;
    std::vector<std::thread> threads;
    threads.resize(remaining);

    unsigned startk = numBatches * batchSize;
    for (unsigned i = 0; i < threads.size(); ++i) {
        unsigned tableId = startk + i;
        threads[i] = std::thread(
                trainSingleTable, 
                data, &pivots[tableId], &thresholds[tableId], param);
        std::cout << "table " + std::to_string(tableId) + " added!\n" << std::endl;
    }

    for (unsigned i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
    std::cout << "last batch finished\n " << std::endl;

}

template<typename DATATYPE>
void lshbox::laSphLsh<DATATYPE>::hash(Matrix<DATATYPE> &data)
{
    progress_display pd(data.getSize());
    for (unsigned i = 0; i != data.getSize(); ++i)
    {
        insert(i, data[i]);
        ++pd;
    }
}
template<typename DATATYPE>
void lshbox::laSphLsh<DATATYPE>::insert(unsigned key, const DATATYPE *domin)
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        BIDTYPE hashVal = getHashVal(k, domin);
        tables[k][hashVal].push_back(key);
    }
}
template<typename DATATYPE>
template<typename PROBER>
int lshbox::laSphLsh<DATATYPE>::probe(unsigned t, BIDTYPE bucketId, PROBER& prober)
{
    int numProbed = 0;
    if (tables[t].find(bucketId) != tables[t].end())
    {
        numProbed = tables[t][bucketId].size();
        for (std::vector<unsigned>::iterator iter = tables[t][bucketId].begin(); iter != tables[t][bucketId].end(); ++iter)
        {
            prober(*iter);
        }
    }
    
    return numProbed;
}
template<typename DATATYPE>
typename lshbox::laSphLsh<DATATYPE>::BIDTYPE lshbox::laSphLsh<DATATYPE>::getHashVal(unsigned k, const DATATYPE *domin)
{

    // c thresholds to get hash value
    BIDTYPE hashVal = 0;
    std::vector<bool> hashbits = getHashBits(k, domin);
    for (unsigned i = 0; i != hashbits.size(); ++i)
    {
        hashVal <<= 1; // hashVal *= 2
        if (hashbits[i])
        {
            hashVal += 1;
        }
    }
    return hashVal;
}
template<typename DATATYPE>
void lshbox::laSphLsh<DATATYPE>::load(const std::string &file)
{
    std::ifstream in(file, std::ios::binary);
    if (!in) {
        std::cout << "cannot open file" << file << std::endl;
        exit(0);
    }
    in.read((char *)&param.M, sizeof(unsigned));
    in.read((char *)&param.L, sizeof(unsigned));
    in.read((char *)&param.D, sizeof(unsigned));
    in.read((char *)&param.N, sizeof(unsigned));
    in.read((char *)&param.S, sizeof(unsigned));
    tables.resize(param.L);
    pivots.resize(param.L);
    thresholds.resize(param.L);
    for (unsigned i = 0; i != param.L; ++i)
    {
        unsigned count;
        in.read((char *)&count, sizeof(unsigned));
        for (unsigned j = 0; j != count; ++j)
        {
            BIDTYPE target;
            in.read((char *)&target, sizeof(BIDTYPE));

            // unsigned fourByteTmp;
            // in.read((char *)&fourByteTmp, sizeof(fourByteTmp));
            // BIDTYPE target;
            // target = fourByteTmp;

            unsigned length;
            in.read((char *)&length, sizeof(unsigned));
            tables[i][target].resize(length);
            in.read((char *) & (tables[i][target][0]), sizeof(unsigned) * length);
        }
        pivots[i].resize(param.N);
        thresholds[i].resize(param.N);
        for (unsigned j = 0; j != param.N; ++j)
        {
            pivots[i][j].resize(param.D);
            in.read((char *)&pivots[i][j][0], sizeof(float) * param.D);
        }
        in.read((char *)&thresholds[i][0], sizeof(float) * param.N);

    }
    in.close();
}
template<typename DATATYPE>
void lshbox::laSphLsh<DATATYPE>::save(const std::string &file)
{
    std::ofstream out(file, std::ios::binary);
    out.write((char *)&param.M, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.L, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.D, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.N, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.S, sizeof(unsigned)); // 4 bytes
    for (int i = 0; i != param.L; ++i)
    {
        // out.write((char *)&rndArray[i][0], sizeof(unsigned) * param.N);  // 4 * N bytes
        unsigned count = unsigned(tables[i].size());
        out.write((char *)&count, sizeof(unsigned));
        for (std::unordered_map<BIDTYPE, std::vector<unsigned> >::iterator iter = tables[i].begin(); iter != tables[i].end(); ++iter)
        {
            BIDTYPE target = iter->first;
            out.write((char *)&target, sizeof(BIDTYPE));
            unsigned length = unsigned(iter->second.size());
            out.write((char *)&length, sizeof(unsigned));
            out.write((char *) & ((iter->second)[0]), sizeof(unsigned) * length);
        }
        for (unsigned j = 0; j != param.N; ++j)
        {
            out.write((char *)&pivots[i][j][0], sizeof(float) * param.D);
        }
        out.write((char *)&thresholds[i][0], sizeof(float) * param.N);
    }
    out.close();
}

template<typename DATATYPE>
std::vector<float> lshbox::laSphLsh<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin)
{
    std::vector<float> hashFloats;
    hashFloats.resize(pivots[k].size());
    for (unsigned i = 0; i < hashFloats.size(); ++i) {
        // hashFloats[i] equals to two norm distance to pivot[q];
        hashFloats[i] = 0;
        for (unsigned idx = 0; idx < pivots[k][i].size(); ++idx) {
            hashFloats[i] += (pivots[k][i][idx] - domin[idx]) * (pivots[k][i][idx] - domin[idx]);
        }
    }

    for (int i = 0; i < hashFloats.size(); ++i) {
        hashFloats[i] = hashFloats[i] -  thresholds[k][i];
    }
    return hashFloats;
}

template<typename DATATYPE>
std::vector<bool> lshbox::laSphLsh<DATATYPE>::quantization(const std::vector<float>& hashFloats)
{
    std::vector<bool> hashBits;
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
std::vector<bool> lshbox::laSphLsh<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin)
{
    std::vector<float> hashFloats = getHashFloats(k, domin);
    std::vector<bool> hashBits = quantization(hashFloats);
    return hashBits;
}
template<typename DATATYPE>
int lshbox::laSphLsh<DATATYPE>::getTableSize()
{
    assert(param.L == 1);
    return tables[0].size();
}
template<typename DATATYPE>
int lshbox::laSphLsh<DATATYPE>::getMaxBucketSize()
{
    assert(param.L == 1);
    int max = 0;
    std::unordered_map<BIDTYPE, std::vector<unsigned> >::const_iterator it;
    for (it = tables[0].begin(); it != tables[0].end(); ++it) {
        if (it->second.size() > max) {
            max = it->second.size();
        }
    }
    return max;
}

template<typename DATATYPE>
template<typename PROBER>
void lshbox::laSphLsh<DATATYPE>::KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems) {
    assert(param.L == 1);

    while(prober.getNumItemsProbed() < numItems && prober.nextBucketExisted()) {
        // <table, bucketId>
        const std::pair<unsigned, BIDTYPE>& probePair = prober.getNextBID();
        probe(probePair.first, probePair.second, prober); 
    }
}
