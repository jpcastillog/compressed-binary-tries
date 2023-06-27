#ifndef FLAT_BINTRIE
#define FLAT_BINTRIE

#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <vector>
#include <queue>
#include <math.h>
#include <thread>
#include "binaryTrie.hpp"

using namespace sdsl;
using namespace std;

template <class rankType>
class flatBinTrie: public binaryTrie{
    private:
        uint16_t height; // original height of trie
        sdsl::bit_vector bTrie;
        sdsl::bit_vector lastLevel;     
        rankType b_rank;        

    public:
        flatBinTrie() = default;
        ~flatBinTrie() = default;

        flatBinTrie(vector<uint64_t> &set, uint64_t u) {
            flatBinTrie::runs_encoded = false;
            uint32_t n = set.size();

            uint16_t height = floor(log2(u - 1)) +  1;
            flatBinTrie::height = height;

            uint64_t max_nodes            = 2 * (pow(2, height) - 1);
            uint64_t max_nodes_last_level = pow(2, height);  
            
            flatBinTrie::bTrie     = bit_vector(max_nodes, 0);
            flatBinTrie::lastLevel = bit_vector(max_nodes_last_level, 0);
            
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
            uint64_t nodes_last_level = 0;

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
                    if ((set[i] >> (j-1)) & 1) {                        
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
                    if (level == height-1)
                        lastLevel[index] = 1;
                    else
                        bTrie[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
                    q.push(left_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    index++;
                    total_nodes++;
                }
                // right child
                if (right_elements > 0) {
                    // write 1
                    if (level == height-1)
                        lastLevel[index] = 1;
                    else
                        bTrie[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
                    q.push(right_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    index++;
                    total_nodes++;
                }

                if (count_nodes == nodes_curr_level) {
                    if (level == height-2){
                        nodes_last_level = nodes_next_level;
                        index = 0;
                    }
                    
                    nodes_curr_level = nodes_next_level;
                    nodes_next_level = 0;
                    count_nodes = 0;
                    level++;
                    
                }
                if (level == flatBinTrie::height) break;
            }

            flatBinTrie::bTrie.resize(total_nodes - 2*nodes_last_level);
            flatBinTrie::lastLevel.resize(2*nodes_last_level);
            flatBinTrie::b_rank = rankType(&bTrie);
        };

        // Build the trie with height log2(max(set)-1)+1
        flatBinTrie(vector<uint64_t> set){
            uint64_t u = set.back();
            flatBinTrie(set, u);
        };


        flatBinTrie(vector<uint64_t> ones_to_write[], uint16_t height, uint64_t* level_pos, bool runs_encoded) {
            binaryTrie::runs_encoded = runs_encoded;
            flatBinTrie::height = height;
            
            uint64_t nbits = 0;
            uint16_t all_levels_empty = 0;
            uint16_t max_level_not_empty = 0;
            
            for (uint16_t level = 0; level < height; ++level) {
                nbits += level_pos[level];
                if (level_pos[level] > 0){
                    max_level_not_empty = level;
                }
                else
                    all_levels_empty++;
            }

            binaryTrie::empty_trie = all_levels_empty == flatBinTrie::height ? true : false;
            binaryTrie::height_with_runs = max_level_not_empty + 1;

            flatBinTrie::bTrie     = bit_vector(nbits - level_pos[max_level_not_empty], 0);
            flatBinTrie::lastLevel = bit_vector(level_pos[max_level_not_empty], 0); 

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    if (level == max_level_not_empty) {
                        uint64_t pos = ones_to_write[level][i];
                        flatBinTrie::lastLevel[pos] = 1;
                    }
                    else {
                        uint64_t pos = global_level_pos + ones_to_write[level][i];
                        flatBinTrie::bTrie[pos] = 1;
                    }
                }
                global_level_pos += level < max_level_not_empty ? level_pos[level] : 0;
            }
            flatBinTrie::b_rank = rankType(&bTrie);
        };


        inline uint16_t getHeight(){
            return flatBinTrie::height;
        };


        inline uint64_t getNode(uint64_t &node_id, uint16_t level) {
            if (level < flatBinTrie::height-1) {
                return ((bTrie[2 * node_id]) << 1) | bTrie[(2 * node_id)+1];
            }
            else {
                return (lastLevel[2*node_id - flatBinTrie::bTrie.size()] << 1) | lastLevel[(2*node_id - flatBinTrie::bTrie.size())+1];
            }
        };


        inline uint64_t getLeftChild(uint64_t &node_id, uint16_t level) {
            // if (level == getHeight()-2)
            //     return flatBinTrie::b_rank((2*node_id));
            // else 
            if (level >= getHeight() - 1)
                return 0;
            else 
                return flatBinTrie::b_rank((2*node_id)+1);
                
        };


        inline uint64_t getRightChild(uint64_t &node_id, uint16_t level) {
            // if (level == getHeight()-2)
            //     return flatBinTrie::b_rank((2*node_id) + 1);
            // else
             if (level >= getHeight() - 1)
                return 0;
            else
                return flatBinTrie::b_rank((2*node_id)+2);
                
        };

        // return size of bytes of all structure
        inline uint64_t size_in_bytes() {
            uint64_t bv_size = sdsl::size_in_bytes(flatBinTrie::bTrie);
            uint64_t lastL_size = sdsl::size_in_bytes(flatBinTrie::lastLevel);
            uint64_t rank_size = sdsl::size_in_bytes(flatBinTrie::b_rank);
            return bv_size +
                    rank_size +
                    lastL_size +
                    2 * sizeof(bool) +
                    2 * sizeof(uint8_t);
        };
        
        uint64_t serialize(std::ostream &out) {

            out.write(reinterpret_cast<char*>(&height)          , sizeof(height));
            out.write(reinterpret_cast<char*>(&(binaryTrie::height_with_runs)), sizeof(binaryTrie::height_with_runs));
            out.write(reinterpret_cast<char*>(&(binaryTrie::empty_trie))      , sizeof(binaryTrie::empty_trie));
            out.write(reinterpret_cast<char*>(&(binaryTrie::runs_encoded))    , sizeof(binaryTrie::runs_encoded));

            uint64_t bvs_size, rank_size;
            bvs_size  = bTrie.serialize(out) + lastLevel.serialize(out);
            rank_size = b_rank.serialize(out);

            return bvs_size + rank_size + 
                   sizeof(height) + sizeof(height_with_runs) +
                   sizeof(empty_trie) + sizeof(runs_encoded);
        }

        // load structure from in "in" stream
        void load(std::istream &in){
            in.read(reinterpret_cast<char*>(&height)          , sizeof(height));
            in.read(reinterpret_cast<char*>(&(binaryTrie::height_with_runs)), sizeof(binaryTrie::height_with_runs));
            in.read(reinterpret_cast<char*>(&(binaryTrie::empty_trie))      , sizeof(binaryTrie::empty_trie));
            in.read(reinterpret_cast<char*>(&(binaryTrie::runs_encoded))    , sizeof(binaryTrie::runs_encoded));

            flatBinTrie::bTrie.load(in);
            flatBinTrie::lastLevel.load(in);
            b_rank.load(in, &bTrie);
        };


        void writeCompressTrie(vector<uint64_t> ones_to_write[], 
                                uint64_t* level_pos, 
                                uint16_t curr_level, uint64_t node_id, bool &its11){
            // End condition
            if (curr_level == (flatBinTrie::height-1)) {
                uint64_t node = getNode(node_id, curr_level);
                if (node == 0b11) {
                    its11 = true;
                }
                if (node == 0b10 || node == 0b11){
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                }
                level_pos[curr_level] += 1;
                if (node == 0b01 || node == 0b11) {
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                }
                level_pos[curr_level] += 1;
                
                return;
            }
            uint64_t node = getNode(node_id, curr_level);
            uint16_t next_level = curr_level + 1;
            uint64_t next_level_pos = level_pos[next_level];

            bool its11_l = false;
            bool its11_r = false;
            bool actualIts11 = false;

            if (node == 0b11) {
                actualIts11 = true;

                uint64_t l_child = getLeftChild(node_id, curr_level);
                uint64_t r_child = l_child + 1;
                writeCompressTrie(ones_to_write, level_pos, next_level, l_child, its11_l);
                writeCompressTrie(ones_to_write, level_pos, next_level, r_child, its11_r);
                
                its11 = true && its11_l && its11_r;
                if (its11) {
                    level_pos[next_level] -= 4;
                    if (curr_level == flatBinTrie::height -2){
                        for (uint64_t i = 0; i < 4; ++i) {
                            ones_to_write[next_level].pop_back();
                        }
                    }
                }
                else {
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    ones_to_write[curr_level].push_back(level_pos[curr_level] + 1);      
                }
                
                level_pos[curr_level] += 2;
            }

            else if (node == 0b10 || node == 0b01){
                if (node == 0b10){
                    uint64_t l_child = getLeftChild(node_id, curr_level);
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    writeCompressTrie(ones_to_write, level_pos, next_level, l_child, its11_l);
                }
                level_pos[curr_level] += 1;
                if (node == 0b01) {
                    uint64_t r_child = getRightChild(node_id, curr_level);
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    writeCompressTrie(ones_to_write, level_pos, next_level, r_child, its11_r);
                }
                level_pos[curr_level] += 1;
            }
        };

        // Method write ones in a bit vector
        void writeOnes(vector<uint64_t> ones_to_write[], uint64_t* level_pos){
            binaryTrie::runs_encoded = true;
            uint64_t bits_n = 0;
            uint16_t last_level = 0;
            uint64_t bits_before_last_level;
            for (uint16_t level = 0; level < flatBinTrie::height; ++level) {
                bits_n += level_pos[level];
                if (level_pos[level] > 0) {
                    last_level = level; 
                }               
            }

            flatBinTrie::height_with_runs = last_level + 1;
            
            flatBinTrie::bTrie     = bit_vector(bits_n - level_pos[last_level], 0);
            flatBinTrie::lastLevel = bit_vector(level_pos[last_level], 0);

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height_with_runs; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    if (level == last_level){
                        uint64_t pos = ones_to_write[level][i];
                        flatBinTrie::lastLevel[pos] = 1;
                    }
                    else {
                        uint64_t pos = global_level_pos + ones_to_write[level][i];
                        flatBinTrie::bTrie[pos] = 1;
                    }
                }
                global_level_pos += level < last_level ? level_pos[level] : 0;
            }
            delete[] level_pos;
            flatBinTrie::b_rank = rankType(&bTrie);
        };


        inline void encodeRuns() {
            flatBinTrie::runs_encoded = true;
            vector<uint64_t> ones_to_write[flatBinTrie::height];
            uint64_t *level_pos = new uint64_t[height];
            for(uint64_t i = 0; i < flatBinTrie::height; ++i) level_pos[i] = 0;
            bool itsOneOne = false;
            flatBinTrie::writeCompressTrie(ones_to_write, level_pos, 0, 0, itsOneOne);
            writeOnes(ones_to_write, level_pos);
        };

        
        inline void recursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            
            if (curr_level == flatBinTrie::height) {
                decoded.push_back(partial_int);
                return;
            }

            uint64_t node = getNode(node_id, curr_level);
            // uint16_t next_level = curr_level + 1;

            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                // uint64_t left_child = getLeftChild(node_id, curr_level);
                recursiveDecode(decoded, leftResult, getLeftChild(node_id, curr_level), curr_level+1);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                // uint64_t right_child = getRightChild(node_id, curr_level);
                recursiveDecode(decoded, rightResult, getRightChild(node_id, curr_level), curr_level+1);
            }
        };


        inline void runsRecursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            if (curr_level == flatBinTrie::height) {
                decoded.push_back(partial_int);
                return;
            }

            uint64_t node = getNode(node_id, curr_level);
            // uint16_t next_level = curr_level + 1;

            if (node == 0b00) { 
                uint64_t below = partial_int;
                uint64_t range = ((uint64_t)1 << (getHeight() - curr_level)) - 1;
                uint64_t above = (partial_int | range);
                for (uint64_t i = below; i <= above; ++i) {
                    decoded.push_back(i);
                }
                return;
            }

            uint64_t leftResult  = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                // uint64_t left_child = getLeftChild(node_id, curr_level);
                runsRecursiveDecode(decoded, leftResult, getLeftChild(node_id, curr_level), curr_level+1);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                // uint64_t right_child = getRightChild(node_id, curr_level);
                runsRecursiveDecode(decoded, rightResult, getRightChild(node_id, curr_level), curr_level+1);
            }
        };


        inline void decode( vector<uint64_t> &decoded) {
            if (binaryTrie::runs_encoded) {
                if (binaryTrie::empty_trie) return;
                else {
                    uint64_t partial_int = 0x00;
                    runsRecursiveDecode(decoded, partial_int, 0, 0);
                }
            }
            else {
                uint64_t partial_int = 0x00;
                recursiveDecode(decoded, partial_int, 0, 0);
            }
        };

        // If runs are encoded, this measure it's trie-run
        inline uint64_t trieMeasure() {
            uint64_t nEdges = 0;
            nEdges += flatBinTrie::b_rank(flatBinTrie::bTrie.size());
            for (uint64_t i = 0; i < flatBinTrie::lastLevel.size(); ++i){
                if (lastLevel[i] == 1) 
                    nEdges++;
            }
            return nEdges;
        };
    
};

#endif