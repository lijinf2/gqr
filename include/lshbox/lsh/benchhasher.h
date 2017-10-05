#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
#include <cmath>
#include <lshbox/matrix.h>
#include <lshbox/eval.h>
#include <lshbox/utils.h>
#include "probing.h"
#include "hasher.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::ifstream;
using std::istringstream;
namespace lshbox
{

template<typename DATATYPE = float>
class BenchHasher : public Hasher <DATATYPE>
{
public:

    typedef typename Hasher<DATATYPE>::BIDTYPE BIDTYPE;

    BenchHasher() : Hasher<DATATYPE>() {};

    vector<bool> getHashBits(unsigned tb, const DATATYPE *domin) override;

    void loadModel(const char* baseBitsFile,  const char* queryBitsFile, const Matrix<DATATYPE>& query, const Benchmark& bench); 

private:
    unordered_map<const DATATYPE*, vector<bool>> queryBits;
    int idxToQueryBits = 0;
};

template<typename DATATYPE>
vector<bool> BenchHasher<DATATYPE>::getHashBits(unsigned tb, const DATATYPE *domin) {
    return queryBits[domin];
}

template<typename DATATYPE>
void BenchHasher<DATATYPE>::loadModel(const char* baseBitsFile, const char* queryBitsFile, const Matrix<DATATYPE>& query, const Benchmark& bench) {
    vector<bool> record;
    string line;
    int tmp;
    int codelength = 0;
    ifstream baseFin(baseBitsFile);
    if (!baseFin) {
        std::cout << "cannot open file " << baseBitsFile << std::endl;
        assert(false);
    }

    // initialized tables
    int itemIdx = 0;
    unordered_map<BIDTYPE, vector<unsigned>> tb;
    while (getline(baseFin, line)) {
        if (codelength == 0) {
            for (int i = 0; i < line.size(); ++i) {
                if (line[i] == '1' || line[i] == '0') {
                    codelength++;
                }
            }
            record.resize(codelength);
        }
        istringstream iss(line);
        for (int i = 0; i < codelength; ++i) {
            iss >> tmp;
            if (tmp == 1) record[i] = 1;
            else if(tmp == 0 || tmp == -1) record[i] = 0;
            else assert(false);
        }
        BIDTYPE hashVal = this->bitsToBucket(record);
        if (tb.find(hashVal) == tb.end())
            tb[hashVal] = vector<unsigned>();
        tb[hashVal].push_back(itemIdx);
        itemIdx++;
    }
    this->tables.emplace_back(tb);
    this->numItems = itemIdx;
    baseFin.close();

    ifstream queryFin(queryBitsFile);
    if (!queryFin) {
        std::cout << "cannot open file " << queryBitsFile << std::endl;
        assert(false);
    }

    for (int count = 0; count < bench.getQ(); ++count) {
        getline(queryFin, line);
        istringstream iss(line);
        for (int i = 0; i < codelength; ++i) {
            iss >> tmp;
            if (tmp == 1) record[i] = 1;
            else if(tmp == 0 || tmp == -1) record[i] = 0;
            else assert(false);
        }
        this->queryBits[query[bench.getQuery(count)]] = record;
    }
    queryFin.close();
 
}
}
