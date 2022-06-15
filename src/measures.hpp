#include <iostream>
#include <cmath>
#include "flatBinTrie.hpp"
#include <sdsl/bit_vectors.hpp>

using namespace std;

class Gap{
    private:
        double GAP;
        uint64_t total_elements;
        uint64_t total_sets;
    public:
        Gap(std::string collection);
}

class RLE{
    private:
        double RLE;
        uint64_t total_elements;
        uint64_t total_sets;
    public:
        RLE(std::string collection);
}

class Trie{
    private:
        uint64_t total_bits;
        uint64_t total_elements;
        uint64_t total_sets;
    public:
        Trie(std::string collection);
}

class TrieRun{
    private:
        uint64_t total_bits;
        uint64_t total_elements;
        uint64_t total_sets;
    public:
        TrieRun(std::string collection);
}

Gap::Gap(std::string collection, uint32_t min_lenght = 4096){
    Gap::GAP = 0.0; //Init gap measure in 0;
    Gap::total_elements = 0;
    Gap::total_sets = 0;

    std::ifstream in;
    in.open(collection, std::ios::binary || std::ios::in);
    assert(in.is_open());

    uint32_t _1, u;
    in.read(reinterpret_cast<char *>(&_1), 4);
    in.read(reinterpret_cast<char *>(&u), 4);

    while(true){
        uint32_t;
        in.read(reinterpret_cast<char*>(&n), 4);
        if (input_stream.eof()) {
            break;
        }
        
        if (n > min_length) {
            uint32_t x_i1;
            uint32_t x_i;
            uint32_t g_i;

            Gap::total_elements += n;
            Gap::total_sets++;

            in.read(reinterpret_cast<char*>(&x_i1), 4);
            for(uint32_t i = 1; i < n; ++i) {\
                in.read(reinterpret_cast<char*>(&x_i), 4);
                g_i = x_i - x_i1 - 1;
                if (g_i == 0) g_i = 1; 
                Gap::GAP += floor(g_i) + 1;

            }
        }
        else {
            in.seekg(4*n, ios::cur);
        }

    }
}

RLE::RLE(std::string collection, uint32_t min_length) {
    RLE::RLE = 0.0; //Init gap measure in 0;
    RLE::total_elements = 0;
    RLE::total_sets = 0;

    std::ifstream in;
    in.open(collection, std::ios::binary || std::ios::in);
    assert(in.is_open());

    uint32_t _1, u;
    in.read(reinterpret_cast<char *>(&_1), 4);
    in.read(reinterpret_cast<char *>(&u), 4);

    while(true){
        uint32_t n;
        in.read(reinterpret_cast<char*>(&n), 4);
        if (input_stream.eof()) {
            break;
        }
        
        if (n > min_length) {
            // Characteristic bit vector (cbv)
            sdsl::bit_vector cbv(0, u+1);
            uint32_t x;
            uint32_t x_i;
            // uint32_t g_i;

            RLE::total_elements += n;
            RLE::total_sets++;

            in.read(reinterpret_cast<char*>(&x_i1), 4);
            for(uint32_t i = 1; i < n; ++i) {
                in.read(reinterpret_cast<char*>(&x), 4);
                cbv[x] = 1;
            } // End create cbv  
            
            uint32_t runLength = 1;
            in.read(reinterpret_cast<char*>(&x), 4);
            for(uint32_t i = 1; i < n; ++i) {
                in.read(reinterpret_cast<char*>(&x_i), 4);
                if (x_i != x) {
                    uint32_t sub = runLength -1;
                    if (sub == 0) 
                        sub = 1;
                    RLE::RLE  += floor(sub) + 1;
                    runLength = 1;
                }
                else {
                    ++runLength;
                }
                x = x_i;
            }

        }
        else {
            in.seekg(4*n, ios::cur);
        }
    }
}
