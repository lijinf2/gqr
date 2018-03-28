This a mini analyzer to analyse KNN graph.

You need an input file: KNN_garph.txt.

Usage:
./KNNGraph_Analyzer ${item_size} ${K}

eg.
./KNNGraph_Analyzer 10677 20 for dataset movielens.

The output file is KNN_graph_analysis.txt

input format:
i-th line: item_index NN_1 NN_2 NN_3 ... NN_K

output format:
i-th line: strongly connected component index, point1, point2, ...

SSC(strongly connected components) are ranked by their size in a descending order.
