#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <fstream>
#include <utility>
#include <lshbox.h>
#include <random>
#include <thread>
#include <functional>
#include <lshbox/matrix.h>
#pragma once
using std::vector;
using std::string;
using std::unordered_map;

/* select k elements of n elements
 * @n: total number of elements 
 * @k: number of selected elements
 * @return vector of bool, consisting of n elements and k of them are true. 
 * */

vector<bool> selection(unsigned n, unsigned k) {
    std::hash<std::thread::id> hasher;
    std::thread::id this_id = std::this_thread::get_id();
    std::mt19937 rng((unsigned) hasher(this_id) + std::time(0));
    std::uniform_int_distribution<unsigned> usBits(0, n - 1);
    usBits(rng);
    vector<bool> selected(n, 0);
    unsigned numSelected = 0;

    for (unsigned idxToSelected = 0; idxToSelected < selected.size(); ++idxToSelected) {
        if (usBits(rng) < k) {
            selected[idxToSelected] = true;
            numSelected++;
        }

        if (numSelected == k) {
            break;
        }
    }

    // if have not selected enough items
    while (numSelected < k) {
        unsigned target = usBits(rng);
        while (selected[target] == true) {
            target = usBits(rng);
        }
        selected[target] = true;
        numSelected++;
    }

    return selected;
}

template<typename ScannerT, typename AnswerT>
bool setStat(
        ScannerT queryScanner, 
        const AnswerT& ans, 
        lshbox::Stat& recall,
        lshbox::Stat& error) {

    auto& queryTopk = queryScanner.getMutableTopk();
    queryTopk.genTopk(); // must getTopk for scanner, other wise will wrong
    float thisRecall = ans.recall(queryTopk);
    float thisError = ans.error(queryTopk);

    recall << thisRecall;
    if (thisError >= 1) {
        error << thisError;
    }

    if (thisRecall > 0.9999) return true;
    else return false;
}

namespace lshbox {

unordered_map<string, string> parseParams(int argc, const char** argv) {
    unordered_map<string, string> params;
    for (int i = 1; i < argc; ++i) {
        const char* pair = argv[i];
        int length = strlen(pair);
        int sepIdx = -1;
        for (int idx = 0; idx < strlen(pair); ++idx) {
            if (pair[idx] == '=')
                sepIdx = idx;
        }
        if (strlen(pair) < 3 || pair[0] != '-' || pair[1] != '-' || sepIdx == -1) {
            std::cout << "arguments error, format should be --[key]=[value]" << std::endl;
            assert(false);
        }
        string key(pair, 2, sepIdx - 2);
        string value(pair + sepIdx + 1);
        params[key] = value;
    }
    return params;
}

template<typename DATATYPE>
void loadFvecs(Matrix<DATATYPE>& data, const string& dataFile) {
    std::ifstream fin(dataFile.c_str(), std::ios::binary | std::ios::ate);
    if (!fin) {
        std::cout << "cannot open file " << dataFile.c_str() << std::endl;
        assert(false);
    }
    unsigned fileSize = fin.tellg();
    fin.seekg(0, fin.beg);
    assert(fileSize != 0);

    int dimension;
    fin.read((char*)&dimension, sizeof(int));
    unsigned bytesPerRecord = dimension * sizeof(DATATYPE) + 4;
    assert(fileSize % bytesPerRecord == 0);
    int cardinality = fileSize / bytesPerRecord;

    data.reset(dimension, cardinality);
    fin.read((char *)(data.getData()), sizeof(float) * dimension);

    int dim;
    for (int i = 1; i < cardinality; ++i) {
        fin.read((char*)&dim, sizeof(int));
        assert(dim == dimension);
        fin.read((char *)(data.getData() + i * dimension), sizeof(float) * dimension);
    }
    fin.close();
}

/*
 * padding meaningless euclidean distance
 * */
string genBenchFromIvecs(const char* ivecBenchFile, int numQueries, int topK) {
    std::ifstream fin(ivecBenchFile, std::ios::binary);
    if (!fin) {
        std::cout << "cannot open file " << ivecBenchFile << std::endl;
        assert(false);
    }
    std::vector<vector<int>> bench;
    bench.resize(numQueries);

    for (int i = 0; i < numQueries; ++i) {
        int length;
        fin.read((char*)&length, sizeof(int));
        assert(length >= topK);
        int nnIdx;
        for (int j = 0; j < length; ++j ) {
            fin.read((char*)&nnIdx, sizeof(int));
            if (j < topK) 
                bench[i].push_back(nnIdx);
        }
    }
    fin.close();

    string lshBenchFile = string(ivecBenchFile) + ".lshbox";
    std::ofstream fout(lshBenchFile.c_str());
    if (!fout) {
        std::cout << "cannot create file " << ivecBenchFile << std::endl;
        assert(false);
    }
    fout << bench.size() << " " << bench[0].size() << std::endl;
    for (int i = 0; i < bench.size(); ++i) {
        fout << i ;
        for (int j = 0; j < bench[i].size(); ++j) {
            fout << "\t" << bench[i][j] << " " << j;
        }
        fout << std::endl;
    }
    fout.close();
    return lshBenchFile;
}

vector<bool> to_bits (unsigned long long num)
{
    vector<bool> bits;
    while(num > 0 ){
        bits.push_back(num % 2);
        num /= 2;
    }
    return bits;
}

unsigned countOnes(unsigned long long xorVal) {
    unsigned hamDist = 0;
    while(xorVal != 0){
        hamDist++;
        xorVal &= xorVal - 1;
    }
    return hamDist;
}
};

namespace std {
    template<typename FIRST, typename SECOND>
        std::string to_string(const vector<std::pair<FIRST, SECOND>>& vec){
            std::string str = "";
            for(int i = 0; i < vec.size(); ++i){
                str += "<" + std::to_string(vec[i].first)
                    + "\t" + std::to_string(vec[i].second) + ">, ";
            }
            return str;
        }
    template<typename T>
        std::string to_string(const vector<T>& vec){
            std::string str = "";
            for(int i = 0; i < vec.size(); ++i){
                str += std::to_string(vec[i]) + ", ";
            }
            return str;
        }

};
