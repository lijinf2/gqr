#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include <chrono>
#include <vector>
#include "lshbox/utils.h"
using namespace std;

#define RealT float

int main (int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: transform.bin ${inputFile} ${outputFile} " << endl;
        return 0;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];


    ofstream fout(outputFile);
    if (!fout) {
        std::cout << "cannot open file " << outputFile << std::endl;
        assert(false);
    }
    ifstream fin(inputFile);
    if (!fin) {
        std::cout << "cannot open file " << inputFile << std::endl;
        assert(false);
    }


    int dimension;

    while (fin.read((char*)(&dimension), sizeof(int))) {

        RealT * buffer = new RealT[dimension];
        fin.read((char *)(buffer), sizeof(RealT) * dimension);

        for (int i = 0; i < dimension; ++i)
        {
            fout << buffer[i] << "\t";
        }
        fout << endl;
    }


    fin.close();
    fout.close();

    return 0;
}
