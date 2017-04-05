#include <vector>
#include <map>
template<typename ACCESSOR>
class HammingRanking{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;

    template<typename LSHTYPE>
    HammingRanking(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : scanner_(scanner) {

        scanner_.reset(domin);
        BIDTYPE hashVal = mylsh.getHashVal(0, domin);

        // ranking by linear sorting
        dstToBks_.resize(mylsh.param.N + 1); // maximum hamming dist is param.N
        unsigned hamDist;
        BIDTYPE xorVal;
        for ( std::map<BIDTYPE, std::vector<unsigned> >::iterator it = mylsh.tables[0].begin(); it != mylsh.tables[0].end(); ++it) {

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
    }

    BIDTYPE getNextBID(){
        numBucketsProbed_++;
        while (proCol_ == dstToBks_[proRow_].size()) {
            proCol_ = 0;
            proRow_++;
        }
        return dstToBks_[proRow_][proCol_++];
    }

    void operator()(unsigned key){
        numItemsProbed_++;
        // scanner_(key);
    }

    void reportCDD(){
        // report probed items
        lshbox::Scanner<ACCESSOR> thisScan = scanner_;
        thisScan.topk().genTopk();
        std::vector<std::pair<float, unsigned>> topk 
            = thisScan.topk().getTopk();
    }

    lshbox::Scanner<ACCESSOR> getScanner(){
        return scanner_;
    }

    int getNumItemsProbed() { // get number of items probed;
        return numItemsProbed_;
    }

    int getNumBucketsProbed() {
        return numBucketsProbed_;
    }
private:
    std::vector<std::vector<BIDTYPE>> dstToBks_;
    int proRow_ = 0;
    int proCol_ = 0;
    lshbox::Scanner<ACCESSOR> scanner_;
    int numBucketsProbed_ = 0;
    int numItemsProbed_ = 0;
};
