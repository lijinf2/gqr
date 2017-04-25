#include <iostream>
#include <string>
#include <utility>
#include <lshbox.h>
#include <random>
#include <thread>
#include <functional>
#pragma once

/* select k elements of n elements
 * @n: total number of elements 
 * @k: number of selected elements
 * @return vector of bool, consisting of n elements and k of them are true. 
 * */

std::vector<bool> selection(unsigned n, unsigned k) {
    std::hash<std::thread::id> hasher;
    std::thread::id this_id = std::this_thread::get_id();
    std::mt19937 rng((unsigned) hasher(this_id));
    std::uniform_int_distribution<unsigned> usBits(0, n - 1);
    usBits(rng);
    std::vector<bool> selected(n, 0);
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
void setStat(
        // lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner, 
        ScannerT scanner, 
        const AnswerT& ans, 
        lshbox::Stat& recall,
        lshbox::Stat& precision) {

    scanner.topk().genTopk(); // must getTopk for scanner, other wise will wrong
    float thisRecall = scanner.topk().recall(ans);

    float matched = thisRecall * scanner.getK(); 
    float thisPrecision;
    assert(scanner.cnt() > 0);
    if(scanner.cnt() == 1)
        thisPrecision = 0;
    else
        thisPrecision = matched / (scanner.cnt() - 1);

    recall << thisRecall;
    precision << thisPrecision;

    return;
}

namespace std {
    template<typename FIRST, typename SECOND>
        std::string to_string(const std::vector<std::pair<FIRST, SECOND>>& vec){
            std::string str = "";
            for(int i = 0; i < vec.size(); ++i){
                str += "<" + std::to_string(vec[i].first)
                    + "\t" + std::to_string(vec[i].second) + ">, ";
            }
            return str;
        }
    template<typename T>
        std::string to_string(const std::vector<T>& vec){
            std::string str = "";
            for(int i = 0; i < vec.size(); ++i){
                str += std::to_string(vec[i]) + ", ";
            }
            return str;
        }

};
