#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <math.h> 
#include <vector>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"

binTrie intersectTries(vector<binTrie> &Bs);


binTrie intersectTriesV2(vector<binTrie> &Bs);

template<class rankType>
flatBinTrie<rankType> intersectFlatTries(vector<flatBinTrie<rankType>> &Bs);


#endif