#ifndef FAST_BINARY_TRIE
#define FAST_BINARY_TRIE

#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support_v.hpp>
#include <sdsl/rank_support_v5.hpp>
#include <vector>
#include <queue>
#include <math.h>
#include <thread>
#include <bit>

using namespace sdsl;
using namespace std;

template <class rankType, class wordType>
class fastBinaryTrie{
    private:
        uint16_t height; // original height of trie
        uint16_t height_with_runs;
        uint16_t height_u;
        bool runs_encoded;
        bool empty_trie;
        sdsl::bit_vector bTrie;
        vector<wordType> lastLevel;     
        rankType b_rank;        

    public:
        fastBinaryTrie() = default;
        ~fastBinaryTrie() = default;

        fastBinaryTrie(vector<uint64_t> &set, uint64_t u) {
            fastBinaryTrie::runs_encoded = false;
            uint32_t n = set.size();

            fastBinaryTrie::height_u = floor(log2(u - 1)) +  1;
            fastBinaryTrie::height = height_u - floor(log2(sizeof(wordType)*8)) + 1; //only works for height > 6
            uint64_t max_nodes     = 2 * (pow(2, height_u) - 1);
            
            fastBinaryTrie::bTrie = bit_vector(max_nodes, 0);

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
                uint64_t l, r, n;
                std::tie(l, r, n) = s;
                q.pop();

                if (level == height-1){
                    wordType word = 0;
                    for (uint64_t i = l; i < r+1; ++i){
                        uint64_t element = set[i] & (((uint64_t)1 << (uint64_t)(log2(sizeof(wordType)*8))) - 1);
                        word |= ((wordType)1 << element);
                    }
                    fastBinaryTrie::lastLevel.push_back(word);
                    if (count_nodes == nodes_curr_level) break;
                }
                else {
                    uint64_t left_elements  = 0;
                    uint64_t right_elements = 0;

                    // j-th most significative bit
                    uint8_t j = height_u - level;
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
                            // index = 0;
                        }
                        nodes_curr_level = nodes_next_level;
                        nodes_next_level = 0;
                        count_nodes = 0;
                        level++;
                    }
                }
                if (level == fastBinaryTrie::height) break;
            }

            fastBinaryTrie::bTrie.resize(index);
            fastBinaryTrie::b_rank = rankType(&bTrie);
        };

        // Build the trie with height log2(max(set)-1)+1
        fastBinaryTrie(vector<uint64_t> set){
            uint64_t u = set.back();
            fastBinaryTrie(set, u);
        };


        inline uint16_t getHeight(){
            return fastBinaryTrie::height;
        };


        inline uint64_t getNode(uint64_t &node_id, uint16_t level) {
            // if (level < fastBinaryTrie::height-1) {
                return ((bTrie[2 * node_id]) << 1) | bTrie[(2 * node_id)+1];
            // }
            // else {
            //     // cout << nodev_id << ", " << (fastBinaryTrie::bTrie.size()/2) << endl;
            //     return lastLevel[node_id - (fastBinaryTrie::bTrie.size()/2)];
            // }
        };

        inline wordType getWord(uint64_t &node_id){
            return lastLevel[node_id - (fastBinaryTrie::bTrie.size()/2)];
        };


        inline uint64_t getLeftChild(uint64_t &node_id, uint16_t level) {
            if (level >= getHeight() - 1)
                return 0;
            else 
                return fastBinaryTrie::b_rank((2*node_id)+1);
        };


        inline uint64_t getRightChild(uint64_t &node_id, uint16_t level) {
             if (level >= getHeight() - 1)
                return 0;
            else
                return fastBinaryTrie::b_rank((2*node_id)+2);
                
        };

        // return size of bytes of all data structure
        inline uint64_t size_in_bytes() {
            uint64_t bv_size = sdsl::size_in_bytes(fastBinaryTrie::bTrie);
            uint64_t lastL_size = sizeof(fastBinaryTrie::lastLevel) + 
                                  sizeof(wordType)*fastBinaryTrie::lastLevel.size();
            uint64_t rank_size = sdsl::size_in_bytes(fastBinaryTrie::b_rank);
            return bv_size +
                    rank_size +
                    lastL_size +
                    2 * sizeof(bool) +
                    2 * sizeof(uint8_t);
        };
        
        uint64_t serialize(std::ostream &out) {
            out.write(reinterpret_cast<char*>(&height)          , sizeof(height));
            out.write(reinterpret_cast<char*>(&(fastBinaryTrie::height_with_runs)), sizeof(fastBinaryTrie::height_with_runs));
            out.write(reinterpret_cast<char*>(&(fastBinaryTrie::empty_trie))      , sizeof(fastBinaryTrie::empty_trie));
            out.write(reinterpret_cast<char*>(&(fastBinaryTrie::runs_encoded))    , sizeof(fastBinaryTrie::runs_encoded));

            uint64_t bvs_size, rank_size;
            bvs_size  = bTrie.serialize(out);
            rank_size = b_rank.serialize(out);
            
            uint64_t size_last_level = lastLevel.size();
            out.write(reinterpret_cast<char*>(&size_last_level), sizeof(uint64_t));
            for (wordType x: lastLevel)
                out.write(reinterpret_cast<char*>(&x), sizeof(wordType));

            return bvs_size + rank_size + sizeof(lastLevel) + 
                   sizeof(height) + sizeof(height_with_runs) +
                   sizeof(empty_trie) + sizeof(runs_encoded);
        }

        // load structure from in "in" stream
        void load(std::istream &in){
            in.read(reinterpret_cast<char*>(&height)          , sizeof(height));
            in.read(reinterpret_cast<char*>(&(fastBinaryTrie::height_with_runs)), sizeof(fastBinaryTrie::height_with_runs));
            in.read(reinterpret_cast<char*>(&(fastBinaryTrie::empty_trie))      , sizeof(fastBinaryTrie::empty_trie));
            in.read(reinterpret_cast<char*>(&(fastBinaryTrie::runs_encoded))    , sizeof(fastBinaryTrie::runs_encoded));

            fastBinaryTrie::bTrie.load(in);
            b_rank.load(in, &bTrie);

            uint64_t sizeLastLevel;
            in.read(reinterpret_cast<char*>(&sizeLastLevel), sizeof(uint64_t));
            lastLevel.reserve(sizeLastLevel);
            for (uint64_t i = 0; i < sizeLastLevel; ++i){
                wordType x;
                in.read(reinterpret_cast<char*>(&x), sizeof(wordType));
                lastLevel.push_back(x);
            }
            height_u = height + (uint16_t)(log2(sizeof(wordType)*8)) - 1;
        };


        void writeCompressTrie(vector<uint64_t> ones_to_write[], //vector array
                                vector<wordType> &newLastLevel,
                                uint64_t* level_pos, 
                                uint16_t curr_level, uint64_t node_id, bool &its11){
            // End condition
            if (curr_level == (fastBinaryTrie::height-1)) {
                wordType w = getWord(node_id);
                if (w == ((wordType)~0)) {
                    its11 = true;
                }
                newLastLevel.push_back(w);
                ones_to_write[curr_level].push_back(level_pos[curr_level]);
                return;
            }

            uint64_t node = getNode(node_id, curr_level);
            uint16_t next_level = curr_level + 1;
            uint64_t next_level_pos = level_pos[next_level];
            bool its11_l = false;
            bool its11_r = false;

            if (node == 0b11) {
                uint64_t l_child = getLeftChild(node_id, curr_level);
                uint64_t r_child = l_child + 1;
                writeCompressTrie(ones_to_write, newLastLevel, level_pos, next_level, l_child, its11_l);
                writeCompressTrie(ones_to_write, newLastLevel, level_pos, next_level, r_child, its11_r);
                
                its11 = its11_l && its11_r;
                if (its11) {
                    if (curr_level == fastBinaryTrie::height - 2){
                        for (uint16_t i = 0; i < 2; ++i) {
                            newLastLevel.pop_back();
                        }
                    }
                    // else 
                        level_pos[next_level] -= 4;
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
                    writeCompressTrie(ones_to_write, newLastLevel, level_pos, next_level, l_child, its11_l);
                }
                level_pos[curr_level] += 1;
                if (node == 0b01) {
                    uint64_t r_child = getRightChild(node_id, curr_level);
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    writeCompressTrie(ones_to_write, newLastLevel, level_pos, next_level, r_child, its11_r);
                }
                level_pos[curr_level] += 1;
            }
        };

        // Method write ones in a bit vector
        void writeOnes(vector<uint64_t> ones_to_write[], uint64_t* level_pos){
            uint64_t bits_n = 0;
            uint16_t last_level = 0;
            uint64_t bits_before_last_level;
            for (uint16_t level = 0; level < fastBinaryTrie::height-1; ++level) {
                bits_n += level_pos[level];
                if (level_pos[level] > 0) {
                    last_level = level;
                }              
            }

            fastBinaryTrie::height_with_runs = last_level + 1;
            fastBinaryTrie::bTrie = bit_vector(bits_n, 0);

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < fastBinaryTrie::height-1; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    uint64_t pos = global_level_pos + ones_to_write[level][i];
                    fastBinaryTrie::bTrie[pos] = 1;
                }                global_level_pos += level_pos[level];
            }
            fastBinaryTrie::b_rank = rankType(&bTrie);
        };


        inline void encodeRuns() {
            vector<uint64_t> ones_to_write[fastBinaryTrie::height];
            uint64_t *level_pos = new uint64_t[fastBinaryTrie::height];
            for(uint64_t i = 0; i < fastBinaryTrie::height; ++i) level_pos[i] = 0;
            
            vector<wordType> newLastLevel;
            bool itsOneOne = false;
            writeCompressTrie(ones_to_write, newLastLevel, level_pos, 0, 0, itsOneOne);
            fastBinaryTrie::lastLevel = newLastLevel;
            
            writeOnes(ones_to_write, level_pos);
            fastBinaryTrie::runs_encoded = true;
            delete[] level_pos;
        };

        
        inline void recursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            
            if (curr_level == fastBinaryTrie::height-1) {
                wordType w = getWord(node_id);
                while (w != 0){
                    wordType t = w & (~w + 1);            
                    uint64_t r = __builtin_ctzll(w);
                    decoded.push_back(r + partial_int);
                    w ^= t;
                }
                return;
            }

            uint64_t node = getNode(node_id, curr_level);
            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                recursiveDecode(decoded, leftResult, getLeftChild(node_id, curr_level), curr_level+1);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | ((uint64_t)1 << (height_u - curr_level - 1)));
                recursiveDecode(decoded, rightResult, getRightChild(node_id, curr_level), curr_level+1);
            }
        };


        inline void runsRecursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            
            if (curr_level == fastBinaryTrie::height-1) {
                wordType w = getWord(node_id);
                while (w != 0){
                    wordType t = w & (~w + 1);            
                    uint64_t r = __builtin_ctzll(w);
                    decoded.push_back(r + partial_int);
                    w ^= t;
                }
                return;
            }

            uint64_t node = getNode(node_id, curr_level);

            if (node == 0b00) { 
                uint64_t below = partial_int;
                uint64_t range = ((uint64_t)1 << (height_u - curr_level)) - 1;
                uint64_t above = (partial_int | range);
                for (uint64_t i = below; i <= above; ++i) {
                    decoded.push_back(i);
                }
                return;
            }

            uint64_t leftResult  = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                runsRecursiveDecode(decoded, leftResult, getLeftChild(node_id, curr_level), curr_level+1);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | ((uint64_t)1 << (height_u - curr_level - 1)));
                runsRecursiveDecode(decoded, rightResult, getRightChild(node_id, curr_level), curr_level+1);
            }
        };


        inline void decode( vector<uint64_t> &decoded) {
            if (fastBinaryTrie::runs_encoded) {
                if (fastBinaryTrie::empty_trie) return;
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
        // inline uint64_t trieMeasure() {
        //     uint64_t nEdges = 0;
        //     nEdges += flatBinTrie::b_rank(flatBinTrie::bTrie.size());
        //     for (uint64_t i = 0; i < flatBinTrie::lastLevel.size(); ++i){
        //         if (lastLevel[i] == 1) 
        //             nEdges++;
        //     }
        //     return nEdges;
        // };
    
};

#endif