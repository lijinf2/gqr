#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include <chrono>
#include <vector>
#include "lshbox/utils.h"
using namespace std;

int sample(const char* inputSampleFile, const char* outputSampleFile, float max_norm) {

    ofstream fout(outputSampleFile, ios::binary);
    if (!fout) {
        std::cout << "cannot open file " << outputFile << std::endl;
        assert(false);
    }
    ifstream fin(inputSampleFile, ios::binary);
    if (!fin) {
        std::cout << "cannot open file " << inputFile << std::endl;
        assert(false);
    }

    int originDim;
    int dimension;


    while (fin.read((char*)(&originDim), sizeof(int))) {

        dimension = originDim + 2;

        float * buffer = new float[dimension];
        fin.read((char *)(buffer), sizeof(float) * originDim);

        // calculate the norm of a row of data
        float norm = 0.0;
        for (int index_dim = 0; index_dim < originDim; index_dim++) {
            norm += buffer[index_dim] * buffer[index_dim];
        }
        buffer[dimension-1] = sqrt( max_norm - norm > 0.0 ? max_norm - norm : 0.0);
        buffer[dimension-2] = 0.0;


        fout.write((char*)&dimension, sizeof(int));
        fout.write((char*)buffer, dimension * sizeof(float));
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


    int originDim;
    int dimension;
    float max_norm = 0;
    vector<float*> data; // save buffered data

    while (fin.read((char*)(&originDim), sizeof(int))) {

        // dimension of tranformed data
        dimension = originDim + 2;
        
        float * buffer = new float[dimension];
        fin.read((char *)(buffer), sizeof(float) * originDim);

        // calculate the norm of a row of data
        float norm = 0.0;
        for (int index_dim = 0; index_dim < originDim; index_dim++) {
            norm += buffer[index_dim] * buffer[index_dim];
        }
        //update max_norm
        if (norm>max_norm)
            max_norm = norm;
        
        // save norm momentarily
        // save buffer in vector
        buffer[dimension-1] = norm; 
        data.push_back(buffer);
    }

    for (int i = 0; i < data.size(); ++i)
    {
        float * buffer = data[i];
        float norm = buffer[dimension-1];

        buffer[dimension-2] = sqrt( max_norm - norm );
        buffer[dimension-1] = 0.0;
        fout.write((char*)&dimension, sizeof(int));
        fout.write((char*)buffer, dimension * sizeof(float));
    }
    fin.close();
    fout.close();


    sample(dimension, sampleFile, max_norm);

    for (auto& v : data) {
        delete v;
    }

    return 0;
}
