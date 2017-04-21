#include <iostream>
#include <string>
#include <utility>
#include <lshbox.h>
#pragma once
template<typename ScannerT, typename AnswerT>
void setStat(
        // lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner, 
        ScannerT scanner, 
        const AnswerT& ans, 
        lshbox::Stat& recall,
        lshbox::Stat& precision) {

    scanner.topk().genTopk(); // must getTopk for scanner, other wise will wrong
    float thisRecall = scanner.topk().recall(ans);

    float matched = thisRecall * (scanner.getK() - 1); 
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
