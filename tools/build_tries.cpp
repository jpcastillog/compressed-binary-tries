#include <iostream>
#include <fstream>
#include <sdsl/bit_vectors.hpp>
#include "../include/flatBinTrie.hpp"
#include "../include/flatBinTrie_il.hpp"
#include "../include/binTrie_il.hpp"
#include "../include/binTrie.hpp"
#include "../include/binaryTrie.hpp"

using namespace std;
using namespace sdsl;

bool verbose = false;
uint32_t block = 512;


vector<uint64_t> read_inv_list(std::ifstream &input_stream, uint32_t n) {

    vector <uint64_t> il;
    il.reserve(n);
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t x;
        input_stream.read(reinterpret_cast<char *>(&x), 4);
        il.push_back((uint64_t)x);
    }
    return il;
}


template <uint32_t block_size>
void buildCollection(std::string input_path, std::string out_path,
                     uint64_t min_size, uint64_t max_size,
                     int rank_type, bool runs, bool levelwise) {
    
    std::ifstream input_stream;
    input_stream.open(input_path, std::ios::binary | std::ios::in);
    if (!input_stream.is_open()) {
        cout << "Can't open input file:  " << input_path << endl;
        return;
    }
    std::ofstream out;
    if (out_path != ""){
        out.open(out_path, ios::out | ios::binary);
        if(!out.is_open()) {
            cout << "Can't open  output file:  " << out_path << endl;
            return;
        }
    }

    uint32_t _1, u;
    // Read universe of collection
    input_stream.read(reinterpret_cast<char *>(&_1), sizeof(_1));
    input_stream.read(reinterpret_cast<char *>(&u), sizeof(u));

    uint32_t nSets = 0;
    while (true) {
        uint32_t n;
        input_stream.read(reinterpret_cast<char *>(&n), 4);
        if (input_stream.eof()){
            break;
        }
        if (n > min_size && n <= max_size) {
            nSets++;
        }
        input_stream.seekg(4*n, ios::cur);
    }
    // Set file pointer on the first set
    input_stream.clear();
    input_stream.seekg(2*sizeof(u), ios::beg);
    // Write universe in out file
    if (out_path != "") {
        out.write(reinterpret_cast<char *> (&rank_type), sizeof(rank_type));
        if (rank_type == 1) 
            out.write(reinterpret_cast<char *> (&block), sizeof(block));
        out.write(reinterpret_cast<char *> (&runs), sizeof(runs));
        out.write(reinterpret_cast<char *> (&levelwise), sizeof(levelwise));
        out.write(reinterpret_cast<char *> (&nSets), sizeof(nSets));
        out.write(reinterpret_cast<char *> (&_1), sizeof(_1));
        out.write(reinterpret_cast<char *> (&u), sizeof(u));
    }
    cout << "Universe: " << u << endl;

    uint64_t total_size = 0;
    uint64_t total_elements = 0;
    uint64_t n_il = 0;
    
    while (true) {
        
        uint32_t n;
        input_stream.read(reinterpret_cast<char *>(&n), 4);
        if (input_stream.eof()){
            break;
        }
        
        uint64_t trie_bytes_size;
        if (n > min_size && n <= max_size){

            vector <uint64_t> il = read_inv_list(input_stream, n);
            uint64_t max_value = il[n - 1];
            
            if (rank_type == 0) {
                if (levelwise){
                    binTrie<rank_support_v<1>> trie_v = binTrie<rank_support_v<1>>(il, u);
                    if (runs)
                        trie_v.encodeRuns();
                    if (out_path != "") {
                        trie_v.serialize(out);
                    }
                    trie_bytes_size = trie_v.size_in_bytes();
                }
                else {    
                    flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(il, u);
                    if (runs)
                        trie_v.encodeRuns();
                    if (out_path != "") {
                        trie_v.serialize(out);
                    }
                    trie_bytes_size = trie_v.size_in_bytes();
                }
            }

            else if (rank_type == 1) {
                if (levelwise) {
                    binTrie_il<block_size> trie_il(il, u);
                    if (runs) {
                        trie_il.encodeRuns();
                    }
                    if (out_path != "") {
                        trie_il.serialize(out);
                    }
                    trie_bytes_size = trie_il.size_in_bytes();
                }
                else {
                    flatBinTrie_il<block_size> trie_il(il, u);
                    if (runs) {
                        trie_il.encodeRuns();
                    }
                    if (out_path != "") {
                        trie_il.serialize(out);
                    }
                    trie_bytes_size = trie_il.size_in_bytes();
                }

            }

            else {
                if (levelwise){
                    binTrie<rank_support_v5<1>> trie_v5 = binTrie<rank_support_v5<1>>(il, u);
                    if (runs)
                        trie_v5.encodeRuns();
                    if (out_path != "") {
                        trie_v5.serialize(out);
                    }
                    trie_bytes_size = trie_v5.size_in_bytes();
                    // vector<uint64_t> decoded;
                    // trie_v5.decode(decoded);
                }
                else {
                    flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(il, u);
                    if (runs)
                        trie_v5.encodeRuns();
                    if (out_path != "") {
                        trie_v5.serialize(out);
                    }
                    trie_bytes_size = trie_v5.size_in_bytes();
                }
            }
            total_size += trie_bytes_size;
            total_elements += n;
            n_il++;

            // cout << "#Elements: " << n << " | Bpi: " << (float)(trie_bytes_size*8)/n << endl;
            if ((n_il % 1000) == 0 && verbose) {
                cout << n_il  <<" Sets processed " << endl;
            }
        }
        else {
            input_stream.seekg(4*n, ios::cur);
        }
        
    }
    input_stream.close();
    out.close();

    cout << "Total inverted lists: " << n_il << "| Bpi: " << (float)(total_size*8)/total_elements << endl;
    cout << "Total ints: " << total_elements << endl;
    cout << "----------------------------------------------------" << endl;
    
    return;
}


int main(int argc, char** argv) {
    int mandatory = 3;

    if (argc < mandatory){
        std::cout   << "collection filename (*)"
                        "[--min_size m] "
                        "[--max_size m] "
                        "[--rank v] (*)"
                        "[--runs r] (*)"
                        "[--out output_filename]"
                    <<
        std::endl;
    }

    int rank = 0;
    uint64_t min_size = 0;
    uint64_t max_size = -1;
    bool runs = false;
    bool levelwise = false;
    std::string output_filename = "";
    std::string input_filename = std::string(argv[1]);
    std::cout << input_filename << endl;
    
    for (int i = 2; i < argc; ++i){
        if (std::string(argv[i]) == "--min_size") {
            ++i;
            min_size = std::stoull(argv[i]);
        }
        if (std::string(argv[i]) == "--max_size") {
            ++i;
            max_size = std::stoull(argv[i]);
        }
        if (std::string(argv[i]) == "--rank") {
            ++i;
            if (std::string(argv[i]) == "v") {
                rank = 0;
            }
            else if (std::string(argv[i]) == "il") {
                rank = 1;
                i++;
                block = std::atoi(argv[i]);
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
        if (std::string(argv[i]) == "--levelwise") {
            ++i;
            if (std::string(argv[i]) == "t")
                levelwise = true;
            else
                levelwise = false;
        }
        if (std::string(argv[i]) == "--verbose") {
            ++i;
            if (std::string(argv[i]) == "t")
                verbose = true;
            else
                verbose = false;
        }
    }
    
    std::cout << "Min size: " << min_size << std::endl;
    std::cout << "Max size: ";
    (max_size == (uint64_t)(-1) ? (std::cout << "inf") : (std::cout << max_size));
    std::cout << endl;
    std::cout << "Type of trie\n";
    std::cout << "* Rank: ";
    if (rank  == 0){
        std::cout << "rank v" << std::endl;
    }
    else if (rank == 1) {
        std::cout << "rank il" << std::endl;
        std::cout << "* Block-size: " << block << std::endl;
    }
    else {
        std::cout << "rank v5" << std::endl;
    }
    std::cout << "* Runs: " << (runs == 1 ? "true" : "false") << std::endl;
    std::cout << "* Level-wise: " << (levelwise == 1 ? "true" : "false") << std::endl;
    std::cout << "Output file name: "<< output_filename << endl;
    
    if (block == 512)
        buildCollection<512>(input_filename, output_filename, min_size, max_size, rank, runs, levelwise);
    else if (block == 256)
        buildCollection<256>(input_filename, output_filename, min_size, max_size, rank, runs, levelwise);
    else if (block == 128)
        buildCollection<128>(input_filename, output_filename, min_size, max_size, rank, runs, levelwise);
    else
        buildCollection<64> (input_filename, output_filename, min_size, max_size, rank, runs, levelwise);

}