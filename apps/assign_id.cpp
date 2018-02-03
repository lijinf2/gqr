#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>

//#include "lshbox/utils.h"
using namespace std;
int main(int argc, char ** argv) {
    if (argc != 3) {
        cout << "Usage: ./assign_id  input_base_fvecs output_base_fvecs" << endl;
    }

    const char* outputFile = argv[2];
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

    ofstream fout(outputFile, ios::binary);
    if (!fout) {
        cout << "cannot open file " << argv[2] << endl;
    }

    int count = 0;
    for (int i = 0; i < data.size(); ++i) {
        fout.write((char*)&count, sizeof(int));
        fout.write((char*)data[i], dimension * sizeof(float));
        count++;
    }
    for (auto& v : data) {
        delete v;
    }
    fout.close();
    fin.close();
    
    return 0;
}
