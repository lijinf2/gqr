#pragma once
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using std::string;
using std::istringstream;
using std::vector;
namespace lshbox
{

template<typename DATATYPE = float>
class KNNGraphH: public Hasher<DATATYPE>
{
public:

    typedef typename Hasher<DATATYPE>::BIDTYPE BIDTYPE;

    KNNGraphH() : Hasher<DATATYPE>() {};

    vector<bool> getHashBits(unsigned k, const DATATYPE *domin) override;

    void loadModel(const string& modelFile); 

private:
    vector<bool> bucketToBits_(BIDTYPE bid);
};
}

template<typename DATATYPE>
vector<bool> lshbox::KNNGraphH<DATATYPE>::getHashBits(unsigned k, const DATATYPE *domin)
{

    // random sample a point
    std::default_random_engine generator((unsigned long) domin);
    std::uniform_int_distribution<unsigned> distribution(0, this->getBaseSize());

    unsigned root_ = distribution(generator); 

    return bucketToBits_(root_);
}

template<typename DATATYPE>
void lshbox::KNNGraphH<DATATYPE>::loadModel(const string& modelFile) {
    string line;
    // initialized statistics and model
    ifstream modelFin(modelFile.c_str());
    if (!modelFin) {
        std::cout << "cannot open file " << modelFile << std::endl;
        assert(false);
    }

    this->codelength = -1; // useless
    this->tables.resize(1);
    this->numTotalItems = 0;

    BIDTYPE src;
    unsigned dst;
    vector<unsigned> nbs;
    while(getline(modelFin, line)) {
        istringstream iss(line);
        iss >> src;
        while(iss >> dst) {
            nbs.push_back(dst);
        }
        assert(this->tables[0].find(src) == this->tables[0].end());
        this->tables[0][src] = nbs; 
        nbs.clear();
    };
    modelFin.close();
}

template<typename DATATYPE>
vector<bool> lshbox::KNNGraphH<DATATYPE>::bucketToBits_(BIDTYPE bid) {
    vector<bool> vec;
    BIDTYPE mask = 1;
    while(bid) {
        vec.push_back(bid & mask);
        bid >>= 1;
    }
    std::reverse(vec.begin(), vec.end());
    return vec;
}
