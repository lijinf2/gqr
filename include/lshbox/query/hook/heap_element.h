#pragma once
// for min heap, smaller distance is better
template<typename DataT>
struct DistDataMin{
    float dist_;
    DataT data_;
    DistDataMin(float dist, DataT data) {
        dist_ = dist;
        data_ = data;
    }

    float getDist() {
        return dist_;
    }

    const DataT& data() const {
        return data_;
    }

    bool operator<(const DistDataMin& other) const  {
        return dist_ > other.dist_;
    }
};
