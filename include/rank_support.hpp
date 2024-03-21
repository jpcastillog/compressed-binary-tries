#ifndef RANK_SUPPORT
#define RANK_SUPPORT

#include <sdsl/bit_vectors.hpp>


class rank_support {
    public:
        rank_support(){};
        virtual rank(bitvector) = 0;
        virtual ~rank() = default;
        virtual size_type rank(size_type idx) = 0;
        virtual size_type operator[](size_type idx) = 0;
        virtual size_type size() = 0;
        virtual size_type serialize(std::ostream& out) = 0;
        virtual void load(std::istream& in) = 0;

}
#endif