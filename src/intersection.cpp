#include <iostream>
#include <vector>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"

using namespace std;

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


binTrie intersectTries(vector<binTrie> &Bs) {
    vector<uint64_t> roots;
    // Get max height of trees
    uint16_t max_level = 0;
    for (uint16_t i = 0; i < Bs.size(); ++i) {
        roots.push_back(0);
        if (Bs[i].bTrie.size() > max_level) max_level = Bs[i].bTrie.size();
    }
    binTrie result = binTrie(max_level);

    vector<uint64_t> last_pos;
    for (uint16_t i = 0; i < max_level; ++i){
        last_pos.push_back(0);
    }

    intersection(Bs, max_level, 0, roots, last_pos, result);
    // Resize all bit vectors of levels of tree
    for (int i = 0; i < result.bTrie.size(); ++i) {
        result.bTrie[i].resize(last_pos[i]);
    }

    // Init rank structure of intersection result
    result.initRank();

    return result;
}


void intersectionV2(vector <binTrie> &Bs, uint16_t max_level, uint16_t curr_level, 
                  vector<uint64_t> roots, vector<uint64_t> &last_pos,
                  vector<uint64_t> ones_to_write[], 
                  vector<uint64_t> &nodes_per_level) {
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
    uint64_t pos_to_write[2] = {0, 0};
    if (result[0] == 1 || result[1] == 1){
        if (result[0] == 1) {
            pos_to_write[0] = last_pos[curr_level];
        }
        if (result[1] == 1) {
            pos_to_write[1] = last_pos[curr_level] + 1;
        }
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
		intersectionV2(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level);
	}
	// Right child
	if (result[1] == 1) {
		vector <uint64_t> right_nodes;
		for (uint64_t i = 0; i < n_tries; ++i) {
			right_nodes.push_back(Bs[i].getRightChild(roots[i], curr_level));
		}
		intersectionV2(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, nodes_per_level);
	}
    // if not exist child nodes in intersection, remove current node
    if (last_pos[next_level] == pos_next_level_before && next_level <= max_level - 1) {
        last_pos[curr_level] -= 2;
    }
    else {
        if (result[0] == 1) {
            ones_to_write[curr_level].push_back(pos_to_write[0]);
        }
        if (result[1] == 1) {
            ones_to_write[curr_level].push_back(pos_to_write[1]);
        }
        nodes_per_level[curr_level]++;
    }    
}


binTrie intersectTriesV2(vector<binTrie> &Bs) {
    vector<uint64_t> roots;
    // Get max height of trees
    uint16_t max_level = 0;
    for (uint16_t i = 0; i < Bs.size(); ++i) {
        roots.push_back(0);
        if (Bs[i].bTrie.size() > max_level) max_level = Bs[i].bTrie.size();
    }

    vector<uint64_t> last_pos;
    for (uint16_t i = 0; i < max_level; ++i){
        last_pos.push_back(0);
    }

    vector<uint64_t> ones_to_write[max_level];
    vector<uint64_t> nodes_per_level(max_level, 0);

    intersectionV2(Bs, max_level, 0, roots, last_pos, ones_to_write, nodes_per_level);
    binTrie result = binTrie(ones_to_write, max_level, nodes_per_level);
    // Resize all bit vectors of levels of tree
    for (int i = 0; i < result.bTrie.size(); ++i) {
        result.bTrie[i].resize(last_pos[i]);
    }

    // Init rank structure of intersection result
    result.initRank();

    return result;
}

// // Overload Function to flat implementation
// template <class rankType>
// void intersectionV2(vector <flatBinTrie<rankType>> &Bs, uint16_t max_level, uint16_t curr_level, 
//                 vector<uint64_t> roots, vector<uint64_t> &last_pos,
//                 vector<uint64_t> ones_to_write[], vector<uint64_t> &nodes_per_level) {

//     // End condition
// 	if (curr_level == max_level - 1) {
//         uint16_t n_tries = Bs.size();
//         bit_vector result = bit_vector(2, 1); // vector de 2 bits con 1's
//         // Compute "bitwise and" between nodes of n_tries  
//         for (uint16_t i = 0; i < n_tries; ++i) {
//             bit_vector node_i = Bs[i].getNode(roots[i]);
//             result &= node_i; 
//         }
//         // Only write when result = 11, 10 or 01
//         if (result[0] || result[1]){
//             if (result[0])
//                 ones_to_write[curr_level].push_back(last_pos[curr_level]);
//             if (result[1])
//                 ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
//             last_pos[curr_level] += 2;
//         }
// 		return;
// 	}
	
// 	uint16_t n_tries = Bs.size();
// 	bit_vector result = bit_vector(2, 1); // vector de 2 bits con 1's
//     // Compute "bitwise and" between nodes of n_tries  
// 	for (uint16_t i = 0; i < n_tries; ++i) {
// 		bit_vector node_i = Bs[i].getNode(roots[i]);
// 		result &= node_i; 
// 	}

// 	uint16_t next_level = curr_level + 1;
//     uint64_t pos_next_level_before = last_pos[next_level];;

//     bool exist_lchild;
//     bool exist_rchild;
// 	// Left child
//     vector <uint64_t> left_nodes;
// 	if (result[0] == 1) {
// 		for (uint64_t i = 0; i < n_tries; ++i) {
// 			left_nodes.push_back(Bs[i].getLeftChild(roots[i]));
// 		}
// 		intersectionV2(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level);
// 	}

//     if (last_pos[next_level] == pos_next_level_before) {
//         exist_lchild = false;
//     }
//     else  {
//         ones_to_write[curr_level].push_back(last_pos[curr_level]);
//         pos_next_level_before = last_pos[next_level];
//         exist_lchild = true;
//     }
// 	// Right child
//     vector <uint64_t> right_nodes;
// 	if (result[1] == 1) {
// 		for (uint64_t i = 0; i < n_tries; ++i) {
//             if (left_nodes.size() > 0) {
//                 right_nodes.push_back(left_nodes[i] + 1);
//             }
//             else {
//                 right_nodes.push_back(Bs[i].getRightChild(roots[i]));
//             }
			
// 		}
// 		intersectionV2(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, nodes_per_level);
// 	}
    
//     if (last_pos[next_level] == pos_next_level_before) {
//         exist_rchild = false;
//     }
//     else {
//         ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
//         exist_rchild = true;
//     }
//     // mark if node is write
//     if (exist_lchild || exist_rchild) {
//         last_pos[curr_level] += 2;
//     }    
// }

// // Overload Function to flat implementation
// template<class rankType>
// flatBinTrie<rankType> intersectFlatTries(vector<flatBinTrie<rankType>> &Bs, bool compressed) {
//     vector<uint64_t> roots;
//     // Get max height of trees
//     uint16_t max_level = 0;
//     for (uint16_t i = 0; i < Bs.size(); ++i) {
//         roots.push_back(0);
//         if (Bs[i].getHeight() > max_level) max_level = Bs[i].getHeight();
//     }

//     vector<uint64_t> last_pos(max_level, 0);
//     vector<uint64_t> ones_to_write[max_level];
//     vector<uint64_t> nodes_per_level(max_level, 0);

//     intersectionV2(Bs, max_level, 0, roots, last_pos, ones_to_write, nodes_per_level);
//     flatBinTrie<rankType> result = flatBinTrie<rankType>(ones_to_write, max_level, last_pos, compressed);    

//     return result;
// }
// template flatBinTrie<rank_support_v5<1>> intersectFlatTries<rank_support_v5<1>>(vector<flatBinTrie<rank_support_v5<1>>> &Bs, bool compressed);
// template flatBinTrie<rank_support_v<1>> intersectFlatTries<rank_support_v<1>>(vector<flatBinTrie<rank_support_v<1>>> &Bs, bool compressed);


template <class rankType>
void compressedIntersection(vector <flatBinTrie<rankType>> &Bs, uint16_t max_level, uint16_t curr_level, 
                uint64_t *roots, vector<uint64_t> &last_pos,
                vector<uint64_t> ones_to_write[], vector<uint64_t> &nodes_per_level, bool *activeTries) {
	
	uint16_t n_tries = Bs.size();
	uint64_t result = 3; // 0....11
    uint64_t node00 = 0; // 0....00
    bool tempActiveTries[16];
    //  = {false, false, false, false,
    //                             false, false, false, false,
    //                             false, false, false, false,
    //                             false, false, false, false};

	for (uint16_t i = 0; i < n_tries; ++i) {
        if (activeTries[i]){
            uint64_t node_i = Bs[i].getNode(roots[i]);
            if (node_i == 0b00)
                tempActiveTries[i] = false;
            else {
                tempActiveTries[i] = true;
                result &= node_i;
                node00 |= node_i;
            }
        }
	}

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
        if (left_one || right_one) {
            if (left_one)
                ones_to_write[curr_level].push_back(last_pos[curr_level]);
            if (right_one)
                ones_to_write[curr_level].push_back(last_pos[curr_level] + 1); 
            last_pos[curr_level] += 2;
        }
		return;
	}
    // all nodes are 00
    if (node00 == 0b00) {
        last_pos[curr_level] += 2;
        return;
    }

    uint16_t next_level = curr_level + 1;
    uint64_t pos_next_level_before = last_pos[next_level];;

    uint64_t left_nodes[16] = {0};
    uint64_t right_nodes[16] = {0};

    bool exist_lchild;
    bool exist_rchild;
	
    // Left child
    if (left_one) {
        for (uint64_t i = 0; i < n_tries; ++i) {
            // if (activeTries[i])
			    left_nodes[i] = Bs[i].getLeftChild(roots[i]);
		}
        compressedIntersection(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level, tempActiveTries);
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
                // if (activeTries[i])
                    right_nodes[i] = Bs[i].getRightChild(roots[i]);
            } 
        }
        compressedIntersection(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, nodes_per_level, tempActiveTries);
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


template <class rankType>
void notCompressedIntersection(vector <flatBinTrie<rankType>> &Bs, uint16_t max_level, uint16_t curr_level, 
                               uint64_t *roots, vector<uint64_t> &last_pos,
                               vector<uint64_t> ones_to_write[], vector<uint64_t> &nodes_per_level) {
	
	uint16_t n_tries = Bs.size();
	uint64_t result = 0b11; // 0...11
    // Compute "bitwise and" between nodes of n_tries  
	for (uint16_t i = 0; i < n_tries; ++i) {
		uint64_t node_i = Bs[i].getNode(roots[i]);
		result &= node_i;
	}

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
		notCompressedIntersection(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level);
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
		notCompressedIntersection(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, nodes_per_level);
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

template<class rankType>
flatBinTrie<rankType>* joinTries(vector<flatBinTrie<rankType>> &Bs, bool compressed, uint64_t &time) {
    
    uint16_t max_level = 0;
    for (uint16_t i = 0; i < Bs.size(); ++i) {
        if (Bs[i].getHeight() > max_level) max_level = Bs[i].getHeight();
    }

    // max 16 relations
    bool activeTries[16] = {true}; // active tries in compressed implementation
    uint64_t roots[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<uint64_t> last_pos(max_level, 0);
    vector<uint64_t> ones_to_write[max_level];
    vector<uint64_t> nodes_per_level(max_level, 0);

    auto start = std::chrono::high_resolution_clock::now();

    if (compressed) {
        compressedIntersection(Bs, max_level, 0, roots, last_pos, ones_to_write, nodes_per_level, activeTries);
    }
    else {
        notCompressedIntersection(Bs, max_level, 0, roots, last_pos, ones_to_write, nodes_per_level);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    time = elapsed.count();
    cout <<"|Time execution: " << (float)time*10e-6 << "[ms]" << endl;

    flatBinTrie<rankType>* result = new flatBinTrie<rankType>(ones_to_write, max_level, last_pos, compressed);

    return result;
}
template flatBinTrie<rank_support_v5<1>>* joinTries<rank_support_v5<1>>(vector<flatBinTrie<rank_support_v5<1>>> &Bs, bool compressed, uint64_t &time);
template flatBinTrie<rank_support_v<1>>* joinTries<rank_support_v<1>>(vector<flatBinTrie<rank_support_v<1>>> &Bs, bool compressed, uint64_t &time);