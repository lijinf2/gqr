This a mini analyzer to analyse KNN graph.

===========================================================================

Usage:
bash KNN_graph_analyzer.sh

You can change input or output file path in KNN_graph_analyzer.sh

===========================================================================

input format:
i-th line: item_index NN_1 NN_2 NN_3 ... NN_K

output format:
i-th line: strongly connected component index, point1, point2, ...

SSC(strongly connected components) are ranked by their size in a descending order.
