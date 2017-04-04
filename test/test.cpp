#include <fstream>
#include <iostream>
#include <vector>
#include <map>
using namespace std;
typedef unsigned long long BIDTYPE;
int main() {
    // char filename[] = "/data/jinfeng/project/github/LSHBOX/release/bin/darwin/gist/gist-20bit-50iter.itq";
    char filename[] = "/data/jinfeng/project/github/LSHBOX/release/bin/darwin/cifar-10/cifar10-32bit-50iter.itq";
    ifstream in(filename, ios::binary);

    if (!in) {
        cout << "cannot open file " << filename << endl;
        return 0;
    }
    unsigned M;
    unsigned L;
    unsigned D;
    unsigned N;
    unsigned S;
    std::vector<std::vector<unsigned> > rndArray;
    std::vector<std::map<BIDTYPE, std::vector<unsigned> > > tables;
    std::vector<std::vector<std::vector<float> > > pcsAll;
    std::vector<std::vector<std::vector<float> > > omegasAll;
    in.read((char*)&M, sizeof(unsigned));
    in.read((char*)&L, sizeof(unsigned));
    in.read((char*)&D, sizeof(unsigned));
    in.read((char*)&N, sizeof(unsigned));
    in.read((char*)&S, sizeof(unsigned));
    tables.resize(L);
    rndArray.resize(L);
    pcsAll.resize(L);
    omegasAll.resize(L);
    for (unsigned i = 0; i != L; ++i)
    {
        rndArray[i].resize(N);
        in.read((char *)&rndArray[i][0], sizeof(unsigned) * N);
        unsigned count;
        in.read((char *)&count, sizeof(unsigned));
        for (unsigned j = 0; j != count; ++j)
        {
            BIDTYPE target;
            in.read((char *)&target, sizeof(BIDTYPE));
            // assert(target < 4.294967296e9);

            // unsigned fourByteTmp;
            // in.read((char *)&fourByteTmp, sizeof(fourByteTmp));
            // BIDTYPE target;
            // target = fourByteTmp;

            unsigned length;
            in.read((char *)&length, sizeof(unsigned));
            tables[i][target].resize(length);
            in.read((char *) & (tables[i][target][0]), sizeof(unsigned) * length);
        }
        pcsAll[i].resize(N);
        omegasAll[i].resize(N);
        for (unsigned j = 0; j != N; ++j)
        {
            pcsAll[i][j].resize(D);
            omegasAll[i][j].resize(N);
            in.read((char *)&pcsAll[i][j][0], sizeof(float) * D);
            in.read((char *)&omegasAll[i][j][0], sizeof(float) * N);
        }
    }
    in.close();
    cout << M << endl;
    cout << L << endl;
    cout << D << endl;
    cout << N << endl;
    cout << S << endl;
}
