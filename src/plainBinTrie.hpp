#ifndef BINTRIE_BV
#define BINTRIE_BV

#include <iostream>
#include <sdsl/bit_vectors.hpp>
// #include <sdsl/int_vector_buffer.hpp>
#include <sdsl/int_vector.hpp>
#include <vector>
#include <queue>
#include <math.h> 

using namespace sdsl;
using namespace std;

class plainBinTrie{
    uint16_t height;

    public:
        bit_vector btrie;
        rank_support_v<1> b_rank;

        binTrie() = default;

        binTrie(int_vector<> &set){
            uint32_t n = set.size();
            uint64_t max_value = set[n-1];
            uint64_t u = pow(2,ceil(log(max_value)/log(2)));
            // binTrie::height = log(u);
            cout << "****** Universe u: " << u << " ******\n";
            cout << "*Initial width: " << (int)set.width() << endl;
            util::bit_compress(set);
            binTrie::height = (uint16_t)set.width();
            cout << "*Compressed Width: " << (int)set.width() << endl;
            
            
            
            uint8_t width = (int)set.width();
            cout << set << endl;
             
            uint64_t max_nodes = 2*(pow(2, width+1) - 1);
            btrie = bit_vector(max_nodes);
            
            queue <int_vector<>> q;
            q.push(set);
            
            uint8_t level = 0;
            uint64_t nodes_cur_level = 1;
            uint64_t nodes_next_level = 0;
            uint64_t count_nodes = 0;
            uint64_t index = 0;
            uint64_t total_nodes = 0;
            
            while (!q.empty()) {
                // cout << "*** Level : " << (int)level << " ***"<< endl;
                // cout << "queue size: " << q.size() << endl;
                int_vector <> s = q.front();
                q.pop();
                count_nodes++;

                uint64_t n = s.size();
                int_vector <> l (n);
                int_vector <> r (n);
                uint64_t r_size = 0;
                uint64_t l_size = 0;
                uint8_t j = width - level;

                for (int i = 0; i < n; ++i){
                    // cout << s[i] << " " << ((s[i] >> j-1) & 1) << " ";
                    if ((s[i] >> j-1) & 1) {
                        r[r_size] = s[i];
                        r_size++;
                    }
                    else {
                        l[l_size] = s[i];
                        l_size++;
                    }
                }

                l.resize(l_size);
                r.resize(r_size);

                // Left node
                if (l_size > 0){
                    btrie[index] = 1;
                    q.push(l);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                }
                else{
                    btrie[index] = 0;
                    index++;
                    total_nodes++;
                }
                // Right node
                if (r_size > 0){
                    btrie[index] = 1;
                    q.push(r);
                    nodes_next_level++;
                    index++;
                    total_nodes++;
                    
                }
                else{
                    btrie[index] = 0;
                    index++;
                    total_nodes++;
                }

                // Condition to change level
                if (count_nodes == nodes_cur_level){
                    // total_nodes += count_nodes;
                    count_nodes = 0;
                    nodes_cur_level = nodes_next_level;
                    nodes_next_level = 0;
                    level ++;

                }
                if (level == width){
                    break;
                }
            }

            btrie.resize(total_nodes);
            b_rank = rank_support_v <1>(&btrie);
        };

        // return number of elements of bit_vector
        uint64_t size(){
            return btrie.size();
        }

        uint16_t getHeight(){
            return binTrie::height;
        };
        
        bit_vector getNode(uint64_t node_id){
            bit_vector node = bit_vector(2, 0);
            node[0] = btrie[2*node_id];
            node[1] = btrie[(2*node_id) + 1];
            return node;
        };

        

        uint64_t leftChid(uint64_t node_id) {
            uint64_t left_child_id = b_rank((2*node_id) + 1);
            return left_child_id;
        };

        uint64_t rightChild(uint64_t node_id) {
            uint64_t right_child_id = b_rank((2*node_id) + 1) + 1;
            return right_child_id;
        };

        uint64_t size_bytes(){
            // Size in bytes of bit_vector of binary trie
            uint64_t size_bit_vector = size_in_bytes(btrie); // 8 bytes to size, 1 byte to width + structure space
            // Size in bytes of rank structure
            uint64_t size_rank = size_in_bytes(b_rank); // +0.25 extra bits
            // return size_bit_vector + size_rank;
            cout << "Size of bit vector: " << size_in_bytes(btrie) << endl;
            cout << "Size of rank structure: " << size_in_bytes(b_rank) << " bytes " << size_in_bytes(b_rank)*8 << endl;
            cout << "Size of rank structure: " << b_rank.size() << " bits" << endl;
            cout << "Verificación rank size: 0.25*n -> " << btrie.size()*1.25 << " bits" << endl;
            return size_bit_vector*8;
            // return (int) btrie.bit_size();
        };
    
};


// uint64_t size_bytes(){
//     // Size in bytes of bit_vector of binary trie
//     uint64_t size_bit_vector = size_in_bytes(btrie);
//     // Size in bytes of rank structure
//     uint64_t size_rank = size_in_bytes(b_rank);
//     return size_bit_vector + size_rank;
// }

#endif
