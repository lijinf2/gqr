////////////////////////////////////////////////////////////////////////////// /// Copyright (C) 2014 Gefu Tang <tanggefu@gmail.com>. All Rights Reserved.  /// /// This file is part of LSHBOX.  /// /// LSHBOX is free software: you can redistribute it and/or modify it under /// the terms of the GNU General Public License as published by the Free /// Software Foundation, either version 3 of the License, or(at your option) /// any later version.  ///
/// LSHBOX is distributed in the hope that it will be useful, but WITHOUT
/// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
/// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
/// more details.
///
/// You should have received a copy of the GNU General Public License along
/// with LSHBOX. If not, see <http://www.gnu.org/licenses/>.
///
/// @version 0.1
/// @author Gefu Tang & Zhifeng Xiao
/// @date 2014.6.30
//////////////////////////////////////////////////////////////////////////////

/**
 * @file newitqlsh_test.cpp
 *
 * @brief Example of using Iterative Quantization LSH index for L2 distance.
 */
#include <lshbox.h>
#include <lshbox/lsh/newitqlsh.h>
#include <map>

int getMaxBucketSize(const std::map<unsigned, std::vector<unsigned> >& m){
    int max = 0;
    std::map<unsigned, std::vector<unsigned> >::const_iterator it;
    for (it = m.begin(); it != m.end(); ++it) {
        if (it->second.size() > max) {
            max = it->second.size();
        }
    }
    return max;
}
int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: ./newitqlsh_test data_file lsh_file benchmark_file" << std::endl;
        return -1;
    }
    std::cout << "Example of using Iterative Quantization" << std::endl << std::endl;
    typedef float DATATYPE;
    std::cout << "LOADING DATA ..." << std::endl;
    lshbox::timer timer;
    lshbox::Matrix<DATATYPE> data(argv[1]);
    std::cout << "LOAD TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "CONSTRUCTING INDEX ..." << std::endl;
    timer.restart();
    std::string file(argv[2]);

    bool use_index = false;
    if (argc >= 5) { 
        std::string cmd4_index(argv[4]);
        if (cmd4_index == "1") {
            use_index = true;
        }
    }
    std::cout << "use_index: " << use_index << std::endl;

    lshbox::newItqLsh<DATATYPE> mylsh;
    if (use_index)
    {
        mylsh.load(file);
    }
    else
    {
        lshbox::newItqLsh<DATATYPE>::Parameter param;
        param.M = 1; // number of buckets in a table, useless in this example
        param.L = 1;  // number of tables
        param.D = data.getDim();
        param.N = 16;  // number of bits
        param.S = 60000; // number of vectors
        param.I = 20;
        mylsh.reset(param);
        mylsh.train(data);
        mylsh.hash(data);
        mylsh.save(file);
    }
    std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "LOADING BENCHMARK ..." << std::endl;
    timer.restart();
    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), L2_DIST);
    lshbox::Benchmark bench;
    std::string benchmark(argv[3]);
    bench.load(benchmark);
    unsigned K = bench.getK();
    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
        accessor,
        metric,
        K
    );
    std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "RUNING QUERY ..." << std::endl;
    timer.restart();

    // cost : retrieved items / total items
    // recall: retrieved top-k answer / total top-k answer
    // precision: retrieved top-k answer / total retrieved answer

    // multiple-buckets probing
    lshbox::Stat cost, recall, precision;
    lshbox::progress_display pd(bench.getQ());
    int maxProbedBK = 16;
    if (argc >= 6)
        maxProbedBK = std::atoi(argv[5]);

    // mylsh.setMeanAndSTD(data);

    // for query multi-assign
    mylsh.rehash(data, maxProbedBK);
    for (unsigned i = 0; i != bench.getQ(); ++i)
    {
        scanner.reset(data[bench.getQuery(i)]);

        // mylsh.queryByLoss(data[bench.getQuery(i)], scanner, maxProbedBK, false); // multi-probe LSH
        // mylsh.queryRanking(data[bench.getQuery(i)], scanner, maxProbedBK);


        // multiple-assignment, together with rehash
        mylsh.queryRehash(data[bench.getQuery(i)], scanner); // multi-assighment LSH

        // parameter order wrong: float thisRecall = bench.getAnswer(i).recall(scanner.topk());
        // parameter order wrong: float thisPrecision = bench.getAnswer(i).precision(scanner.topk());
        float thisRecall = scanner.topk().recall(bench.getAnswer(i));

        // not true precision, library is wrong
        // float thisPrecision = scanner.topk().precision(bench.getAnswer(i));

        // modified version
        float matched = thisRecall * (K - 1); 
        float thisPrecision;
        assert(scanner.cnt() > 0);
        if(scanner.cnt() == 1)
            thisPrecision = 0;
        else
            thisPrecision = matched / (scanner.cnt() - 1);
        float thisCost = float(scanner.cnt()) / float(data.getSize());

        std::cout << "above is query " << i
            << ", recall " << thisRecall
            << ", precision " << thisPrecision
            << ", cost " << thisCost << std::endl << std::endl;
            
        recall << thisRecall;
        precision << thisPrecision;
        cost << thisCost;
        ++pd;
    }
    std::cout << "MEAN QUERY TIME: " << timer.elapsed() / bench.getQ() << "s." << std::endl;
    std::cout << "RECALL   : " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "COST     : " << cost.getAvg() << " +/- " << cost.getStd() << std::endl;
    std::cout << "PRECISION     : " << precision.getAvg() << " +/- " << precision.getStd() << std::endl;
    std::cout << "HASH TABLE SIZE    : " << mylsh.getBuckets().size() << std::endl;
    std::cout << "LARGEST BUCKET SIZE    : " << getMaxBucketSize(mylsh.getBuckets()) << std::endl;

    // mylsh.getMeanAndSTD(data);

    // mylsh.query(data[0], scanner);
    // std::vector<std::pair<float, unsigned> > res = scanner.topk().getTopk();
    // for (std::vector<std::pair<float, unsigned> >::iterator it = res.begin(); it != res.end(); ++it)
    // {
    //     std::cout << it->second << ": " << it->first << std::endl;
    // }
    // std::cout << "DISTANCE COMPARISON TIMES: " << scanner.cnt() << std::endl;
}
