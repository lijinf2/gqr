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
#include "transform.h"
#include <climits>

using namespace std;

template <typename T>

void freeVectors(vector<T*>& data) {
    for (int i = 0; i < data.size(); ++i) {
        delete[] data[i];
    }
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

    Transform transformer;

    vector<float* > data ;
    vector<float* > queryData ;
    int dimension;
    int queryDimension;
    int m = 3;
    int U = 0.83;

    int placeholder = 2+2+m;

    dimension = transformer.loadData(inputFile, data, placeholder);
    queryDimension = transformer.loadData(inputSampleFile, queryData, placeholder);

    if(dimension<0 || queryDimension<0 || dimension!=queryDimension) {
        std::cout << "dimension un_compatible" << std::endl
                  << "data dim:" << dimension << std::endl
                  << "query dim:" << queryDimension << std::endl;
        assert(false);
    }


    string e2m("e2m");
    string m2a("m2a");
    string alsh("alsh");

    for (int i = transform_arg; i < argc; ++i) {

        string operation(argv[i]);

        if (e2m==operation) {
            dimension = transformer.euclidToMIP(data, queryData, dimension);
        } else if (m2a==operation) {
            dimension = transformer.mipToAngular(data, queryData, dimension);
        } else if (alsh==operation) {
            dimension = transformer.mipToEuclid(data, queryData, dimension, m, U);
        }
        else {
            std::cout << "invalid operation " << operation << std::endl;
        }
    }

    transformer.dumpData(outputFile, data, dimension);
    transformer.dumpData(outputSampleFile, queryData, dimension);

    freeVectors(data);
    freeVectors(queryData);

    return 0;
}


