#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>

#include "lshbox/utils.h"
using namespace std;

void alertOutputFormatError() {
    cout << "output format error!" << std::endl;
    cout << "we currently only support outputs in fvecs or idfvecs" << std::endl;
    cout << "please provide a output path that ends with fvecs or idfvecs such as data_query.fvecs" << std::endl;
}

int main(int argc, char ** argv) {
    if (argc != 4) {
        cout << "Usage: ./sample_queries input_base_fvecs num_queries output_query_fvecs" << endl;
    }

    string outputFile = argv[3];
    if (outputFile.size() < 7 
        || (outputFile.substr(outputFile.size() - 5, 5) != "fvecs" && outputFile.substr(outputFile.size() - 7, 7) != "idfvecs")) {
        alertOutputFormatError();
    }

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
    ofstream fout(outputFile.c_str(), ios::binary);
    if (!fout) {
        cout << "cannot open file " << argv[3] << endl;
    }

    string logFileName(outputFile.c_str());
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

            if (outputFile.substr(outputFile.size() - 7, 7) == "idfvecs") {
                fout.write((char*)&i, sizeof(int));
            }
            fout.write((char*)&dimension, sizeof(int));
            fout.write((char*)data[i], dimension * sizeof(float));
        }
    }
    cout << "sampled queries are written into " << outputFile << endl;
    cout << "mappings are written into " << logFileName << endl;
    for (auto& v : data) {
        delete v;
    }
    fout.close();
    fin.close();
    indexFout.close();
    return 0;
}
