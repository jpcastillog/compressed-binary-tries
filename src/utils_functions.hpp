#ifndef UTILS_FUNCTIONS_H_
#define UTILS_FUNCTIONS_H_

#include "binTrie.hpp"
#include "flatBinTrie.hpp"

void decodeBinTrie(binTrie b, vector<uint64_t> &decoded, bit_vector partial_result, uint64_t node_id, uint16_t curr_level, uint16_t max_level);
void decodeBinTrie(flatBinTrie b, vector<uint64_t> &decoded, bit_vector partial_result, uint64_t node_id, uint16_t curr_level, uint16_t max_level);
sdsl::int_vector<>* read_inverted_list(std::ifstream &input_stream, uint64_t n);
void read_inverted_index(string file_path);

#endif