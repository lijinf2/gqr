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
/**
 * Locality-Sensitive Hashing Scheme Based on Iterative Quantization.
 *
 *
 * For more information on iterative quantization based LSH, see the following reference.
 *
 *     Gong Y, Lazebnik S, Gordo A, et al. Iterative quantization: A procrustean
 *     approach to learning binary codes for large-scale image retrieval[J].
 *     Pattern Analysis and Machine Intelligence, IEEE Transactions on, 2013,
 *     35(12): 2916-2929.
 */
template<typename DATATYPE = float>
class laItqLsh 
{
public:
    typedef unsigned long long BIDTYPE;
    struct Parameter
    {
        /// Hash table size
        unsigned M;
        /// Number of hash tables
        unsigned L;
        /// Dimension of the vector, it can be obtained from the instance of Matrix
        unsigned D;
        /// Binary code bytes
        unsigned N;
        /// Size of vectors in train
        unsigned S;
        /// Training iterations
        unsigned I;
    };
    laItqLsh() {}
    laItqLsh(const Parameter &param_)
    {
        reset(param_);
    }
    ~laItqLsh() {}
    /**
     * Reset the parameter setting
     *
     * @param param_ A instance of laItqLsh<DATATYPE>::Parametor, which contains
     * the necessary parameters
     */
    void reset(const Parameter &param_);
    /**
     * Train the data to get several groups of suitable vector for index.
     *
     * @param data A instance of Matrix<DATATYPE>, most of the time, is the search dataset.
     */
    void train(Matrix<DATATYPE> &data);
    /**
     * Hash the dataset.
     *
     * @param data A instance of Matrix<DATATYPE>, it is the search dataset.
     */
    void hash(Matrix<DATATYPE> &data);
    /**
     * calculate mean of each dimension.
     *
     * @param data A instance of Matrix<DATATYPE>, it is the search dataset.
     */
    std::vector<std::vector<float>> getMeanAndSTD(Matrix<DATATYPE> &data);
    void setMeanAndSTD(Matrix<DATATYPE> &data);
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
    int probe(int t, BIDTYPE bucketId, PROBER &prober);
    /**
     * Query the approximate nearest neighborholds.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     */
    template<typename SCANNER>
    void query(const DATATYPE *domin, SCANNER &scanner);
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
     * convert unsigned int to vector of boolbucketss.
     */
    std::vector<bool> unsignedToBools(unsigned num);
    /**
     * ranking hash code to query the approximate nearest neighborholds.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     * @param maxNumBuckets Maximum number of buckets to probe
     * */
    template<typename PROBER>
    void queryRankingByHamming(const DATATYPE *domin, PROBER &prober, int maxNumBuckets);

    /**
     * ranking hash code to query the approximate nearest neighborholds.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     * @param numItem number of buckets to return
     * */
    template<typename PROBER>
    void KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems);
    /**
     * ranking hash code to query the approximate nearest neighborholds by considering the query quantization error.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     * @param maxNumBuckets Maximum number of buckets to probe
     * */
    template<typename PROBER>
    void queryRankingByLoss(const DATATYPE *domin, PROBER &prober, int maxNumBuckets);
    /**
     * expand hash code by loss to query the approximate nearest neighborholds.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     * @param maxNumBuckets Maximum number of buckets to probe
     * */
    template<typename SCANNER>
    void queryProbeByLoss(const DATATYPE *domin, SCANNER &scanner, int maxNumBuckets, bool withMeanAndSTD = false);

    /**
     * re Hash the dataset to L buckets, by multi assignment.
     *
     * @param data A instance of Matrix<DATATYPE>, it is the search dataset.
     */
    void rehash(Matrix<DATATYPE> &data, int numTables);
    /**
     * Query the rehashed tables.
     *
     * @param domin   The pointer to the vector
     * @param scanner Top-K scanner, use for scan the approximate nearest neighborholds
     */
    template<typename SCANNER>
    void queryRehash(const DATATYPE *domin, SCANNER &scanner);

    Parameter param;
    std::vector<std::unordered_map<BIDTYPE, std::vector<unsigned> > > tables;

private:
    std::vector<std::vector<std::vector<float> > > pcsAll;
    std::vector<std::vector<std::vector<float> > > omegasAll;
    std::vector<std::vector<unsigned> > rndArray;
    std::vector<std::vector<float>> meanAndSTD;
};
}

// ------------------------- implementation -------------------------
template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::reset(const Parameter &param_)
{
    param = param_;
    tables.resize(param.L);
    rndArray.resize(param.L);
    pcsAll.resize(param.L);
    omegasAll.resize(param.L);
    std::mt19937 rng(unsigned(std::time(0)));
    std::uniform_int_distribution<unsigned> usArray(0, param.M - 1);
    for (std::vector<std::vector<unsigned> >::iterator iter = rndArray.begin(); iter != rndArray.end(); ++iter)
    {
        for (unsigned i = 0; i != param.N; ++i)
        {
            iter->push_back(usArray(rng));
        }
    }
}
template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::train(Matrix<DATATYPE> &data)
{
    int npca = param.N;
    std::mt19937 rng(unsigned(std::time(0)));
    std::normal_distribution<float> nd;
    std::uniform_int_distribution<unsigned> usBits(0, data.getSize() - 1);
    for (unsigned k = 0; k != param.L; ++k)
    {
        std::cout << "start PCA " << std::endl;
        std::vector<unsigned> seqs;
        while (seqs.size() != param.S)
        {
            unsigned target = usBits(rng);
            if (std::find(seqs.begin(), seqs.end(), target) == seqs.end())
            {
                seqs.push_back(target);
            }
        }
        std::sort(seqs.begin(), seqs.end());
        Eigen::MatrixXf tmp(param.S, data.getDim());
        for (unsigned i = 0; i != tmp.rows(); ++i)
        {
            std::vector<float> vals(0);
            for (int j = 0; j != data.getDim(); ++j)
            {
                vals.push_back(data[seqs[i]][j]);
            }
            tmp.row(i) = Eigen::Map<Eigen::VectorXf>(&vals[0], data.getDim());
        }
        Eigen::MatrixXf centered = tmp.rowwise() - tmp.colwise().mean();
        Eigen::MatrixXf cov = (centered.transpose() * centered) / float(tmp.rows() - 1);
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eig(cov);
        Eigen::MatrixXf mat_pca = eig.eigenvectors().rightCols(npca);
        Eigen::MatrixXf mat_c = tmp * mat_pca;
        Eigen::MatrixXf R(npca, npca);
        for (unsigned i = 0; i != R.rows(); ++i)
        {
            for (unsigned j = 0; j != R.cols(); ++j)
            {
                R(i, j) = nd(rng);
            }
        }
        Eigen::JacobiSVD<Eigen::MatrixXf> svd(R, Eigen::ComputeThinU | Eigen::ComputeThinV);
        R = svd.matrixU();
        std::cout << "finish PCA " << std::endl;
        for (unsigned iter = 0; iter != param.I; ++iter)
        {
            std::cout << "start iteration: " << iter << std::endl;
            Eigen::MatrixXf Z = mat_c * R;
            Eigen::MatrixXf UX(Z.rows(), Z.cols());
            for (unsigned i = 0; i != Z.rows(); ++i)
            {
                for (unsigned j = 0; j != Z.cols(); ++j)
                {
                    if (Z(i, j) > 0)
                    {
                        UX(i, j) = 1;
                    }
                    else
                    {
                        UX(i, j) = -1;
                    }
                }
            }
            Eigen::JacobiSVD<Eigen::MatrixXf> svd_tmp(UX.transpose() * mat_c, Eigen::ComputeThinU | Eigen::ComputeThinV);
            R = svd_tmp.matrixV() * svd_tmp.matrixU().transpose();
        }
        omegasAll[k].resize(npca);
        for (unsigned i = 0; i != omegasAll[k].size(); ++i)
        {
            omegasAll[k][i].resize(npca);
            for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
            {
                omegasAll[k][i][j] = R(j, i);
            }
        }
        pcsAll[k].resize(npca);
        for (unsigned i = 0; i != pcsAll[k].size(); ++i)
        {
            pcsAll[k][i].resize(param.D);
            for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
            {
                pcsAll[k][i][j] = mat_pca(j, i);
            }
        }
    }
}
template<typename DATATYPE>
std::vector<std::vector<float>> lshbox::laItqLsh<DATATYPE>::getMeanAndSTD(Matrix<DATATYPE> &data)
{
    // calculate mean
    std::vector<float> sumPositive;
    std::vector<float> sumNegative;
    sumPositive.resize(param.N);
    sumNegative.resize(param.N);

    std::vector<int> countPositive;
    std::vector<int> countNegative;
    countPositive.resize(param.N);
    countNegative.resize(param.N);
    for (int i = 0; i < param.N; ++i) {
        sumPositive[i] = 0;
        sumNegative[i] = 0;
        countPositive[i] = 0;
        countNegative[i] = 0;
    }

    for (unsigned dIdx = 0; dIdx != data.getSize(); ++dIdx)
    {
        std::vector<float> hashFloats = getHashFloats(0, data[dIdx]);
        for (int hfIdx = 0; hfIdx < hashFloats.size(); ++hfIdx) {
            if (hashFloats[hfIdx] >= 0) {
                sumPositive[hfIdx] += hashFloats[hfIdx];
                countPositive[hfIdx]++;
            } else {
                sumNegative[hfIdx] += hashFloats[hfIdx];
                countNegative[hfIdx]++;
            }
        }
    }
    for (int i = 0; i < param.N; ++i) {
        if(countPositive[i] != 0) sumPositive[i] /= countPositive[i];
        if(countNegative[i] != 0) sumNegative[i] /= countNegative[i];
    }

    // std::cout << "Positive mean: ";
    // for (int i = 0; i < param.N; ++i) {
    //     std::cout << sumPositive[i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "Negative mean: ";
    // for (int i = 0; i < param.N; ++i) {
    //     std::cout << sumNegative[i] << " ";
    // }
    // std::cout << std::endl;

    // return std::pair<std::vector<float>, std::vector<float>>(sumPositive, sumNegative);
    // calculate STD
    std::vector<float> stdPositive;
    std::vector<float> stdNegative;
    stdPositive.resize(param.N);
    stdNegative.resize(param.N);
    for (int i = 0; i < param.N; ++i) {
        stdPositive[i] = 0;
        stdNegative[i] = 0;
    }
    for (unsigned dIdx = 0; dIdx < data.getSize(); ++dIdx) {
        std::vector<float> hashFloats = getHashFloats(0, data[dIdx]);
        for (int hfIdx = 0; hfIdx < hashFloats.size(); ++hfIdx) {
            if ( hashFloats[hfIdx] >= 0 ) {
                stdPositive[hfIdx] += (hashFloats[hfIdx] - sumPositive[hfIdx]) * (hashFloats[hfIdx] - sumPositive[hfIdx]);
            } else {
                stdNegative[hfIdx] += (hashFloats[hfIdx] - sumNegative[hfIdx]) * (hashFloats[hfIdx] - sumNegative[hfIdx]);
            }
        }
    } 
    for (int i = 0; i < param.N; ++i) {
        if(countPositive[i] != 0) stdPositive[i] /= countPositive[i];
        stdPositive[i] = sqrt(stdPositive[i]);
        assert(stdPositive[i] > 0);

        if(countNegative[i] != 0) stdNegative[i] /= countNegative[i];
        stdNegative[i] = sqrt(stdNegative[i]);
        assert(stdNegative[i] > 0);
    }

    // std::cout << "Positive std: ";
    // for (int i = 0; i < param.N; ++i) {
    //     std::cout << stdPositive[i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "Negative std: ";
    // for (int i = 0; i < param.N; ++i) {
    //     std::cout << stdNegative[i] << " ";
    // }
    // std::cout << std::endl;
    //
    std::vector<std::vector<float>> result;
    result.push_back(sumPositive);
    result.push_back(sumNegative);
    result.push_back(stdPositive);
    result.push_back(stdNegative);
    return result;
}
template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::setMeanAndSTD(Matrix<DATATYPE> &data){
    meanAndSTD = getMeanAndSTD(data);
}

template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::hash(Matrix<DATATYPE> &data)
{
    progress_display pd(data.getSize());
    for (unsigned i = 0; i != data.getSize(); ++i)
    {
        insert(i, data[i]);
        ++pd;
    }
}
template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::insert(unsigned key, const DATATYPE *domin)
{
    for (unsigned k = 0; k != param.L; ++k)
    {
        BIDTYPE hashVal = getHashVal(k, domin);
        tables[k][hashVal].push_back(key);
    }
}
template<typename DATATYPE>
template<typename PROBER>
int lshbox::laItqLsh<DATATYPE>::probe(int t, BIDTYPE bucketId, PROBER& prober)
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
template<typename SCANNER>
void lshbox::laItqLsh<DATATYPE>::query(const DATATYPE *domin, SCANNER &scanner)
{
    scanner.reset(domin);
    assert(param.L == 1);
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned hashVal = getHashVal(k, domin);
        // std::cout << "hashVal " << hashVal << std::endl;
        probe(k, hashVal, scanner);
    }
    scanner.topk().genTopk();
}
template<typename DATATYPE>
typename lshbox::laItqLsh<DATATYPE>::BIDTYPE lshbox::laItqLsh<DATATYPE>::getHashVal(unsigned k, const DATATYPE *domin)
{
    std::vector<float> domin_pc(pcsAll[k].size());
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
        {
            domin_pc[i] += domin[j] * pcsAll[k][i][j];
        }
    }

    BIDTYPE hashVal = 0;
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        float product = 0;
        for (unsigned j = 0; j != omegasAll[k][i].size(); ++j)
        {
            product += float(domin_pc[j] * omegasAll[k][i][j]);
        }
        hashVal <<= 1; // hashVal *= 2
        if (product > 0)
        {
            hashVal += 1;
        }
    }
    return hashVal;
}
template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::load(const std::string &file)
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
    rndArray.resize(param.L);
    pcsAll.resize(param.L);
    omegasAll.resize(param.L);
    for (unsigned i = 0; i != param.L; ++i)
    {
        rndArray[i].resize(param.N);
        in.read((char *)&rndArray[i][0], sizeof(unsigned) * param.N);
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
        pcsAll[i].resize(param.N);
        omegasAll[i].resize(param.N);
        for (unsigned j = 0; j != param.N; ++j)
        {
            pcsAll[i][j].resize(param.D);
            omegasAll[i][j].resize(param.N);
            in.read((char *)&pcsAll[i][j][0], sizeof(float) * param.D);
            in.read((char *)&omegasAll[i][j][0], sizeof(float) * param.N);
        }
    }
    in.close();
}
template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::save(const std::string &file)
{
    std::ofstream out(file, std::ios::binary);
    out.write((char *)&param.M, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.L, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.D, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.N, sizeof(unsigned)); // 4 bytes
    out.write((char *)&param.S, sizeof(unsigned)); // 4 bytes
    for (int i = 0; i != param.L; ++i)
    {
        out.write((char *)&rndArray[i][0], sizeof(unsigned) * param.N);  // 4 * N bytes
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
            out.write((char *)&pcsAll[i][j][0], sizeof(float) * param.D);
            out.write((char *)&omegasAll[i][j][0], sizeof(float) * param.N);
        }
    }
    out.close();
}
template<typename DATATYPE>
std::vector<float> lshbox::laItqLsh<DATATYPE>::getHashFloats(unsigned k, const DATATYPE *domin)
{
    std::vector<float> domin_pc(pcsAll[k].size());
    for (unsigned i = 0; i != domin_pc.size(); ++i)
    {
        for (unsigned j = 0; j != pcsAll[k][i].size(); ++j)
        {
            domin_pc[i] += domin[j] * pcsAll[k][i][j];
        }
    }

    std::vector<float> hashFloats;
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
std::vector<bool> lshbox::laItqLsh<DATATYPE>::quantization(const std::vector<float>& hashFloats)
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
std::vector<bool> lshbox::laItqLsh<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin)
{
    std::vector<float> hashFloats = getHashFloats(k, domin);
    std::vector<bool> hashBits = quantization(hashFloats);
    return hashBits;
}
template<typename DATATYPE>
int lshbox::laItqLsh<DATATYPE>::getTableSize()
{
    assert(param.L == 1);
    return tables[0].size();
}
template<typename DATATYPE>
int lshbox::laItqLsh<DATATYPE>::getMaxBucketSize()
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
std::vector<bool> lshbox::laItqLsh<DATATYPE>::unsignedToBools(unsigned num)
{
    int nBits = param.N;
    std::vector<bool> bits;
    bits.resize(nBits);
    
    while(num > 0 ){
        bits[--nBits] = num % 2;
        num /= 2;
    }
    assert(bits.size() == param.N);
    return bits;
}

template<typename DATATYPE>
template<typename PROBER>
void lshbox::laItqLsh<DATATYPE>::queryRankingByHamming(const DATATYPE *domin, PROBER &prober, int maxNumBuckets)
{
    assert(param.L == 1);

    // noted that the prober will persist the last probed results, so probed maxNumBuckets/2 buckets more
    for (int bId = maxNumBuckets / 2; bId < tables[0].size() && bId < maxNumBuckets; ++bId) {

        const BIDTYPE& probedBId = prober.getNextBID();
        probe(0, probedBId, prober);
    }

}

template<typename DATATYPE>
template<typename PROBER>
void lshbox::laItqLsh<DATATYPE>::KItemByProber(const DATATYPE *domin, PROBER &prober, int numItems) {
    assert(param.L == 1);

    while(prober.getNumItemsProbed() < numItems && prober.nextBucketExisted()) {
        const BIDTYPE& probedBId = prober.getNextBID();
        probe(0, probedBId, prober); 
    }
}

template<typename DATATYPE>
template<typename PROBER>
void lshbox::laItqLsh<DATATYPE>::queryRankingByLoss(const DATATYPE *domin, PROBER &prober, int maxNumBuckets)
{
    assert(param.L == 1);
    for (unsigned k = 0; k != param.L; ++k)
    {

        for (int bId = maxNumBuckets/2; bId < tables[0].size() && bId < maxNumBuckets; ++bId) {
            unsigned probedBId = prober.getNextBID();

            probe(k, probedBId, prober);
        }
    }
}
template<typename DATATYPE>
template<typename SCANNER>
void lshbox::laItqLsh<DATATYPE>::queryProbeByLoss(const DATATYPE *domin, SCANNER &scanner, int maxNumBuckets, bool withMeanAndSTD)
{
    scanner.reset(domin);
    assert(param.L == 1);
    for (unsigned k = 0; k != param.L; ++k)
    {
        unsigned hashVal = getHashVal(k, domin);
        std::vector<bool> hashBits = getHashBits(k, domin);
        std::vector<float> hashFloats = getHashFloats(k, domin);
        
        assert(hashBits.size() == param.N);
        assert(hashFloats.size() == param.N);

        // query the first bucket
        unsigned probedBId = hashVal;
        probe(0, probedBId, scanner);

        Probing pro(hashBits, hashFloats, false);
        // multi-probing
        for (int numBk = 1; numBk < maxNumBuckets; ++numBk) {
            probedBId = pro.pop();
            // std::cout << std::endl 
            //     << "the second probed bucket is:" << probedBId
            //     << std::endl;
            // std::cout << "hashFloats: ";
            // for(auto e : hashFloats)
            //     std::cout<< e << ",";
            // std::cout << std::endl;
            // std::cout << "hashBits: ";
            // for(auto e: hashBits)
            //     std::cout<< e << ",";
            // std::cout << std::endl;
            probe(0, probedBId, scanner);
        }
    }
    scanner.topk().genTopk(); // must getTopk for scanner, other wise will wrong
}
template<typename DATATYPE>
void lshbox::laItqLsh<DATATYPE>::rehash(Matrix<DATATYPE> &data, int numTables)
{
    if (numTables == 1) return;
    tables.clear();
    tables.resize(numTables);
    for (unsigned i = 0; i != data.getSize(); ++i)
    {
        unsigned hashVal = getHashVal(0, data[i]);
        std::vector<bool> hashBits = getHashBits(0, data[i]);
        std::vector<float> hashFloats = getHashFloats(0, data[i]);
        tables[0][hashVal].push_back(i);

        Probing pro(hashBits, hashFloats, false);
        for (unsigned k = 1; k != tables.size(); ++k) {
            hashVal = pro.pop();
            tables[k][hashVal].push_back(i);
        }
    }
}
template<typename DATATYPE>
template<typename SCANNER>
void lshbox::laItqLsh<DATATYPE>::queryRehash(const DATATYPE *domin, SCANNER &scanner)
{
    scanner.reset(domin);
    assert(param.L == 1);
    unsigned hashVal = getHashVal(0, domin);
    // std::cout << "hashVal " << hashVal << std::endl;
    for (unsigned k = 0; k != tables.size(); ++k)
    {
        probe(k, hashVal, scanner);
    }
    scanner.topk().genTopk();
}
