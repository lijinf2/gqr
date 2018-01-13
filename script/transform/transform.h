#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;
class Transform {
public:
    int euclidToMIP(vector<float* >& data, vector<float* >& queryData, int dimension);

    int mipToAngular(vector<float* >& data, vector<float* >& queryData, int dimension);

    /**
     * load data from fvecs file.
     * @param file
     * @param data
     * @param placeholder
     * @return
     */
    int loadData(const char* file, vector<float*>& data, int placeholder);

    /**
     * write data to binary file as file.fvecs format.
     * @param file
     * @param data
     * @param dimension
     * @return
     */
    int dumpData(const char* file, vector<float*>& data, int dimension);

    /**
     * write data to text file.
     * @param file
     * @param data
     * @param dimension
     * @return
     */
    int dumpText(const char* file, vector<float*>& data, int dimension);

private:
    float calNormSquare(float* data, int dimension);

    float calMaxNormSquare(vector<float* >& data, int dimension);

    void scale(vector<float* >& data, vector<float* >& queryData, int dimension, float scalar);
};

int Transform::euclidToMIP(vector<float* >& data, vector<float* >& queryData, int dimension) {

    float dataMaxNormSquare = calMaxNormSquare(data, dimension);
    float queryMaxNormSquare = calMaxNormSquare(queryData, dimension);
    float max_norm_square = std::max(dataMaxNormSquare, queryMaxNormSquare);
    float scalar = sqrt(max_norm_square);

    scale(data, queryData, dimension, scalar);

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);
        buffer[dimension] = norm_square;
        buffer[dimension+1] = -0.5f;
    }

    for (int j = 0; j < queryData.size(); ++j) {
        float * buffer = queryData[j];
        float norm_square = calNormSquare(buffer, dimension);
        buffer[dimension+1] = norm_square;
        buffer[dimension] = -0.5f;
    }

    return dimension+2;
}

int Transform::mipToAngular(vector<float* >& data, vector<float* >& queryData, int dimension) {

    float dataMaxNormSquare = calMaxNormSquare(data, dimension);
    float queryMaxNormSquare = calMaxNormSquare(queryData, dimension);
    float max_norm_square = std::max(dataMaxNormSquare, queryMaxNormSquare);

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);

        buffer[dimension] = sqrt(max_norm_square - norm_square);
        buffer[dimension+1] = 0.0f;
    }

    for (int j = 0; j < queryData.size(); ++j) {
        float * buffer = queryData[j];
        float norm_square = calNormSquare(buffer, dimension);

        buffer[dimension] = 0.0f;
        buffer[dimension+1] = sqrt(max_norm_square - norm_square);
    }

    float scalar = sqrt(max_norm_square);
    int newDimension = dimension + 2;
    scale(data, queryData, newDimension, scalar);
    return newDimension;
}

int Transform::loadData(const char* file, vector<float*>& data, int placeholder) {

    ifstream fin(file, ios::binary);
    if (!fin) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }

    int originDim;
    while (fin.read((char*)(&originDim), sizeof(int))) {

        float * buffer = new float[originDim + placeholder];
        fin.read((char *)(buffer), sizeof(float) * originDim);
        data.push_back(buffer);
    }

    fin.close();
    return originDim;
}

int Transform::dumpData(const char* file, vector<float*>& data, int dimension) {
    ofstream fout(file, ios::binary);
    if (!fout) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }

    for (int i = 0; i < data.size(); ++i)
    {
        fout.write((char*)&dimension, sizeof(int));
        fout.write((char*)data[i], dimension * sizeof(float));
    }

    fout.close();
    return dimension;
}

int Transform::dumpText(const char* file, vector<float*>& data, int dimension) {

    ofstream fout(file);
    if (!fout) {
        std::cout << "cannot open file " << file << std::endl;
        assert(false);
    }

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];

        for (int j = 0; j < dimension; ++j) {
            fout << buffer[j] << "\t";
        }
        fout << endl;
    }

    fout.close();
}

float Transform::calNormSquare(float* data, int dimension) {
    float norm_square = 0.0f;
    for (int index_dim = 0; index_dim < dimension; index_dim++) {
        norm_square += data[index_dim] * data[index_dim];
    }
    return norm_square;
}

float Transform::calMaxNormSquare(vector<float* >& data, int dimension) {

    float max_norm_square = 0.0f;

    for (int i = 0; i < data.size(); ++i) {
        float * buffer = data[i];
        float norm_square = calNormSquare(buffer, dimension);
        if (norm_square > max_norm_square)
            max_norm_square = norm_square;
    }

    return max_norm_square;
}

void Transform::scale(vector<float* >& data, vector<float* >& queryData, int dimension, float scalar) {
    for (int line = 0; line < data.size(); ++line) {
        for (int dim = 0; dim < dimension; ++dim) {
            data[line][dim] /= scalar;
        }

    }

    for (int line = 0; line < queryData.size(); ++line) {
        for (int dim = 0; dim < dimension; ++dim) {
            queryData[line][dim] /= scalar;
        }
    }
}
