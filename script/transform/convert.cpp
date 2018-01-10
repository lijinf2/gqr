//
// Created by darxan on 2018/1/10.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include <chrono>
#include <vector>
#include "lshbox/utils.h"
#include <climits>

#define ENABLE_SCALE (1)
#define SCALE_TO     (1.0f)

using namespace std;

template <typename T>


void freeVectors(vector<T*>& data) {
    for (int i = 0; i < data.size(); ++i) {
        delete[] data[i];
    }
}


float inline calNormSquare(float* data, int dimension) {
    float norm_square = 0.0f;
    for (int index_dim = 0; index_dim < dimension; index_dim++) {
        norm_square += data[index_dim] * data[index_dim];
    }
    return norm_square;
}


float calMaxNormSquare(vector<float* >& data, int dimension) {

    float max_norm_square = 0.0f;

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);
        if (norm_square > max_norm_square)
            max_norm_square = norm_square;
    }

    return max_norm_square;
}


int euclidToMIP(vector<float* >& data, vector<float* >& queryData, int dimension) {

    std::cout << "----[euclidToMIP] transforming data" << std::endl;

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);
        buffer[dimension] = norm_square;
        buffer[dimension+1] = -0.5f;
    }

    for (int j = 0; j < queryData.size(); ++j) {
        float * buffer = queryData[j];
        float norm_square = calNormSquare(buffer, dimension);
        buffer[dimension+1] = norm_square;
        buffer[dimension] = -0.5f;
    }


    std::cout << "----[euclidToMIP] transforming data, done" << std::endl;

    return dimension+2;
}


int mipToAngular(vector<float* >& data, vector<float* >& queryData, int dimension) {

    std::cout << "----[mipToAngular] transforming data" << std::endl;

    float max_norm_square = std::max( calMaxNormSquare(data, dimension), calMaxNormSquare(queryData, dimension) );

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);

        buffer[dimension] = sqrt(max_norm_square - norm_square);
        buffer[dimension+1] = 0.0f;
    }

    for (int j = 0; j < queryData.size(); ++j) {
        float * buffer = queryData[j];
        float norm_square = calNormSquare(buffer, dimension);

        buffer[dimension] = 0.0f;
        buffer[dimension+1] = sqrt(max_norm_square - norm_square);
    }

    if (ENABLE_SCALE) {

        float scale = sqrt(max_norm_square) / SCALE_TO;
        std::cout << "----[mipToAngular] scale = " << sqrt(max_norm_square) << "/" <<  SCALE_TO << " = " << scale << std::endl;
        for (int line = 0; line < data.size(); ++line) {
            for (int dim = 0; dim < dimension+2; ++dim) {
                data[line][dim] /= scale;
            }

        }

        for (int line = 0; line < queryData.size(); ++line) {
            for (int dim = 0; dim < dimension+2; ++dim) {
                data[line][dim] /= scale;
            }
        }
    }

    std::cout << "----[mipToAngular] transforming data, done" << std::endl;

    return dimension+2;
}


/**
 * load data from fvecs file.
 * @param file
 * @param data
 * @param placeholder
 * @return
 */
int loadData(const char* file, vector<float*>& data, int placeholder) {

    ifstream fin(file, ios::binary);
    if (!fin) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }
    std::cout << "----[loadData] reading data from file:" << file << std::endl;

    int originDim;
    while (fin.read((char*)(&originDim), sizeof(int))) {

        float * buffer = new float[originDim + placeholder];
        fin.read((char *)(buffer), sizeof(float) * originDim);
        data.push_back(buffer);
    }

    std::cout << "----[loadData] read data, done" << std::endl;

    fin.close();
    return originDim;
}

/**
 * write data to binary file as file.fvecs format.
 * @param file
 * @param data
 * @param dimension
 * @return
 */
int dumpData(const char* file, vector<float*>& data, int dimension) {
    ofstream fout(file, ios::binary);
    if (!fout) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }
    std::cout << "----[dumpData] writing data to file:" << file << std::endl;

    for (int i = 0; i < data.size(); ++i)
    {
        fout.write((char*)&dimension, sizeof(int));
        fout.write((char*)data[i], dimension * sizeof(float));
    }
    std::cout << "----[dumpData] wrote data, done" << std::endl;

    fout.close();
    return dimension;
}

/**
 * write data to text file.
 * @param file
 * @param data
 * @param dimension
 * @return
 */
int dumpText(const char* file, vector<float*>& data, int dimension) {


    std::cout << "dump data " << file << std::endl;

    ofstream fout(file);
    if (!fout) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];

        for (int j = 0; j < dimension; ++j) {
            fout << buffer[j] << "\t";
        }
        fout << endl;
    }

    fout.close();

    std::cout << "dump data, done " << file << std::endl;
}


int main(int argc, char** argv) {

    int transform_arg = 5;

    if (argc < transform_arg) {
        cout << "Usage: transform.bin ${inputFile} ${outputFile} ${inputSampleFile} "
             <<"${outputSampleFile} ${mipToAngular|euclidToMIP}"
             << endl;
        return 0;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* inputSampleFile = argv[3];
    const char* outputSampleFile = argv[4];

    vector<float* > data ;
    vector<float* > queryData ;
    int dimension;
    int queryDimension;

    dimension = loadData(inputFile, data, 4);
    queryDimension = loadData(inputSampleFile, queryData, 4);

    if(dimension<0 || queryDimension<0 || dimension!=queryDimension) {
        std::cout << "dimension un_compatible" << std::endl
                  << "data dim:" << dimension << std::endl
                  << "query dim:" << queryDimension << std::endl;
        assert(false);
    }


    string e2m("e2m");
    string m2a("m2a");

    for (int i = transform_arg; i < argc; ++i) {

        string operation(argv[i]);

        if (e2m==operation) {
            dimension = euclidToMIP(data, queryData, dimension);
        } else if (m2a==operation) {
            dimension = mipToAngular(data, queryData, dimension);
        } else {
            std::cout << "invalid operation " << operation << std::endl;
        }
    }

    dumpData(outputFile, data, dimension);
    dumpData(outputSampleFile, queryData, dimension);

    freeVectors(data);
    freeVectors(queryData);

    return 0;
}


