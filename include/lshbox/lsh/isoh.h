#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <eigen/Eigen/Dense>
#include <cmath>
#include <lshbox/utils.h>
#include "hasher.h"
#include "pcarr.h"

using std::vector;
using std::string;
namespace lshbox
{

template<typename DATATYPE = float>
class IsoH: public PCARR<DATATYPE>
{
public:
    IsoH() : PCARR<DATATYPE>() {};
};
}
