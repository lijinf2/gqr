#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include "lshbox/bench/bencher.h"
using namespace std;
using std::vector;
using std::pair;

/*
 * transform square distance from OPQ to Euclidean distance
 * */
Bencher opq_to_bencher(const vector<vector<pair<float, int>>>& result, bool formatted = false) {
    vector<vector<pair<unsigned, float>>> target;
    for (int i = 0; i < result.size(); ++i) {
        const vector<pair<float, int>>& src = result[i];

        vector<pair<unsigned, float>> dst;
        dst.resize(src.size());
        for (int idx = 0; idx < src.size(); ++idx) {
            dst[idx].first = src[idx].second;
            dst[idx].second = sqrt(src[idx].first);
        }

        // sort dst
        std::sort(dst.begin(), dst.end(),
                [](const pair<unsigned, float>& a, const pair<unsigned, float>&b) {
                if (fabs(a.second - b.second) > 0.00001)
                return a.second < b.second;
                else 
                return a.first < b.first; 
                });

        target.emplace_back(dst);
    }
    return Bencher(target, true);
}

float cal_opq_avg_error(const Bencher& bench, const vector< vector< pair<float, int> > >& result, bool formatted = false) {
    Bencher res = opq_to_bencher(result, formatted);
    return bench.avg_error(res);
}

float cal_opq_avg_recall(const Bencher& bench, const vector< vector< pair<float, int> > >& result, bool formatted = false) {
    Bencher res = opq_to_bencher(result, formatted);
    return bench.avg_recall(res);
}

// Bencher to_bencher(const vector<vector<pair<float, unsigned>>>& results, bool formatted) {
//     assert(formatted == true);
//     vector<vector<pair<unsigned, float>>> target;
//     for (int i = 0; i < result.size(); ++i) {
//         const vector<pair<float, int>>& src = result[i];
//
//         vector<pair<unsigned, float>> dst;
//         dst.resize(src.size());
//         for (int idx = 0; idx < src.size(); ++idx) {
//             dst[idx].first = src[idx].second;
//             dst[idx].second = sqrt(src[idx].first);
//         }
//
//         // sort dst
//         std::sort(dst.begin(), dst.end(),
//                 [](const pair<unsigned, float>& a, const pair<unsigned, float>&b) {
//                 if (fabs(a.second - b.second) > 0.00001)
//                 return a.second < b.second;
//                 else 
//                 return a.first < b.first; 
//                 });
//
//         target.emplace_back(dst);
//     }
//     return Bencher(target, true);
// }

float cal_avg_error(const Bencher& bench, const vector<vector<pair<unsigned, float>>>& results, bool formatted) {
    assert(formatted == true);
    Bencher res(results, formatted);
    return bench.avg_error(res);
}

float cal_avg_recall(const Bencher& bench, const vector<vector<pair<unsigned, float>>>& results, bool formatted) {
    assert(formatted == true);
    Bencher res(results, formatted);
    return bench.avg_recall(res);
}
