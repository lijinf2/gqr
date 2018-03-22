#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <lshbox.h>
#include <util/stat.h>
#include <lshbox/bench/bencher.h>
using namespace std;
int main(int argc, char** argv) {
    if (argc < 5) {
        cout << "Usage: mips_analyzer base_fvecs query_fvecs lshbox outputFile" << endl;
        return 0;
    }
    string dataFile(argv[1]);
    string queryFile(argv[2]);
    const char* benchFile = argv[3];
    const char* outputFile = argv[4];

    lshbox::Matrix<float> data(dataFile);
    vector<float> dataNorms = data.calNorms();

    lshbox::Matrix<float> query(queryFile);
    vector<float> queryNorms = query.calNorms();


    ofstream fout(outputFile);
    fout << "queryId ,nb_id nb_2norm nb_cos," << endl;

    Bencher bencher(benchFile);
    for (int i = 0; i < bencher.size(); ++i) {
        const BenchRecord& queryRecord = bencher.getRecord(i);
        const vector<pair<unsigned, float>>& nbDist = queryRecord.getKNN();
        unsigned queryid = queryRecord.getId();
        fout << queryid;
        for (int j = 0; j < nbDist.size(); ++j) {
            unsigned nbid = nbDist[j].first;
            float dotProduct = calProduct(data[nbid], query[queryid], data.getDim());
            fout << " ," << nbid 
                << " " << dataNorms[nbid] 
                << " " << dotProduct / dataNorms[nbid] / queryNorms[queryid];
        }
        fout << endl;
    }
    fout.close();
}
