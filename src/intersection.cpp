#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include "binTrie.hpp"
#include "flatBinTrie_il.hpp"
#include "flatBinTrie.hpp"
#include "binTrie_il.hpp"
#include "binaryTrie.hpp"
#include <math.h>
#include "parallel_for.hpp"

using namespace std;


void runsAND(vector<binaryTrie*> &Ts, uint64_t nTries, uint64_t &maxLevel,
        uint64_t currLevel, uint64_t roots[], bool activeTries[], 
        uint64_t prefix, vector<uint64_t> &r){
    if (currLevel == maxLevel) {
        r.push_back(prefix);
        return;
    }
    uint64_t i;
    bool tempActiveTries[16];
    uint64_t andResult = 0b11;
    uint64_t orResult = 0b00;
    for (i = 0; i < nTries; ++i) {
        if (activeTries[i]) {
            uint64_t node_i = Ts[i] -> getNode(roots[i], currLevel);
            if (node_i != 0b00) {
                tempActiveTries[i] = true;
                andResult &= node_i;
            } else {
                tempActiveTries[i] = false;
            }
            orResult |= node_i;
        } else {
            tempActiveTries[i] = false;
        }
    }

    if (andResult == 0b00 && orResult != 0b00)
        return;

    if (orResult = 0b00) {
        uint64_t below = prefix;
        uint64_t range = ((uint64_t)1 << (maxLevel - currLevel))- 1;
        uint64_t above = prefix | range;
        std::generate_n(std::back_inserter(r), above-below+1,[&]()mutable{return r.size();});
        // for (uint64_t i = below; i <= above; ++i) {
        //     r.push_back(i);
        // }
        return;
    }

    bool existLeft, existRight;
    uint64_t nextLevel = currLevel + 1;
    if(andResult == 0b10) {
        uint64_t leftNodes[16];
        uint64_t leftResult = prefix;
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel - 1) 
                leftNodes[i] = Ts[i] -> getLeftChild(roots[i], currLevel);
        }
        runsAND(Ts, nTries, maxLevel, nextLevel, leftNodes,
                        tempActiveTries, leftResult, r);
    }
    else if (andResult == 0b01) {
        uint64_t rightNodes[16];
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel -1)
                rightNodes[i] = Ts[i] -> getRightChild(roots[i], currLevel);
        }
        runsAND(Ts, nTries, maxLevel, nextLevel, rightNodes,
                        tempActiveTries, rightResult, r);

    }
    else if (andResult == 0b11) {
        uint64_t leftNodes[16];
        uint64_t rightNodes[16];
        uint64_t leftResult = prefix;
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));;
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel - 1) {
                uint64_t leftNode = Ts[i] -> getLeftChild(roots[i], currLevel);
                leftNodes[i] = leftNode;
                rightNodes[i] = leftNode + 1;
            }
        }
        runsAND(Ts, nTries, maxLevel, nextLevel, leftNodes,
                        tempActiveTries, leftResult, r);
        runsAND(Ts, nTries, maxLevel, nextLevel, rightNodes,
                        tempActiveTries, rightResult, r);
    }
    return;
};


bool AND(vector<binaryTrie*> &Ts, uint64_t nTries, uint64_t &maxLevel,
        uint64_t currLevel, uint64_t roots[], uint64_t prefix, vector<uint64_t> &r){
    if (currLevel == maxLevel) {
        r.push_back(prefix);
        return true;
    }
    uint64_t i;
    uint64_t andResult = 0b11;
    for (i = 0; i < nTries; ++i) {
        uint64_t node_i = Ts[i] -> getNode(roots[i], currLevel);
        andResult &= node_i;
        
    }
    // Can't go any further down in that branch.
    if (andResult == 0b00)
        return false;

    bool existLeft, existRight;
    uint64_t nextLevel = currLevel + 1;
    if(andResult == 0b10) {
        uint64_t leftNodes[16];
        uint64_t leftResult = prefix;
        for (i = 0; i < nTries; ++i) {
            if (currLevel != maxLevel - 1) 
                leftNodes[i] = Ts[i] -> getLeftChild(roots[i], currLevel);
        }
        existLeft = AND(Ts, nTries, maxLevel, nextLevel, leftNodes, leftResult, r);
    }
    else if (andResult == 0b01) {
        uint64_t rightNodes[16];
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));
        for (i = 0; i < nTries; ++i) {
            if (currLevel != maxLevel -1)
                rightNodes[i] = Ts[i] -> getRightChild(roots[i], currLevel);
        }
        existRight = AND(Ts, nTries, maxLevel, nextLevel, rightNodes, rightResult, r);

    }
    else if (andResult == 0b11) {
        uint64_t leftNodes[16];
        uint64_t rightNodes[16];
        uint64_t leftResult = prefix;
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));;
        for (i = 0; i < nTries; ++i) {
            if (currLevel != maxLevel - 1) {
                uint64_t leftNode = Ts[i] -> getLeftChild(roots[i], currLevel);
                leftNodes[i] = leftNode;
                rightNodes[i] = leftNode + 1;
            }
        }
        existLeft = AND(Ts, nTries, maxLevel, nextLevel, leftNodes, leftResult, r);
        existRight = AND(Ts, nTries, maxLevel, nextLevel, rightNodes, rightResult, r);
    }
    if (existLeft || existRight)
        return true;
    return false;
};


void partialAND(vector<binaryTrie*> &Ts, uint16_t n_tries, uint64_t max_level, uint64_t curr_level, 
                uint64_t cut_level, uint64_t roots[], bool activeTries[], uint64_t partial_int,
                vector <uint64_t> &r, vector<uint64_t> &partial_ints, vector<uint64_t*> &threads_roots,
                vector<bool*> &threads_activeTries, bool runs) {
    // End condition
    if (curr_level == cut_level) {
        partial_ints.push_back(partial_int);
        uint64_t* rts = new uint64_t[16];
        bool* atrs;
        // if (runs) 
            atrs = new bool[16];
        for (uint64_t i = 0; i < n_tries; ++i){
            rts[i] = roots[i];
            if (runs)
                atrs[i] = activeTries[i];
        }
        threads_roots.push_back(rts);
        // if (runs) 
            threads_activeTries.push_back(atrs);
        return;
    }
    
    bool tempActiveTries [16]; 
    uint64_t result = 0b11;
    uint64_t node00 = 0b00;
    for (uint16_t i = 0;  i < n_tries; ++i) {
        if (runs){
            if (activeTries[i]) {
                uint64_t node_i = Ts[i] -> getNode(roots[i], curr_level);
                if (node_i || runs) {
                    tempActiveTries[i] = true;
                    result &= node_i;
                }
                else tempActiveTries[i] = false;
                node00 |= node_i;
            }
            else {
                tempActiveTries[i] = false;
            }
        }
        else {
            uint64_t node_i = Ts[i] -> getNode(roots[i], curr_level);
            result &= node_i;
        }
    }

    if ((!runs) && (result == 0b00)) 
        return;
    if (runs && node00 == 0b00) {
        uint64_t below = partial_int;
        uint64_t range = ((uint64_t)1 << (max_level - curr_level))- 1;
        uint64_t above = partial_int | range;
        std::generate_n(std::back_inserter(r), above-below+1,[&]()mutable{return r.size();});
        // for (uint64_t i = below; i <= above; ++i) {
        //     r.push_back(i);
        // }
        return;
    }


    if (result == 0b10) {
        uint64_t left_nodes[16];
        uint64_t leftResult = partial_int;
        for (uint64_t i = 0; i < n_tries; ++i) {
            if (runs && tempActiveTries[i])
                left_nodes[i] = Ts[i] -> getLeftChild(roots[i], curr_level);
            else 
                left_nodes[i] = Ts[i] -> getLeftChild(roots[i], curr_level);
        }
        partialAND(Ts, n_tries, max_level, curr_level + 1, cut_level, 
            left_nodes, tempActiveTries, leftResult, r, partial_ints,
            threads_roots, threads_activeTries, runs);

    }
    else if (result == 0b01){
        uint64_t right_nodes[16];
        uint64_t rightResult = partial_int;
        rightResult = (rightResult | (1ULL << (max_level- curr_level - 1)));
        for (uint64_t i = 0; i < n_tries; ++i) {
            if (runs && tempActiveTries[i])
                right_nodes[i] = Ts[i] -> getRightChild(roots[i], curr_level);
            else
                right_nodes[i] = Ts[i] -> getRightChild(roots[i], curr_level);
        }
        partialAND(Ts, n_tries, max_level, curr_level + 1, cut_level,
            right_nodes, tempActiveTries, rightResult, r, partial_ints,
            threads_roots, threads_activeTries, runs);

    }
    else if (result == 0b11) {
        uint64_t left_nodes[16];
        uint64_t right_nodes[16];
        uint64_t leftResult = partial_int;
        uint64_t rightResult = partial_int;
        rightResult = (rightResult | (1ULL << (max_level- curr_level - 1)));
        for(uint64_t i = 0; i < n_tries; ++i) {
            if (runs && tempActiveTries[i]) {
                uint64_t left_node = Ts[i] -> getLeftChild(roots[i], curr_level);
                left_nodes[i]  = left_node;
                right_nodes[i] = left_node + 1; 
            }
            else {
                uint64_t left_node = Ts[i] -> getLeftChild(roots[i], curr_level);
                left_nodes[i]  = left_node;
                right_nodes[i] = left_node + 1;
            }
        }
        // Left Childs
        partialAND(Ts, n_tries, max_level, curr_level + 1, cut_level, 
            left_nodes, tempActiveTries, leftResult, r, partial_ints,
            threads_roots, threads_activeTries, runs);
        // Right Childs
        partialAND(Ts, n_tries, max_level, curr_level + 1, cut_level,
            right_nodes, tempActiveTries, rightResult, r, partial_ints,
            threads_roots, threads_activeTries, runs);
    }
}


vector<uint64_t> Intersect(vector<binaryTrie*> &Ts, bool runs){
    uint64_t max_level = Ts[0] -> getHeight();
    uint64_t n_tries = Ts.size();
    // max 16 relations
    bool activeTries[16] = { true, true, true, true,
                             true, true, true, true,
                             true, true, true, true,
                             true, true, true, true };
    uint64_t roots[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0 };
    uint64_t curr_level = 0;
    // Hint of threads avaible in our CPU
    unsigned nb_threads_hint = std::thread::hardware_concurrency();
    uint64_t level_of_cut     = floor(log2(nb_threads_hint));
    unsigned int nb_threads   = pow(2, level_of_cut);
    
    vector<bool*>       activeTries2;
    vector<uint64_t*>   roots2;
    vector<uint64_t>    partial_solutions;
    vector<uint64_t> intersection;
    intersection.reserve(1000000);
    
    uint64_t partial_int = 0;
    partialAND(Ts, n_tries, max_level, curr_level, level_of_cut, roots, activeTries, partial_int, intersection,
                partial_solutions, roots2, activeTries2, runs);
    uint16_t real_threads = roots2.size();
    uint16_t init_threads = real_threads;
    vector<uint64_t> init_level(real_threads, level_of_cut);
    uint64_t i = 0;
    while (nb_threads - real_threads > 1 && i < init_threads) {
        partialAND( Ts, n_tries, max_level, level_of_cut, level_of_cut + 1,
                    roots2[i], activeTries2[i], partial_solutions[i], intersection,
                    partial_solutions, roots2, activeTries2, runs
                    );
        uint64_t dif = roots2.size() - real_threads;
        for (uint64_t j = 0; j < dif; ++j) {
            init_level.push_back(level_of_cut + 1);
        }
         ++i;
        real_threads = roots2.size() - i ;
    } 
    // init the vector to contain threads solutions
    vector<vector<uint64_t>> threads_results;//(real_threads);
    for (uint64_t t = 0; t < real_threads; ++t){
        vector<uint64_t> r;
        threads_results.push_back(r);
        threads_results[t].reserve(1000000);
    }
    // cout << "Parallel intersection"<< endl;
    if (runs){
        parallel_for(real_threads, real_threads, [&](int start, int end) {
            for (uint16_t threadId = start; threadId < end; ++threadId) {
                runsAND(Ts, n_tries, max_level, init_level[threadId+i], 
                    roots2[threadId+i], activeTries2[threadId+i],
                    partial_solutions[threadId+i], threads_results[threadId]);
            }
        });
    }
    else {
        parallel_for(real_threads, real_threads, [&](int start, int end) {
            for (uint16_t threadId = start; threadId < end; ++threadId) {
                AND(Ts, n_tries, max_level, init_level[threadId+i], 
                    roots2[threadId+i], partial_solutions[threadId+i], 
                    threads_results[threadId]);
            }
        });

    }
    // cout << "end parallel intersection" << endl;
    uint64_t output_size = intersection.size(); 
    vector<uint64_t> shifts(real_threads);
    uint64_t shift = intersection.size();
    for(uint64_t t = 0; t < real_threads; ++t){
        output_size += threads_results[t].size();
        shifts[t] = shift;
        shift += threads_results[t].size();
    }
    // Write in parallel threads result
    if (output_size > 450000){
        intersection.resize(output_size);
        parallel_for(real_threads, real_threads, [&](int start, int end) {
            for (uint16_t threadId = start; threadId < end; ++threadId) {
                for (uint64_t j = 0; j < threads_results[threadId].size(); ++j) {
                    intersection[j+shifts[threadId]] = threads_results[threadId][j];
                } 
            }        
        });
    } 
    else {
        // Concatenate solutions of threads
        for(uint64_t t=0; t < real_threads; ++t){
            intersection.insert(intersection.end(), 
                                threads_results[t].begin(),
                                threads_results[t].end()
                                );
        }
    }
    // Free memory
    for (uint64_t i = 0; i < real_threads; ++i) {
        if (runs) 
            delete[] activeTries2[i];
        delete[] roots2[i];
    }
    return intersection;
}