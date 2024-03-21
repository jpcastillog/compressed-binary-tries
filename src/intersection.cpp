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
#include <algorithm>
#ifdef LINUX
#include "parallel_for.hpp"
#elif MACOS
#include "parallel_for_macos.hpp"
#else
#error No operating system supported to parallel for
#endif


using namespace std;


vector<uint64_t> forceBruteIntersection(vector<vector<uint64_t>> &S){
    sort(S.begin(), S.end(),
         [](const vector<uint64_t> &a, const vector<uint64_t> &b){
            return a.size() < b.size();
         }
    ); 
    uint64_t k = S.size();
    vector<uint64_t> intersection = S[0];
    vector<uint64_t>aux_intersection;
    for (uint16_t i = 1; i<k; ++i){
        aux_intersection.clear();
        set_intersection(intersection.begin(), intersection.end(),
                        S[i].begin(), S[i].end(),
                        back_inserter(aux_intersection));
        intersection = aux_intersection;
    }
    return intersection;
}


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

    if (orResult == 0b00) {
        uint64_t below = prefix;
        uint64_t range = ((uint64_t)1 << (maxLevel - currLevel))- 1;
        uint64_t above = prefix | range;
        // std::generate_n(std::back_inserter(r), above-below+1,[&]()mutable{return r.size();});
        for (uint64_t i = below; i <= above; ++i) {
            r.push_back(i);
        }
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
        uint64_t rightResult = (prefix | ((uint64_t)1 << (maxLevel- currLevel - 1)));
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
        uint64_t rightResult = (prefix | ((uint64_t)1 << (maxLevel- currLevel - 1)));
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
        if (runs) 
            atrs = new bool[16];
        for (uint64_t i = 0; i < n_tries; ++i){
            rts[i] = roots[i];
            if (runs)
                atrs[i] = activeTries[i];
        }
        threads_roots.push_back(rts);
        if (runs) 
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
        // std::generate_n(std::back_inserter(r), above-below+1,[&]()mutable{return r.size();});
        for (uint64_t i = below; i <= above; ++i) r.push_back(i);
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


vector<uint64_t> Intersect(vector<binaryTrie*> &Ts, bool runs, bool parallel){
    uint64_t height = Ts[0] -> getHeight();
    for (auto T: Ts){
        if (T->getHeight() != height){
            cerr << "All tries need to be of same height\n"; 
            return vector<uint64_t>(); // return empty vector
        }
    }
    uint64_t nTries = Ts.size();
    // max 16 relations
    bool activeTries[16] = { true, true, true, true,
                             true, true, true, true,
                             true, true, true, true,
                             true, true, true, true };
    uint64_t roots[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0 };

    // Hint of threads avaible in our CPU
    unsigned nb_threads_hint = std::thread::hardware_concurrency();
    uint64_t level_of_cut   = floor(log2(nb_threads_hint));
    unsigned int nThreads   = pow(2, level_of_cut); // Max number of threads to use in level of cut
    
    vector<uint64_t> intersection;
    intersection.reserve(1000000);
    if (parallel && level_of_cut <= height-2){
        vector<bool*> threadActiveTries;
        vector<uint64_t*> threadRoots;
        vector<uint64_t> tpartialSolutions;

        partialAND(Ts, nTries, height, 0, level_of_cut, 
                    roots, activeTries, 0, intersection,
                    tpartialSolutions, threadRoots, threadActiveTries, runs);
        
        if (threadRoots.size() == 0) {
            return intersection;
        }

        uint16_t usedThreads = threadRoots.size(); //Number of threads to use until level of cut
        uint16_t totalThreads = usedThreads;
        vector<uint64_t> initLevel(usedThreads, level_of_cut);
        // Go down one more level to fill all threads
        vector<bool*> nextActiveTries;
        vector<uint64_t*> nextRoots;
        vector<uint64_t> nextPartialSolutions;
        uint64_t i = 0;
        while ((nThreads - usedThreads > 1) && (i < totalThreads)) {
            partialAND(Ts, nTries, height, level_of_cut, level_of_cut+1, threadRoots[i], 
                        runs?threadActiveTries[i]:activeTries, tpartialSolutions[i], intersection, 
                        nextPartialSolutions, nextRoots, nextActiveTries, runs);
            usedThreads = totalThreads + nextRoots.size() - (++i);
        }
        vector<uint64_t> nextInitLevel(nextRoots.size(), level_of_cut+1);

        for(uint16_t j = i; j < totalThreads; ++j) {
            nextRoots.push_back(threadRoots[j]);
            if (runs)
                nextActiveTries.push_back(threadActiveTries[j]);
            nextPartialSolutions.push_back(tpartialSolutions[j]);
            nextInitLevel.push_back(level_of_cut);
        }
        // Init the vector to contain threads solutions
        vector<vector<uint64_t>> threads_results(usedThreads);
        for (auto tr: threads_results) tr.reserve(1000000);
        
        if (nextRoots.size() == 0) {
            return intersection;
        }
        
        if (runs){
            parallel_for(usedThreads, usedThreads, [&](int start, int end) {
                for (uint16_t threadId = start; threadId < end; ++threadId) {
                    runsAND(Ts, nTries, height, nextInitLevel[threadId], 
                        nextRoots[threadId], nextActiveTries[threadId],
                        nextPartialSolutions[threadId], threads_results[threadId]);
                }
            });
        }
        else {
            parallel_for(usedThreads, usedThreads, [&](int start, int end) {
                for (uint16_t threadId = start; threadId < end; ++threadId) {
                    AND(Ts, nTries, height, nextInitLevel[threadId], 
                        nextRoots[threadId], nextPartialSolutions[threadId], 
                        threads_results[threadId]);
                }
            });

        }
        uint64_t output_size = intersection.size(); 
        vector<uint64_t> shifts(usedThreads);
        uint64_t shift = intersection.size();
        for(uint64_t t = 0; t < usedThreads; ++t){
            output_size += threads_results[t].size();
            shifts[t] = shift;
            shift += threads_results[t].size();
        }
        // Write in parallel threads result
        if (output_size > 450000){
            intersection.resize(output_size);
            parallel_for(usedThreads, usedThreads, [&](int start, int end) {
                for (uint16_t threadId = start; threadId < end; ++threadId) {
                    for (uint64_t j = 0; j < threads_results[threadId].size(); ++j) {
                        intersection[j+shifts[threadId]] = threads_results[threadId][j];
                    } 
                }        
            });
        } 
        else {
            // Concatenate solutions of threads
            for(uint64_t t=0; t < usedThreads; ++t){
                intersection.insert(intersection.end(), 
                                    threads_results[t].begin(),
                                    threads_results[t].end()
                                    );
            }
        }
        // Free memory
        for (uint64_t i = 0; i < usedThreads; ++i) {
            if (runs) 
                delete[] nextActiveTries[i];
            delete[] nextRoots[i];
        }
    }
    else {
        if (runs)
            runsAND(Ts, nTries, height, 0, roots, activeTries, 0, intersection);
        else 
            AND(Ts, nTries, height, 0, roots, 0, intersection);
    }

    return intersection;
}