#pragma once
#include <utility>
using std::pair;
// for min heap, smaller distance is better
template<typename DataT>
class DistData {
protected:
    float dist_;
    DataT data_;

public: 
    DistData(float dist, const DataT& data) {
        dist_ = dist;
        data_ = data;
    }

    DistData(const pair<float, DataT>& p) {
        dist_ = p.first;
        data_ = p.second;
    }

    float getDist() const {
        return dist_;
    }

    const DataT& data() const {
        return data_;
    }
};

// for min heap
template<typename DataT>
class DistDataMin : public DistData<DataT> {
public:
    DistDataMin(float dist, const DataT& data) : DistData<DataT>(dist, data) {}

    bool operator<(const DistDataMin& other) const  {
        return this->dist_ > other.dist_;
    }
};

// for max heap
template<typename DataT>
class DistDataMax : public DistData<DataT> {
public:
    DistDataMax(float dist, const DataT& data) : DistData<DataT>(dist, data) {}
    bool operator<(const DistDataMax& other) const  {
        return this->dist_ < other.dist_;
    }
};

// for smaller comparator
template<typename DataT>
class DistDataSmaller: public DistData<DataT> {
public:
    DistDataSmaller(float dist, const DataT& data) : DistData<DataT>(dist, data) {}

    bool operator<(const DistDataSmaller& other) const  {
        return this->dist_ < other.dist_;
    }
};

// for larger comparator
template<typename DataT>
class DistDataLarger: public DistData<DataT> {
public:
    DistDataLarger(float dist, const DataT& data) : DistData<DataT>(dist, data) {}

    bool operator<(const DistDataLarger& other) const  {
        return this->dist_ > other.dist_;
    }
};
