#include <vector>
#include <map>
template<typename ACCESSOR>
class LossRanking{
public:
    typedef typename ACCESSOR::Value value;
    typedef typename ACCESSOR::DATATYPE DATATYPE;

    template<typename LSHTYPE>
    LossRanking(
        const DATATYPE* domin,
        lshbox::Scanner<ACCESSOR>& scanner,
        LSHTYPE& mylsh) : scanner_(scanner) {

        scanner_.reset(domin);
        std::vector<bool> queryBits = mylsh.getHashBits(0, domin);
        std::vector<float> queryFloats = mylsh.getHashFloats(0, domin);
        
        // ranking by std::sort, low efficiency
        for ( std::map<unsigned, std::vector<unsigned> >::iterator it = mylsh.tables[0].begin(); it != mylsh.tables[0].end(); ++it) {

            std::vector<bool> bucketBits = mylsh.unsignedToBools(it->first);
            float dst = 0;
            for (int bIdx = 0; bIdx < queryBits.size(); ++bIdx) {
                if (queryBits[bIdx] != bucketBits[bIdx]){
                    dst += fabs(queryFloats[bIdx]); 
                }
            }
            dstToBks_.push_back(std::pair<float, unsigned>(dst, it->first));
        }
        assert(dstToBks.size() == mylsh.tables[0].size());

        std::sort(dstToBks_.begin(), 
            dstToBks_.end(), 
            [] (const std::pair<float, unsigned>& a, const std::pair<float, unsigned>& b ) {
                return a.first < b.first;
            });
    }

    unsigned getNextBID(){
        return dstToBks_[index++].second;
        // while (proCol_ == dstToBks_[proRow_].size()) {
        //     proCol_ = 0;
        //     proRow_++;
        // }
        // return dstToBks_[proRow_][proCol_++];
    }

    void operator()(unsigned key){
        scanner_(key);
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
private:
    std::vector<std::pair<float, unsigned> > dstToBks_;
    unsigned long long index = 0;
    lshbox::Scanner<ACCESSOR> scanner_;
};
