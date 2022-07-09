#ifndef UTILS_FUNCTIONS_H_
#define UTILS_FUNCTIONS_H_

#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include <queue>
#include <tuple>
#include <vector>

// void decodeBinTrie(binTrie &b, vector<uint64_t> &decoded, bit_vector partial_result, uint64_t node_id, uint16_t curr_level, uint16_t max_level);

// template<class rankType>
// void decodeBinTrie(flatBinTrie<rankType> &b, vector<uint64_t> &decoded, bit_vector partial_result, uint64_t node_id, uint16_t curr_level, uint16_t max_level);

vector<uint64_t>* read_inverted_list(std::ifstream &input_stream, uint64_t n);

void testSpaceII(string file_path);

void testSpaceInvertedIndex(std::string ii_path);

void force_brute_intersection(vector<uint64_t> Sets[], uint16_t k, vector<uint64_t> &intersection);

void performQueryLog(string query_log_path, string ii_path);

void randomQueries(string file_path);

void splitUniverse(std::vector<uint64_t> &set, std::queue<tuple<uint64_t, uint64_t, uint64_t>> &q,  
                   sdsl::bit_vector *bTrie, sdsl::bit_vector *lastLevel, 
                   uint64_t *level_pos, uint16_t height, uint16_t level_of_cut);

#endif