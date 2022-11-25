#include <iostream>
#include <fstream>
#include <vector>
#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>

using namespace std;
using namespace sdsl;

template<class bv>
bv* cbvCoding(std::vector<uint64_t> &v, uint32_t u){
    cout << "Creating bit vector" << endl;
    // sdsl::bit_vector* b =  new sdsl::bit_vector(u+1, 0);
    sdsl::bit_vector b =  sdsl::bit_vector(u+1, 0);

    cout << "Ok bit vector" << endl;
    // std::vector<uint64_t>::iterator it;
    uint64_t i = 0;
    for (; i<v.size(); ++i) {
        uint64_t index = v[i];
        b[index] = 1;
    }
    // for (it = v.begin(); it != v.end(); it++) {
    //     (*b)[(*it)] = 1;
    //     i++;
    //     // cout << "i: " << i << endl; 
    // }
    // cout<< "Creating compressed bv" << endl;
    // bv* cbv = new bv(*b);
    bv* cbv = new bv(b);
    // delete b;
    // cout << "Ok bv" << endl;
    return cbv;
}
template sdsl::rrr_vector<63>* cbvCoding<sdsl::rrr_vector<63>>  (std::vector<uint64_t> &v, uint32_t u);
template sdsl::sd_vector<>*    cbvCoding<sdsl::sd_vector<>>     (std::vector<uint64_t> &v, uint32_t u);
template sdsl::hyb_vector<>*   cbvCoding<sdsl::hyb_vector<>>    (std::vector<uint64_t> &v, uint32_t u);



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

    // uint64_t gap = 0, rle = 0, trie = 0, trie_run = 0; 
    uint64_t rrr = 0, sd = 0, hyb = 0;
    uint64_t nElements = 0, nSets = 0;
    uint64_t n_il = 0;
    uint32_t n, x;
    uint32_t u, _1;
    in.read(reinterpret_cast<char*>(&_1), 4);
    in.read(reinterpret_cast<char*>(&u), 4);
    cout << "Universe: " << u << endl;
    cout << "_1: " << _1 << endl;
    while (true) {
        if (in.eof()) {
            break;
        }
        in.read(reinterpret_cast<char*>(&n), 4);
        // cout << "n: " << n << endl;
        if (n > min_length){
            nElements += n;
            nSets++;
            std::vector<uint64_t> s;
            s.reserve(n);
            for (uint64_t i = 0; i < n; ++i) {
                in.read(reinterpret_cast<char*>(&x), 4);
                s.push_back((uint64_t)x);
            }
            // cout << "max_value: " << s[s.size()-1] << endl;
            uint64_t max_value = s[s.size()-1];
            // cout << "End load vector" <<endl;
            // Encode sets
            // sdsl::enc_vector<sdsl::coder::elias_delta>* ed;
            // sdsl::enc_vector<sdsl::coder::elias_gamma>* eg;
            // sdsl::enc_vector<sdsl::coder::fibonacci>* fib;
            
            // ed  = classicEncode<sdsl::coder::elias_delta>(s);
            // eg  = classicEncode<sdsl::coder::elias_gamma>(s);
            // fib = classicEncode<sdsl::coder::fibonacci>(s);
            sdsl::bit_vector bv (max_value+1, 0);
            for (uint64_t i = 0; i < s.size(); ++i){
                bv[s[i]] = 1;
            }

            // sdsl::rrr_vector<63>* rrrb;
            // sdsl::sd_vector<>*    sdb;
            // sdsl::hyb_vector<>*   hybb;

            // rrrb    = cbvCoding<sdsl::rrr_vector<63>>(s, u);
            // cout << "Ok rrrb" << endl;
            // sdb     = cbvCoding<sdsl::sd_vector<>>(s, u);
            // hybb    = cbvCoding<sdsl::hyb_vector<>>(s, u);
            // sdsl::rrr_vector<255> rrrb(bv);
            // sdsl::sd_vector<> sdb(bv);
            sdsl::hyb_vector<512> hybb(bv);

            // cout << "OK encoding" << endl;

            // Size in bytes
            // rrr     += sdsl::size_in_bytes(rrrb);
            // sd      += sdsl::size_in_bytes(sdb);
            hyb     += sdsl::size_in_bytes(hybb);
            n_il++;
            // cout << "OK FIRST IL" << endl;
            // delete rrrb;
            // delete sdb;
            // delete hybb;
            
        } else {
            break;
            in.seekg(4*n, std::ios::cur);
        }
        if (n_il %1000 == 0)
            std::cout << n_il << " sets processed" << std::endl;
    }
    in.close();
    std::cout << "#Ints: " << nElements << std::endl;
    std::cout << "#Sets: " << nSets << std::endl;
    std::cout << "Avg Raman Raman y Rao: " << (double)rrr*8/nElements << " bpi" << std::endl;
    std::cout << "Avg Sadakane: " << (double)sd*8/nElements << " bpi" << std::endl;
    std::cout << "Avg Hybrid bit vector: " << (double)hyb*8/nElements << " bpi" << std::endl;
    // std::cout << "Trie run: " << (double)trie_run/nElements << "bpi" << std::endl;
    return 0;
}