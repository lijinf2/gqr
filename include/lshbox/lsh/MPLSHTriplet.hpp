//for 2M value, can be absorted into PTBSet.hpp
class MPLSHTriplet{
public:
    float distance; //here denote the distance instead of the score
    int position;
    int delta;
    MPLSHTriplet(){}
    MPLSHTriplet(float a, int b, int c){
        this->distance = a; 
        this->position = b;
        this->delta = c;
    }
    bool operator < ( const MPLSHTriplet& other){
        return this->distance < other.distance;
    }

    std::string toString() const {
        std::string str = "(";
        str += "distance: " + std::to_string( this->distance ) + "\t" +
               "position: " + std::to_string( this->position ) + " " +
               "delta: " + std::to_string( this->delta ) + ")";
        return str;
    }
};

namespace std{
    std::string to_string(const MPLSHTriplet& triplet){
        return triplet.toString();
    }
};
#endif
