#include <vector>
#include <map>
#include <queue>
#include <vector>
#include "lshbox/query/scoreidxpair.h"
class LRTable {
public:
    typedef unsigned long long BIDTYPE;
    typedef std::unordered_map<BIDTYPE, std::vector<unsigned> > TableT;

    LRTable(
        BIDTYPE hashVal, 
        const std::vector<float>& queryFloats, 
        const TableT& table){

        // should get the number of hashbuckets! from mylsh
        dstToBks_.reserve(table.size()); 
        BIDTYPE xorVal;
        float dst;
        for ( TableT::const_iterator it = table.begin(); it != table.end(); ++it) {

            // should be improved by xor operations

            xorVal = hashVal ^ it->first;
            dst = 0;
            for (int idx = queryFloats.size() - 1; idx >=0 ; --idx) {
                if (xorVal & 1 == 1) {
                    dst += queryFloats[idx]; 
                }
                xorVal >>= 1;
            }

            dstToBks_.emplace_back(std::pair<float, BIDTYPE>(dst, it->first));
        }
        assert(dstToBks_.size() == table.size());

        std::sort(dstToBks_.begin(), 
            dstToBks_.end(), 
            [] (const std::pair<float, BIDTYPE>& a, const std::pair<float, BIDTYPE>& b ) {
                return a.first < b.first;
            });

        iterator = 0;
    }

    bool reset() {
        iterator = 0;
    }

    float getCurScore() {
        return dstToBks_[iterator].first;
    }

    BIDTYPE getCurBucket() {
        return dstToBks_[iterator].second;
    }
    
    // move to next, if exist return true and otherwise false
    bool moveForward() {
        iterator++;
        return iterator < dstToBks_.size();
    }

private:
    std::vector<std::pair<float, BIDTYPE> > dstToBks_;
    unsigned iterator = 0;
};

template<typename ACCESSOR>
class LossRanking : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
    // typedef std::pair<float, unsigned > PairT; // <score, tableIdx> 
    typedef ScoreIdxPair PairT;

    template<typename LSHTYPE>
    LossRanking(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        allTables_.reserve(mylsh.tables.size());
        for (int i = 0; i < mylsh.tables.size(); ++i) {

            BIDTYPE hashValue = mylsh.getHashVal(i, domin);
            std::vector<float> queryFloats = mylsh.getHashFloats(i, domin);

            for (auto& e : queryFloats) {
                e = fabs(e);
            }

            allTables_.emplace_back(
                LRTable(hashValue, queryFloats, mylsh.tables[i]));
        }

        for (unsigned i = 0; i != allTables_.size(); ++i) {
            float score = allTables_[i].getCurScore();
            heap_.push(PairT(score , i));
        }
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;
        unsigned tb = heap_.top().index_;
        heap_.pop();

        BIDTYPE nextBucket = allTables_[tb].getCurBucket();
        if (allTables_[tb].moveForward()) {
            float score = allTables_[tb].getCurScore();
            heap_.push(PairT(score, tb));
        }
        return std::make_pair(tb, nextBucket);
    }

private:
    std::vector<LRTable> allTables_;

    std::priority_queue<PairT> heap_;
};
