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

// template<class trieType>
// void join(vector<trieType> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel);

// template<class trieType>
// bool parallelAND(vector<trieType> &Ts, uint64_t nTries, uint64_t &nThreads, 
//         vector<thread> &threads,  cpu_set_t cpuset, uint64_t &maxLevel,
//         uint64_t &currLevel, uint64_t &levelOfCut, uint64_t roots[],
//         bool activeTries[], uint64_t prefix, vector<uint64_t> &r);
template <class trieType>
void Intersect(vector<trieType> &Bs, vector<uint64_t> &result, bool runs);

// template
// bool parallelAND<flatBinTrie<rank_support_v5<1>>>(vector<flatBinTrie<rank_support_v5<1>>> &Ts, uint64_t nTries, uint64_t &nThreads, vector<thread> &threads,  cpu_set_t cpuset, uint64_t &maxLevel, uint64_t &currLevel, uint64_t &levelOfCut, uint64_t roots[], bool activeTries[], uint64_t prefix, vector<uint64_t> &r);
// template
// bool parallelAND<flatBinTrie<rank_support_v<1>>>(vector<flatBinTrie<rank_support_v<1>>> &Ts, uint64_t nTries, uint64_t &nThreads, vector<thread> &threads,  cpu_set_t cpuset, uint64_t &maxLevel, uint64_t &currLevel, uint64_t &levelOfCut, uint64_t roots[], bool activeTries[], uint64_t prefix, vector<uint64_t> &r);
// template
// bool parallelAND<binTrie_il<512>>(vector<binTrie_il<512>> &Ts, uint64_t nTries, uint64_t &nThreads, vector<thread> &threads,  cpu_set_t cpuset, uint64_t &maxLevel, uint64_t &currLevel, uint64_t &levelOfCut, uint64_t roots[], bool activeTries[], uint64_t prefix, vector<uint64_t> &r);
// template
// bool parallelAND<binTrie_il<256>>(vector<binTrie_il<256>> &Ts, uint64_t nTries, uint64_t &nThreads,  vector<thread> &threads,  cpu_set_t cpuset, uint64_t &maxLevel, uint64_t &currLevel, uint64_t &levelOfCut, uint64_t roots[], bool activeTries[], uint64_t prefix, vector<uint64_t> &r);
// template
// bool parallelAND<binTrie_il<128>>(vector<binTrie_il<128>> &Ts, uint64_t nTries, uint64_t &nThreads, vector<thread> &threads,  cpu_set_t cpuset, uint64_t &maxLevel, uint64_t &currLevel, uint64_t &levelOfCut, uint64_t roots[], bool activeTries[], uint64_t prefix, vector<uint64_t> &r);
// template
// bool parallelAND<binTrie_il<64>>(vector<binTrie_il<64>> &Ts, uint64_t nTries, uint64_t &nThreads, vector<thread> &threads,  cpu_set_t cpuset, uint64_t &maxLevel, uint64_t &currLevel, uint64_t &levelOfCut, uint64_t roots[], bool activeTries[], uint64_t prefix, vector<uint64_t> &r);

#endif