#include <iostream>
#include <fstream>
#include "../src/flatBinTrie.hpp"
#include <sdsl/bit_vectors.hpp>

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

    uint32_t a, u;
    // Read universe of collection
    input_stream.read(reinterpret_cast<char *>(&a), sizeof(a));
    input_stream.read(reinterpret_cast<char *>(&u), sizeof(u));

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
                flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(*il, max_value);
                if (runs)
                    trie_v.encodeRuns();
                
                trie_bytes_size = trie_v.size_in_bytes();
                trie_v.free();
            }

            else {
                flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(*il, max_value);
                if (runs)
                    trie_v5.encodeRuns();
                // Write trie in file here!!
                //
                trie_bytes_size = trie_v5.size_in_bytes();
                trie_v5.free();
            }
            total_size += trie_bytes_size;
            total_elements += n;
            n_il++;

            // cout << "#Elements: " << n << " | Bpi: " << (float)(trie_bytes_size*8)/n << endl;
            
            delete il;
        }

        else {
            input_stream.seekg(4*n, ios::cur);
        }
        
    }
    input_stream.close();

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
    uint64_t min_size;
    bool runs;
    std::string output_filename = "";
    std::string input_filename = std::string(argv[1]);
    cout << argv[0] << endl;
    for (int i = 1; i < argc; ++i){
        if (std::string(argv[i]) == "--min_size") {
            ++i;
            min_size = std::atoi(argv[i]);
        }
        if (std::string(argv[i]) == "--rank") {
            ++i;
            if (std::string(argv[i]) == "v") {
                rank = 0;
            }
            else {
                rank = 1;
            }
        }
        if (std::string(argv[i]) == "--runs") {
            ++i;
            if (std::string(argv[i]) == "t")
                runs = true;
            else
                runs = false;
        }
        if (std::string(argv[i]) == "--output_filename") {
            ++i;
            output_filename = std::string(argv[i]);
        }
    }
    
    // std::cout << "Min size: " << min_size << std::endl;
    // std::cout << "Rank: " << rank << std::endl;
    // std::cout << "Runs: " << runs << std::endl;
    
    // Call function here
    buildCollection(input_filename, output_filename, min_size, rank, runs);

}