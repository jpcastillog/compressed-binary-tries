#include <iostream>
#include <vector>
#include "binTrie.hpp"

using namespace std;

int intersection(vector<binTrie> &S, uint16_t cur_level, uint16_t max_level, 
				 binTrie *Intersection){
				
	if (cur_level == max_level){
		return 0;
	}
	else {
		// next level
		uint16_t next_level;
		intersection(bTrie bTrie_A, bTrie_B, next_level, max_level)
	}

}

void levelTraversal(binTrie &S, uint8_t cur_level, uint64_t cur_pos, uint64_t child_number){
	if (cur_level == S.height-1) {
		return;
	}

	for (int i = 0; i < child_number; ++i) {
		cout << S.btrie[cur_pos + 2*i] << S.btrie[cur_pos + 2*(i+1)] << endl; 
	}
	cur_level ++;
	uint64_t next_childs = b_rank(cur_pos+child_number) - b_rank(cur_pos+1);
	uint64_t next_pos = cur_pos + child_number;
	levelTraversal(S, cur_level, next_pos, next_childs);
}


void preorder(binTrie &S, uint64_t node_pos){
	cout << S.btrie[node_pos] << S.btrie[node_pos+1] << endl;

	uint64_t node_left_pos = ;
	uint64_t node_right_pos = node_left_pos+1;

	cout << "node_left_pos: " << node_left_pos << endl;
	cout << "node_right_pos" << node_right_pos << endl;
	cout << "---------------------------------" << endl;

	if (S.btrie[node_pos] != 0) {
		preorder(S, node_left_pos);
	}
	if (S.btrie[node_pos+1] != 0){
		preorder(S, node_right_pos);
	}
	return;
}

// agregar arreglo con los padres.
void intersection(vector <binTrie>* Bs, uint16_t max_level, uint16_t curr_level, vector<uint64_t> roots) {
	if (curr_level == max_level) {
		return;
	}
	
	uint16_t n_tries = Bs -> size();
	bit_vector result = bit_vector(2, 1); // vector de 2 bits con 1's

	for (uint16_t i; i < n_tries; ++i) {
		bit_vector node_i = Bs[i].getNode(roots[i]);
		result &= node_i;
		next_roots.push_back()
	}
	uint16_t next_level = curr_level + 1;
	// Left child
	if (result[0]) {
		vector <uint64_t> left_nodes;
		for (uint64_t i; i < n_tries; ++i) {
			left_nodes.push_back(Bs[i].leftChild(roots[i]));
		}
		intersection(Bs, max_level, next_level, left_nodes);
	}
	// Right child
	if (result[1]) {
		vector <uint64_t> right_nodes;
		for (uint64_t i; i < n_tries; ++i) {
			left_nodes.push_back(Bs[i].rightChild(roots[i]));
		}
		intersection(Bs, max_level, next_level, right_nodes);
	} 
}

// get all nodes in current level
void currentLevel(vector<binTrie>* Bs, uint64_t root, uint16_t level) {
	if (root == -1) return;
	if (level == 0) {
		cout << "root" << endl;
	}
	else if (level > 0) {
		currentLevel(Bs, leftChild(root), level - 1);
		currentLevel(Bs, rightChid(root), level - 1);
	}
}

void levelOrder(vector<binTrie>* Bs, uint16_t max_level) {
	for (uint16_t i, i < max_level, ++i) {
		currentLevel(Bs, 0, i);
	}

}

void intersection2(vector<binTrie>* Bs, uint16_t max_level) {

	
}