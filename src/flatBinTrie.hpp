#ifndef BINTRIE_BV
#define BINTRIE_BV

#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <vector>
#include <queue>
#include <math.h> 

using namespace sdsl;
using namespace std;

class flatBinTrie{

    uint16_t height;

    public:
        bit_vector bTrie;
        rank_support_v<1> b_rank;
        vector<uint64_t> level_pos;

        flatBinTrie() = default;

        flatBinTrie(int_vector<> &set) {
            uint32_t n = set.size();
            
            util::bit_compress(set);
            flatBinTrie::height = (uint16_t)set.width();
            flatBinTrie::level_pos = vector<uint64_t>(height, 0);
            
            uint64_t max_nodes = 2*(pow(2, height+1) - 1);
            flatBinTrie::bTrie = bit_vector(max_nodes, 0); 
            

            queue<tuple<uint64_t, uint64_t, uint64_t>> q;
            
            // add all set to split
            tuple<uint64_t, uint64_t, uint64_t> split(0, n-1, n);
            q.push(split);

            uint16_t level            = 0;
            uint64_t nodes_curr_level = 1; 
            uint64_t count_nodes      = 0;
            uint64_t nodes_next_level = 0;
            uint64_t index            = 0;
            uint64_t total_nodes      = 0;

            while (!q.empty()) {
                count_nodes++; // count node visited
                // Get node to write
                tuple<uint64_t, uint64_t, uint64_t> s = q.front();
                q.pop(); 
                
                uint64_t l, r, n;
                std::tie(l, r, n) = s;
                uint64_t left_elements  = 0;
                uint64_t right_elements = 0;

                // j-th most significative bit
                uint8_t j = height - level;
                uint64_t ll, lr, rl, rr;
                for (uint64_t i = l; i < r+1; ++i) {
                    if ((set[i] >> j-1) & 1) {                        
                        right_elements = r-i+1;
                        rl = i;
                        rr = r;
                        break;
                    }
                    else {
                        if (i == l){
                            ll = l;
                        }
                        lr = i;    
                        left_elements++;
                    }
                }
                // Add to queue split sets and write nodes
                tuple<uint64_t,uint64_t,uint64_t> left_split;
                tuple<uint64_t,uint64_t,uint64_t> right_split;
                // left child
                if (left_elements > 0) {
                    // write 1
                    bTrie[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
                    q.push(left_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    bTrie[index] = 0;
                    index++;
                    total_nodes++;
                }
                // right child
                if (right_elements > 0) {
                    // write 1
                    bTrie[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
                    q.push(right_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    bTrie[index] = 0;
                    index++;
                    total_nodes++;
                }

                if (count_nodes == nodes_curr_level) {
                    // bTrie[level].resize(2*count_nodes);
                    level_pos[level] = index;
                    nodes_curr_level = nodes_next_level;
                    nodes_next_level = 0;
                    count_nodes = 0;
                    level++;
                    
                }
                if (level == flatBinTrie::height) {
                    break;
                }
            }
            flatBinTrie::bTrie.resize(total_nodes);
            flatBinTrie::b_rank = rank_support_v<1>(&bTrie);
        };


        flatBinTrie(vector<uint64_t> ones_to_write[], uint16_t height, vector<uint64_t> level_pos) {
            flatBinTrie::height = height;
            uint64_t bits_n = 0;
            for (uint16_t level = 0; level < height; ++level) {
                bits_n += level_pos[level];
            }
            flatBinTrie::bTrie = bit_vector(bits_n, 0);
            flatBinTrie::level_pos = vector<uint64_t>(height, 0);

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    uint64_t pos = global_level_pos + ones_to_write[level][i];
                    flatBinTrie::bTrie[pos] = 1;
                }
                global_level_pos += level_pos[level];
                flatBinTrie::level_pos[level] = global_level_pos;
            }
            flatBinTrie::b_rank = rank_support_v<1>(&bTrie);
        };

        // return number of elements of bit_vector
        uint64_t size(){
            return bTrie.size();
        };


        uint16_t getHeight(){
            return flatBinTrie::height;
        };
        

        bit_vector getNode(uint64_t node_id){
            bit_vector node = bit_vector(2, 0);
            node[0] = bTrie[2*node_id];
            node[1] = bTrie[(2*node_id) + 1];
            return node;
        };


        uint64_t getLeftChild(uint64_t node_id) {
            uint64_t left_child_id = b_rank((2*node_id) + 1);
            return left_child_id;
        };


        uint64_t getRightChild(uint64_t node_id) {
            uint64_t right_child_id = b_rank((2*node_id) + 1) + 1;
            return right_child_id;
        };


        uint64_t size_in_bytes(){
            uint64_t bv_size = sdsl::size_in_bytes(flatBinTrie::bTrie);
            uint64_t rank_size = sdsl::size_in_bytes(flatBinTrie::b_rank);
            return bv_size +
                    rank_size +
                    sizeof(uint64_t);
        }

        // uint64_t size_bytes(){
        //     // Size in bytes of bit_vector of binary trie
        //     uint64_t size_bit_vector = size_in_bytes(btrie); // 8 bytes to size, 1 byte to width + structure space
        //     // Size in bytes of rank structure
        //     uint64_t size_rank = size_in_bytes(b_rank); // +0.25 extra bits
        //     // return size_bit_vector + size_rank;
        //     cout << "Size of bit vector: " << size_in_bytes(btrie) << endl;
        //     cout << "Size of rank structure: " << size_in_bytes(b_rank) << " bytes " << size_in_bytes(b_rank)*8 << endl;
        //     cout << "Size of rank structure: " << b_rank.size() << " bits" << endl;
        //     cout << "Verificación rank size: 0.25*n -> " << btrie.size()*1.25 << " bits" << endl;
        //     return size_bit_vector*8;
        //     // return (int) btrie.bit_size();
        // };
    
};

#endif
