#ifndef BINTRIE
#define BINTRIE

#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <queue>
#include <math.h>
#include <vector>
#include <tuple>

using namespace sdsl;
using namespace std;

class binTrie {

    uint16_t height;
    
    public:
        vector <sdsl::bit_vector> bTrie;
        sdsl::rank_support_v<>* bv_rank;

        binTrie() = default;

        binTrie(int_vector<> &set) {
            uint64_t n = set.size();
            
            util::bit_compress(set);
            binTrie::height = (uint16_t)set.width();
            binTrie::bv_rank = new rank_support_v<1>[height];
            
            for (int level = 0; level < binTrie::height; ++level) {
                uint64_t max_nodes_level = 2 * pow(2, level);
                bit_vector level_bv = bit_vector(max_nodes_level, 0);
                bTrie.push_back(level_bv);
            }

            queue<tuple<uint64_t, uint64_t, uint64_t>> q;
            
            // add all set to split
            tuple<uint64_t, uint64_t, uint64_t> split ((uint64_t)0, n-1, n);
            q.push(split);

            uint16_t level            = 0;
            uint64_t nodes_curr_level = 1; 
            uint64_t count_nodes      = 0;
            uint64_t nodes_next_level = 0;
            uint64_t index            = 0;

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
                
                // left child
                if (left_elements > 0) {
                    // write 1
                    bTrie[level][index] = 1;
                    // Add to queue split sets and write nodes
                    tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
                    q.push(left_split);
                    nodes_next_level++;
                    index++;
                }
                else {
                    // write 0
                    bTrie[level][index] = 0;
                    index++;
                }
                // right child
                if (right_elements > 0) {
                    // write 1
                    bTrie[level][index] = 1;
                    // Add to queue split sets and write nodes
                    tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
                    q.push(right_split);
                    nodes_next_level++;
                    index++;
                }
                else {
                    // write 0
                    bTrie[level][index] = 0;
                    index++;
                }

                if (count_nodes == nodes_curr_level) {
                    bTrie[level].resize(2*count_nodes);
                    nodes_curr_level = nodes_next_level;
                    nodes_next_level = 0;
                    count_nodes = 0;
                    index = 0;
                    level++;
                    
                }
                if (level == binTrie::height) {
                    break;
                }
            }

            for (uint16_t i = 0; i < height; ++i) {
                binTrie::bv_rank[i] = sdsl::rank_support_v<1>(&bTrie[i]);
            }
        };

        // Empty binary trie constructor
        binTrie(uint16_t height) {
            binTrie::height = height;
            binTrie::bv_rank = new rank_support_v<1>[height];
            for (uint16_t level = 0; level < binTrie::height; ++level) {
                uint64_t max_nodes_level = 2 * pow(2, level);
                bit_vector level_bv = bit_vector(max_nodes_level, 0);
                binTrie::bTrie.push_back(level_bv);
            }
        };


        binTrie(vector<uint64_t> ones_to_write[], uint16_t height, vector<uint64_t> nodes_per_level) {
            binTrie::height = height;
            binTrie::bv_rank = new rank_support_v<1>[height];
            for (uint16_t level = 0; level < binTrie::height; ++level) {
                uint64_t n_nodes_level = 2 * nodes_per_level[level];
                bit_vector level_bv = bit_vector(n_nodes_level, 0);
               
                for (uint64_t j = 0; j < ones_to_write[level].size(); ++j) {
                    uint64_t pos = ones_to_write[level][j];
                    level_bv[pos] = 1;
                }
                binTrie::bTrie.push_back(level_bv);
            }
        }
        

        void initRank(){
            for (uint16_t i = 0; i < binTrie::height; ++i) {
                binTrie::bv_rank[i] = sdsl::rank_support_v<1>(&binTrie::bTrie[i]);
            }
        }


        bit_vector getNode(uint64_t node_id, uint16_t level){
            bit_vector node = bit_vector(2, 0);
            node[0] = binTrie::bTrie[level][2*node_id];
            node[1] = binTrie::bTrie[level][(2*node_id) + 1];
            return node;
        };


        uint16_t getHeight(){
            return binTrie::bTrie.size();
        };

        
        uint64_t getLeftChild(uint64_t node_id, uint16_t level) {
            uint64_t node = binTrie::bv_rank[level].rank(2*node_id + 1);
            return node - 1;
        };

        uint64_t getRightChild(uint64_t node_id, uint16_t level) {
            uint64_t node = binTrie::bv_rank[level].rank((2*node_id + 1) + 1);
            return node - 1;
        };
};
#endif