//
// Created by darxan on 2018/1/8.
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

using namespace std;


int sample(const char* inputSampleFile, const char* outputSampleFile, float max_norm_square) {

    ofstream fout(outputSampleFile, ios::binary);
    if (!fout) {
        std::cout << "cannot open file " << outputSampleFile << std::endl;
        assert(false);
    }
    ifstream fin(inputSampleFile, ios::binary);
    if (!fin) {
        std::cout << "cannot open file " << inputSampleFile << std::endl;
        assert(false);
    }

    int originDim;
    int dimension;
    float scale = max_norm_square+0.5f;

    while (fin.read((char*)(&originDim), sizeof(int))) {

        dimension = originDim + 4;

        float * buffer = new float[dimension];
        fin.read((char *)(buffer), sizeof(float) * originDim);

        // calculate the norm of a row of data
        float norm_square = 0.0;
        for (int index_dim = 0; index_dim < originDim; index_dim++) {
            norm_square += buffer[index_dim] * buffer[index_dim];
        }

        for (int j = 0; j < originDim; ++j) {
            buffer[j] /= scale;
        }
        buffer[dimension-4] = -0.5f / scale;
        buffer[dimension-3] = norm_square / scale;;
        buffer[dimension-2] = 0;
        buffer[dimension-1] = SQRT( 1.0f - SQUARE((norm_square+0.5f)/(max_norm_square+0.5f)) );


        fout.write( (char*) &dimension, sizeof(int));
        fout.write( (char*) buffer, dimension * sizeof(float));
    }



    fout.close();
    fin.close();
    return 0;
}

int main (int argc, char** argv) {
    if (argc != 5) {
        cout << "Usage: transform.bin ${inputFile} ${outputFile} ${inputSampleFile} ${outputSampleFile} " << endl;
        return 0;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* inputSampleFile = argv[3];
    const char* outputSampleFile = argv[4];


    ofstream fout(outputFile, ios::binary);
    if (!fout) {
        std::cout << "cannot open file " << outputFile << std::endl;
        assert(false);
    }
    ifstream fin(inputFile, ios::binary);
    if (!fin) {
        std::cout << "cannot open file " << inputFile << std::endl;
        assert(false);
    }


    int originDim = -1;
    int dimension = -1;
    int transformed_dimension = 4;
    float max_norm_square = 0;
    vector<float*> data; // save buffered data

    while (fin.read((char*)(&originDim), sizeof(int))) {

        // dimension of tranformed data
        dimension = originDim + transformed_dimension;

        float * buffer = new float[dimension];
        fin.read((char *)(buffer), sizeof(float) * originDim);

        // calculate the norm of a row of data
        float norm_square = 0.0;
        for (int index_dim = 0; index_dim < originDim; index_dim++) {
            norm_square += buffer[index_dim] * buffer[index_dim];
        }
        //update max_norm
        if (norm_square>max_norm_square)
            max_norm_square = norm_square;

        // save norm momentarily
        // save buffer in vector
        buffer[dimension-1] = norm_square;
        data.push_back(buffer);
    }

    if (originDim < 0 || dimension < 0 )
        cout << "read nothing" << endl;

    // 3rdTerm = sqrt( M^4 + M^2 + 1/4 - (X^4 + x^2 + 1/4) ) = sqrt( ( M^2+1/2)^2 - (X^2+1/2)^2)
    // x -> x, |x|^2, -1/2, 3rdTerm , 0
    // scale = (M^4 + M^2 + 1/4) ^ (1/2) = M^2 + 1/2
    // calculate :  /=  scale
    // note: M^2 = max_norm_square
    // note: |x|^2 = norm_square
    // note: 3rdTerm / scale = sqrt( ( M^2+1/2)^2 - (X^2+1/2)^2) / (M^2 + 1/2)
    //                       = sqrt( 1 - [(X^2+1/2)/(M^2 + 1/2)]^2 )
    float scale = max_norm_square+0.5f;
    for (int i = 0; i < data.size(); ++i)
    {
        float * buffer = data[i];
        float norm_square = buffer[dimension-1];

        for (int j = 0; j < originDim; ++j) {
            buffer[j] /= scale;
        }
        buffer[dimension-4] = norm_square / scale;
        buffer[dimension-3] = -0.5f / scale;
        buffer[dimension-2] = sqrt(1.0f - SQUARE((norm_square+0.5f)/(max_norm_square+0.5f)));
        buffer[dimension-1] = 0;

        fout.write((char*)&dimension, sizeof(int));
        fout.write((char*)buffer, dimension * sizeof(float));
    }
    fin.close();
    fout.close();


    sample(inputSampleFile, outputSampleFile, max_norm_square);

    for (auto& v : data) {
        delete v;
    }

    return 0;
}
