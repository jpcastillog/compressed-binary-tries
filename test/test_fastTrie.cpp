#include <iostream>
#include<vector>
#include <fstream>
#include <string>
#include "fastBinaryTrie.hpp"

using namespace std;


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


template<class wordType>
void verifyEncode(std::string input_path, uint32_t min_size, bool rank_type, bool runs) {
    std::ifstream input_stream;
    input_stream.open(input_path, std::ios::binary | std::ios::in);
    if (!input_stream.is_open()) {
        cout << "Can't open input file:  " << input_path << endl;
        return;
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
        if (n > min_size) {
            nSets++;
        }
        input_stream.seekg(4*n, ios::cur);
    }
    // Set file pointer on the first set
    input_stream.clear();
    input_stream.seekg(2*sizeof(u), ios::beg);

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
        if (n > min_size){
            vector <uint64_t> il = read_inv_list(input_stream, n);
            if (rank_type == 0) {
                // cout << "rank v " << runs << " " << sizeof(wordType) << "\n ";
                fastBinaryTrie<rank_support_v<1>, wordType> trie = fastBinaryTrie<rank_support_v<1>, wordType>(il, u);
                if (runs){
                    // cout << "encoding runs\n";
                    trie.encodeRuns();
                }
                trie_bytes_size = trie.size_in_bytes();
                vector<uint64_t> decoded;
                trie.decode(decoded);
                if (decoded != il) {
                    cout << "set " << n_il << " is not correct encoded\n";
                    cout << il.size() << ", " << decoded.size() << "\n";
                    for (int i = 0; i < 10; ++i ){
                        cout << il[i] << " ";
                    }
                    cout << "\n";
                    for (int i = 0; i < 10; ++i ){
                        cout << decoded[i] << " ";
                    }
                    cout << "\n";
                }
            }
            else {
                // cout << "rank v5\n";
                fastBinaryTrie<rank_support_v5<1>, wordType> trie(il, u);
                if (runs)
                    trie.encodeRuns();
                trie_bytes_size = trie.size_in_bytes();
                vector<uint64_t> decoded;
                trie.decode(decoded);
                if (decoded != il) {
                    cout << "set " << n_il << "is not correct encoded\n";
                }
            }
            total_size += trie_bytes_size;
            total_elements += n;
            n_il++;

            // cout << "#Elements: " << n << " | Bpi: " << (float)(trie_bytes_size*8)/n << endl;
            if ((n_il % 1000) == 0) {
                cout << n_il  <<" correct sets decoded " << endl;
            }
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
    int mandatory = 2;

    if (argc < mandatory){
        std::cout   << "collection filename "
                        "[--min_size m] "
                    <<
        std::endl;
        return 1;
    }

    int rank = 0;
    uint16_t wsize = 64;
    uint64_t min_size = 0;
    uint32_t block_size = 512;
    bool runs = false;
    std::string output_filename = "";
    std::string input_filename = std::string(argv[1]);
    std::cout << input_filename << endl;
    
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
            else if (std::string(argv[i]) == "il") {
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
        if (std::string(argv[i]) == "--wsize") {
            ++i;
            wsize= stoi(argv[i]);

        }
    }
    
    std::cout << "Min size: " << min_size << std::endl;
    std::cout << "Word size: " << wsize << std:: endl;
    std::cout << "Rank: ";
    if (rank  == 0){
        std::cout << "rank v" << std::endl;
    }
    else if (rank == 1) {
        std::cout << "rank il" << std::endl;
    }
    else {
        std::cout << "rank v5" << std::endl;
    }
    std::cout << "Runs: " << (runs == 1 ? "true" : "false") << std::endl;
    std::cout << "Output file name: "<< output_filename << endl;
    
    if (wsize == 64)
        verifyEncode<uint64_t>(input_filename, min_size, rank, runs);
    else if (wsize == 32)
        verifyEncode<uint32_t>(input_filename, min_size, rank, runs);
    else if (wsize == 16)
        verifyEncode<uint16_t>(input_filename, min_size, rank, runs);
    else
        verifyEncode<uint8_t> (input_filename, min_size, rank, runs);
    
    return 0;
}