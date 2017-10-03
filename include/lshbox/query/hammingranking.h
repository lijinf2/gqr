#include <vector>
#include <unordered_map>
#include <lshbox/query/prober.h>

class HRTable {
public:
    typedef unsigned long long BIDTYPE;
    HRTable(
            BIDTYPE hashVal, // hash value of query q
            unsigned paramN, // number of bits per binary code
            const std::unordered_map<BIDTYPE, std::vector<unsigned> >& table
           ){
        // ranking by linear sorting
        dstToBks_.resize(paramN + 1); // maximum hamming dist is paramN
        unsigned hamDist;
        BIDTYPE xorVal;
        for ( std::unordered_map<BIDTYPE, std::vector<unsigned> >::const_iterator it = table.begin(); it != table.end(); ++it) {

            const BIDTYPE& bucketVal = it->first;
            xorVal = hashVal ^ bucketVal;

            hamDist = 0;
            // cal Number of 1, algorithm1, faster than 2, three instructions in while loop
            // key idea: always deduct a one from xorVal
            while(xorVal != 0){
                hamDist++;
                xorVal &= xorVal - 1; 
                // property of xorVal - 1: digits from last one will be flipped
                // e.g.   000100 100 - 1 = 000100 011
                // i.e. can stop earlier compare with check bit by bit
            }

            // algorithm2, slower than 1, five instructions in while loop
            // while (xorVal != 0) {
            //     if (xorVal & 1 == 1)
            //         hamDist++;
            //     xorVal >>= 1;
            // }
            assert(hamDist < dstToBks_.size());
            dstToBks_[hamDist].push_back(bucketVal);
        }
        // setNextRowCol_();
    }

    int getNumBuckets(int hamDist) {
        return dstToBks_[hamDist].size();
    }

    const std::vector<BIDTYPE>& getBuckets(int hamDist) {
        return dstToBks_[hamDist];
    } 
private:
    std::vector<std::vector<BIDTYPE>> dstToBks_;

};

template<typename ACCESSOR>
class HammingRanking : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    HammingRanking(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        allTables_.reserve(mylsh.tables.size());
        for (int i = 0; i < mylsh.tables.size(); ++i) {
            BIDTYPE hashValue = mylsh.getHashVal(i, domin);
            allTables_.emplace_back(HRTable(hashValue, this->R_, mylsh.tables[i]));
        }
        table_ = 0;
        iterator_ = 0;
        dist_ = 0;
    }

    std::pair<unsigned, BIDTYPE> getNextBID(){
        this->numBucketsProbed_++;

        if (iterator_ < allTables_[table_].getBuckets(dist_).size()) {
            BIDTYPE nextBucketID = allTables_[table_].getBuckets(dist_)[iterator_++];
            return std::make_pair(table_, nextBucketID);
        }

        // the following is the else logic
        
        // find next valid bucketID
        iterator_ = 0;
        table_++;
        while (true) {
            if (table_ == allTables_.size()) {
                dist_++;
                table_ = 0;
            }

            if (allTables_[table_].getBuckets(dist_).size() > 0)
                break;
            else 
                table_++;
        }
        
        BIDTYPE nextBucketID = allTables_[table_].getBuckets(dist_)[iterator_++];
        return std::make_pair(table_, nextBucketID);
    }

private:
    std::vector<HRTable> allTables_;
    unsigned table_ = 0;
    unsigned iterator_ = 0; // iterator to allTables[table_].getBuckets(dist_);
    
    unsigned dist_ = 0; // current probing hamming distance
    // std::vector<BIDTYPE>* currentTable_ = NULL;
};
