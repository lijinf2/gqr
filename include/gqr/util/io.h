#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;

namespace lshbox{
vector<float> readFloatVectorFromLine(const string& line, int size) {
    vector<float> vec(size);
    istringstream iss(line);
    for (int i = 0; i < vec.size(); ++i) {
        iss >> vec[i];
    }
    return vec;
}
};
