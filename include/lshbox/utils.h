#include <iostream>
#include <string>
#include <utility>
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
