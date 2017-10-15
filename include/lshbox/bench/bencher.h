/*
 * Bencher loads benchfile.lshbox and provides evaluations
 * */
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include "benchrecord.h"
using std::vector;
using std::pair;
using std::cout;
using std::endl;
using std::ifstream;

class Bencher {
public:
    Bencher (const char* benchFile) {
        ifstream fin(benchFile);
        if (!benchFile) {
            cout << "cannot open benchFile: " << benchFile << endl;
        }
        int numQueries;
        int K;

        fin >> numQueries >> K;
        nns.reserve(numQueries);

        unsigned qid;
        unsigned nbid;
        float dist;
        for (int i = 0; i < numQueries; ++i) {
            fin >> qid;
            BenchRecord record(qid);
            record.reserve(K);
            for (int j = 0; j < K; ++j) {
                fin >> nbid >> dist;
                record.add(nbid, dist);
            }
            nns.emplace_back(record);
        }
        fin.close();
    }

    Bencher (const vector<vector<pair<unsigned, float>>>& source) {
        int numQueries = source.size();
        nns.reserve(numQueries);

        unsigned qid;
        unsigned nbid;
        float dist;
        for (int i = 0; i < numQueries; ++i) {
            auto src = source[i];
            qid = i;
            BenchRecord record(qid);

            // sort record by float
            std::sort(src.begin(), src.end()
                , [](const pair<unsigned, float>&a, const pair<unsigned, float>&b ) {
                    return a.second < b.second;
                });

            int K = src.size();
            for (int idx = 0; idx < K; ++idx) {
                nbid =  src[idx].first;
                dist = src[idx].second;
                record.add(nbid, dist);
            }
            nns.emplace_back(record);
        }
    }

    const BenchRecord& getRecord(unsigned qId) const {
        return this->nns[qId];
    }

    unsigned size() const {
        return this->nns.size();
    }

    float avg_recall(const Bencher& given) const {
        assert(this->nns.size() == given.size());

        float sumRecall = 0;
        for (int i = 0; i < this->nns.size(); ++i) {
            const BenchRecord& givenRecord = given.getRecord(i);
            sumRecall += this->nns[i].recall(givenRecord);
        }
        return sumRecall / this->nns.size();
    }

    float avg_error(const Bencher& given) const {
        assert(this->nns.size() == given.size());
        float sumError = 0;
        int count = 0;
        for (int i = 0; i < this->nns.size(); ++i) {
            const BenchRecord& givenRecord = given.getRecord(i);;
            float er = this->nns[i].error(givenRecord);
            if (er >= 1){ 
                sumError += er;
                count++;
            }
        }
        if (count == 0) return -1;
        else return sumError / count;
    }
private:
    vector<BenchRecord> nns;
};
