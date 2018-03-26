#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
using namespace std;

int item_size = 10677;

int K = 20;

int color = -1;

ifstream input("./KNN_graph.txt");
ofstream output("./KNN_graph_analysis.txt");

vector<vector<int>> g = vector<vector<int>>(item_size, vector<int>(K, 0));

vector<int> vis = vector<int>(item_size, -1);  //vis[i] represent its color and visited(0 means not visited, other means visited)
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


void dfs(int x){
    vis[x] = color;
    for (int i = 0; i < K; i++) {
        if (vis[g[x][i]]<0) dfs(g[x][i]);
    }
}



int main(int argc, const char * argv[]) {
    int t;
    if (argc != 3) {
        cout << "Please input item size and K\n";
        return 1;
    }
    item_size = atoi(argv[1]);
    K = atoi(argv[2]);
    for (int i = 0; i < item_size; i++) {
        input >> t;
        for (int j = 0; j < g[i].size(); j++) {
            input >> g[i][j];
        }
    }
    
    for (int i = 0; i < item_size; i++) {
        if (vis[i] < 0) {
            color++;
            dfs(i);
        }
    }
    for (int i = 0; i < item_size; i++) {
        cluster_group[vis[i]].push_back(i);
    }
    
    for (int i = 0; i <= color; i++) {
        cluster_node t;
        t.pos = i;
        t.size = cluster_group[i].size();
        t.first_index = cluster_group[i][0];
        cluster.push_back(t);
    }
    
    sort(cluster.begin(), cluster.end(), compare);
    
    for (int i = 0; i <= color; i++) {
        output << cluster[i].size << " ";
        for (int j = 0; j < cluster_group[cluster[i].pos].size(); j++) {
            output << cluster_group[cluster[i].pos][j] << " ";
        }
        output << endl;
    }
}
