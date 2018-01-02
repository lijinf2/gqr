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
#include <lshbox/lsh/pcah.h>
#include "hasher.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::ifstream;
using std::istringstream;
namespace lshbox
{

template<typename DATATYPE = float>
    // symmetric inner product hasher
class SIMH: public PCAH<DATATYPE>
{

};

}

