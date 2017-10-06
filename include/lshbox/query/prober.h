#pragma once
template<typename ACCESSOR>
class Prober {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    typedef unsigned long long BIDTYPE;
    template<typename LSHTYPE>
    Prober(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : scanner_(scanner) {

        // initialize scanner_, this->hashBits_, and this->R_
        scanner_.reset(domin);

        hashBits_.resize(mylsh.tables.size());
        for (unsigned tb = 0; tb < hashBits_.size(); ++tb) {
            hashBits_[tb] = mylsh.getHashBits(tb, domin);
        }
        R_ = hashBits_[0].size();

        totalItems_ = mylsh.getBaseSize();
    }

    const lshbox::Scanner<ACCESSOR>& getScanner(){
        return scanner_;
    }

    unsigned int getNumItemsProbed() { // get number of items probed;
        return scanner_.cnt();
    }

    void operator()(unsigned key){
        scanner_(key);
    }

    bool nextBucketExisted() {
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

    ~Prober(){
        // delete[] visited;
    }
protected:
    unsigned int numBucketsProbed_ = 0;
    unsigned R_;
    std::vector<std::vector<bool>> hashBits_; // L hash tables

private:
    lshbox::Scanner<ACCESSOR> scanner_;
    unsigned totalItems_; // 
};
