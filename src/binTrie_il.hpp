#ifndef BINTRIE_IL
#define BINTRIE_IL


#include <iostream>
#include <vector>
#include <queue>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/bit_vector_il.hpp>

using namespace std;
using namespace sdsl;


template <uint32_t block_size = 512>
class binTrie_il{
    private:
        uint16_t height; // original height of trie
        uint16_t height_with_runs; // height with runs encoded
        bool empty_trie = false;
        bool runs_encoded;

        sdsl::bit_vector_il<block_size> *bTrie;
        sdsl::bit_vector *lastLevel;     
        rank_support_il<1, block_size> b_rank;
        // vector<uint64_t> level_pos;
        uint64_t* level_pos;

    public:
        binTrie_il() = default;


        binTrie_il(vector<uint64_t> &set, uint64_t u) {
            binTrie_il::runs_encoded = false;
            uint32_t n = set.size();

            uint16_t height = floor(log2(u - 1)) +  1;
            binTrie_il::height = height;
            // binTrie_il::level_pos = vector<uint64_t>(height, 0);
            binTrie_il::level_pos = new uint64_t[height];
            
            // uint64_t max_nodes = 2*(pow(2, height+1) - 1);
            uint64_t max_nodes            = 2 * (pow(2, height) - 1);
            uint64_t max_nodes_last_level = pow(2, height);  
            
            bit_vector* _bTrie     = new bit_vector(max_nodes, 0);
            binTrie_il::lastLevel = new bit_vector(max_nodes_last_level, 0);
            
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
                        (*lastLevel)[index] = 1;
                    else
                        (*_bTrie)[index] = 1;
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
                        (*lastLevel)[index] = 1;
                    else
                        (*_bTrie)[index] = 1;
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
                    level_pos[level] = index;
                    if (level == height-2){
                        nodes_last_level = nodes_next_level;
                        index = 0;
                    }
                    
                    nodes_curr_level = nodes_next_level;
                    nodes_next_level = 0;
                    count_nodes = 0;
                    level++;
                    
                }
                if (level == binTrie_il::height) {
                    break;
                }
            }

            _bTrie -> resize(total_nodes - 2*nodes_last_level);
            lastLevel -> resize(2*nodes_last_level);
            binTrie_il::bTrie = new bit_vector_il<block_size>(*_bTrie);
            delete _bTrie;
            binTrie_il::b_rank = rank_support_il<1, block_size>(bTrie);
        }


        binTrie_il(vector<uint64_t> ones_to_write[], uint16_t height, 
                    // vector<uint64_t> &level_pos,
                    uint64_t* level_pos, 
                    bool runs_encoded) {
            binTrie_il::runs_encoded = runs_encoded;
            binTrie_il::height = height;
            
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

            if (all_levels_empty == binTrie_il::height) {
                binTrie_il::empty_trie = true;
            }
            else {
                binTrie_il::empty_trie = false;
            }

            binTrie_il::height_with_runs = max_level_not_empty + 1;

            bit_vector* _bTrie    = new bit_vector(bits_n - level_pos[max_level_not_empty], 0);
            binTrie_il::lastLevel = new bit_vector(level_pos[max_level_not_empty], 0); 
            // binTrie_il::level_pos = vector<uint64_t>(height, 0);
            binTrie_il::level_pos = new uint64_t[height]; 

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    if (level == max_level_not_empty) {
                        uint64_t pos = ones_to_write[level][i];
                        (*binTrie_il::lastLevel)[pos] = 1;
                    }
                    else {
                        uint64_t pos = global_level_pos + ones_to_write[level][i];
                        (*_bTrie)[pos] = 1;
                    }
                }
                if (level == max_level_not_empty) {
                    binTrie_il::level_pos[level] = level_pos[level];
                }
                else {
                    global_level_pos += level_pos[level];
                    binTrie_il::level_pos[level] = global_level_pos;
                }
                
            }
            binTrie_il::bTrie = new bit_vector_il<block_size>(*_bTrie);
            delete _bTrie;
            binTrie_il::b_rank = rank_support_il<1, block_size>(binTrie_il::bTrie);
        };

        // free memory of bit vectors
        inline void free(){
            delete binTrie_il::bTrie;
            delete binTrie_il::lastLevel;
        }

        // return number of elements of bit_vector
        inline uint64_t size(){
            return bTrie->size();
        };


        inline uint16_t getHeight(){
            return binTrie_il::height;
        };

        // Return number of elements coded in a trie
        inline uint64_t elements_coded() {
            uint64_t counter_ones = 0;
            for (uint64_t i = 0; i < binTrie_il::lastLevel -> size(); ++i) {
                if ((*binTrie_il::lastLevel)[i] == 1) 
                    counter_ones++;
            }
            return counter_ones;
        }

        
        inline uint64_t getNode(uint64_t node_id) {
            // uint64_t node = 0;
            // if ((*bTrie)[2*node_id] == 1)
            //     node = (node | (1ULL << 1));

            // if ((*bTrie)[(2*node_id) + 1] == 1)
            //     node = (node | (1ULL << 0));
            // return node;

            uint64_t node = 0;
            uint64_t pos;
            if ((2*node_id) >= (binTrie_il::bTrie -> size())) {
                pos = (2*node_id) - (binTrie_il::bTrie -> size());
                if ((*lastLevel)[pos])
                    node = (node | (1ULL << 1));

                if ((*lastLevel)[pos + 1] == 1)
                    node = (node | (1ULL << 0));
            }
            else { 
                pos = 2*node_id;
                if ((*bTrie)[pos])
                    node = (node | (1ULL << 1));

                if ((*bTrie)[pos + 1] == 1)
                    node = (node | (1ULL << 0));
            }

            return node;
        };

        inline uint64_t getNode1(uint64_t &node_id) {
            uint64_t pos  = 2 * node_id;
            // if (cur_level < flatBinTrie::height - 1)
            // if (pos < bTrie -> size() )
                return (((*bTrie)[pos]) << 1) | (*bTrie)[pos+1];
            
            // else{
                // pos -= flatBinTrie::bTrie -> size();
                // return (((*lastLevel)[pos]) << 1) | (*lastLevel)[pos+1];
            // }
            // return node;
        }

        uint64_t getNode2(uint64_t &node_id) {
            uint64_t pos = 2*node_id - (bTrie -> size());
            return ((*lastLevel)[pos] << 1) | (*lastLevel)[pos+1];
        }


        inline uint64_t getLeftChild(uint64_t &node_id) {
            // if (2*node_id + 1 <= binTrie_il::bTrie -> size()){
                // auto start = std::chrono::high_resolution_clock::now();
                uint64_t rank = binTrie_il::b_rank((2*node_id) + 1);
                // auto end = std::chrono::high_resolution_clock::now();
                // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                // auto time = elapsed.count();
                // cout << "Tiempo por rank: "<< time <<" [ns]" << endl;
                return rank;
            // }
                // return binTrie_il::b_rank((2*node_id) + 1);
            // else 
            //     return 0;
        };


        inline uint64_t getRightChild(uint64_t &node_id) {
            // if (2*node_id + 2 <= binTrie_il::bTrie -> size())
                return binTrie_il::b_rank((2*node_id) + 2);
            // else 
                // return 0;
        };


        inline uint64_t size_in_bytes() {
            uint64_t bv_size    = sdsl::size_in_bytes(*(binTrie_il::bTrie));
            uint64_t lastL_size = sdsl::size_in_bytes(*(binTrie_il::lastLevel));
            uint64_t rank_size  = sdsl::size_in_bytes(binTrie_il::b_rank);
            return bv_size +
                    rank_size +
                    lastL_size +
                    2 * sizeof(bool) +
                    2 * sizeof(uint8_t);
        };

        // return size of bytes of all structure
        inline uint64_t serialize(std::ostream &out) {

            out.write(reinterpret_cast<char*>(&height)          , sizeof(height));
            out.write(reinterpret_cast<char*>(&height_with_runs), sizeof(height_with_runs));
            out.write(reinterpret_cast<char*>(&empty_trie)      , sizeof(empty_trie));
            out.write(reinterpret_cast<char*>(&runs_encoded)    , sizeof(runs_encoded));

            uint64_t bvs_size, rank_size;
            bvs_size  = bTrie -> serialize(out) + lastLevel -> serialize(out);
            rank_size = b_rank.serialize(out);

            return bvs_size + rank_size + 
                   sizeof(height) + sizeof(height_with_runs) +
                   sizeof(empty_trie) + sizeof(runs_encoded);
        }

        // load structure from in stream
        inline void load(std::istream &in){
            in.read(reinterpret_cast<char*>(&height)          , sizeof(height));
            in.read(reinterpret_cast<char*>(&height_with_runs), sizeof(height_with_runs));
            in.read(reinterpret_cast<char*>(&empty_trie)      , sizeof(empty_trie));
            in.read(reinterpret_cast<char*>(&runs_encoded)    , sizeof(runs_encoded));

            binTrie_il::bTrie      = new sdsl::bit_vector_il<block_size>();
            binTrie_il::lastLevel = new sdsl::bit_vector();

            bTrie     -> load(in);
            lastLevel -> load(in);
            b_rank.load(in, bTrie);
        }


        inline void print() {
            uint64_t i = 0;
            for (uint16_t level=0; level < binTrie_il::height; ++level) {
                uint64_t next_level_pos = binTrie_il::level_pos[level];
                if (level == binTrie_il::height - 1) {
                    // next_level_pos = ;
                    i = 0;
                }
                while (i < next_level_pos) {
                    // cout << (*binTrie_il::bTrie)[i] << (*binTrie_il::bTrie)[i+1] << " ";
                    // ++(++i);

                    if (level < binTrie_il::height - 1)
                        cout << (*binTrie_il::bTrie)[i] << (*binTrie_il::bTrie)[i+1] << " ";
                    else{ 
                        // uint64_t i_hat = i - (binTrie_il::bTrie -> size());
                        cout << (*binTrie_il::lastLevel)[i] << (*binTrie_il::lastLevel)[i+1] << " ";
                    }
                    ++(++i);
                }
                cout << endl;
            }
        };


        inline void writeCompressTrie(vector<uint64_t> ones_to_write[], 
                                // vector<uint64_t> &level_pos,
                                uint64_t* level_pos, 
                                uint16_t curr_level, uint64_t node_id, bool &its11){
            // cout << "node_id: " << node_id << " current level: " << curr_level << endl;
            
            // End condition
            if (curr_level == (binTrie_il::height-1)) {
                uint64_t node = getNode2(node_id);
                // if node == 11
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
            uint64_t node = getNode1(node_id);
            uint16_t next_level = curr_level + 1;
            uint64_t next_level_pos = level_pos[next_level];

            bool its11_l = false;
            bool its11_r = false;
            bool actualIts11 = false;

            if (node == 0b11) {
                actualIts11 = true;

                uint64_t l_child = getLeftChild(node_id);
                uint64_t r_child = l_child + 1;
                writeCompressTrie(ones_to_write, level_pos, next_level, l_child, its11_l);
                writeCompressTrie(ones_to_write, level_pos, next_level, r_child, its11_r);
                
                its11 = true && its11_l && its11_r;
                if (its11) {
                    level_pos[next_level] -= 4;
                    if (curr_level == binTrie_il::height -2){
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
                    uint64_t l_child = getLeftChild(node_id);
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    writeCompressTrie(ones_to_write, level_pos, next_level, l_child, its11_l);
                }
                level_pos[curr_level] += 1;
                if (node == 0b01) {
                    uint64_t r_child = getRightChild(node_id);
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    writeCompressTrie(ones_to_write, level_pos, next_level, r_child, its11_r);
                }
                level_pos[curr_level] += 1;
            }
        };

        // Method write ones in bit vector
        inline void writeOnes(vector<uint64_t> ones_to_write[], 
                                // vector<uint64_t> level_pos
                                uint64_t* level_pos){
            // cout << "Escribiendo unos" << endl;
            binTrie_il::runs_encoded = true;
            uint64_t bits_n = 0;
            uint16_t last_level = 0;
            uint64_t bits_before_last_level;
            for (uint16_t level = 0; level < binTrie_il::height; ++level) {
                bits_n += level_pos[level];
                if (level_pos[level] > 0) {
                    last_level = level; 
                }
                // else {
                //     bits_before_last_level = bits_n - level_pos[last_level];
                // }
                
            }

            binTrie_il::height_with_runs = last_level + 1;
            // cout << "comienza el delete" << endl;
            delete binTrie_il::bTrie;
            delete binTrie_il::lastLevel;
            // cout << "termina el delete" << endl;
            // cout << "New de los bit_vectors" << endl;
            // binTrie_il::bTrie     = new bit_vector(bits_n, 0);
            bit_vector* _bTrie     = new bit_vector(bits_n - level_pos[last_level], 0);
            binTrie_il::lastLevel  = new bit_vector(level_pos[last_level], 0);
            // binTrie_il::level_pos = vector<uint64_t>(height, 0);
            // delete[] binTrie_il::level_pos;
            // cout << "Antes del new" << endl;
            binTrie_il::level_pos = new uint64_t[height];
            // cout << "Ok operador new " << endl;
            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height_with_runs; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    // uint64_t pos = global_level_pos + ones_to_write[level][i];
                    // (*binTrie_il::bTrie)[pos] = 1;

                    if (level == last_level){
                        uint64_t pos = ones_to_write[level][i];
                        (*binTrie_il::lastLevel)[pos] = 1;
                    }
                    else {
                        uint64_t pos = global_level_pos + ones_to_write[level][i];
                        (*_bTrie)[pos] = 1;
                        
                    }
                }

                if (level == last_level) {
                    binTrie_il::level_pos[level] = level_pos[level];
                }
                else {
                    global_level_pos += level_pos[level];
                    binTrie_il::level_pos[level] = global_level_pos;
                }
                // global_level_pos += level_pos[level];
                // binTrie_il::level_pos[level] = global_level_pos;
            }
            // cout << "antes del rank" << endl;
            binTrie_il::bTrie = new bit_vector_il<block_size>(*_bTrie);
            delete _bTrie;
            binTrie_il::b_rank = rank_support_il<1, block_size>(binTrie_il::bTrie);
            delete[] level_pos;
        };


        inline void encodeRuns() {
            // bit_vector new_bv = new bit_vector(binTrie_il::bTrie -> size(), 0);
            vector<uint64_t> ones_to_write[binTrie_il::height];
            // vector<uint64_t> level_pos(binTrie_il::height, 0);
            uint64_t* level_pos = new uint64_t[height];
            for (uint64_t i = 0; i < height; ++i) level_pos[i] = 0;
            bool itsOneOne = false;
            // cout << "init recursive func" << endl;
            binTrie_il::writeCompressTrie(ones_to_write, level_pos, 0, 0, itsOneOne);
            // cout << "end recursive func" << endl;
            writeOnes(ones_to_write, level_pos);
        };

        
        inline void recursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            
            if (curr_level == binTrie_il::height) {
                decoded.push_back(partial_int);
                return;
            }

            uint64_t node = getNode(node_id);
            uint16_t next_level = curr_level + 1;

            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node == 0b10 || node == 0b11) {
                uint64_t left_child = getLeftChild(node_id);
                recursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                uint64_t right_child = getRightChild(node_id);
                recursiveDecode(decoded, rightResult, right_child, next_level);
            }
        };


        inline void runsRecursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            if (curr_level == binTrie_il::height) {
                // cout << "antes" << endl;
                decoded.push_back(partial_int);
                // cout << "despues" << endl;
                return;
            }

            uint64_t node = getNode(node_id);
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
                uint64_t left_child = getLeftChild(node_id);
                runsRecursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node == 0b01 || node == 0b11) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                uint64_t right_child = getRightChild(node_id);
                runsRecursiveDecode(decoded, rightResult, right_child, next_level);
            }
        }


        inline void decode( vector<uint64_t> &decoded) {
            if (binTrie_il::runs_encoded) {
                if (binTrie_il::empty_trie) {
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
        }
    
};

#endif