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
 * @file laitqlsh_test.cpp
 *
 * @brief Example of using Iterative Quantization LSH index for L2 distance.
 */
#include <lshbox.h>
#include <lshbox/lsh/laitqlsh.h>
#include <map>
#include <fstream>
#include <lshbox/lsh/hashlookup.h>
#include <lshbox/lsh/lossranking.h>

#include <lshbox/query/fv.h>
#include <lshbox/query/losslookup.h>
#include <lshbox/query/hammingranking.h>
#include <lshbox/query/hashlookupPP.h>
#include <lshbox/utils.h>

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: ./laitqlsh_test data_file lsh_file benchmark_file" << std::endl;
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

    lshbox::laItqLsh<DATATYPE> mylsh;
    if (use_index)
    {
        mylsh.load(file);
    }
    else
    {
        lshbox::laItqLsh<DATATYPE>::Parameter param;
        param.L = 1;  // number of tables
        param.D = data.getDim();
        param.N = 20;  // number of bits
        param.S = 60000; //must be the size of data, which will be used to init tables,  number of vectors in the training set
        param.I = 50;
        mylsh.reset(param);
        // mylsh.train(data);
        mylsh.trainAll(data, 5); // the second parameter: parallelism, more parallelism requires more memory and CPU
        mylsh.hash(data);
        mylsh.save(file);
        std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
        return 0;
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

    // initialize scanner
    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> initScanner(
        accessor,
        metric,
        K
    );
    std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "RUNING QUERY ..." << std::endl;
    timer.restart();

    int maxProbedBK = 16;
    if (argc >= 6)
        maxProbedBK = std::atoi(argv[5]);

    // initialize prober
    // typedef HashLookup<lshbox::Matrix<DATATYPE>::Accessor> PROBER;
    // typedef LossRanking<lshbox::Matrix<DATATYPE>::Accessor> PROBER;
    
    // typedef HammingRanking<lshbox::Matrix<DATATYPE>::Accessor> PROBER;
    //
    // void* raw_memory = operator new[]( 
    //     sizeof(PROBER) * bench.getQ());
    // PROBER* probers = static_cast<PROBER*>(raw_memory);
    // for (int i = 0; i < bench.getQ(); ++i) {
    //     new(&probers[i]) PROBER(
    //         data[bench.getQuery(i)],
    //         initScanner,
    //         mylsh);// for non losslookup probers
    // }


    // // // initialize losslookup probers
    // typedef LossLookup<lshbox::Matrix<DATATYPE>::Accessor> PROBER;
    typedef HashLookupPP<lshbox::Matrix<DATATYPE>::Accessor> PROBER;


    FV fvs(mylsh.param.N);

    void* raw_memory = operator new[]( 
        sizeof(PROBER) * bench.getQ());
    PROBER* probers = static_cast<PROBER*>(raw_memory);
    for (int i = 0; i < bench.getQ(); ++i) {
        new(&probers[i]) PROBER(
            data[bench.getQuery(i)],
            initScanner,
            mylsh,
            &fvs);// for non losslookup probers
    }

    double initTime = timer.elapsed();
    std::cout << "init time: " << initTime << std::endl;

    // probe
    // std::ofstream fout("result.csv");
    std::cout << "probed items, " << "overall query time, " 
        << "avg recall, " << "avg precision" <<"\n";

    timer.restart();
    for (unsigned numItems = 1; numItems <= maxProbedBK; numItems *= 2) { //  # of probed items must be the power of two
        // std::cout << "start queries " << std::endl;
        lshbox::Stat recall, precision;
        for (unsigned i = 0; i != bench.getQ(); ++i)
        {
            // queries are applied incrementally, i.e. the result of this round depends on the last round
            mylsh.KItemByProber(data[bench.getQuery(i)], probers[i], numItems);

            // collect accuracy information
            setStat(probers[i].getScanner(), bench.getAnswer(i), recall, precision);
        }
        double retTime = timer.elapsed();
        std::cout << numItems << ", " << retTime <<", "
            << recall.getAvg() << ", " << precision.getAvg() << "\n";
    }
    // fout.close();

    // release memory of prober;
    std::cout << "bench.getQ: " << bench.getQ() << std::endl;
    for (unsigned i = bench.getQ() - 1; i !=0; --i) {
        probers[i].~PROBER();
    }

    std::cout << "HASH TABLE SIZE    : " << mylsh.getTableSize() << std::endl;
    std::cout << "LARGEST BUCKET SIZE    : " << mylsh.getMaxBucketSize() << std::endl;
    std::cout << "end of program" << std::endl;
    // operator delete[](raw_memory);
}
