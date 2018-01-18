#include <vector>
#include <utility>
#include <lshbox/query/treelookup.h>
using std::vector;
using std::pair;
class Hooker {
public:
    typedef unsigned long long BIDTYPE;

    template<typename DATATYPE, typename LSHTYPE, typename SCANNER>
    Hooker(
        int degree, // the number of buckets recommended by an item
        const lshbox::Matrix<DATATYPE>& data,
        SCANNER initScanner,
        LSHTYPE& mylsh) {

        // initialized bucketLists by tree lookup items
        bucketLists_.resize(data.getSize());
        int tmp = bucketLists_.size();
        for (auto& l : bucketLists_) {
            l.resize(degree);
        }

        typedef TreeLookup<typename lshbox::Matrix<DATATYPE>::Accessor> GQRT;
        Tree fvs(mylsh.getCodeLength());

        for (int i = 0; i < data.getSize(); ++i) {
            GQRT prober(data[i], initScanner, mylsh, &fvs);
            for (int dg = 0; dg < degree; ++dg) {
                const std::pair<unsigned, BIDTYPE>& tableBucket = prober.getNextBID();
                bucketLists_[i][dg] = tableBucket;
            }
        }
    };

    const vector<pair<unsigned, BIDTYPE>>& getBucketList(unsigned itemId) const {
        return bucketLists_[itemId];
    }

    int getDegree() const {
        return bucketLists_[0].size();
    }
private: 

    // item->list of (table, bucket)
    vector<vector<pair<unsigned, BIDTYPE>>> bucketLists_;
};
