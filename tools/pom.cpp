#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <sdsl/vectors.hpp>

using namespace std;

template<class uint_type>
uint_type prefixOf(uint_type x, uint_type y, uint16_t nBits) {
    uint_type bx, by;
    uint16_t shift = 0;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF; // 111111..1 in 64 bits
    for (uint16_t i = nBits; i > 0; --i) {
        bx = (x>>(i-1)) & 1;
        by = (y>>(i-1)) & 1;
        if (bx == by) { 
            shift++;
        }
        else {
            mask = mask>>shift;
            return x &= mask;
        }
    }
    return x;
}
//  SDSL Coders
// sdsl::coder::elias_delta
// sdsl::coder::elias_gamma
// sdsl::coder::fibonacci
template<class Coder>
sdsl::vlc_vector<Coder, 128, 0>* classicEncode(std::vector<uint64_t> &v) {
    sdsl::int_vector<> int_v(v.size());
    
    std::vector<uint64_t>::iterator it;
    uint64_t i = 0;
    for (it = v.begin(); it != v.end(); it++) {
        int_v[i] = (uint32_t)(*it);
        ++i;
    }
    // sdsl::util::bit_compress(int_v);
    sdsl::vlc_vector<Coder, 128,0>*ev;
    ev = new sdsl::vlc_vector<Coder, 128, 0>(int_v);
    return ev;
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

    uint64_t elias_delta = 0, elias_gamma = 0, fibonacci = 0;
    uint64_t nElements = 0, nSets = 0;
    uint64_t n_il = 0;
    uint32_t n, x;
    uint32_t u, _1;
    uint64_t size_in_bits = 0;
    in.read(reinterpret_cast<char*>(&u), 4);
    in.read(reinterpret_cast<char*>(&_1), 4);
    while (true) {
        if (in.eof()) {
            break;
        }
        in.read(reinterpret_cast<char*>(&n), 4);
        if (n > min_length){
            nElements += n;
            nSets++;
            std::vector<uint64_t> s;
            std::vector<uint64_t> sp;
            s.reserve(n);
            sp.reserve(n);
            for (uint64_t i = 0; i < n; ++i) {
                in.read(reinterpret_cast<char*>(&x), 4);
                s.push_back((uint64_t)x);
                if (i == 0) {
                    sp.push_back((uint64_t)x);
                    size_in_bits += (uint64_t)(2*floor(log2(x)) + 1);
                }
                else {
                    uint64_t xp = prefixOf<uint64_t>((uint64_t)x, s[i-1], 64);
                    sp.push_back(xp);
                    size_in_bits += (uint64_t)(2*floor(log2(xp)) + 1);
                }
            }
            sort(sp.begin(), sp.end());
            // Encode sets
            sdsl::vlc_vector<sdsl::coder::elias_delta, 128, 0>* ed;
            sdsl::vlc_vector<sdsl::coder::elias_gamma, 128, 0>* eg;
            sdsl::vlc_vector<sdsl::coder::fibonacci, 128, 0>* fib;
            
            ed  = classicEncode<sdsl::coder::elias_delta>(sp);
            eg  = classicEncode<sdsl::coder::elias_gamma>(sp);
            fib = classicEncode<sdsl::coder::fibonacci>(sp);
            // Size in bytes
            elias_delta += sdsl::size_in_bytes(*ed);
            elias_gamma += sdsl::size_in_bytes(*eg);
            fibonacci   += sdsl::size_in_bytes(*fib);
            n_il++;

            delete ed;
            delete eg;
            delete fib;
            
        } else {
            in.seekg(4*n, std::ios::cur);
        }
        if (n_il %1000 == 0)
            std::cout << n_il << " sets processed" << std::endl;
    }
    in.close();
    std::cout << "#Ints: " << nElements << std::endl;
    std::cout << "#Sets: " << nSets << std::endl;
    std::cout << "Avg Elias Delta: " << (double)(elias_delta*8)/nElements << " bpi" << std::endl;
    std::cout << "Avg Elias Gamma: " << (double)(elias_gamma*8)/nElements << " bpi" << std::endl;
    std::cout << "Avg Fibonacci: " << (double)(fibonacci*8)/nElements << " bpi" << std::endl;
    std::cout << "Avg Elias Gamma Theory: " << (double)size_in_bits/nElements << " bpi" << std::endl;
    return 0;
}