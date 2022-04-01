#ifndef BINTRIE_IL
#define BINTRIE_IL


#include <iostream>
#include <vector>
#include <queue>
#include <sdsl/bit_vectors.hpp>
#include "flatBinTrie.hpp"
// #define rankType rank_support_il<1, block_size>

using namespace std;
using namespace sdsl;

template <uint32_t block_size, class rankType>
class binTrie_il : public flatBinTrie<rankType>{
    private:
        /* data */
        sdsl::bit_vector_il<block_size>* bv;
        sdsl::bit_vector_il<block_size>* lastLevel_bv;
    
    public:
        binTrie_il() = default;
        binTrie_il(vector<uint64_t> &set, uint64_t u) {
            flatBinTrie<rankType>::runs_encoded = false;
            uint32_t n = set.size();

            uint16_t height = floor(log2(u - 1)) +  1;
            flatBinTrie<rankType>::height = height;
            flatBinTrie<rankType>::level_pos = vector<uint64_t>(height, 0);
            
            // uint64_t max_nodes = 2*(pow(2, height+1) - 1);
            uint64_t max_nodes            = 2 * (pow(2, height) - 1);
            uint64_t max_nodes_last_level = pow(2, height);  
            
            flatBinTrie<rankType>::bTrie     = new bit_vector(max_nodes, 0);
            flatBinTrie<rankType>::lastLevel = new bit_vector(max_nodes_last_level, 0);
            
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
                    if (level == height-1)
                        (*flatBinTrie<rankType>::lastLevel )[index] = 1;
                    else
                        (*flatBinTrie<rankType>::bTrie)[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
                    q.push(left_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    // (*bTrie)[index] = 0;
                    index++;
                    total_nodes++;
                }
                // right child
                if (right_elements > 0) {
                    // write 1
                    if (level == height-1)
                        (*flatBinTrie<rankType>::lastLevel)[index] = 1;
                    else
                        (*flatBinTrie<rankType>::bTrie)[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
                    q.push(right_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    // (*bTrie)[index] = 0;
                    index++;
                    total_nodes++;
                }

                if (count_nodes == nodes_curr_level) {
                    // bTrie[level].resize(2*count_nodes);'
                    flatBinTrie<rankType>::level_pos[level] = index;
                    if (level == height-2){
                        nodes_last_level = nodes_next_level;
                        index = 0;
                    }
                    
                    nodes_curr_level = nodes_next_level;
                    nodes_next_level = 0;
                    count_nodes = 0;
                    level++;
                    
                }
                if (level == flatBinTrie<rankType>::height) {
                    break;
                }
            }

            flatBinTrie<rankType>::bTrie      -> resize(total_nodes - 2*nodes_last_level);
            flatBinTrie<rankType>::lastLevel  -> resize(2*nodes_last_level);
            binTrie_il::bv           = new bit_vector_il<block_size>(flatBinTrie<rankType>::bTrie);
            binTrie_il::lastLevel_bv = new bit_vector_il<block_size>(flatBinTrie<rankType>::lastLevel);
            
            delete flatBinTrie<rankType>::bTrie;
            delete flatBinTrie<rankType>::lastLevel;

            flatBinTrie<rankType>::b_rank = rankType(bv);
        }


        binTrie_il(vector<uint64_t> ones_to_write[], uint16_t height, vector<uint64_t> &level_pos, bool runs_encoded) {
            flatBinTrie<rankType>::runs_encoded = runs_encoded;
            flatBinTrie<rankType>::height = height;
            
            uint64_t bits_n = 0;
            uint16_t all_levels_empty = 0;
            uint16_t max_level_not_empty = 0;
            
            for (uint16_t level = 0; level < height; ++level) {
                bits_n += level_pos[level];
                if (level_pos[level] > 0){
                    max_level_not_empty = level;
                }
                else {
                    all_levels_empty++;
                }
                    
            }

            if (all_levels_empty == flatBinTrie<rankType>::height) {
                flatBinTrie<rankType>::empty_trie = true;
            }
            else {
                flatBinTrie<rankType>::empty_trie = false;
            }

            flatBinTrie<rankType>::height_with_runs = max_level_not_empty + 1;

            flatBinTrie<rankType>::bTrie     = new bit_vector(bits_n - level_pos[max_level_not_empty], 0);
            flatBinTrie<rankType>::lastLevel = new bit_vector(level_pos[max_level_not_empty], 0); 
            flatBinTrie<rankType>::level_pos = vector<uint64_t>(height, 0);

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    if (level == max_level_not_empty) {
                        uint64_t pos = ones_to_write[level][i];
                        (*flatBinTrie<rankType>::lastLevel)[pos] = 1;
                    }
                    else {
                        uint64_t pos = global_level_pos + ones_to_write[level][i];
                        (*flatBinTrie<rankType>::bTrie)[pos] = 1;
                    }
                }
                // cout << endl;
                if (level == max_level_not_empty) {
                    flatBinTrie<rankType>::level_pos[level] = level_pos[level];
                }
                else {
                    global_level_pos += level_pos[level];
                    flatBinTrie<rankType>::level_pos[level] = global_level_pos;
                }
                
            }
            binTrie_il::bv           = new bit_vector_il<block_size>(flatBinTrie<rankType>::bTrie);
            binTrie_il::lastLevel_bv = new bit_vector_il<block_size>(flatBinTrie<rankType>::lastLevel);

            delete flatBinTrie<rankType>::bTrie;
            delete flatBinTrie<rankType>::lastLevel;

            flatBinTrie<rankType>::b_rank = rankType(bv);
        }


        inline uint64_t getNode(uint64_t node_id) {
            uint64_t node = 0;
            uint64_t pos;
            if ((2*node_id) >= (binTrie_il::bv -> size())) {
                pos = (2*node_id) - (binTrie_il::bv -> size());
                if ((*binTrie_il::lastLevel_bv)[pos] == 1)
                    node = (node | (1ULL << 1));

                if ((*binTrie_il::lastLevel_bv)[pos + 1] == 1)
                    node = (node | (1ULL << 0));
            }
            else { 
                pos = 2*node_id;
                if ((*binTrie_il::bv)[pos] == 1)
                    node = (node | (1ULL << 1));

                if ((*binTrie_il::bv)[pos + 1] == 1)
                    node = (node | (1ULL << 0));
            }

            return node;
        }


        inline uint64_t size_in_bytes() {
            uint64_t bv_size = sdsl::size_in_bytes(*(binTrie_il::bv));
            uint64_t lastL_size = sdsl::size_in_bytes(*(binTrie_il::lastLevel_bv));
            uint64_t rank_size = sdsl::size_in_bytes(flatBinTrie<rankType>::b_rank);
            return bv_size +
                    rank_size +
                    lastL_size +
                    2 * sizeof(bool) +
                    2 * sizeof(uint8_t);
        }


        inline uint64_t serialize(std::ostream &out) {

            out.write(reinterpret_cast<char*>(&flatBinTrie<rankType>::height)          , sizeof(flatBinTrie<rankType>::height));
            out.write(reinterpret_cast<char*>(&flatBinTrie<rankType>::height_with_runs), sizeof(flatBinTrie<rankType>::height_with_runs));
            out.write(reinterpret_cast<char*>(&flatBinTrie<rankType>::empty_trie)      , sizeof(flatBinTrie<rankType>::empty_trie));
            out.write(reinterpret_cast<char*>(&flatBinTrie<rankType>::runs_encoded)    , sizeof(flatBinTrie<rankType>::runs_encoded));

            uint64_t bvs_size, rank_size;
            bvs_size  = binTrie_il::bv -> serialize(out) + 
                        binTrie_il::lastLevel_bv -> serialize(out);
            rank_size = flatBinTrie<rankType>::b_rank.serialize(out);

            return bvs_size + rank_size + 
                    sizeof(flatBinTrie<rankType>::height) + sizeof(flatBinTrie<rankType>::height_with_runs) +
                    sizeof(flatBinTrie<rankType>::empty_trie) + sizeof(flatBinTrie<rankType>::runs_encoded);
        }


        inline void load(std::istream &in){
            in.read(reinterpret_cast<char*>(&(flatBinTrie<rankType>::height))          , sizeof(flatBinTrie<rankType>::height));
            in.read(reinterpret_cast<char*>(&(flatBinTrie<rankType>::height_with_runs)), sizeof(flatBinTrie<rankType>::height_with_runs));
            in.read(reinterpret_cast<char*>(&(flatBinTrie<rankType>::empty_trie))      , sizeof(flatBinTrie<rankType>::empty_trie));
            in.read(reinterpret_cast<char*>(&(flatBinTrie<rankType>::runs_encoded))    , sizeof(flatBinTrie<rankType>::runs_encoded));

            binTrie_il::bv           = new sdsl::bit_vector_il<block_size>();
            binTrie_il::lastLevel_bv = new sdsl::bit_vector_il<block_size>();

            binTrie_il::bv     -> load(in);
            binTrie_il::lastLevel_bv -> load(in);\
            flatBinTrie<rankType>::b_rank.load(in, bv);
        }


        inline void writeOnes(vector<uint64_t> ones_to_write[], vector<uint64_t> level_pos){
            flatBinTrie<rankType>::runs_encoded = true;
            uint64_t bits_n = 0;
            uint16_t last_level = 0;
            uint64_t bits_before_last_level;
            for (uint16_t level = 0; level < flatBinTrie<rankType>::height; ++level) {
                bits_n += level_pos[level];
                if (level_pos[level] > 0) {
                    last_level = level; 
                }
            }

            flatBinTrie<rankType>::height_with_runs = last_level + 1;

            delete flatBinTrie<rankType>::bTrie;
            delete flatBinTrie<rankType>::lastLevel;
            
            flatBinTrie<rankType>::bTrie     = new bit_vector(bits_n - level_pos[last_level], 0);
            flatBinTrie<rankType>::lastLevel = new bit_vector(level_pos[last_level], 0);
            flatBinTrie<rankType>::level_pos = vector<uint64_t>(flatBinTrie<rankType>::height, 0);
            
            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < flatBinTrie<rankType>::height_with_runs; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    if (level == last_level){
                        uint64_t pos = ones_to_write[level][i];
                        (*flatBinTrie<rankType>::lastLevel)[pos] = 1;
                    }
                    else {
                        uint64_t pos = global_level_pos + ones_to_write[level][i];
                        (*flatBinTrie<rankType>::bTrie)[pos] = 1;
                    }
                }

                if (level == last_level) {
                    flatBinTrie<rankType>::level_pos[level] = level_pos[level];
                }
                else {
                    global_level_pos += level_pos[level];
                    flatBinTrie<rankType>::level_pos[level] = global_level_pos;
                }
            }
            binTrie_il::bv           = new bit_vector_il<block_size>(flatBinTrie<rankType>::bTrie);
            binTrie_il::lastLevel_bv = new bit_vector_il<block_size>(flatBinTrie<rankType>::lastLevel);

            delete flatBinTrie<rankType>::bTrie;
            delete flatBinTrie<rankType>::lastLevel;

            flatBinTrie<rankType>::b_rank = rankType(flatBinTrie<rankType>::bTrie);
        }


        inline void encodeRuns() {
            vector<uint64_t> ones_to_write[flatBinTrie<rankType>::height];
            vector<uint64_t> level_pos(flatBinTrie<rankType>::height, 0);
            bool itsOneOne = false;

            flatBinTrie<rankType>::writeCompressTrie(ones_to_write, level_pos, 0, 0, itsOneOne);
            binTrie_il::writeOnes(ones_to_write, level_pos);
        }


        inline void recursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            
            if (curr_level == flatBinTrie<rankType>::height) {
                decoded.push_back(partial_int);
                return;
            }

            uint64_t node = binTrie_il::getNode(node_id);
            uint16_t next_level = curr_level + 1;

            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                uint64_t left_child = flatBinTrie<rankType>::getLeftChild(node_id);
                recursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (flatBinTrie<rankType>::getHeight() - curr_level - 1)));
                uint64_t right_child = flatBinTrie<rankType>::getRightChild(node_id);
                recursiveDecode(decoded, rightResult, right_child, next_level);
            }
        }


        inline void runsRecursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            if (curr_level == flatBinTrie<rankType>::height) {
                decoded.push_back(partial_int);
                return;
            }

            uint64_t node = binTrie_il::getNode(node_id);
            uint16_t next_level = curr_level + 1;

            if (node == 0b00) { 
                uint64_t below = partial_int;
                uint64_t range = ((uint64_t)1 << (flatBinTrie<rankType>::getHeight() - curr_level)) - 1;
                uint64_t above = (partial_int | range);
                for (uint64_t i = below; i <= above; ++i) {
                    decoded.push_back(i);
                }
                return;
            }

            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                uint64_t left_child = flatBinTrie<rankType>::getLeftChild(node_id);
                runsRecursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (flatBinTrie<rankType>::getHeight() - curr_level - 1)));
                uint64_t right_child = flatBinTrie<rankType>::getRightChild(node_id);
                runsRecursiveDecode(decoded, rightResult, right_child, next_level);
            }
        }


        inline void decode( vector<uint64_t> &decoded) {
            if (flatBinTrie<rankType>::runs_encoded) {
                if (flatBinTrie<rankType>::empty_trie) {
                    return;
                }
                else {
                    uint64_t partial_int = 0x00;
                    binTrie_il::runsRecursiveDecode(decoded, partial_int, 0, 0);
                }
                
                
            }
            else {
                uint64_t partial_int = 0x00;
                binTrie_il::recursiveDecode(decoded, partial_int, 0, 0);
            }
        }
};

#endif // DEBUG