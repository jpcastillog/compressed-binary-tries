#include <iostream>
#include <fstream>
#include "../src/flatBinTrie.hpp"
#include <sdsl/bit_vectors.hpp>
#include "../src/binTrie_il.hpp"

using namespace std;
using namespace sdsl;

vector<uint64_t>* read_inv_list(std::ifstream &input_stream, uint32_t n) {

    vector <uint64_t>* il = new vector<uint64_t>();
    il -> reserve(n);
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t x;
        input_stream.read(reinterpret_cast<char *>(&x), 4);
        il -> push_back((uint64_t)x);
    }
    return il;
}

template <uint32_t block_size>
void buildCollection(std::string input_path, std::string out_path,
                     uint64_t min_size, int rank_type, bool runs) {

    if (min_size == 0) 
        min_size = 0xffffffffffffffff;
    
    std::ifstream input_stream;
    input_stream.open(input_path, std::ios::binary | std::ios::in);
    if (!input_stream.is_open()) {
        cout << "Can't open file:  " << input_path << endl;
        return;
    }
    std::ofstream out(out_path, ios::out | ios::binary);

    uint32_t _1, u;
    // Read universe of collection
    input_stream.read(reinterpret_cast<char *>(&_1), sizeof(_1));
    input_stream.read(reinterpret_cast<char *>(&u), sizeof(u));
    
    // Write universe in out file
    if (out_path != "") {
        out.write(reinterpret_cast<char *> (&_1), sizeof(_1));
        out.write(reinterpret_cast<char *> (&u), sizeof(u));
    }
    cout << "Universe: " << u << endl;

    uint64_t total_size = 0;
    uint64_t total_elements = 0;
    uint64_t n_il = 0;
    
    // for (uint32_t i = 0; i < u; ++i) {
    while (true) {
        
        uint32_t n;
        input_stream.read(reinterpret_cast<char *>(&n), 4);

        if (input_stream.eof()){
            break;
        }
        
        uint64_t trie_bytes_size;
        if (n > min_size){

            vector <uint64_t> *il = read_inv_list(input_stream, n);
            uint64_t max_value = (*il)[n - 1];
            
            if (rank_type == 0) {
                // flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(*il, max_value);
                flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(*il, u);
                if (runs)
                    trie_v.encodeRuns();
                if (out_path != "") {
                    trie_v.serialize(out);
                }
                trie_bytes_size = trie_v.size_in_bytes();
                trie_v.free();
            }

            else if (rank_type == 1) {
                binTrie_il<block_size> trie_il(*il, u);
                cout << "Ok create tries" << endl;
                if (runs)
                    cout << "Init encodeRuns" << endl;
                    trie_il.encodeRuns();
                    cout << "OK encode runs" << endl;
                if (out_path != "") {
                    trie_il.serialize(out);
                }
                trie_bytes_size = trie_il.size_in_bytes();
                trie_il.free();
            }

            else {
                flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(*il, u);
                if (runs)
                    trie_v5.encodeRuns();
                if (out_path != "") {
                    trie_v5.serialize(out);
                }
                trie_bytes_size = trie_v5.size_in_bytes();
                trie_v5.free();
            }
            total_size += trie_bytes_size;
            total_elements += n;
            n_il++;

            // cout << "#Elements: " << n << " | Bpi: " << (float)(trie_bytes_size*8)/n << endl;
            if ((n_il % 1000) == 0) {
                cout << n_il  <<" lists processed " << endl;
            }

            delete il;
        }

        else {
            input_stream.seekg(4*n, ios::cur);
        }
        
    }
    input_stream.close();
    out.close();

    cout << "Total inverted lists: " << n_il << "| Bpi: " << (float)(total_size*8)/total_elements << endl;
    cout << "Total ints: " << total_elements << endl;
    
    return;
}


int main(int argc, char** argv) {
    int mandatory = 3;

    if (argc < mandatory){
        std::cout   << "collection filename"
                        "[--min_size m]"
                        "[--rank v]"
                        "[--runs r]"
                        "[--out output_filename]"
                    <<
        std::endl;
        return 1;
    }

    int rank;
    uint64_t min_size = 0;
    uint32_t block_size = 512;
    bool runs;
    std::string output_filename = "";
    std::string input_filename = std::string(argv[1]);
    
    for (int i = 2; i < argc; ++i){
        if (std::string(argv[i]) == "--min_size") {
            ++i;
            min_size = std::stoull(argv[i]);
        }
        if (std::string(argv[i]) == "--rank") {
            ++i;
            if (std::string(argv[i]) == "v") {
                rank = 0;
            }
            if (std::string(argv[i]) == "il") {
                rank = 1;
                i++;
                block_size = std::atoi(argv[i]);
            }
            else {
                rank = 2;
            }
        }
        if (std::string(argv[i]) == "--runs") {
            ++i;
            if (std::string(argv[i]) == "t")
                runs = true;
            else
                runs = false;
        }
        if (std::string(argv[i]) == "--out") {
            ++i;
            output_filename = std::string(argv[i]);
        }
    }
    
    std::cout << "Min size: " << min_size << std::endl;
    std::cout << "Rank: " << rank << std::endl;
    std::cout << "Runs: " << runs << std::endl;
    std::cout << output_filename << endl;
    
    // Call function here
    if (block_size == 512)
        buildCollection<512>(input_filename, output_filename, min_size, rank, runs);
    else if (block_size == 256)
        buildCollection<256>(input_filename, output_filename, min_size, rank, runs);
    else if (block_size == 128)
        buildCollection<128>(input_filename, output_filename, min_size, rank, runs);
    else
        buildCollection<64> (input_filename, output_filename, min_size, rank, runs);

}