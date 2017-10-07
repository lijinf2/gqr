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

    void loadModel(const string& modelFile, const string& baseBitsFile,  const string& queryBitsFile, const Matrix<DATATYPE>& query, const Benchmark& bench); 

private:
    vector<unordered_map<const DATATYPE*, vector<bool>>> queryBits;
    int idxToQueryBits = 0;
};

template<typename DATATYPE>
vector<bool> BenchHasher<DATATYPE>::getHashBits(unsigned tb, const DATATYPE *domin) {
    return queryBits[tb][domin];
}

template<typename DATATYPE>
void BenchHasher<DATATYPE>::loadModel(const string& modelFile, const string& baseBitsFile, const string& queryBitsFile, const Matrix<DATATYPE>& query, const Benchmark& bench) {
    string line;
    // initialized statistics
    ifstream modelFin(modelFile.c_str());
    if (!modelFin) {
        std::cout << "cannot open file " << modelFile << std::endl;
        assert(false);
    }
    getline(modelFin, line);
    istringstream statIss(line);
    int numTables, tableDim, tableCodelen, tableNumItems, tableNumQueries;
    statIss >> numTables >> tableDim >> tableCodelen >> tableNumItems >> tableNumQueries;
    modelFin.close();
    
    // initialized numTotalItems,tables and queryBits
    this->initBaseHasher(baseBitsFile, numTables, tableNumItems, tableCodelen);

    int tmp;
    vector<bool> record(tableCodelen);
    ifstream queryFin(queryBitsFile.c_str());
    if (!queryFin) {
        std::cout << "cannot open file " << queryBitsFile << std::endl;
        assert(false);
    }

    this->queryBits.resize(numTables);
    for (int tb = 0; tb < numTables; ++tb) {
        for (int count = 0; count < bench.getQ(); ++count) {
            getline(queryFin, line);
            istringstream iss(line);
            for (int i = 0; i < tableCodelen; ++i) {
                iss >> tmp;
                if (tmp == 1) record[i] = 1;
                else if(tmp == 0 || tmp == -1) record[i] = 0;
                else assert(false);
            }
            this->queryBits[tb][query[bench.getQuery(count)]] = record;
        }
    }
    queryFin.close();
}
}
