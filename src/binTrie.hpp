#ifndef BINTRIE
#define BINTRIE

#include <iostream>
#include <sdsl/int_vector.hpp>
#include <queue>
#include <math.h>
#include <vector>
#include <tuple>

using namespace sdsl;
using namespace std;

class binTrie {

    uint16_t height;

    public:
        vector <bit_vector> bTrie;
        vector < rank_support_v<> > rank_vector;

        binTrie() = default;

        binTrie(int_vector<> &set) {
            uint32_t n = set.size();
            
            util::bit_compress(set);
            binTrie::height = (uint16_t)set.width();

            for (int level = 0; level < binTrie::height; ++level) {
                uint64_t max_nodes_level = 2 * pow(2, level);
                bit_vector level_bv = bit_vector(max_nodes_level, 0);
                bTrie.push_back(level_bv);
            }

            queue<tuple<uint64_t, uint64_t, uint64_t>> q;
            
            // add all set to split
            tuple<uint64_t, uint64_t, uint64_t> split = {0, n-1, n};
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
                // Add to queue split sets and write nodes
                tuple<uint64_t,uint64_t,uint64_t> left_split;
                tuple<uint64_t,uint64_t,uint64_t> right_split;
                // left child
                if (left_elements > 0) {
                    // write 1
                    bTrie[level][index] = 1;
                    left_split = {ll, lr, left_elements};
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
                    right_split = {rl, rr, right_elements};
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
        };

        uint16_t getHeight(){
            return binTrie::bTrie.size() - 1;
        }


        uint64_t getLeftChild(uint64_t node_id, uint16_t level) {
            uint64_t node = binTrie::bTrie[level].rank(2*node_id + 1);
            return node;
        };

        uint64_t getRightChild(uint64_t node_id, uint16_t level) {
            uint64_t node = binTrie::bTrie[level].rank((2*node_id + 1) + 1);
            return node;
        };


};
#endif