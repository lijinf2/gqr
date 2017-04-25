#include <vector>
#include <unordered_map>
#include <lshbox/query/prober.h>
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
        LSHTYPE& mylsh) : Prober<ACCESSOR>(domin, scanner) {

        BIDTYPE hashVal = mylsh.getHashVal(0, domin);

        // ranking by linear sorting
        dstToBks_.resize(mylsh.param.N + 1); // maximum hamming dist is param.N
        unsigned hamDist;
        BIDTYPE xorVal;
        for ( std::unordered_map<BIDTYPE, std::vector<unsigned> >::iterator it = mylsh.tables[0].begin(); it != mylsh.tables[0].end(); ++it) {

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
        setNextRowCol_();
    }

    bool nextBucketExisted() {

        if (proRow_ == - 1  && proCol_ == -1) {
            return false;
        } else {
            return true;
        }
    }

    BIDTYPE getNextBID(){
        this->numBucketsProbed_++;

        // get current bucket to be returned
        BIDTYPE nextBID = dstToBks_[proRow_][proCol_];

        // set proRow_ or proCol_ to the next bucket
        setNextRowCol_();

        return nextBID;

    }

private:
    std::vector<std::vector<BIDTYPE>> dstToBks_;
    int proRow_ = 0;
    int proCol_ = -1;

    // set proRow_ and proCol_ to the position of next bucket
    // If there is no next bucket, proRow_ and proCol will be both set to -1
    void setNextRowCol_() {
        proCol_++;
        while(proRow_ < dstToBks_.size() && proCol_ >= dstToBks_[proRow_].size()) {
            proRow_++;
            proCol_ = 0;
        }

        // set proRow_ and proCol_ to -1 to denote no bucket
        if (proRow_ == dstToBks_.size()) {
            proRow_ = -1;
            proCol_ = -1;
        }
    }
};
