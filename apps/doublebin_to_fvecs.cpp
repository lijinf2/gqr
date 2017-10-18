#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
using namespace std;
int main(int argc, char** argv) {
    if (argc <= 1) {
        cout << "Usage: doublebin_to_fvecs binary_file_path output_fvecs_file_path dimension" << endl;
        return -1;
    }
    const char* binaryPath = argv[1];
    const char* outputPath = argv[2];
    int dimension = atoi(argv[3]);

    ifstream fin(binaryPath, ios::binary);
    if (!fin) {
        cout << "cannot open file " << binaryPath << endl;
        return -1;
    }

    ofstream fout(outputPath, ios::binary);
    if (!fout) {
        cout << "cannot create file " << outputPath << endl;
        return -1;
    }

    double* buffer = new double[dimension];
    float floatWord;
    while(fin.read((char*)buffer, sizeof(double) * dimension)) {
        fout.write((char*)(&dimension), sizeof(int));
        for(int i = 0; i < dimension; ++i) {
            floatWord = (float) buffer[i];
            fout.write((char*)(&floatWord), sizeof(float));
        }
    }

    fin.close();
    fout.close();
    delete buffer;
    return 0;
}
