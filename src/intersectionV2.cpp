#include <iostream>
#include <vector>
#include <stack>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <tuple>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include "binTrie_il.hpp"
#include "util_functions.hpp"
#include <math.h>

using namespace std;


template<class trieType>
uint64_t seqAND(vector<trieType> &Ts, uint64_t maxLevel) {
    vector<bool*> threadsActiveTries;
    uint64_t nTries = Ts.size();
    uint16_t i;

    bool* activeTries = new bool[16];
    uint64_t* roots = new uint64_t[16];
    for (i = 0; i < 16; ++i) {
        roots[i] = 0;
        activeTries[i] = true;
    } 

    queue <tuple<uint64_t*, bool*>> q;
    tuple<uint64_t*, bool*> firstNode(roots, activeTries);
    q.push(firstNode);
    
    uint64_t level          = 0;
    uint64_t nodesCurrLevel = 1;
    uint64_t cNodes         = 0;
    uint64_t nodesNextLevel = 0;

    while (!q.empty()) {
        cNodes++;
        tuple<uint64_t*, bool*> node = q.front();
        q.pop();

        bool* tempActiveTries2;
        uint64_t* tempRoots;
        tie(tempRoots, tempActiveTries2) = node;

        bool* tempActiveTries = new bool[16];
        for (int j = 0; j < 16; ++j){
            tempActiveTries[j] = tempActiveTries2[j];
        }
        
        uint64_t andResult = 0b11;
        uint64_t orResult = 0b00;
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i]) {
                uint64_t node_i = (level == maxLevel -1) ?
                                    Ts[i].getNode2(tempRoots[i]):
                                    Ts[i].getNode1(tempRoots[i]);
                if (node_i) {
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
        if (andResult == 0b10) {
            if (level+1 < maxLevel){
                uint64_t* leftNodes = new uint64_t[16];
                for (i = 0; i < nTries; ++i) {
                    if (tempActiveTries[i] && level != maxLevel - 1) 
                        leftNodes[i] = Ts[i].getLeftChild(tempRoots[i]);
                }
                tuple<uint64_t*, bool*> left(leftNodes, tempActiveTries);
                q.push(left);
                nodesNextLevel++;
            } else {
                uint64_t* leftNodes = new uint64_t[16];
                for (i = 0; i < nTries; ++i) {
                    if (tempActiveTries[i] && level != maxLevel - 1) 
                        leftNodes[i] = Ts[i].getLeftChild(tempRoots[i]);
                }
                delete[] leftNodes;
            }
        }
        else if (andResult == 0b01) {
            if (level + 1 < maxLevel){
                uint64_t* rightNodes = new uint64_t[16];
                for (i = 0; i < nTries; ++i) {
                    if (tempActiveTries[i] && level != maxLevel -1)
                        rightNodes[i] = Ts[i].getRightChild(tempRoots[i]);
                }
                tuple<uint64_t*, bool*> right(rightNodes, tempActiveTries);
                q.push(right);
                nodesNextLevel++;
            } else {
                uint64_t* rightNodes = new uint64_t[16];
                for (i = 0; i < nTries; ++i) {
                    if (tempActiveTries[i] && level != maxLevel -1)
                        rightNodes[i] = Ts[i].getRightChild(tempRoots[i]);
                }
                delete[] rightNodes;
            }
        }
        else if (andResult == 0b11) {
            if (level + 1 < maxLevel){
                uint64_t* leftNodes = new uint64_t[16];
                uint64_t* rightNodes = new uint64_t[16];
                bool* tempActiveTriesR = new bool[16];
                for(int j = 0; j < 16; j++) tempActiveTriesR[j] = tempActiveTries[j];
                for (i = 0; i < nTries; ++i) {
                    if (tempActiveTries[i] && level != maxLevel - 1) {
                        uint64_t leftNode = Ts[i].getLeftChild(tempRoots[i]);
                        leftNodes[i] = leftNode;
                        rightNodes[i] = leftNode + 1;
                    }
                }
                tuple<uint64_t*, bool*> left(leftNodes, tempActiveTries);
                tuple<uint64_t*, bool*> right(rightNodes, tempActiveTriesR);
                q.push(left);
                q.push(right);
                nodesNextLevel += 2;
            } else {
                for (i = 0; i < nTries; ++i) {
                    uint64_t* leftNodes = new uint64_t[16];
                    uint64_t* rightNodes = new uint64_t[16];
                    if (tempActiveTries[i] && level != maxLevel - 1) {
                        uint64_t leftNode = Ts[i].getLeftChild(tempRoots[i]);
                        leftNodes[i] = leftNode;
                        rightNodes[i] = leftNode + 1;
                    }
                    delete[] leftNodes;
                    delete[] rightNodes;
                }
            }
            
        }
        delete[] tempActiveTries2;
        delete[] tempRoots;

        if (cNodes == nodesCurrLevel) {
            nodesCurrLevel = nodesNextLevel;
            nodesNextLevel = 0;
            cNodes = 0;
            level++;
        }
        if (level == maxLevel){
            break;
        }   
        
    } 
    return 1;
}

template<class trieType>
uint64_t determineLevelOfCut(vector<trieType> &Ts, uint64_t maxLevel, 
                             uint64_t nThreads) {
    vector<uint64_t *> threadsRoots;
    vector<bool*> threadsActiveTries;
    uint64_t nTries = Ts.size();
    uint16_t i;
    uint64_t idealCut = floor(log2(nThreads));
    bool* activeTries = new bool[16];
    uint64_t* roots = new uint64_t[16];
    for (i = 0; i < 16; ++i) {
        roots[i] = 0;
        activeTries[i] = true;
    } 

    queue <tuple<uint64_t*, bool*>> q;
    tuple<uint64_t*, bool*> firstNode(roots, activeTries);
    q.push(firstNode);
    
    uint64_t level          = 0;
    uint64_t nodesCurrLevel = 1;
    uint64_t cNodes         = 0;
    uint64_t nodesNextLevel = 0;

    while (!q.empty()) {
        cNodes++;
        tuple<uint64_t*, bool*> node = q.front();
        q.pop();

        bool* tempActiveTries2;
        uint64_t* tempRoots;
        tie(tempRoots, tempActiveTries2) = node;

        bool* tempActiveTries = new bool[16];
        for (int j = 0; j < 16; ++j){
            tempActiveTries[j] = tempActiveTries2[j];
        }
        
        uint64_t andResult = 0b11;
        uint64_t orResult = 0b00;
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i]) {
                uint64_t node_i = (level == maxLevel -1) ?
                                    Ts[i].getNode2(tempRoots[i]):
                                    Ts[i].getNode1(tempRoots[i]);
                if (node_i) {
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
        if (andResult == 0b10) {
            uint64_t* leftNodes = new uint64_t[16];
            for (i = 0; i < nTries; ++i) {
                if (tempActiveTries[i] && level != maxLevel - 1) 
                    leftNodes[i] = Ts[i].getLeftChild(tempRoots[i]);
            }
            tuple<uint64_t*, bool*> left(leftNodes, tempActiveTries);
            q.push(left);
            nodesNextLevel++;
        }
        else if (andResult == 0b01) {
            uint64_t* rightNodes = new uint64_t[16];
            for (i = 0; i < nTries; ++i) {
                if (tempActiveTries[i] && level != maxLevel -1)
                    rightNodes[i] = Ts[i].getRightChild(tempRoots[i]);
            }
            tuple<uint64_t*, bool*> right(rightNodes, tempActiveTries);
            q.push(right);
            nodesNextLevel++;
        }
        else if (andResult == 0b11) {
            uint64_t* leftNodes = new uint64_t[16];
            uint64_t* rightNodes = new uint64_t[16];
            bool* tempActiveTriesR = new bool[16];
            for(int j = 0; j < 16; j++) tempActiveTriesR[j] = tempActiveTries[j];
            for (i = 0; i < nTries; ++i) {
                if (tempActiveTries[i] && level != maxLevel - 1) {
                    uint64_t leftNode = Ts[i].getLeftChild(tempRoots[i]);
                    leftNodes[i] = leftNode;
                    rightNodes[i] = leftNode + 1;
                }
            }
            tuple<uint64_t*, bool*> left(leftNodes, tempActiveTries);
            tuple<uint64_t*, bool*> right(rightNodes, tempActiveTriesR);
            q.push(left);
            q.push(right);
            nodesNextLevel += 2;
        }
        delete[] tempActiveTries2;
        delete[] tempRoots;
        if (cNodes == nodesCurrLevel) {
            if ((nodesNextLevel > nThreads) || (level == idealCut + 2)) return level;
            nodesCurrLevel = nodesNextLevel;
            nodesNextLevel = 0;
            cNodes = 0;
            level++;
        }
        if (level == maxLevel){
            break;
        }
        
        
    } 
    return floor(log2(nThreads));
}

template<class trieType>
// bool
void runsAND2(vector<trieType> &Ts, uint64_t nTries, uint64_t &maxLevel,
        uint64_t currLevel, uint64_t roots[], bool activeTries[], 
        uint64_t prefix, vector<uint64_t> &r){
    if (currLevel == maxLevel) {
        r.push_back(prefix);
        // return true;
        return;
    }
    uint64_t i;
    bool tempActiveTries[16];
    uint64_t andResult = 0b11;
    uint64_t orResult = 0b00;
    for (i = 0; i < nTries; ++i) {
        if (activeTries[i]) {
            uint64_t node_i = (currLevel == maxLevel -1) ?
                                Ts[i].getNode2(roots[i]):
                                Ts[i].getNode1(roots[i]);
            if (node_i) {
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
        // return false;

    if (orResult = 0b00) {
        uint64_t below = prefix;
        uint64_t range = ((uint64_t)1 << (maxLevel - currLevel))- 1;
        uint64_t above = prefix | range;
        // std::generate_n(std::back_inserter(r), above-below+1,[&]()mutable{return r.size();});
        for (uint64_t i = below; i <= above; ++i) {
            // sharedMutex.lock();
            r.push_back(i);
            // sharedMutex.unlock();
            // dummy = i;
        }
        return;
        // return true;
    }

    bool existLeft, existRight;
    uint64_t nextLevel = currLevel + 1;
    if(andResult == 0b10) {
        uint64_t leftNodes[16];
        uint64_t leftResult = prefix;
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel - 1) 
                leftNodes[i] = Ts[i].getLeftChild(roots[i]);
        }
        // existLeft = 
        runsAND2(Ts, nTries, maxLevel, nextLevel, leftNodes,
                        tempActiveTries, leftResult, r);
    }
    else if (andResult == 0b01) {
        uint64_t rightNodes[16];
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel -1)
                rightNodes[i] = Ts[i].getRightChild(roots[i]);
        }
        // existRight = 
        runsAND2(Ts, nTries, maxLevel, nextLevel, rightNodes,
                        tempActiveTries, rightResult, r);

    }
    else if (andResult == 0b11) {
        uint64_t leftNodes[16];
        uint64_t rightNodes[16];
        uint64_t leftResult = prefix;
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));;
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel - 1) {
                uint64_t leftNode = Ts[i].getLeftChild(roots[i]);
                leftNodes[i] = leftNode;
                rightNodes[i] = leftNode + 1;
            }
        }
        // existLeft = 
        runsAND2(Ts, nTries, maxLevel, nextLevel, leftNodes,
                        tempActiveTries, leftResult, r);
        // existRight = 
        runsAND2(Ts, nTries, maxLevel, nextLevel, rightNodes,
                        tempActiveTries, rightResult, r);
    }
    // if (existLeft || existRight)
    //     return true;
    // return false;
    return;
};


template<class trieType>
bool AND2(vector<trieType> &Ts, uint64_t nTries, uint64_t &maxLevel,
        uint64_t currLevel, uint64_t roots[], uint64_t prefix, vector<uint64_t> &r){
    if (currLevel == maxLevel) {
        r.push_back(prefix);
        return true;
    }
    uint64_t i;
    uint64_t andResult = 0b11;
    for (i = 0; i < nTries; ++i) {
        uint64_t node_i = (currLevel == maxLevel -1) ?
                            Ts[i].getNode2(roots[i]):
                            Ts[i].getNode1(roots[i]);
        andResult &= node_i;
        
    }
    // Can't go any further down that branch.
    if (andResult == 0b00)
        return false;

    bool existLeft, existRight;
    uint64_t nextLevel = currLevel + 1;
    if(andResult == 0b10) {
        uint64_t leftNodes[16];
        uint64_t leftResult = prefix;
        for (i = 0; i < nTries; ++i) {
            if (currLevel != maxLevel - 1) 
                leftNodes[i] = Ts[i].getLeftChild(roots[i]);
        }
        existLeft = AND2(Ts, nTries, maxLevel, nextLevel, leftNodes, leftResult, r);
    }
    else if (andResult == 0b01) {
        uint64_t rightNodes[16];
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));
        for (i = 0; i < nTries; ++i) {
            if (currLevel != maxLevel -1)
                rightNodes[i] = Ts[i].getRightChild(roots[i]);
        }
        existRight = AND2(Ts, nTries, maxLevel, nextLevel, rightNodes, rightResult, r);

    }
    else if (andResult == 0b11) {
        uint64_t leftNodes[16];
        uint64_t rightNodes[16];
        uint64_t leftResult = prefix;
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));;
        for (i = 0; i < nTries; ++i) {
            if (currLevel != maxLevel - 1) {
                uint64_t leftNode = Ts[i].getLeftChild(roots[i]);
                leftNodes[i] = leftNode;
                rightNodes[i] = leftNode + 1;
            }
        }
        existLeft = AND2(Ts, nTries, maxLevel, nextLevel, leftNodes, leftResult, r);
        existRight = AND2(Ts, nTries, maxLevel, nextLevel, rightNodes, rightResult, r);
    }
    if (existLeft || existRight)
        return true;
    return false;
};

// Parallel implementation using threads
template<class trieType>
void parallelAND(vector<trieType> &Ts, uint64_t nTries, uint64_t &nThreads, vector<thread> &threads,
                cpu_set_t &cpuset, uint64_t &maxLevel, uint64_t currLevel, uint64_t levelOfCut,
                uint64_t roots[], vector<uint64_t[16]> &threadsRoots,
                bool activeTries[], vector<bool[16]> &threadsActiveTries, 
                uint64_t prefix, vector<uint64_t> &threadsPrefix, 
                vector<vector<uint64_t>> &threadsR) {
    if (currLevel == maxLevel){
        // r.push_back(prefix);
        return;
    }
    if (currLevel == levelOfCut) {
        uint64_t threadId = nThreads;
        // threadsActiveTries[threadId] = activeTries;
        threadsPrefix[threadId] = prefix;
        // threadsRoots[threadId] = roots;
        copy(activeTries, activeTries+16, threadsActiveTries[threadId]);
        copy(roots, roots+16, threadsRoots[threadId]); 
        // cout << "Init creating thread: " << threadId << " currLevel: " << currLevel << endl;
        threads[threadId] = std::thread(AND2<trieType>, ref(Ts), nTries, ref(maxLevel), currLevel,
                                        ref(threadsRoots[threadId]), ref(threadsActiveTries[threadId]),
                                        threadsPrefix[threadId], ref(threadsR[threadId]));
        // cout << "Ok thread: " << threadId << endl;
        // threads.push_back(t);
        // cpu_set_t cpuset;
        // CPU_ZERO(&cpuset);
        CPU_SET(threadId, &cpuset);
        int rc = pthread_setaffinity_np(threads[threadId].native_handle(),
                                        sizeof(cpuset), &cpuset);
        if (rc != 0) {
            std::cerr << "Error calling pthread_setaffinity_np: " << rc << endl;
        }
        // cout << "Thread id " << threadId << " result: " << r_t.size() << endl;
        // threads[threadId].join();
        nThreads++;
        return;
    }

    uint64_t i;
    bool tempActiveTries[16];
    uint64_t andResult = 0b11;
    uint64_t orResult = 0b00;
    for (i = 0; i < nTries; ++i) {
        if (activeTries[i]) {
            uint64_t node_i = (currLevel == maxLevel -1) ?
                                Ts[i].getNode2(roots[i]):
                                Ts[i].getNode1(roots[i]);
            if (node_i) {
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
        return ;

    if (orResult = 0b00) {
        uint64_t below = prefix;
        uint64_t range = ((uint64_t)1 << (maxLevel - currLevel))- 1;
        uint64_t above = prefix | range;
        // std::generate_n(std::back_inserter(r), above-below+1,[&]()mutable{return r.size();});
        return;
    }

    bool existLeft, existRight;
    uint64_t nextLevel = currLevel + 1;
    if(andResult == 0b10) {
        uint64_t leftNodes[16];
        uint64_t leftResult = prefix;
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel - 1) 
                leftNodes[i] = Ts[i].getLeftChild(roots[i]);
        }
        // existLeft = 
        parallelAND(Ts, nTries, nThreads, threads, cpuset, maxLevel, nextLevel, levelOfCut, 
                    leftNodes, threadsRoots, tempActiveTries, threadsActiveTries,
                    leftResult, threadsPrefix,
                    threadsR);
    }
    else if (andResult == 0b01) {
        uint64_t rightNodes[16];
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel -1)
                rightNodes[i] = Ts[i].getRightChild(roots[i]);
        }
        // existRight = 
        parallelAND(Ts, nTries, nThreads, threads, cpuset, maxLevel, nextLevel, levelOfCut, 
                    rightNodes, threadsRoots, tempActiveTries, threadsActiveTries,
                    rightResult, threadsPrefix,
                    threadsR);
    }
    else if (andResult == 0b11) {
        uint64_t leftNodes[16];
        uint64_t rightNodes[16];
        uint64_t leftResult = prefix;
        uint64_t rightResult = (prefix | (1ULL << (maxLevel- currLevel - 1)));
        for (i = 0; i < nTries; ++i) {
            if (tempActiveTries[i] && currLevel != maxLevel - 1) {
                uint64_t leftNode = Ts[i].getLeftChild(roots[i]);
                leftNodes[i] = leftNode;
                rightNodes[i] = leftNode + 1;
            }
        }
        // existLeft = 
        parallelAND(Ts, nTries, nThreads, threads, cpuset, maxLevel, nextLevel, levelOfCut, 
                    leftNodes, threadsRoots, tempActiveTries, threadsActiveTries,
                    leftResult, threadsPrefix,
                    threadsR);
        // existRight = 
        parallelAND(Ts, nTries, nThreads, threads, cpuset, maxLevel, nextLevel, levelOfCut, 
                    rightNodes, threadsRoots, tempActiveTries, threadsActiveTries,
                    rightResult, threadsPrefix,
                    threadsR);
    }
    // if (existLeft || existRight)
    //     return true;
    // return false;
}


// template<class trieType>
// void join(vector<trieType> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel) {
//     r.reserve(10000000);
//     uint64_t prefix = 0;
//     uint64_t maxLevel = Ts[0].getHeight();
//     // max 16 relations
//     bool activeTries[16] = { true, true, true, true,
//                              true, true, true, true,
//                              true, true, true, true,
//                              true, true, true, true };
//     uint64_t roots[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
//                            0, 0, 0, 0, 0, 0, 0, 0 };
//     uint64_t currLevel = 0;
//     if (parallel) {
//         // Avaible threads in our system
//         unsigned nb_threads_hint = std::thread::hardware_concurrency();
//         // uint64_t levelOfCut =  floor(log2(nb_threads_hint));
//         uint64_t nThreads = 0;

//         uint64_t levelOfCut = determineLevelOfCut(Ts, maxLevel, nb_threads_hint);
//         // cout << "levelOfCut: " << levelOfCut << endl;
        
//         // Threads var
//         vector<std::thread> threads(nb_threads_hint); 
//         vector<bool [16]> threadsActiveTries(nb_threads_hint);
//         vector<uint64_t [16]> threadsRoots(nb_threads_hint);
//         vector<vector<uint64_t>> threadsR;
//         vector<uint64_t> threadsPrefix(nb_threads_hint);
//         for (uint64_t t = 0; t < nb_threads_hint; ++t) {
//             // bool rts[16];
//             // bool actvTries[16];
//             vector<uint64_t> tr;
//             tr.reserve(100000);
//             // threadsRoots.push_back(rts);
//             // threadsActiveTries.push_back(actvTries);
//             threadsR.push_back(tr);
//         }
        
//         cpu_set_t cpuset;
//         CPU_ZERO(&cpuset);
//         // std::mutex sharedMutex;
//         // Waiting the threads ending
//         parallelAND<trieType>(Ts, Ts.size(), nThreads, threads, cpuset, maxLevel, currLevel, levelOfCut, 
//                             roots, threadsRoots,
//                             activeTries, threadsActiveTries,
//                             prefix, threadsPrefix, 
//                             threadsR);
//         for (int i = 0; i < nThreads; ++i) {
//             threads[i].join();
//         }        

//     } else {
//         bool e = AND2(Ts, Ts.size(), maxLevel, currLevel, roots, activeTries, prefix, r);
//         // seqAND(Ts, maxLevel);
//     }
// }
// template 
// void join<flatBinTrie<rank_support_v5<1>>>(vector<flatBinTrie<rank_support_v5<1>>> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel);
// template 
// void join<flatBinTrie<rank_support_v<1>>>(vector<flatBinTrie<rank_support_v<1>>> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel);
// template
// void join<binTrie_il<512>>(vector<binTrie_il<512>> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel);
// template
// void join<binTrie_il<256>>(vector<binTrie_il<256>> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel);
// template
// void join<binTrie_il<128>>(vector<binTrie_il<128>> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel);
// template
// void join<binTrie_il<64>>(vector<binTrie_il<64>> &Ts, bool runs_encoded, vector<uint64_t> &r, bool parallel);



template <class trieType>
void partialAND(vector<trieType> &Ts, uint16_t n_tries, uint64_t max_level, uint64_t curr_level, 
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
                uint64_t node_i = (curr_level == max_level - 1) ?
                                    Ts[i].getNode2(roots[i]):
                                    Ts[i].getNode1(roots[i]);
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
            uint64_t node_i = (curr_level == max_level - 1) ?
                                    Ts[i].getNode2(roots[i]):
                                    Ts[i].getNode1(roots[i]);
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
                left_nodes[i] = Ts[i].getLeftChild(roots[i]);
            else 
                left_nodes[i] = Ts[i].getLeftChild(roots[i]);
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
                right_nodes[i] = Ts[i].getRightChild(roots[i]);
            else
                right_nodes[i] = Ts[i].getRightChild(roots[i]);
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
                uint64_t left_node = Ts[i].getLeftChild(roots[i]);
                left_nodes[i]  = left_node;
                right_nodes[i] = left_node + 1; 
            }
            else {
                uint64_t left_node = Ts[i].getLeftChild(roots[i]);
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


template <class trieType>
void Intersect(vector<trieType> &Bs, vector<uint64_t> &result, bool runs){
    
    uint64_t max_level = Bs[0].getHeight();
    uint64_t n_tries = Bs.size();
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
    uint64_t init_of_level     = pow(2, level_of_cut) - 1;
    
    vector<bool*>       activeTries2;
    vector<uint64_t*>   roots2;
    vector<uint64_t>    partial_solutions;
    
    result.reserve(1000000);
    
    uint64_t partial_int = 0;
    // Resolve problem until level of cut
    partialAND(Bs, n_tries, max_level, curr_level, level_of_cut, roots, activeTries, partial_int, result,
                partial_solutions, roots2, activeTries2, runs);
    // cout << "ok first partial and" << endl;
    uint16_t real_threads = roots2.size();
    uint16_t init_threads = real_threads;
    vector<uint64_t> init_level(real_threads, level_of_cut);
    uint64_t i = 0;
    while (nb_threads - real_threads > 1 && i < init_threads) {
        partialAND( Bs, n_tries, max_level, level_of_cut, level_of_cut + 1,
                    roots2[i], activeTries2[i], partial_solutions[i], result,
                    partial_solutions, roots2, activeTries2, runs
                    );
        uint64_t dif = roots2.size() - real_threads;
        for (uint64_t j = 0; j < dif; ++j) {
            init_level.push_back(level_of_cut + 1);
        }
         ++i;
        real_threads = roots2.size() - i ;
    }
    // cout << "ok partial and" << endl;
    vector<vector<uint64_t>> threads_results;//(real_threads);
    for (uint64_t t = 0; t < real_threads; ++t){
        vector<uint64_t> r;
        // r.reserve(100000);
        threads_results.push_back(r);
        threads_results[t].reserve(1000000);
    }
    if (runs){
        parallel_for(real_threads, real_threads, [&](int start, int end) {
            for (uint16_t threadId = start; threadId < end; ++threadId) {
                runsAND2(Bs, n_tries, max_level, init_level[threadId+i], 
                    roots2[threadId+i], activeTries2[threadId+i],
                    partial_solutions[threadId+i], threads_results[threadId]);
            }
        });
    }
    else {
        parallel_for(real_threads, real_threads, [&](int start, int end) {
            for (uint16_t threadId = start; threadId < end; ++threadId) {
                AND2(Bs, n_tries, max_level, init_level[threadId+i], 
                    roots2[threadId+i], partial_solutions[threadId+i], 
                    threads_results[threadId]);
            }
        });

    }
        
    uint64_t output_size = result.size(); 
    // cout << result.size() << endl;
    vector<uint64_t> shifts(real_threads);
    uint64_t shift = result.size();
    for(uint64_t t = 0; t < real_threads; ++t){
        output_size += threads_results[t].size();
        shifts[t] = shift;
        shift += threads_results[t].size();
    }
    // Write in parallel threads result
    if (output_size > 450000000){
        result.resize(output_size);
        parallel_for(real_threads, real_threads, [&](int start, int end) {
            for (uint16_t threadId = start; threadId < end; ++threadId) {
                for (uint64_t j = 0; j < threads_results[threadId].size(); ++j) {
                    result[j+shifts[threadId]] = threads_results[threadId][j];
                } 
            }        
        });
    } 
    else {
        // Concatenate solutions of threads
        for(uint64_t t=0; t < real_threads; ++t){
            result.insert(  result.end(), 
                            threads_results[t].begin(),
                            threads_results[t].end()
                        );
        }

    }
    
    // Free memory
    for (uint64_t i = 0; i < real_threads; ++i) {
        // if (runs) 
        delete[] activeTries2[i];
        delete[] roots2[i];
    }
    
    return;
}
template void 
Intersect<flatBinTrie<rank_support_v5<1>>>(vector<flatBinTrie<rank_support_v5<1>>> &Bs, vector<uint64_t> &result, bool runs);
template void 
Intersect<flatBinTrie<rank_support_v<1>>>(vector<flatBinTrie<rank_support_v<1>>> &Bs, vector<uint64_t> &result, bool runs);
template void 
Intersect<binTrie_il<512>>(vector<binTrie_il<512>> &Bs, vector<uint64_t> &result, bool runs);
template void
Intersect<binTrie_il<256>>(vector<binTrie_il<256>> &Bs, vector<uint64_t> &result, bool runs);
template void
Intersect<binTrie_il<128>>(vector<binTrie_il<128>> &Bs, vector<uint64_t> &result, bool runs);
template void 
Intersect<binTrie_il<64>>(vector<binTrie_il<64>> &Bs, vector<uint64_t> &result, bool runs);