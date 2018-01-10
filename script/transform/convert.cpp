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

#define SQRT(X) ( (X)>0 ? sqrt(X) : 0.0f)
#define SQUARE(X) ( (X)*(X) )
#define ENABLE_SCALE (1)
#define SCALE_TO (1.0f)

using namespace std;

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
        // calculate the norm of a row of data
        float norm_square = calNormSquare(buffer, dimension);
        //update max_norm
        if (norm_square>max_norm_square)
            max_norm_square = norm_square;
    }
    return max_norm_square;
}


int euclidToMIP(vector<float* >& data, vector<float* >& sampleData, int dimension) {

    std::cout << "[euclidToMIP] transforming data" << std::endl;

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);
        buffer[dimension] = norm_square;
        buffer[dimension+1] = -0.5f;
    }

    for (int j = 0; j < sampleData.size(); ++j) {
        float * buffer = sampleData[j];
        float norm_square = calNormSquare(buffer, dimension);
        buffer[dimension+1] = norm_square;
        buffer[dimension] = -0.5f;
    }


    std::cout << "[euclidToMIP] transforming data, done" << std::endl;

    return dimension+2;
}


int mipToAngular(vector<float* >& data, vector<float* >& sampleData, int dimension) {

    std::cout << "[mipToAngular] transforming data" << std::endl;

    float max_norm_square = std::max( calMaxNormSquare(data, dimension), calMaxNormSquare(sampleData, dimension) );

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);

        buffer[dimension] = sqrt(max_norm_square - norm_square);
        buffer[dimension+1] = 0.0f;
    }

    for (int j = 0; j < sampleData.size(); ++j) {
        float * buffer = sampleData[j];
        float norm_square = calNormSquare(buffer, dimension);

        buffer[dimension] = 0.0f;
        buffer[dimension+1] = sqrt(max_norm_square - norm_square);
    }

    if (ENABLE_SCALE) {

        float scale = sqrt(max_norm_square) / SCALE_TO;
        std::cout << "scale = " << sqrt(max_norm_square) << "/" <<  SCALE_TO << " = " << scale << std::endl;
        for (int line = 0; line < data.size(); ++line) {
            for (int dim = 0; dim < dimension+2; ++dim) {
                data[line][dim] /= scale;
            }

        }

        for (int line = 0; line < sampleData.size(); ++line) {
            for (int dim = 0; dim < dimension+2; ++dim) {
                data[line][dim] /= scale;
            }
        }
    }

    std::cout << "[mipToAngular] transforming data, done" << std::endl;

    return dimension+2;
}


int loadData(const char* file, vector<float*>& data, int placeholder) {

    ifstream fin(file, ios::binary);
    if (!fin) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }
    std::cout << "[loadData] reading data from file:" << file << std::endl;

    int originDim;
    while (fin.read((char*)(&originDim), sizeof(int))) {

        float * buffer = new float[originDim + placeholder];
        fin.read((char *)(buffer), sizeof(float) * originDim);
        data.push_back(buffer);
    }

    std::cout << "[loadData] read data, done" << std::endl;

    fin.close();
    return originDim;
}


int dumpData(const char* file, vector<float*>& data, int dimension) {
    ofstream fout(file, ios::binary);
    if (!fout) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }
    std::cout << "[dumpData] writing data to file:" << file << std::endl;

    for (int i = 0; i < data.size(); ++i)
    {
        fout.write((char*)&dimension, sizeof(int));
        fout.write((char*)data[i], dimension * sizeof(float));
    }
    std::cout << "[dumpData] wrote data, done" << std::endl;

    fout.close();
    return dimension;
}


int main(int argc, char** argv) {

    if (argc <= 5) {
        cout << "Usage: transform.bin ${inputFile} ${outputFile} ${inputSampleFile} ${outputSampleFile} ${mipToAngular|euclidToMIP}" << endl;
        return 0;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* inputSampleFile = argv[3];
    const char* outputSampleFile = argv[4];

    vector<float* > data ;
    vector<float* > sampleData ;
    int dimension;
    int sampleDimension;

    dimension = loadData(inputFile, data, 4);
    sampleDimension = loadData(inputSampleFile, sampleData, 4);

    if(dimension<0 || sampleDimension<0 || dimension!=sampleDimension) {
        std::cout << "dimension un_compatible" << std::endl
                  << "data dim:" << dimension << std::endl
                  << "sample dim:" << sampleDimension << std::endl;
        assert(false);
    }

    string e2m("e2m");
    string m2a("m2a");

    for (int i = 5; i < argc; ++i) {

        string operation(argv[i]);

        if (e2m==operation) {
            dimension = euclidToMIP(data, sampleData, dimension);
        } else if (m2a==operation) {
            dimension = mipToAngular(data, sampleData, dimension);
        } else {
            std::cout << "invalid operation " << operation << std::endl;
        }
    }

    dumpData(outputFile, data, dimension);
    dumpData(outputSampleFile, sampleData, dimension);

    return 0;
}


