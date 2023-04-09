#ifndef INTERSECTIONV2_H_
#define INTERSECTIONV2_H_

#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <math.h> 
#include <vector>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include "binTrie_il.hpp"

template <class trieType>
void Intersect(vector<trieType> &Bs, vector<uint64_t> &result, bool runs);

#endif