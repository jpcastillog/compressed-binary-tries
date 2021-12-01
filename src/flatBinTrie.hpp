#ifndef BINTRIE_BV
#define BINTRIE_BV

#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <vector>
#include <queue>
#include <math.h> 
// #include "utils_functions.hpp"

using namespace sdsl;
using namespace std;

template <class rankType>
class flatBinTrie{
    private:
        uint16_t height;
        bool compressed;

    public:
        bit_vector *bTrie;        rankType b_rank;
        vector<uint64_t> level_pos;

        flatBinTrie() = default;

        flatBinTrie(vector<uint64_t> &set, uint64_t u) {
            flatBinTrie::compressed = false;
            uint32_t n = set.size();

            uint16_t height = floor(log2(u - 1)) +  1;
            flatBinTrie::height = height;
            flatBinTrie::level_pos = vector<uint64_t>(height, 0);
            
            uint64_t max_nodes = 2*(pow(2, height+1) - 1);
            flatBinTrie::bTrie = new bit_vector(max_nodes, 0); 
            
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
                    (*bTrie)[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
                    q.push(left_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    (*bTrie)[index] = 0;
                    index++;
                    total_nodes++;
                }
                // right child
                if (right_elements > 0) {
                    // write 1
                    (*bTrie)[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
                    q.push(right_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    (*bTrie)[index] = 0;
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

            flatBinTrie::bTrie -> resize(total_nodes);
            flatBinTrie::b_rank = rankType(bTrie);
        }

        
        flatBinTrie(int_vector<> &set) {
            flatBinTrie::compressed = false;
            uint32_t n = set.size();
            
            util::bit_compress(set);
            flatBinTrie::height = (uint16_t)set.width();
            flatBinTrie::level_pos = vector<uint64_t>(height, 0);
            
            uint64_t max_nodes = 2*(pow(2, height+1) - 1);
            flatBinTrie::bTrie = new bit_vector(max_nodes, 0); 
            

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
                    (*bTrie)[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
                    q.push(left_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    (*bTrie)[index] = 0;
                    index++;
                    total_nodes++;
                }
                // right child
                if (right_elements > 0) {
                    // write 1
                    (*bTrie)[index] = 1;
                    tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
                    q.push(right_split);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else {
                    // write 0
                    (*bTrie)[index] = 0;
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
            flatBinTrie::bTrie -> resize(total_nodes);
            flatBinTrie::b_rank = rankType(bTrie);
        };


        flatBinTrie(vector<uint64_t> ones_to_write[], uint16_t height, vector<uint64_t> level_pos) {
            flatBinTrie::compressed = false;
            flatBinTrie::height = height;
            uint64_t bits_n = 0;
            for (uint16_t level = 0; level < height; ++level) {
                bits_n += level_pos[level];
            }
            cout << bits_n << endl;
            bTrie = new bit_vector(bits_n, 0);
            flatBinTrie::level_pos = vector<uint64_t>(height, 0);

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    uint64_t pos = global_level_pos + ones_to_write[level][i];
                    (*flatBinTrie::bTrie)[pos] = 1;
                    // new_bv[pos] = 1;
                }
                global_level_pos += level_pos[level];
                flatBinTrie::level_pos[level] = global_level_pos;
            }
            b_rank = rankType(flatBinTrie::bTrie);
        };

        // return number of elements of bit_vector
        uint64_t size(){
            return bTrie->size();
        };


        uint16_t getHeight(){
            return flatBinTrie::height;
        };
        

        bit_vector getNode(uint64_t node_id){
            bit_vector node = bit_vector(2, 0);
            node[0] = (*bTrie)[2*node_id];
            node[1] = (*bTrie)[(2*node_id) + 1];
            return node;
        };


        uint64_t getLeftChild(uint64_t node_id) {
            uint64_t left_child_id = flatBinTrie::b_rank((2*node_id) + 1);
            return left_child_id;
        };


        uint64_t getRightChild(uint64_t node_id) {
            uint64_t right_child_id = b_rank((2*node_id) + 2) ;
            return right_child_id;
        };


        uint64_t size_in_bytes() {
            uint64_t bv_size = sdsl::size_in_bytes(*(flatBinTrie::bTrie));
            uint64_t rank_size = sdsl::size_in_bytes(flatBinTrie::b_rank);
            return bv_size +
                    rank_size +
                    sizeof(uint8_t);
        };


        void print() {
            uint64_t i = 0;
            for (uint16_t level=0; level < flatBinTrie::height; ++level) {
                uint64_t next_level_pos = flatBinTrie::level_pos[level];
                while (i < next_level_pos) {
                    cout << (*flatBinTrie::bTrie)[i] << (*flatBinTrie::bTrie)[i+1] << " ";
                    ++(++i);
                }
                cout << endl;
            }
        };


        void writeCompressTrie(vector<uint64_t> ones_to_write[], vector<uint64_t> &level_pos, 
                                uint16_t curr_level, uint64_t node_id, bool &its11){
            // cout << "node_id: " << node_id << " current level: " << curr_level << endl;
            bit_vector node = getNode(node_id);
            // End condition
            if (curr_level == (flatBinTrie::height-1)) {
                // if node == 11
                if (node[0] && node[1]) {
                    its11 = true;
                }
                if (node[0]){
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                }
                level_pos[curr_level] += 1;
                if (node[1]) {
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                }
                level_pos[curr_level] += 1;
                
                return;
            }

            uint16_t next_level = curr_level + 1;
            uint64_t next_level_pos = level_pos[next_level];

            bool its11_l = false;
            bool its11_r = false;
            bool actualIts11 = false;

            if (node[0] && node[1]) {
                actualIts11 = true;

                uint64_t l_child = getLeftChild(node_id);
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

            else if (node[0] || node[1]){
                if (node[0]){
                    uint64_t l_child = getLeftChild(node_id);
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    writeCompressTrie(ones_to_write, level_pos, next_level, l_child, its11_l);
                }
                level_pos[curr_level] += 1;
                if (node[1]) {
                    uint64_t r_child = getRightChild(node_id);
                    ones_to_write[curr_level].push_back(level_pos[curr_level]);
                    writeCompressTrie(ones_to_write, level_pos, next_level, r_child, its11_r);
                }
                level_pos[curr_level] += 1;
            }
        };


        void writesOnes(vector<uint64_t> ones_to_write[], vector<uint64_t> level_pos){
            flatBinTrie::compressed = true;
            uint64_t bits_n = 0;
            for (uint16_t level = 0; level < flatBinTrie::height; ++level) {
                bits_n += level_pos[level];
            }
            // cout << "number of bits: " << bits_n << endl;
            delete flatBinTrie::bTrie;
            flatBinTrie::bTrie = new bit_vector(bits_n, 0);
            flatBinTrie::level_pos = vector<uint64_t>(height, 0);

            uint64_t global_level_pos = 0;
            for (uint16_t level = 0; level < height; ++level) {
                for (uint64_t i = 0; i < ones_to_write[level].size(); ++i) {
                    uint64_t pos = global_level_pos + ones_to_write[level][i];
                    (*flatBinTrie::bTrie)[pos] = 1;
                }
                global_level_pos += level_pos[level];
                flatBinTrie::level_pos[level] = global_level_pos;
            }
            b_rank = rankType(flatBinTrie::bTrie);
        };


        void compress() {
            // bit_vector new_bv = new bit_vector(flatBinTrie::bTrie -> size(), 0);
            vector<uint64_t> ones_to_write[flatBinTrie::height];
            vector<uint64_t> level_pos(flatBinTrie::height, 0);
            bool itsOneOne = false;
            flatBinTrie::writeCompressTrie(ones_to_write, level_pos, 0, 0, itsOneOne);
            writesOnes(ones_to_write, level_pos);
        };

        
        void recursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            if (curr_level == flatBinTrie::height) {
                decoded.push_back(partial_int);
                return;
            }

            bit_vector node = getNode(node_id);
            uint16_t next_level = curr_level + 1;

            uint64_t leftResult = partial_int;
            uint64_t rightResult = partial_int;

            if (node[0]) {
                uint64_t left_child = getLeftChild(node_id);
                recursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node[1]) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                uint64_t right_child = getRightChild(node_id);
                recursiveDecode(decoded, rightResult, right_child, next_level);
            }
        };


        void compressRecursiveDecode(vector<uint64_t> &decoded, uint64_t partial_int, uint64_t node_id, uint16_t curr_level) {
            if (curr_level == flatBinTrie::height) {
                decoded.push_back(partial_int);
                return;
            }

            bit_vector node = getNode(node_id);
            uint16_t next_level = curr_level + 1;

            if (node[0] == 0 && node[1] == 0) { 
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

            if (node[0]) {
                uint64_t left_child = getLeftChild(node_id);
                compressRecursiveDecode(decoded, leftResult, left_child, next_level);
            }
            if (node[1]) {
                rightResult = (rightResult | (1ULL << (getHeight() - curr_level - 1)));
                uint64_t right_child = getRightChild(node_id);
                compressRecursiveDecode(decoded, rightResult, right_child, next_level);
            }
        }


        void decode( vector<uint64_t> &decoded) {
            if (flatBinTrie::compressed) {
                uint64_t partial_int = 0x00;
                compressRecursiveDecode(decoded, partial_int, 0, 0);
            }
            else {
                uint64_t partial_int = 0x00;
                recursiveDecode(decoded, partial_int, 0, 0);
            }
        }
    
};

#endif
