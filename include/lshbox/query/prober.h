template<typename ACCESSOR>
class Prober {
public:
    typedef typename ACCESSOR::DATATYPE DATATYPE;
    Prober(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner) : scanner_(scanner) {
        scanner_.reset(domin);
    }

    const lshbox::Scanner<ACCESSOR>& getScanner(){
        return scanner_;
    }

    unsigned int getNumItemsProbed() { // get number of items probed;
        return numItemsProbed_;
    }
    void operator()(unsigned key){
        numItemsProbed_++;
        scanner_(key);
    }
protected:
    unsigned int numBucketsProbed_ = 0;

private:
    lshbox::Scanner<ACCESSOR> scanner_;
    unsigned int numItemsProbed_ = 0;
};
