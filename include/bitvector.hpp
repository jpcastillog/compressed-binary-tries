#ifndef BITVECTOR
#define BITVECTOR

#include <sdsl/bit_vectors.hpp>

class bitvector {
    public:
        bitvector(){};
        virtual ~bitvector() = default;
        virtual size_type size() = 0;
        virtual size_type serialize(std::ostream &out)=0;
        virtual void load(std::istream &in) = 0;
        virtual value_type operator[](size_type i) = 0;
}
#endif