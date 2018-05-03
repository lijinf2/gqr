#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <utility>
#include <lshbox.h>
#include <random>
#include <thread>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <lshbox/matrix.h>

#include "gqr/util/random.h"
using std::vector;
using std::string;
using std::unordered_map;
using std::unordered_set;

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
