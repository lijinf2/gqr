#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
int main(int argc, char** argv) {
    if (argc <= 1) {
        cout << "Usage: bin_to_fvecs binary_file_path output_fvecs_file_path dimension" << endl;
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

    char* buffer = new char[sizeof(float) * dimension];
    while(fin.read(buffer, sizeof(float) * dimension)) {
        fout.write((char*)&dimension, sizeof(int));
        fout.write(buffer, sizeof(float) * dimension);
    }

    fin.close();
    fout.close();
    delete buffer;
    return 0;
}
