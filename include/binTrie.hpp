#ifndef BINTRIE
#define BINTRIE

#include <iostream>
// #include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <queue>
#include <math.h>
#include <vector>
#include <tuple>
#include "binaryTrie.hpp"

using namespace sdsl;
using namespace std;

template <typename rankType>
class binTrie : public binaryTrie{
    
    private:
        vector <sdsl::bit_vector> bTrie;
        vector<rankType> bv_rank;
    
    public:
        binTrie()=default;

        ~binTrie()=default;

        binTrie(vector<uint64_t> set, uint64_t u) {
            uint64_t n = set.size();
            uint16_t height = floor(log2(u - 1)) +  1;
            binTrie::bTrie = vector<sdsl::bit_vector>();
            for (uint16_t level = 0; level < height; ++level) {
                uint64_t max_nodes_level = 2 * pow(2, level);
                bit_vector level_bv = bit_vector(max_nodes_level, 0);
                bTrie.push_back(level_bv);
            }

            queue<tuple<uint64_t, uint64_t, uint64_t>> q;
            
            // add all set to split
            tuple<uint64_t, uint64_t, uint64_t> split(0, n-1, n);
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
                    // (*bTrie[level])[index] = 0;
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
                if (level == height) {
                    break;
                }
            }

            for (uint16_t i = 0; i < height; ++i) {
                if (i != height -1)
                    binTrie::bv_rank.push_back(rankType(&(bTrie[i])));
            }
        };

        // Build the trie with height log2(max(set)-1)+1
        binTrie(vector<uint64_t> set){
            uint64_t u = set.back();
            binTrie(set, u);
        };

        binTrie(vector<uint64_t> ones_to_write[], uint16_t height, vector<uint64_t> nodes_per_level) {
            for (uint16_t level = 0; level < height; ++level) {
                uint64_t n_nodes_level = 2 * nodes_per_level[level];
                bit_vector level_bv =  bit_vector(n_nodes_level, 0);
               
                for (uint64_t j = 0; j < ones_to_write[level].size(); ++j) {
                    uint64_t pos = ones_to_write[level][j];
                    level_bv[pos] = 1;
                }
                binTrie::bTrie.push_back(level_bv);
            }
            for (uint16_t i = 0; i < getHeight(); ++i) {
                binTrie::bv_rank[i].push_back(rankType(&(binTrie::bTrie)[i]));
            }
        }

        inline uint64_t getNode(uint64_t &node_id, uint16_t level){
            return  ((bTrie[level][2*node_id]) << 1 ) | (bTrie[level][(2*node_id) + 1]);
        };


        inline uint16_t getHeight(){
            return binTrie::bTrie.size();
        };

        
        inline uint64_t getLeftChild(uint64_t &node_id, uint16_t level) {
            if (level >= getHeight()-1) 
                return 0;
            else
                return binTrie::bv_rank[level](2*node_id);
        };


        inline uint64_t getRightChild(uint64_t &node_id, uint16_t level) {
            if (level >= getHeight()-1)
                return 0;
            else
                return binTrie::bv_rank[level](2*node_id + 1);
        };
        
        
        uint64_t size_in_bytes() {
            uint64_t bvs_size = 0;
            uint64_t ranks_size = 0;
            for (uint16_t level = 0; level < getHeight(); ++level) {
                bvs_size += sdsl::size_in_bytes(binTrie::bTrie[level]);
                if (level < getHeight()-1)
                    ranks_size += sdsl::size_in_bytes(binTrie::bv_rank[level]);
            }

            return bvs_size +
                    ranks_size +  
                    2 * sizeof(bool) +
                    2 * sizeof(uint8_t);
        };

        // Implementar esto ...
        uint64_t serialize(std::ostream &out){
            uint16_t height = (uint16_t)(binTrie::bTrie.size());
            
            out.write(reinterpret_cast<char*>(&height) , sizeof(height));
            out.write(reinterpret_cast<char*>(&(binTrie::height_with_runs)), sizeof(binTrie::height_with_runs));
            out.write(reinterpret_cast<char*>(&(binTrie::empty_trie))      , sizeof(binTrie::empty_trie));
            out.write(reinterpret_cast<char*>(&(binTrie::runs_encoded))    , sizeof(binTrie::runs_encoded));
            
            uint64_t bvs_size=0, rank_size=0;
            
            for (uint16_t lvl=0; lvl < height; ++lvl) {
                bvs_size += binTrie::bTrie[lvl].serialize(out);                    
            }
            for (uint16_t lvl=0; lvl < height-1; ++lvl) {
                rank_size += binTrie::bv_rank[lvl].serialize(out);
            } 
            return bvs_size + rank_size + 
                   sizeof(height) + sizeof(height_with_runs) +
                   sizeof(empty_trie) + sizeof(runs_encoded);
        };

        
        void load(std::istream &in){
            uint16_t height;
            in.read(reinterpret_cast<char*>(&height)          , sizeof(height));
            in.read(reinterpret_cast<char*>(&height_with_runs), sizeof(height_with_runs));
            in.read(reinterpret_cast<char*>(&empty_trie)      , sizeof(empty_trie));
            in.read(reinterpret_cast<char*>(&runs_encoded)    , sizeof(runs_encoded));

            binTrie::bTrie = vector<sdsl::bit_vector>(height);
            binTrie::bv_rank = vector<rankType>(height-1);
            
            for (uint16_t lvl=0; lvl < height; ++lvl)
                binTrie::bTrie[lvl].load(in);
            for (uint16_t lvl=0; lvl < height-1; ++lvl)
                binTrie::bv_rank[lvl].load(in, &(binTrie::bTrie[lvl]));
        };


        void writeCompressTrie(vector<uint64_t> ones_to_write[], 
                                uint64_t* level_pos, 
                                uint16_t curr_level, uint64_t node_id, bool &its11){
            uint64_t node = getNode(node_id, curr_level);
            // End condition
            if (curr_level == (getHeight()-1)) {
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
            // uint64_t node = getNode(node_id, curr_level);
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
                    if (curr_level == getHeight() -2){
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

        
        void writeOnes(vector<uint64_t> ones_to_write[], uint64_t* level_pos){
            vector <sdsl::bit_vector> new_bTrie(getHeight());
            vector <rankType> new_b_rank(getHeight()-1);
            uint16_t last_level = 0;
            for (uint16_t level = 0; level < getHeight(); ++level) {
                if (level_pos[level] > 0) {
                    last_level = level;
                    new_bTrie[level] = sdsl::bit_vector(level_pos[level],0);
                }               
            }

            binTrie::height_with_runs = last_level + 1;
            for (uint16_t level = 0; level < getHeight(); ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    uint64_t pos = ones_to_write[level][i];
                    new_bTrie[level][pos] = 1;
                }
            }
            binTrie::bTrie = new_bTrie;
            for (uint16_t i = 0; i < getHeight()-1; ++i) {
                new_b_rank[i] = rankType(&(binTrie::bTrie[i]));
            }   
            binTrie::bv_rank = new_b_rank;
        };


        inline void encodeRuns() {
            uint16_t height = getHeight();
            binTrie::runs_encoded = true;

            vector<uint64_t> ones_to_write[height];
            vector<uint64_t> level_pos(height, 0);
            bool itsOneOne = false;
            
            writeCompressTrie(ones_to_write, level_pos.data(), 0, 0, itsOneOne);
            writeOnes(ones_to_write, level_pos.data());
        };


        inline void recursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            
            if (curr_level == getHeight()) {
                decoded.push_back(partial_int);
                return;
            }

            uint64_t node = getNode(node_id, curr_level);
            uint16_t next_level = curr_level + 1;

            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                uint64_t left_child = getLeftChild(node_id, curr_level);
                recursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                uint64_t right_child = getRightChild(node_id, curr_level);
                recursiveDecode(decoded, rightResult, right_child, next_level);
            }
        };


        inline void runsRecursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            if (curr_level == binTrie::height_with_runs) {
                decoded.push_back(partial_int);
                return;
            }

            uint64_t node = getNode(node_id, curr_level);
            uint16_t next_level = curr_level + 1;

            if (node == 0b00) { 
                uint64_t below = partial_int;
                uint64_t range = ((uint64_t)1 << (getHeight() - curr_level)) - 1;
                uint64_t above = (partial_int | range);
                for (uint64_t i = below; i <= above; ++i) {
                    decoded.push_back(i);
                }
                return;
            }

            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                uint64_t left_child = getLeftChild(node_id, curr_level);
                runsRecursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                uint64_t right_child = getRightChild(node_id, curr_level);
                runsRecursiveDecode(decoded, rightResult, right_child, next_level);
            }
        };


        inline void decode( vector<uint64_t> &decoded) {
            if (binaryTrie::runs_encoded) {
                if (binaryTrie::empty_trie) {
                    return;
                }
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


        inline uint64_t trieMeasure() {
            uint64_t nEdges = 0;
            for (uint16_t lvl = 0; lvl < getHeight()-1; ++lvl) 
                nEdges += binTrie::bv_rank[lvl](binTrie::bTrie[lvl].size());
            for (uint64_t i = 0; i < binTrie::bTrie[getHeight()-1]; ++i) {
                if (bTrie[getHeight()-1][i] == 1) 
                    nEdges++;
            }
            return nEdges;
        };
};
#endif