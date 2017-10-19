***************************************************************************************

sample_queries.sh
    params: $base_file, $num_queries
    output: $query_file

cal_groundtruth.sh
    params: $base_file, $query_file, $topk, $numThreads
    output: $benchmark_file
    
First, sample_queries.sh is used to produce $query_file.
Then cal_groundtruth.sh is used to produce $benchmark_file ($query_file is one of the input parameters).
Finally, we use all these files ($base_file, $query_file, $benchmark file) to do search in search.sh.
