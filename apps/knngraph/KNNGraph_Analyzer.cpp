// Created By Sun Haopeng in March 28th, 2018.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <cmath>

using namespace std;

int item_size = 10677;

int K = 20;

ifstream input("./KNN_graph.txt");
ofstream output("./KNN_graph_analysis.txt");

vector<vector<int>> g = vector<vector<int>>(item_size, vector<int>(K, 0));

vector<int> color = vector<int>(item_size, -1);  //color[i] represent its color or strongly connected component index;
vector<int> dfn = vector<int>(item_size, -1);
vector<int> low = vector<int>(item_size, -1);
vector<int> on_stack = vector<int>(item_size, 0);
vector<int> stack = vector<int>(item_size, 0);

vector<vector<int>> cluster_group = vector<vector<int>>(item_size,vector<int>()); //cluster_group[i] represent items in cluster[i]

class cluster_node{
public:
    int pos; // position in cluster_group
    int first_index; //index of first node of this cluster.
    int size;
};

vector<cluster_node> cluster;

bool compare(const cluster_node& a, const cluster_node& b){
    if (a.size > b.size) {return true;}
    if (a.size == b.size) {
        if (a.first_index < b.first_index) {
            return true;
        }
    }
    return false;
}

int dfn_index = 0;
int scc_index = 0;
int tmp;
int st = 0;

void tarjan(int u) {
    dfn[u] = low[u] = ++dfn_index;
    on_stack[u] = 1; stack[st++] = u;
    for(int i = 0; i < K; i++){
        int v = g[u][i];
        if(dfn[v] < 0){
            tarjan(v);
            if(low[v] < low[u])
                low[u] = low[v];
        } else if(on_stack[v] && dfn[v] < low[u]) {
            low[u] = dfn[v];
        }
    }
    if(low[u] == dfn[u]){
        
        ++scc_index;
        tmp = -1;
        while(tmp != u)
        {
            tmp = stack[--st];
            color[tmp] = scc_index;
            on_stack[tmp] = 0;
        }
    }
}




int main(int argc, const char * argv[]) {
    if (argc != 3) {
        cout << "Please input item size and K\n";
        return 1;
    }
    item_size = atoi(argv[1]);
    K = atoi(argv[2]);
    if (!input.is_open()) {
        cout << "input file cannot open!\n";
        return 1;
    }
    if (!output.is_open()){
        cout << "output file cannot open\n";
    }
    int t;
    for (int i = 0; i < item_size; i++) {
        input >> t;
        for (int j = 0; j < g[i].size(); j++) {
            input >> g[t][j];
        }
    }
    
    for (int i = 0; i < item_size; i++) {
        if (dfn[i] < 0) {
            tarjan(i);
        }
    }
    for (int i = 0; i < item_size; i++) {
        cluster_group[color[i]].push_back(i);
    }

    for (int i = 1; i <= scc_index; i++) {
        cluster_node t;
        t.pos = i;
        t.size = cluster_group[i].size();
        t.first_index = cluster_group[i][0];
        cluster.push_back(t);
    }

    sort(cluster.begin(), cluster.end(), compare);

    for (int i = 0; i < scc_index; i++) {
        output << cluster[i].size << " ";
        for (int j = 0; j < cluster_group[cluster[i].pos].size(); j++) {
            output << cluster_group[cluster[i].pos][j] << " ";
        }
        output << endl;
    }
    input.close();
    output.close();
}
