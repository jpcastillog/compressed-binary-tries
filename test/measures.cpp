#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "../src/flatBinTrie.hpp"
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>

using namespace std;
using namespace sdsl;

// Compile 
// g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib measures.cpp -o measures.out -lsdsl -ldivsufsort -ldivsufsort64

uint64_t Gap(std::vector<uint64_t> &v) {
    uint64_t gap = 0;
    uint64_t x_i1, x_i, g_i;
    
    std::vector<uint64_t>::iterator it;
    x_i1 = v[0];
    // g1
    uint64_t g_1 = x_i1;
    if (g_1 == 0)
        g_1 = 1;
    gap += floor(log2(g_1)) + 1;
    // it = v.begin();
    // it++;
    // for (; it != v.end(); it++) {
    for (uint32_t i = 1; i < v.size(); i++){
        // x_i = *it;
        x_i = v[i];
        g_i = x_i - x_i1 - 1;
        if (x_i - x_i1 <=  0)
            cout << "Conjunto no ordenado" << endl;
        if (g_i == 0)
            g_i = 1;
        gap += floor(log2(g_i)) + 1;
        x_i1 = x_i;
    }
    return gap;
}

uint64_t Rle(std::vector<uint64_t> &v) {
    uint64_t rle = 0;
    // uint64_t u = *v.end();
    // uint64_t u = v[v.size()-1];
    // First create cbv
    // sdsl::bit_vector cbv(u+1, 0); //characteristic bit vector (cbv)
    std::vector<uint64_t>::iterator it;
    // for(it = v.begin()+1; it != v.end(); it++)
    //     cbv[*it] = 1;
    // End create cbv
    uint64_t x, x_i;
    uint64_t runLength = 1;
    it = v.begin();
    // x = *it;
    x = v[0];
    it++;
    for (; it != v.end(); it++){
        x_i = *it;
        uint64_t delta = x_i - x;
        uint64_t sub0, sub1;
        if (delta == 1){
            runLength++;
        } else {
            sub0 = (delta - 1) - 1;
            sub1 = runLength - 1;
            if (sub0 == 0) sub0 = 1;
            if (sub1 == 0) sub1 = 1;
            rle += floor(log2(sub0)) + 1;
            rle += floor(log2(sub1)) + 1;
            runLength = 1;
        }
        x = x_i;
    }
    // x = cbv[1];
    // for(uint64_t i = 1; i < cbv.size(); ++i) {
    //     x_i = cbv[i];
    //     if (x_i != x) {
    //         uint64_t sub = runLength - 1;
    //         if (sub == 0)
    //             sub = 1;
    //         rle += floor(log2(sub)) + 1;
    //         runLength = 1;
    //     }
    //     else {
    //         ++runLength;
    //     }
    //     x = x_i;
    // }

    return rle;
}

// if encodeRuns is true the measure is trie-run
uint64_t Trie(std::vector<uint64_t> &v, bool encodeRuns) {
    // uint64_t u = *v.end();
    uint64_t u = v[v.size() - 1];
    flatBinTrie<rank_support_v<1>> t = flatBinTrie<rank_support_v<1>>(v, u);
    if (encodeRuns)
        t.encodeRuns();
    uint64_t size = t.trieMeasure();
    t.free();
    return size;
}


int main(int argc, const char** argv) {
    std::string collection = "";
    uint64_t min_length = 0;
    int mandatory = 2;

    if (argc < mandatory){
        std::cout   << "collection filename"
                        "[--min_length l]"
                    <<
        std::endl;
        return 1;
    } 

    collection = std::string(argv[1]);
    for (int i = 0; i < argc; ++i) {
        if(std::string(argv[i]) == "--min_length"){
            ++i;
            min_length = std::stoull(argv[i]);
        }
    }

    std::ifstream in;
    in.open(collection, std::ios::binary | std::ios::in);
    if (!in.is_open()) {
        std::cout << "Can't open file " << collection << std::endl;
    }

    uint64_t gap = 0;
    uint64_t rle = 0, trie = 0, trie_run = 0; 
    uint64_t nElements = 0, nSets = 0;
    uint64_t n_il = 0;
    // uint32_t n, x;
    uint32_t u, _1;
    in.read(reinterpret_cast<char*>(&u), 4);
    in.read(reinterpret_cast<char*>(&_1), 4);
    while (true) {
        uint32_t n;
        in.read(reinterpret_cast<char*>(&n), 4);
        if (in.eof()) {
            break;
        }
        if (n > min_length){
            nElements += n;
            nSets++;
            std::vector<uint64_t> s;
            s.reserve(n);
            for (uint64_t i = 0; i < n; ++i) {
                uint32_t x;
                in.read(reinterpret_cast<char*>(&x), 4);
                s.push_back((uint64_t)x);
            }
            // std::cout << "Size of vector: " << s.size() << std::endl;
            // Calculate Measures
            gap += Gap(s);
            // rle += Rle(s);
            // trie += Trie(s, false);
            // trie_run += Trie(s, true);
            // delete s;
            n_il++;
            if (n_il %1000 == 0)
                std::cout << n_il << " sets processed" << std::endl;
            
        } 
        else {
            in.seekg(4*n, std::ios::cur);
        }
    }
    in.close();
    std::cout << "#Ints: " << nElements << std::endl;
    std::cout << "#Sets: " << nSets << std::endl;
    std::cout << "Avg Gap: " << (float)gap/nElements << " bpi" << std::endl;
    std::cout << "Avg RLE: " << (double)rle/nElements << " bpi" << std::endl;
    std::cout << "Avg Trie: " << (double)trie/nElements << " bpi" << std::endl;
    std::cout << "Trie run: " << (double)trie_run/nElements << " bpi" << std::endl;

    return 0;
}