#include <iostream>
#include <vector>
#include <stack>
#include <chrono>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include "binTrie_il.hpp"
#include "parallel_for.hpp"

using namespace std;
using namespace sdsl;




// Global vars ... 
vector <uint64_t>* ones_to_write;
uint64_t* last_pos;
uint64_t n_tries, max_level;


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
void runsEncodedIntersection(vector <trieType> &Bs, uint64_t &n_tries, 
                uint64_t &max_level, uint64_t curr_level, 
                uint64_t roots[], 
                // vector<uint64_t> &last_pos,
                uint64_t* last_pos,
                vector<uint64_t> ones_to_write[], bool activeTries[]
                ,uint64_t &n_ranks) {
	
	uint64_t result = 0b11; // 0....11
    uint64_t node00 = 0b00; // 0....00
    // bool tempActiveTries[16] = {false, false, false, false,
    //                             false, false, false, false,
    //                             false, false, false, false,
    //                             false, false, false, false};
    bool tempActiveTries[16];
	for (uint64_t i = 0; i < n_tries; ++i) {
        if (activeTries[i]){
            // auto start   = std::chrono::high_resolution_clock::now();
            uint64_t node_i = (curr_level == max_level - 1) ? 
                                Bs[i].getNode2(roots[i]) :
                                Bs[i].getNode1(roots[i]);
            // auto end     = std::chrono::high_resolution_clock::now();
            // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            // auto time    = elapsed.count();
            // cout << "Tiempo por rank: "<< time <<" [ns]" << endl;
            // n_ranks+=time;
            // if (node_i != 0b00) {
            if (node_i){
                tempActiveTries[i] = true;
                result &= node_i;
                node00 |= node_i;
            }
            else
                tempActiveTries[i] = false;
        }
        else {
           tempActiveTries[i] = false;
        }
	}

    // bool left_one;
    // bool right_one;

    // switch (result) {
    //     case 0b00:  left_one = false;
    //                 right_one = false;
    //                 break;
    //     case 0b01:  left_one = false;
    //                 right_one = true;
    //                 break;
    //     case 0b10:  left_one = true;
    //                 right_one = false;
    //                 break;
    //     case 0b11:  left_one = true;
    //                 right_one = true;
    //                 break;
    // }

    // End condition
	if (curr_level == max_level - 1) {
        // Only write when result = 11, 10 or 01
        if (result ==  0b11 || result == 0b01 || result == 0b10) {
            // if (left_one)
            if (result == 0b10 || result == 0b11)
                ones_to_write[curr_level].push_back(last_pos[curr_level]);
            // if (right_one)
            if (result == 0b01 || result == 0b11)
                ones_to_write[curr_level].push_back(last_pos[curr_level] + 1); 
            // last_pos[curr_level] += 2*(left_one || right_one);
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

    bool exist_lchild, exist_rchild;
	
    // Left child
    if (result == 0b10 || result == 0b11) {
    // if (left_one) {
        for (uint64_t i = 0; i < n_tries; ++i) {
            // if (activeTries[i])
            if (tempActiveTries[i]){
                // auto start   = std::chrono::high_resolution_clock::now();

			    left_nodes[i] = Bs[i].getLeftChild(roots[i]);
                
                // auto end     = std::chrono::high_resolution_clock::now();
                // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                // auto time    = elapsed.count();
                // cout << "Tiempo por rank: "<< time <<" [ns]" << endl;
                // n_ranks+=time;
            }
		}
        runsEncodedIntersection(Bs, n_tries, max_level, next_level, left_nodes, last_pos, ones_to_write, tempActiveTries, n_ranks);
    }
    // if (left_one)
    //     runsEncodedIntersection(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level, tempActiveTries);

    if (last_pos[next_level] == pos_next_level_before) {
        exist_lchild = false;
    }
    // exist_lchild = (last_pos[next_level] == pos_next_level_before);
    // if (exist_lchild) {
    //     ones_to_write[curr_level].push_back(last_pos[curr_level]);
    //     pos_next_level_before = last_pos[next_level];
    // }
    else  {
        ones_to_write[curr_level].push_back(last_pos[curr_level]);
        pos_next_level_before = last_pos[next_level];
        exist_lchild = true;
    }
    
    // Right child
    if (result == 0b01 || result == 0b11) {
    // if (right_one) {
        for (uint64_t i = 0; i < n_tries; ++i) {
            // if (left_one && activeTries[i]) {
            if (tempActiveTries[i]){
                // auto start = std::chrono::high_resolution_clock::now();

                right_nodes[i] = (result == 0b11) ? left_nodes[i] + 1 : Bs[i].getRightChild(roots[i]);

                // auto end = std::chrono::high_resolution_clock::now();
                // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                // auto time = elapsed.count();
                // n_ranks += time;
                // if (left_one){
                //     right_nodes[i] = left_nodes[i] + 1;
                // }
                // else {
                //     right_nodes[i] = Bs[i].getRightChild(roots[i]);
                //     // n_ranks++;
                // } 
            }
        }
        runsEncodedIntersection(Bs, n_tries, max_level, next_level, right_nodes, last_pos, ones_to_write, tempActiveTries, n_ranks);
    }
    // if (right_one)
    //     runsEncodedIntersection(Bs, max_level, next_level, right_nodes, last_pos, ones_to_write, nodes_per_level, tempActiveTries);
    
    if (last_pos[next_level] == pos_next_level_before) {
        exist_rchild = false;
    }
    // exist_rchild = (last_pos[next_level] == pos_next_level_before);
    else {
        ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
        exist_rchild = true;
    }
    // if (exist_rchild){
    //     ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
    // }
    // mark if node is write
    if (exist_lchild || exist_rchild) {
        last_pos[curr_level] += 2;
    }  
    // last_pos[curr_level] += 2 * (exist_lchild || exist_rchild);
}


template <class trieType>
void runsEncodedIntersection(vector <trieType> &Bs,  uint64_t curr_level, 
                                uint64_t roots[], bool activeTries[]) {
	
	uint64_t result = 0b11; // 0....11
    uint64_t node00 = 0b00; // 0....00
    bool tempActiveTries[16];

	for (uint64_t i = 0; i < n_tries; ++i) {
        if (activeTries[i]) {
            uint64_t node_i = (curr_level == max_level - 1) ? 
                                Bs[i].getNode2(roots[i]) :
                                Bs[i].getNode1(roots[i]);
            if (node_i) {
                tempActiveTries[i] = true;
                result &= node_i;
            }
            else
                tempActiveTries[i] = false;

            node00 |= node_i;
        }
        else {
           tempActiveTries[i] = false;
        }
	}
    
    // End condition
	if (curr_level == max_level - 1) {
        // Only write when result = 11, 10 or 01
        if (result == 0b01 || result == 0b10 || result ==  0b11) {
            if (result == 0b10 || result == 0b11)
                ones_to_write[curr_level].push_back(last_pos[curr_level]);
            if (result == 0b01 || result == 0b11)
                ones_to_write[curr_level].push_back(last_pos[curr_level] + 1); 
            last_pos[curr_level] += 2;
        }
		return;
	}

    // all nodes are 00...0
    if ( node00 == 0b00 ) {
        last_pos[curr_level] += 2;
        return;
    }

    uint64_t next_level = curr_level + 1;
    uint64_t pos_next_level_before = last_pos[next_level];

    if ( result == 0b10 ) {
        uint64_t left_nodes[16];

        for (uint64_t i = 0; i < n_tries; ++i) {
            if (tempActiveTries[i]) {
                left_nodes[i] = Bs[i].getLeftChild(roots[i]);
            }
        }

        runsEncodedIntersection(Bs, next_level, left_nodes, tempActiveTries);
        
        if (last_pos[next_level] != pos_next_level_before) {
            ones_to_write[curr_level].push_back(last_pos[curr_level]);
            pos_next_level_before = last_pos[next_level];
            last_pos[curr_level] += 2;
        }
    }

    else if ( result == 0b01) {
        uint64_t right_nodes[16];

        for(uint64_t i = 0; i < n_tries; ++i) {
            if (tempActiveTries[i]) {
                right_nodes[i] = Bs[i].getRightChild(roots[i]);
            }
        }

        runsEncodedIntersection(Bs, next_level, right_nodes, tempActiveTries);
        
        if (last_pos[next_level] != pos_next_level_before) {
            ones_to_write[curr_level].push_back(last_pos[curr_level]+1);
            last_pos[curr_level] += 2;
        }
    }

    // result its 11
    else if (result == 0b11) {
        bool exist_lchild, exist_rchild;
        uint64_t left_nodes[16];
        uint64_t right_nodes[16];
        for (uint64_t i = 0; i < n_tries; ++i) {
            if (tempActiveTries[i]){
                uint64_t left_node = Bs[i].getLeftChild(roots[i]); 
                left_nodes[i]  = left_node;
                right_nodes[i] = left_node + 1;
            }
        }
        runsEncodedIntersection(Bs, next_level, left_nodes, tempActiveTries);
        if (last_pos[next_level] == pos_next_level_before) {
            exist_lchild = false;
        }
        else  {
            ones_to_write[curr_level].push_back(last_pos[curr_level]);
            pos_next_level_before = last_pos[next_level];
            exist_lchild = true;
        }
        runsEncodedIntersection(Bs, next_level, right_nodes, tempActiveTries);
        if (last_pos[next_level] == pos_next_level_before) {
            exist_rchild = false;
        }
        // exist_rchild = (last_pos[next_level] == pos_next_level_before);
        else {
            ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
            exist_rchild = true;
        }
        // mark if node is write
        if (exist_lchild || exist_rchild) {
            last_pos[curr_level] += 2;
        } 
    }  
}

template <class trieType>
void parRunsEncodedIntersection(uint16_t nb_threads, uint16_t threadId, std::mutex &sharedMutex, vector <trieType> &Bs,  uint64_t curr_level, 
                                uint64_t roots[], bool activeTries[]) {
	
	uint64_t result = 0b11; // 0....11
    uint64_t node00 = 0b00; // 0....00
    bool tempActiveTries[16];
    // cout << "Thread Id: " << threadId << endl;
    // sharedMutex.lock();
    // cout << "roots: "; 
    // for (uint16_t i = 0; i < 16; ++i) cout << roots[i] << " ";
    // cout << "|"<< endl;
    // sharedMutex.unlock();
	for (uint64_t i = 0; i < n_tries; ++i) {
        if (activeTries[i]) {
            uint64_t node_i = (curr_level == max_level - 1) ? 
                                Bs[i].getNode2(roots[i]) :
                                Bs[i].getNode1(roots[i]);
            // if (node_i%nb_threads != threadId) continue;
            if (node_i) {
                tempActiveTries[i] = true;
                result &= node_i;
            }
            else
                tempActiveTries[i] = false;

            node00 |= node_i;
        }
        else {
           tempActiveTries[i] = false;
        }
	}
    
    // End condition
	if (curr_level == max_level - 1) {
        // Only write when result = 11, 10 or 01
        if (result == 0b01 || result == 0b10 || result ==  0b11) {
            if (result == 0b10 || result == 0b11)
                // sharedMutex.lock();
                ones_to_write[curr_level].push_back(last_pos[curr_level]);
                // sharedMutex.unlock();
            if (result == 0b01 || result == 0b11)
                // sharedMutex.lock();
                ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
                // sharedMutex.unlock(); 
            last_pos[curr_level] += 2;
        }
		return;
	}

    // all nodes are 00...0
    if ( node00 == 0b00 ) {
        last_pos[curr_level] += 2;
        return;
    }

    uint64_t next_level = curr_level + 1;
    uint64_t pos_next_level_before = last_pos[next_level];

    if ( result == 0b10 ) {
        uint64_t left_nodes[16];

        for (uint64_t i = 0; i < n_tries; ++i) {
            if (tempActiveTries[i]) {
                left_nodes[i] = Bs[i].getLeftChild(roots[i]);
            }
        }
        // sharedMutex.lock();
        parRunsEncodedIntersection(nb_threads, threadId, sharedMutex, Bs, next_level, left_nodes, tempActiveTries);
        // sharedMutex.unlock();
        if (last_pos[next_level] != pos_next_level_before) {
            // sharedMutex.lock();
            ones_to_write[curr_level].push_back(last_pos[curr_level]);
            // sharedMutex.unlock();
            pos_next_level_before = last_pos[next_level];
            last_pos[curr_level] += 2;
        }
    }

    else if ( result == 0b01) {
        uint64_t right_nodes[16];

        for(uint64_t i = 0; i < n_tries; ++i) {
            if (tempActiveTries[i]) {
                right_nodes[i] = Bs[i].getRightChild(roots[i]);
            }
        }
        // sharedMutex.lock();
        parRunsEncodedIntersection(nb_threads, threadId, sharedMutex, Bs, next_level, right_nodes, tempActiveTries);
        // sharedMutex.unlock();
        if (last_pos[next_level] != pos_next_level_before) {
            // sharedMutex.lock();
            ones_to_write[curr_level].push_back(last_pos[curr_level]+1);
            // sharedMutex.unlock();
            last_pos[curr_level] += 2;
        }
    }

    // result its 11
    else if (result == 0b11) {
        bool exist_lchild, exist_rchild;
        uint64_t left_nodes[16];
        uint64_t right_nodes[16];
        for (uint64_t i = 0; i < n_tries; ++i) {
            if (tempActiveTries[i]){
                uint64_t left_node = Bs[i].getLeftChild(roots[i]); 
                left_nodes[i]  = left_node;
                right_nodes[i] = left_node + 1;
            }
        }
        // sharedMutex.lock();
        parRunsEncodedIntersection(nb_threads, threadId, sharedMutex, Bs, next_level, left_nodes, tempActiveTries);
        // sharedMutex.unlock();
        if (last_pos[next_level] == pos_next_level_before) {
            exist_lchild = false;
        }
        else  {
            // sharedMutex.lock();
            ones_to_write[curr_level].push_back(last_pos[curr_level]);
            // sharedMutex.unlock();
            pos_next_level_before = last_pos[next_level];
            exist_lchild = true;
        }
        // sharedMutex.lock();
        parRunsEncodedIntersection(nb_threads, threadId, sharedMutex, Bs, next_level, right_nodes, tempActiveTries);
        // sharedMutex.unlock();
        if (last_pos[next_level] == pos_next_level_before) {
            exist_rchild = false;
        }
        // exist_rchild = (last_pos[next_level] == pos_next_level_before);
        else {
            // sharedMutex.lock();
            ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
            // sharedMutex.unlock();
            exist_rchild = true;
        }
        // mark if node is write
        if (exist_lchild || exist_rchild) {
            last_pos[curr_level] += 2;
        } 
    }  
}


template <class trieType>
trieType* parJoin(vector<trieType> &Bs){
    
    max_level = Bs[0].getHeight();
    // max 16 relations
    
    
    last_pos = new uint64_t[max_level];
    for (uint64_t i = 0; i < max_level; ++i) 
        last_pos[i] = 0;
    ones_to_write = new vector<uint64_t>[max_level];
    uint64_t curr_level = 0;
    n_tries = Bs.size();
    

    unsigned nb_threads_hint = THREADS_BY_CORE * std::thread::hardware_concurrency();
    unsigned nb_threads = nb_threads_hint == 0 ? 8 : (nb_threads_hint);
    std::mutex tuplesMutex;
    parallel_for(nb_threads, [&](int start, int end) {
        for (int threadId = start; threadId < end; ++threadId)
        {
            bool activeTries[16] = { true, true, true, true,
                             true, true, true, true,
                             true, true, true, true,
                             true, true, true, true }; 
                            // active tries in runs encode implementation
            uint64_t roots[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0 };
            for (uint64_t i = 0; i < max_level; ++i) 
                last_pos[i] = 0;
            parRunsEncodedIntersection(nb_threads, threadId, tuplesMutex, Bs, curr_level, roots, activeTries);
            // parAND(nb_threads, threadId, levelOfCut, tuplesMutex, Q_star, Q_roots, Q.size(), 0, height - 1, bv, last_pos, ancestor, A.size(), bounded_result, UPPER_BOUND);
        }
    });
    
    trieType* result = new trieType(ones_to_write, max_level, last_pos, true);
    delete[] last_pos;
    return result;
    // return;
}
template 
flatBinTrie<rank_support_v5<1>>* 
parJoin<flatBinTrie<rank_support_v5<1>>>(vector<flatBinTrie<rank_support_v5<1>>> &Bs);
template flatBinTrie<rank_support_v<1>>* 
parJoin<flatBinTrie<rank_support_v<1>>>(vector<flatBinTrie<rank_support_v<1>>> &Bs);
template binTrie_il<512>* 
parJoin<binTrie_il<512>>(vector<binTrie_il<512>> &Bs);
template binTrie_il<256>* 
parJoin<binTrie_il<256>>(vector<binTrie_il<256>> &Bs);
template binTrie_il<128>* 
parJoin<binTrie_il<128>>(vector<binTrie_il<128>> &Bs);
template binTrie_il<64>* 
parJoin<binTrie_il<64>>(vector<binTrie_il<64>> &Bs);


template <class trieType>
void notRunsEncodedIntersection(vector <trieType> &Bs, uint16_t max_level, uint16_t curr_level, 
                               uint64_t *roots, 
                            //    vector<uint64_t> &last_pos,
                               uint64_t* last_pos,
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

    bool left_one;
    bool right_one;

    switch (result) {
        case 0b00:  left_one = false;
                    right_one = false;
                    break;
        case 0b01:  left_one = false;
                    right_one = true;
                    break;
        case 0b10:  left_one = true;
                    right_one = false;
                    break;
        case 0b11:  left_one = true;
                    right_one = true;
                    break;
    }

     // End condition
	if (curr_level == max_level - 1) {
        // Only write when result = 11, 10 or 01
        // if (left_one || right_one){
        if (left_one)
            ones_to_write[curr_level].push_back(last_pos[curr_level]);
        if (right_one)
            ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
        last_pos[curr_level] += 2*(left_one || right_one);
        // }
		return;
	}

	uint16_t next_level = curr_level + 1;
    uint64_t pos_next_level_before = last_pos[next_level];

    uint64_t left_nodes[16];
    uint64_t right_nodes[16];

    bool exist_lchild, exist_rchild;

	// Left child
	if (left_one) {
		for (uint64_t i = 0; i < n_tries; ++i) {
			left_nodes[i] = Bs[i].getLeftChild(roots[i]);
		}
		notRunsEncodedIntersection(Bs, max_level, next_level, left_nodes, last_pos, ones_to_write, nodes_per_level);
	}
    
    // if (last_pos[next_level] == pos_next_level_before) {
    //     exist_lchild = false;
    exist_lchild = (last_pos[next_level] == pos_next_level_before);
    // }
    // else  {
    //     ones_to_write[curr_level].push_back(last_pos[curr_level]);
    //     pos_next_level_before = last_pos[next_level];
    //     exist_lchild = true;
    // }
    if (exist_lchild) {
        ones_to_write[curr_level].push_back(last_pos[curr_level]);
        pos_next_level_before = last_pos[next_level];
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
    
    // if (last_pos[next_level] == pos_next_level_before) {
        // exist_rchild = false;
        exist_rchild = (last_pos[next_level] == pos_next_level_before);
        if (exist_rchild) ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
    // }
    // else {
    //     ones_to_write[curr_level].push_back(last_pos[curr_level] + 1);
    //     exist_rchild = true;
    // }
    // mark if node is write
    // if (exist_lchild || exist_rchild) {
    last_pos[curr_level] += 2*(exist_lchild || exist_rchild);
    // } 
}

// template<class trieType>
// void* iterativeJoin(vector<trieType> &Bs, bool runs_encoded){
//     uint64_t n_tries = Bs.size();
//     bool activeTries[16] = { true, true, true, true,
//                              true, true, true, true,
//                              true, true, true, true,
//                              true, true, true, true };
//     uint64_t roots[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
//                            0, 0, 0, 0, 0, 0, 0, 0 };
//     stack<uint64_t> stack;
//     stack.push(0);
//     while (!stack.empty) {

//         uint64_t result = 0b11;
//         for (uint16_t = 0; i < n_tries; ++i) {
//             uint64_t node_i = Bs[i].getNode(roots[i]);}
//             result &= node_i;
//         }

//         if (result == 0b11) {
//             uint64_t leftNodes[16];
//             uint64_t rightNodes[16];
//             for (uint16_t i = 0; i < n_tries; ++i){
//                 uint64_t leftNode = B[i].getLeftChild(roots[i]);
//                 leftNodes[i] = leftNode;
//                 rightNodes[i] = leftNode + 1;
//             }
//         }
//         else if (result == 0b10) {
//             uint64_t leftNodes[16];
//             for (uint16_t i = 0; i < n_tries; ++i){
//                 uint64_t leftNode = B[i].getLeftChild(roots[i]);
//                 leftNodes[i] = leftNode;
//             }
//         }
//         else if (result == 0b01) {
//             uint64_t rightNodes[16];
//             for (uint16_t i = 0; i < n_tries; ++i){
//                 uint64_t rightNode = B[i].getRightChild(roots[i]);
//                 rightNodes[i] = rightNode;
//             }

//         }
//         else {

//         }

//     }



// }

template<class trieType>
trieType* joinTries(vector<trieType> &Bs, bool runs_encoded, uint64_t &n_ranks) {
    
    // uint64_t max_level = Bs[0].getHeight();
    max_level = Bs[0].getHeight();
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
    
    // uint64_t *last_pos = new uint64_t[max_level];
    last_pos = new uint64_t[max_level];
    for (uint64_t i = 0; i < max_level; ++i) 
        last_pos[i] = 0;
    // vector<uint64_t> last_pos(max_level, 0);
    // vector<uint64_t> ones_to_write[max_level];
    ones_to_write = new vector<uint64_t>[max_level];
    // vector<uint64_t> nodes_per_level(max_level, 0);

    // for (uint16_t i = 0; i < max_level; ++i) {
    //     ones_to_write[i].reserve(pow(2, i));
    // }
    

    // auto start = std::chrono::high_resolution_clock::now();
    uint64_t curr_level = 0;
    // uint64_t n_tries = Bs.size();
    n_tries = Bs.size();
    if (runs_encoded) {
        // runsEncodedIntersection(Bs, n_tries, max_level, curr_level, roots, last_pos, ones_to_write, activeTries, n_ranks);
        runsEncodedIntersection(Bs, curr_level, roots, activeTries);
    }
    // else {
    //     notRunsEncodedIntersection(Bs, max_level, 0, roots, last_pos, ones_to_write, nodes_per_level);
    // }
    
    // auto end = std::chrono::high_resolution_clock::now();
    // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    // time = elapsed.count();
    // cout <<"|Time execution: " << (float)time*10e-6 << "[ms]" << endl;

    // flatBinTrie<rankType>* result;
    // result = new flatBinTrie<rankType>(ones_to_write, max_level, last_pos, runs_encoded);
    // trieType* result = new trieType();
    trieType* result = new trieType(ones_to_write, max_level, last_pos, runs_encoded);
    delete[] last_pos;
    return result;
}
template flatBinTrie<rank_support_v5<1>>* 
joinTries<flatBinTrie<rank_support_v5<1>>>(vector<flatBinTrie<rank_support_v5<1>>> &Bs, bool runs_encodedm, uint64_t &n_ranks);
template flatBinTrie<rank_support_v<1>>* 
joinTries<flatBinTrie<rank_support_v<1>>>(vector<flatBinTrie<rank_support_v<1>>> &Bs, bool runs_encoded, uint64_t &n_ranks);
template binTrie_il<512>* 
joinTries<binTrie_il<512>>(vector<binTrie_il<512>> &Bs, bool runs_encoded, uint64_t &n_ranks);
template binTrie_il<256>* 
joinTries<binTrie_il<256>>(vector<binTrie_il<256>> &Bs, bool runs_encoded, uint64_t &n_ranks);
template binTrie_il<128>* 
joinTries<binTrie_il<128>>(vector<binTrie_il<128>> &Bs, bool runs_encoded, uint64_t &n_ranks);
template binTrie_il<64>* 
joinTries<binTrie_il<64>>(vector<binTrie_il<64>> &Bs, bool runs_encoded, uint64_t &n_ranks);