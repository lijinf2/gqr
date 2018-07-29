#include "base/onetableprober.h"
#include "gqr/util/gqrhash.h"
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
using std::vector;
using std::pair;

namespace lshbox {
class NRItemList : public OneTableProber<vector<int>>{
public:
    typedef vector<int> BIDTYPE;
    NRItemList(
        const unordered_map<BIDTYPE, std::vector<unsigned>, gqrhash<BIDTYPE>>& table,
        std::function<float (const BIDTYPE&)> distor) {
        
        for (typename unordered_map<BIDTYPE, std::vector<unsigned>, typename lshbox::gqrhash<BIDTYPE>>::const_iterator it = table.begin(); it != table.end(); ++it) {

            const BIDTYPE& bucket = it->first;
            float dist = distor(it->first);

            numAllItem += it->second.size();
            for (const auto& item : it->second) {
                itemlist.emplace_back(pair<float, unsigned>(dist, item));
            }
        }
        std::sort(
            itemlist.begin(), 
            itemlist.end(), 
            [](const pair<float, unsigned>& a, const pair<float, unsigned>& b){
            if (a.first != b.first)
                return a.first < b.first;
            else 
                return a.second < b.second;
        });
        current.first = itemlist[numVisitedItem].first;
        current.second = vector<int>(1, itemlist[numVisitedItem].second);
                       
    }

    bool hasNext() override {
        return numVisitedItem < numAllItem;
    }

    const pair<float, vector<int>>& next() override {
        current.first = itemlist[numVisitedItem].first;
        current.second[0] = itemlist[numVisitedItem].second; 
        numVisitedItem++;
        return current;
    }
protected:
    vector<pair<float, unsigned>> itemlist;
    pair<float, vector<int>> current;
    unsigned numVisitedItem = 0;
    unsigned numAllItem = 0; 
};
}
