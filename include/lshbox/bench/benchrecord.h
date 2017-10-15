#include <vector>
#include <algorithm>
#include <utility>
#include <unordered_set>
using std::vector;
using std::pair;
using std::unordered_set;
class BenchRecord {
public:
    BenchRecord(unsigned qId){
        this->queryId = qId;
    }
    
    void add(unsigned nbid, float dist) {
        this->knn.push_back(std::make_pair(nbid, dist));
        this->knnIvecs.insert(nbid);
    }

    void reserve(unsigned size) {
        this->knn.reserve(size);
    }

    unsigned getId() const {
        return this->queryId;
    }

    const vector<pair<unsigned, float>>& getKNN() const {
        return this->knn;
    }
    
    unsigned size() {
        return this->knn.size();
    }

    float recall(const BenchRecord& other) const {
        return recall(other.getId(), other.getKNN());
    }

    float error(const BenchRecord& other) const {
        return error(other.getId(), other.getKNN());
    }

    float recall(unsigned qId, const vector<pair<unsigned, float>>& givenKNN) const {
        assert(this->queryId == qId);
        vector<unsigned> ivecs;
        ivecs.resize(givenKNN.size());
        for (int i = 0; i < givenKNN.size(); ++i) {
            ivecs[i] = givenKNN[i].first;
        }
        return recall_ivecs(qId, ivecs);
    }

    float recall_ivecs(unsigned qId, const vector<unsigned>& ivecs) const {
        assert(this->queryId == qId);
        int matched = 0;
        for (vector<unsigned>::const_iterator it = ivecs.begin(); it != ivecs.end(); ++it) {
            if (this->knnIvecs.find(*it) != this->knnIvecs.end()) {
                matched++;
            }
        }
        return matched / float(this->knn.size());
    }

    float error(unsigned qId, const vector<pair<unsigned, float>>& givenKNN) const {
        if (givenKNN.size() == 0) return -1;

        float error = 0;
        int size = std::min(givenKNN.size(), this->knn.size());

        int idx = 0;
        int idxGiven = 0;
        int count = 0;
        while(idx < size && idxGiven < size) {
            if (this->knn[idx].second < 0.0001 && givenKNN[idxGiven].second > 0.0001) {
                idx++;
                continue;
            }
            if (this->knn[idx].second > 0.0001 && givenKNN[idxGiven].second < 0.0001) {
                assert(false);
            }

            if (this->knn[idx].second < 0.0001 && givenKNN[idxGiven].second < 0.0001) {
                error += 1;
            } else {
                float thisError = givenKNN[idxGiven].second / this->knn[idx].second;
                error += thisError;
            }

            count++;
            idx++;
            idxGiven++;
        }
        
        if (count == 0) return -1;
        else return error / count;
    }
private:
    unsigned queryId;
    vector<pair<unsigned, float>> knn;
    unordered_set<unsigned> knnIvecs;
};

