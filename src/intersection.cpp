#include <iostream>
#include <vector>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include "binTrie_il.hpp"

using namespace std;
using namespace sdsl;

void intersection(vector <binTrie> &Bs, uint16_t max_level, uint16_t curr_level, 
                  vector<uint64_t> roots, vector<uint64_t> &last_pos,
                  binTrie &intersection_result) {
    // End condition
	if (curr_level == max_level) {
		return;
	}
	
	uint16_t n_tries = Bs.size();
	bit_vector result = bit_vector(2, 1); // vector de 2 bits con 1's

	for (uint16_t i = 0; i < n_tries; ++i) {
		bit_vector node_i = Bs[i].getNode(roots[i], curr_level);
		result &= node_i; 
	}

    if (result[0] == 1 || result[1] == 1){ 
        uint64_t pos_to_write = last_pos[curr_level];
        intersection_result.bTrie[curr_level][pos_to_write] = result[0];
        intersection_result.bTrie[curr_level][pos_to_write + 1] = result[1];
        last_pos[curr_level]+=2;
    }

	uint16_t next_level = curr_level + 1;
    uint64_t pos_next_level_before = 0;
    if (curr_level < max_level - 1) {
        uint64_t pos_next_level_before = last_pos[next_level];
    }
	// Left child
	if (result[0] == 1) {
		vector <uint64_t> left_nodes;
		for (uint64_t i = 0; i < n_tries; ++i) {
			left_nodes.push_back(Bs[i].getLeftChild(roots[i], curr_level));
		}
		intersection(Bs, max_level, next_level, left_nodes, last_pos, intersection_result);
	}
	// Right child
	if (result[1] == 1) {
		vector <uint64_t> right_nodes;
		for (uint64_t i = 0; i < n_tries; ++i) {
			right_nodes.push_back(Bs[i].getRightChild(roots[i], curr_level));
		}
		intersection(Bs, max_level, next_level, right_nodes, last_pos, intersection_result);
	}
    // if not exist child nodes in intersection, remove current node
    if (last_pos[next_level] == pos_next_level_before && next_level <= max_level - 1)
        last_pos[curr_level] -= 2;
}


template <class trieType>
void runsEncodedIntersection(vector <trieType> &Bs, uint16_t &max_level, uint16_t &curr_level, 
                uint64_t roots[], vector<uint64_t> &last_pos,
                vector<uint64_t> ones_to_write[], bool activeTries[]) {
	
	uint16_t n_tries = Bs.size();
	uint64_t result = 0b11; // 0....11
    uint64_t node00 = 0b00; // 0....00
    bool tempActiveTries[16] = {false, false, false, false,
                                false, false, false, false,
                                false, false, false, false,
                                false, false, false, false};

	for (uint16_t i = 0; i < n_tries; ++i) {
        if (activeTries[i]){
            uint64_t node_i = Bs[i].getNode(roots[i]);
            if (node_i != 0b00){
                tempActiveTries[i] = true;
                result &= node_i;
                node00 |= node_i;
            }
        }
        else {
           tempActiveTries[i] = false;
        }
	}

    bool left_one = false;
    bool right_one = false;

    switch (result) {
        case 0b00:  
            left_one = false;
            right_one = false;
            break;
        case 0b01:
            left_one = false;  
            right_one = true;
            break;
        case 0b10:  
            left_one = true;
            right_one = false;
            break;
        case 0b11:
            left_one = true;
            right_one = true;
            break;
    }

    // End condition
	if (curr_level == max_level - 1) {
        // Only write when result = 11, 10 or 01
        if (left_one || right_one) {
            if (left_one)
                ones_to_write[curr_level].push_back(last_pos[curr_level]);
            if (right_one)
                ones_to_write[curr_level].push_back(last_pos[curr_level] + 1); 
            last_pos[curr_level] += 2;
        }
		return;
	}
    // all nodes are 00...0
    if (node00 == 0b00) {
        last_pos[curr_level] += 2;
        return;
    }

    uint16_t next_level = curr_level + 1;
    uint64_t pos_next_level_before = last_pos[next_level];;

    uint64_t left_nodes[16];
    uint64_t right_nodes[16];

    bool exist_lchild;
    bool exist_rchild;
	
    // Left child
    if (left_one) {
        for (uint64_t i = 0; i < n_tries; ++i) {
            if (activeTries[i])
			    left_nodes[i] = Bs[i].getLeftChild(roots[i]);
		}
        runsEncodedIntersection(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, tempActiveTries);
    }
    // if (left_one)
    //     runsEncodedIntersection(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level, tempActiveTries);

    if (last_pos[next_level] == pos_next_level_before) {
        exist_lchild = false;
    }
    else  {
        ones_to_write[curr_level].push_back(last_pos[curr_level]);
        pos_next_level_before = last_pos[next_level];
        exist_lchild = true;
    }
    
    // Right child
    if (right_one) {
        for (uint64_t i = 0; i < n_tries; ++i) {
            if (left_one && activeTries[i]) {
                right_nodes[i] = left_nodes[i] + 1;
            }
            else {
                if (activeTries[i])
                    right_nodes[i] = Bs[i].getRightChild(roots[i]);
            } 
        }
        runsEncodedIntersection(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, tempActiveTries);
    }
    // if (right_one)
    //     runsEncodedIntersection(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, nodes_per_level, tempActiveTries);
    
    if (last_pos[next_level] == pos_next_level_before) {
        exist_rchild = false;
    }
    else {
        ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
        exist_rchild = true;
    }
    // mark if node is write
    if (exist_lchild || exist_rchild) {
        last_pos[curr_level] += 2;
    }    
}


template <class trieType>
void notRunsEncodedIntersection(vector <trieType> &Bs, uint16_t max_level, uint16_t curr_level, 
                               uint64_t *roots, vector<uint64_t> &last_pos,
                               vector<uint64_t> *ones_to_write, vector<uint64_t> &nodes_per_level) {
	
	uint16_t n_tries = Bs.size();
	uint64_t result = 0b11; // 0...11
    // Compute "bitwise and" between nodes of n_tries  
	for (uint16_t i = 0; i < n_tries; ++i) {
		uint64_t node_i = Bs[i].getNode(roots[i]);
        // cout << node_i << " ";
		result &= node_i;
	}

    // cout  << endl;

    bool left_one = false;
    bool right_one = false;

    switch (result) {
        case 0b00:  left_one = false;
                    right_one = false;
                    break;
        case 0b01:  right_one = true;
                    break;
        case 0b10:  left_one = true;
                    break;
        case 0b11:  left_one = true;
                    right_one = true;
                    break;
    }

     // End condition
	if (curr_level == max_level - 1) {
        // Only write when result = 11, 10 or 01
        if (left_one || right_one){
            if (left_one)
                ones_to_write[curr_level].push_back(last_pos[curr_level]);
            if (right_one)
                ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
            last_pos[curr_level] += 2;
        }
		return;
	}

	uint16_t next_level = curr_level + 1;
    uint64_t pos_next_level_before = last_pos[next_level];

    uint64_t left_nodes[16];
    uint64_t right_nodes[16];

    bool exist_lchild;
    bool exist_rchild;

	// Left child
	if (left_one) {
		for (uint64_t i = 0; i < n_tries; ++i) {
			left_nodes[i] = Bs[i].getLeftChild(roots[i]);
		}
		notRunsEncodedIntersection(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level);
	}

    if (last_pos[next_level] == pos_next_level_before) {
        exist_lchild = false;
    }
    else  {
        ones_to_write[curr_level].push_back(last_pos[curr_level]);
        pos_next_level_before = last_pos[next_level];
        exist_lchild = true;
    }

	// Right child
	if (right_one) {
		for (uint64_t i = 0; i < n_tries; ++i) {
            if (left_one) {
                right_nodes[i] = left_nodes[i] + 1;
            }
            else {
                right_nodes[i] = Bs[i].getRightChild(roots[i]);
            }
			
		}
		notRunsEncodedIntersection(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, nodes_per_level);
	}
    
    if (last_pos[next_level] == pos_next_level_before) {
        exist_rchild = false;
    }
    else {
        ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
        exist_rchild = true;
    }
    // mark if node is write
    if (exist_lchild || exist_rchild) {
        last_pos[curr_level] += 2;
    } 
}


// template<class rankType>
// flatBinTrie<rankType>* joinTries(vector<flatBinTrie<rankType>> &Bs, bool runs_encoded) {
    
//     uint16_t max_level = 0;
//     for (uint16_t i = 0; i < Bs.size(); ++i) {
//         if (Bs[i].getHeight() > max_level) 
//             max_level = Bs[i].getHeight();
//     }

//     // max 16 relations
//     bool activeTries[16] = { true, true, true, true,
//                              true, true, true, true,
//                              true, true, true, true,
//                              true, true, true, true }; 
//                             // active tries in runs encode implementation
//     uint64_t roots[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
//                            0, 0, 0, 0, 0, 0, 0, 0 };
    
//     vector<uint64_t> last_pos(max_level, 0);
//     vector<uint64_t> ones_to_write[max_level];
//     vector<uint64_t> nodes_per_level(max_level, 0);

//     // auto start = std::chrono::high_resolution_clock::now();

//     if (runs_encoded) {
//         runsEncodedIntersection(Bs, max_level, 0, roots, last_pos, ones_to_write, activeTries);
//     }
//     else {
//         notRunsEncodedIntersection(Bs, max_level, 0, roots, last_pos, ones_to_write, nodes_per_level);
//     }
    
//     // auto end = std::chrono::high_resolution_clock::now();
//     // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//     // time = elapsed.count();
//     // cout <<"|Time execution: " << (float)time*10e-6 << "[ms]" << endl;

//     flatBinTrie<rankType>* result;
//     result = new flatBinTrie<rankType>(ones_to_write, max_level, last_pos, runs_encoded);

//     return result;
// }
// template flatBinTrie<rank_support_v5<1>>* joinTries<rank_support_v5<1>>(vector<flatBinTrie<rank_support_v5<1>>> &Bs, bool runs_encoded);
// template flatBinTrie<rank_support_v<1>>* joinTries<rank_support_v<1>>(vector<flatBinTrie<rank_support_v<1>>> &Bs, bool runs_encoded);


template<class trieType>
trieType* joinTries(vector<trieType> &Bs, bool runs_encoded) {
    
    uint16_t max_level = Bs[0].getHeight();
    // for (uint16_t i = 0; i < Bs.size(); ++i) {
    //     if (Bs[i].getHeight() > max_level) 
    //         max_level = Bs[i].getHeight();
    // }

    // max 16 relations
    bool activeTries[16] = { true, true, true, true,
                             true, true, true, true,
                             true, true, true, true,
                             true, true, true, true }; 
                            // active tries in runs encode implementation
    uint64_t roots[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0 };
    
    vector<uint64_t> last_pos(max_level, 0);
    vector<uint64_t> ones_to_write[max_level];
    vector<uint64_t> nodes_per_level(max_level, 0);

    for (uint16_t i = 0; i < max_level; ++i) {
        ones_to_write[i].reserve(pow(2, i));
    }
    

    // auto start = std::chrono::high_resolution_clock::now();
    uint16_t curr_level = 0;
    if (runs_encoded) {
        runsEncodedIntersection(Bs, max_level, curr_level, roots, last_pos, ones_to_write, activeTries);
    }
    else {
        notRunsEncodedIntersection(Bs, max_level, 0, roots, last_pos, ones_to_write, nodes_per_level);
    }
    
    // auto end = std::chrono::high_resolution_clock::now();
    // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    // time = elapsed.count();
    // cout <<"|Time execution: " << (float)time*10e-6 << "[ms]" << endl;

    // flatBinTrie<rankType>* result;
    // result = new flatBinTrie<rankType>(ones_to_write, max_level, last_pos, runs_encoded);
    trieType* result = new trieType();
    // result = new trieType(ones_to_write, max_level, last_pos, runs_encoded);

    return result;
}
template flatBinTrie<rank_support_v5<1>>* 
joinTries<flatBinTrie<rank_support_v5<1>>>(vector<flatBinTrie<rank_support_v5<1>>> &Bs, bool runs_encoded);
template flatBinTrie<rank_support_v<1>>* 
joinTries<flatBinTrie<rank_support_v<1>>>(vector<flatBinTrie<rank_support_v<1>>> &Bs, bool runs_encoded);
template binTrie_il<512>* 
joinTries<binTrie_il<512>>(vector<binTrie_il<512>> &Bs, bool runs_encoded);
template binTrie_il<256>* 
joinTries<binTrie_il<256>>(vector<binTrie_il<256>> &Bs, bool runs_encoded);
template binTrie_il<128>* 
joinTries<binTrie_il<128>>(vector<binTrie_il<128>> &Bs, bool runs_encoded);
template binTrie_il<64>* 
joinTries<binTrie_il<64>>(vector<binTrie_il<64>> &Bs, bool runs_encoded);