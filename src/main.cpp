#include <iostream>
#include <sdsl/int_vector.hpp>
#include "binTrie.hpp"
#include <ctime>


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
        // cout << roots[i] << " "; 
	}
    // cout << endl;
     // cout << "pos: " << last_pos [curr_level] << endl;s
    // Write result in a Trie if exist child node
    if (result[0] == 1 || result[1] == 1){ 
        uint64_t pos_to_write = last_pos[curr_level];
        intersection_result.bTrie[curr_level][pos_to_write] = result[0];
        intersection_result.bTrie[curr_level][pos_to_write + 1] = result[1];
        last_pos[curr_level]+=2;
    }
    // cout << "level: "<< curr_level << "->" << result << endl;
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
        // cout << "left" << endl;
		intersection(Bs, max_level, next_level, left_nodes, last_pos, intersection_result);
	}
	// Right child
	if (result[1] == 1) {
		vector <uint64_t> right_nodes;
		for (uint64_t i = 0; i < n_tries; ++i) {
			right_nodes.push_back(Bs[i].getRightChild(roots[i], curr_level));
		}
        //  cout << "right" << endl;
		intersection(Bs, max_level, next_level, right_nodes, last_pos, intersection_result);
	}
    // if not exist child nodes in intersection, remove current node
    if (last_pos[next_level] == pos_next_level_before && next_level <= max_level - 1)
        last_pos[curr_level] -= 2;
}

binTrie intersectTries(vector<binTrie> &Bs) {
    vector<uint64_t> roots;
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

    for (int i = 0; i < result.bTrie.size(); ++i) {
        result.bTrie[i].resize(last_pos[i]);
    }

    result.initRank();

    return result;
}


int main(){
    // int_vector<> s = {1, 2, 3, 5};
    int_vector<> s1 = {1, 3, 7, 8, 9, 11, 12};
    int_vector<> s2 = {2, 5, 7, 12, 15};
    
    binTrie b1 = binTrie(s1);
    binTrie b2 = binTrie(s2);
    cout << "-----------b1-----------" << endl;
    for (int i = 0; i < b1.bTrie.size(); ++i) {
        // cout << b.bTrie[i]<< endl;
        for (int j = 0; j < b1.bTrie[i].size(); ++(++j)) {
            cout << b1.bTrie[i][j] << b1.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }
    cout << "-----------b2-----------" << endl;
    for (int i = 0; i < b2.bTrie.size(); ++i) {
        // cout << b.bTrie[i]<< endl;
        for (int j = 0; j < b2.bTrie[i].size(); ++(++j)) {
            cout << b2.bTrie[i][j] << b2.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }
    // cout << b1.bv_rank[3](5) << endl;

    vector<binTrie> Bs = {b1, b2};
    binTrie result = intersectTries(Bs);
    cout << "-----------result-----------" << endl;
    for (int i = 0; i < result.bTrie.size(); ++i) {
        // result.bTrie[i].resize(last_pos[i]);
        for (int j = 0; j < result.bTrie[i].size(); ++(++j)) {
            cout << result.bTrie[i][j] << result.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }
    return 0;

}