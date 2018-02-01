#pragma once
#include <cmath>
#include "lshbox/utils.h"
template<typename ACCESSOR, typename BIDTYPE>
class BaseProber {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;

    template<typename LSHTYPE>
    BaseProber(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : scanner_(scanner) {

        // initialize scanner_, this->hashBits_, and this->R_
        scanner_.reset(domin);

        buckets_.resize(mylsh.tables.size());
        for (unsigned tb = 0; tb < buckets_.size(); ++tb) {
            buckets_[tb] = mylsh.getBuckets(tb, domin);
        }
        R_ = buckets_[0].size();

        totalItems_ = mylsh.getBaseSize();
    }

    lshbox::Scanner<ACCESSOR>& getScanner(){
        return scanner_;
    }

    unsigned int getNumItemsProbed() { // get number of items probed;
        return scanner_.cnt();
    }

    virtual void operator()(unsigned key){
        scanner_(key);
    }

    /*
     * return (unvisited, distance)
     * if unvisited = false, variable distance has no meaning
     * */
    pair<bool, float> evaluate(unsigned key) {
        return this->scanner_.evaluate(key);
    }

    virtual bool nextBucketExisted() {
        if (getNumItemsProbed() < totalItems_)
            return true;
        else return false;
    }

    virtual std::pair<unsigned, BIDTYPE> getNextBID() = 0; 

    void reportCDD(){
        // report probed items
        lshbox::Scanner<ACCESSOR> thisScan = scanner_;
        thisScan.topk().genTopk();
        std::vector<std::pair<float, unsigned>> topk 
            = thisScan.topk().getTopk();
    }

protected:
    unsigned int numBucketsProbed_ = 0;
    unsigned R_;
    std::vector<BIDTYPE> buckets_; // L hash tables

private:
    lshbox::Scanner<ACCESSOR> scanner_;
    unsigned totalItems_; // 
};
