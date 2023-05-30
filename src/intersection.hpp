#ifndef INTERSECTION
#define INTERSECTION

#include <vector>
#include "binaryTrie.hpp"

// template <class trieType>
// void Intersect(vector<trieType> &Bs, vector<uint64_t> &result, bool runs);
vector<uint64_t> Intersect(vector<binaryTrie*> &Ts, bool runs);

#endif