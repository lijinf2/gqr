#include <vector>
#include <unordered_map>
#include <lshbox/query/prober.h>

typedef unsigned long long BIDTYPE;
class HRTable {
public:
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
            // cal Number of 1
            while(xorVal != 0){
                hamDist++;
                xorVal &= xorVal - 1;
            }
            assert(hamDist < dstToBks_.size());
            dstToBks_[hamDist].push_back(bucketVal);
        }
        // setNextRowCol_();
    }

    // bool reachEnd() {
    //     if (proRow_ == - 1  && proCol_ == -1) {
    //         return false;
    //     } else {
    //         return true;
    //     }
    // }

    // BIDTYPE getNextBID(){
    //
    //     // get current bucket to be returned
    //     BIDTYPE nextBID = dstToBks_[proRow_][proCol_];
    //
    //     // set proRow_ or proCol_ to the next bucket
    //     setNextRowCol_();
    //
    //     return nextBID;
    // }
    int getNumBuckets(int hamDist) {
        return dstToBks_[hamDist].size();
    }

    const std::vector<BIDTYPE>& getBuckets(int hamDist) {
        return dstToBks_[hamDist];
    } 
private:
    std::vector<std::vector<BIDTYPE>> dstToBks_;

    // // iterator is a pair (proRow_, proCol_), always points to the next valid bucket
    // int proRow_ = 0;
    // int proCol_ = -1;

    // set proRow_ and proCol_ to the position of next bucket
    // If there is no next bucket, proRow_ and proCol will be both set to -1
    // void setNextRowCol_() {
    //     proCol_++;
    //     while(proRow_ < dstToBks_.size() && proCol_ >= dstToBks_[proRow_].size()) {
    //         proRow_++;
    //         proCol_ = 0;
    //     }
    //
    //     // set proRow_ and proCol_ to -1 to denote no bucket
    //     if (proRow_ == dstToBks_.size()) {
    //         proRow_ = -1;
    //         proCol_ = -1;
    //     }
    // }
};

template<typename ACCESSOR>
class HammingRanking : public Prober<ACCESSOR>{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;

    template<typename LSHTYPE>
    HammingRanking(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner, mylsh) {

        allTables_.reserve(mylsh.param.L);
        for (int i = 0; i < mylsh.param.L; ++i) {
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
