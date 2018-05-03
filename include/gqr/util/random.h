#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <random>
using std::vector;
using std::string;
using std::unordered_map;
using std::unordered_set;
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

// return k index that are sampled, index ranges from 0 to n - 1
unordered_set<unsigned> sampleRand(unsigned n, unsigned k, int seed = 0) {

    std::default_random_engine generator(seed);
    std::uniform_int_distribution<unsigned> distribution(0, n - 1);

    unordered_set<unsigned> s;

    for (int idx = 0; idx < n; ++idx) {
        // if selected
        if (distribution(generator) < k) {
            s.insert(idx);
        }

        if (s.size() == k) {
            break;
        }
    }

    // if have not selected enough items
    while (s.size() < k) {
        unsigned target = distribution(generator);
        while (s.find(target) != s.end()) {
            target = distribution(generator);
        }
        s.insert(target);
    }

    return s;
}
