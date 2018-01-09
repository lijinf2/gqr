#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <chrono>
using namespace std;
int main (int argc, char** argv) {
    if (argc != 6) {
        cout << "Usage: random.bin ${outputFile} ${numRecords} ${dimension} ${mean} ${standard deviation}" << endl;
        return 0;
    }
    const char* outputFile = argv[1];
    int numRecord = atoi(argv[2]);
    int dimension = atoi(argv[3]);
    int mean = atoi(argv[4]);
    int stdDev = atoi(argv[5]);

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    normal_distribution<float> mean_distribution(mean, stdDev);
    
    ofstream fout(outputFile, ios::binary);
    float number;
    for (int i = 0; i < numRecord; ++i) {
        fout.write((char*)&dimension, sizeof(dimension));

        float record_mean = mean_distribution(generator);
        normal_distribution<float> distribution(record_mean, stdDev);
        

        for (int j = 0; j < dimension; ++j) {
            number = distribution(generator);
            fout.write((char*)&number, sizeof(number));
        }
    }
    fout.close();
}
