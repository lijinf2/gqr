#include <lshbox/matrix.h>
#include <vector>
#include <cmath>
using std::vector;
template<typename DATATYPE>
float calProduct(const DATATYPE* vec1, const DATATYPE* vec2, int dimension) {
    float product = 0;
    for (int i = 0; i < dimension; ++i) {
        product = vec1[i] * vec2[i];
    }
    return product;
}
