#ifndef UTILS_FUNCTIONS_H_
#define UTILS_FUNCTIONS_H_

#include "binTrie.hpp"

void decodeBinTrie(binTrie b, vector<uint64_t> &decoded, bit_vector partial_result, uint64_t node_id, uint16_t curr_level, uint16_t max_level);

#endif