#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
int main(int argc, char ** argv) {
    ifstream fin(argv[1]);
    if (!fin) {
        cout << "cannot open file " << argv[1] << endl;
    }
    ofstream fout(argv[2]);
    if (!fout) {
        cout << "cannot open file " << argv[2] << endl;
    }
    string line;
    string word;
    int dimension = 0;
    float* buffer = NULL;
    while(getline(fin, line)) {
        if (dimension == 0) {
            istringstream countIss(line);
            countIss >> word;
            string tmp;
            while(countIss >> tmp) {
                dimension++;
            }
            buffer = new float[dimension];
        } 

        istringstream iss(line);
        iss >> word;
        for (int i = 0; i < dimension; ++i) {
            iss >> buffer[i];
        }
        fout.write((char*)&dimension, sizeof(int));
        fout.write((char*)buffer, sizeof(float) * dimension);
    }
    delete buffer;
    fin.close();
    fout.close();
    return 0;
}
