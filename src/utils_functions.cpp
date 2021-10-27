#include <iostream>
#include "binTrie.hpp"
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <math.h>


void decodeBinTrie(binTrie b, vector<uint64_t> &decoded, bit_vector partial_result,
                   uint64_t node_id, uint16_t curr_level, uint16_t max_level) {
   
    if (curr_level == max_level) {
        uint64_t number = 0;
        for (uint16_t i = 0; i <  b.getHeight(); ++i) {
            number += partial_result[i]*pow(2, i);  
        }
        decoded.push_back(number);
        return;
    }

    bit_vector node = b.getNode(node_id, curr_level);
    uint16_t next_level = curr_level + 1;

    bit_vector leftResult  = partial_result;
    bit_vector rightResult = partial_result;

    if (node[0] == 1) {
        leftResult[b.getHeight() - curr_level - 1] = 0; 
        uint64_t left_child = b.getLeftChild(node_id, curr_level);
        decodeBinTrie(b, decoded, leftResult, left_child, next_level, max_level);
    }

    if (node[1] == 1) {
        rightResult[b.getHeight() - curr_level - 1] = 1;
        uint64_t right_child = b.getRightChild(node_id, curr_level);
        decodeBinTrie(b, decoded, rightResult, right_child, next_level, max_level);
    }
}


sdsl::int_vector<>* read_inverted_list(std::ifstream &input_stream, uint64_t n){
    uint64_t x;
    uint64_t i;

    int_vector<>* inverted_list  = new int_vector<>(n);
    for (i = 0; i < n; i++){
        input_stream >> x;
        (*inverted_list)[i] = x;
    }

    return inverted_list;
}