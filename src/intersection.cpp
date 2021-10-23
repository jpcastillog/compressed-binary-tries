#include <iostream>
#include <vector>
#include "binTrie.hpp"

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