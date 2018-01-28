#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>

#include "lshbox/utils.h"
using namespace std;
int main(int argc, char ** argv) {
    if (argc != 5) {
        cout << "Usage: ./sample_queries input_fvecs num_queries output_query_fvecs output_base_fvecs" << endl;
        return 0;
    }

    const char* outputFile = argv[3];
    const char* outputBaseFile = argv[4];
    ifstream fin(argv[1], ios::binary);
    if (!fin) {
        cout << "cannot open file " << argv[1] << endl;
    }

    int dimension;
    vector<float*> data;
    while (fin.read((char*)(&dimension), sizeof(int))) {
        float* buffer = new float[dimension];
        fin.read((char*)buffer, dimension * sizeof(float));
        data.push_back(buffer);
    }
    int num_samples = atoi(argv[2]);

    vector<bool> selected = selection(data.size(), num_samples);
    ofstream fout(outputFile, ios::binary);
    if (!fout) {
        cout << "cannot open file " << argv[3] << endl;
        return 0;
    }

    ofstream baseFout(outputBaseFile, ios::binary);
    if (!baseFout) {
        cout << "cannot open file " << argv[3] << endl;
        return 0;
    }
    string logFileName(outputFile);
    logFileName += ".idx.txt";
    ofstream indexFout(logFileName.c_str());
    cout << "selected items: " << endl;
    indexFout << "selected items: " << endl;
    int queryIdx = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (selected[i]) {
            cout << queryIdx << " -> " << i << endl;
            indexFout << queryIdx << " -> " << i << endl;
            queryIdx++;

            fout.write((char*)&dimension, sizeof(int));
            fout.write((char*)data[i], dimension * sizeof(float));
        } else {
            baseFout.write((char*)&dimension, sizeof(int));
            baseFout.write((char*)data[i], dimension * sizeof(float));
        }
    }
    for (auto& v : data) {
        delete v;
    }
    fout.close();
    baseFout.close();
    fin.close();
    indexFout.close();
    return 0;
}
