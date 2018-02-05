#pragma once
template<typename BIDTYPE>
class OneTableProber {
public:
    virtual bool hasNext() = 0;
    virtual const pair<float, BIDTYPE>& next() = 0;
};
