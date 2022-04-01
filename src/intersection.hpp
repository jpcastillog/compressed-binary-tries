#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <math.h> 
#include <vector>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"

// template<class rankType>
// flatBinTrie<rankType>* joinTries(vector<flatBinTrie<rankType>> &Bs, bool runs_encoded);
template<class trieType>
trieType* joinTries(vector<trieType> &Bs, bool runs_encoded);


#endif