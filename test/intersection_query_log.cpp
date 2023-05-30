#include <iostream>
#include <vector>
#include <fstream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/rank_support_v.hpp>
#include <sdsl/rank_support_v5.hpp>
// #include "../src/intersection.hpp"
#include "../src/binTrie.hpp"
#include "../src/flatBinTrie.hpp"
#include "../src/binTrie_il.hpp"
#include "../src/flatBinTrie_il.hpp"
#include "../src/intersection.hpp"
#include "../src/binaryTrie.hpp"

using namespace std;
using namespace sdsl;

int rankType = 0;
bool runs = false;
bool levelwise = false;
uint32_t block_size = 512; //Only needed on binTrie_il


vector<binaryTrie*> loadSequences(std::ifstream &in) {
    vector<binaryTrie*> sequences;
    if (!in.is_open()) {
        cout << "File is not open" << endl;
        return sequences;
    }

    uint32_t _1, u, n;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    in.read(reinterpret_cast<char*>(&_1), sizeof(_1));
    in.read(reinterpret_cast<char*>(&u), sizeof(u));
    std::cout << "Num. of sets: " << n << std::endl;
    std::cout << "Universe: "<< u << std::endl;
    
    uint64_t count = 0;
    while ( count < n ) {
        if (in.eof()){
            break;
        }
        binaryTrie* trie;
        if (rankType == 0){
            if (levelwise)
                trie = new binTrie<sdsl::rank_support_v<1>>();
            else 
                trie = new flatBinTrie<rank_support_v<1>>();
        }
        else if(rankType == 1){
            if(block_size = 128){
                if (levelwise)
                    trie = new binTrie_il<128>;
                else 
                    trie = new flatBinTrie_il<128>;
            }
            if (block_size = 256){
                if (levelwise)
                    trie = new binTrie_il<256>;
                else 
                    trie = new flatBinTrie_il<256>;
            }
            if (block_size = 512){
                if (levelwise)
                    trie = new binTrie_il<512>;
                else 
                    trie = new flatBinTrie_il<512>;
            }
        }
        else{
            if (levelwise)
                trie = new binTrie<rank_support_v5<1>>;
            else
                trie = new flatBinTrie<rank_support_v5<1>>;
        }
        trie -> load(in);
        sequences.push_back(trie);
        count++;
    }
    return sequences;
}


vector<vector<uint32_t>> loadQueryLog(std::string path) {
    vector<vector<uint32_t>> queries;
    std::ifstream in(path);
    if (!in.is_open()) {
        cout << "Cant't open file: " << path << endl;
        return queries;
    }
    std::string line;
    while(std::getline(in, line)) {
        std::vector<uint32_t> query;

        std::istringstream iss(line);
        for (std::string s; iss >> s;) {
            uint32_t id = (uint32_t)stoi(s);
            query.push_back(id);
        }
        queries.push_back(query);
    }
    in.close();
    return queries;
}


void performIntersections( std::ifstream &in_sequences, std::string query_path,
                           std::string out_path, bool runs_encoded) {
    uint64_t trep = 10;
    std::ofstream out;
    if (out_path != "") {
        out.open(out_path, std::ios::out);
        out << "elements_per_query,time execution,size_intersection" << std::endl; 
    }
    
    vector<vector<uint32_t>> queries;
    vector<binaryTrie*> sequences;

    sequences = loadSequences(in_sequences);
    cout << "Sequences loaded succefully, Total: " << sequences.size() << endl;
    queries   = loadQueryLog(query_path);
    cout << "Queries loaded succefully, Total: " << queries.size() << "" << endl;

    uint64_t n_ranks = 0;

    uint64_t nq = 0;
    long unsigned int total_time = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto q: queries) {
        vector<binaryTrie*> QTries;
        // for(uint32_t j = 0; j < q.size(); ++j)
        for (auto i: q)
            QTries.push_back(sequences[i]);
            // QTries.push_back(sequences[q[j]]);
        // cout << "Elements in Query: " << QTries.size() << "\n";
        vector<uint64_t> intersection;
        if (QTries.size() <= 16){
            uint64_t time_10 = 0;
            for(int rep = 0; rep < trep; ++rep) {
                // cout << "Begin intersection" << endl;
                auto start = std::chrono::high_resolution_clock::now();
                intersection = Intersect(QTries, runs_encoded);
                auto end = std::chrono::high_resolution_clock::now();
                // cout << "End intersection" << endl;
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                auto time = elapsed.count();
                total_time += time;
                time_10 += time;
                // if (rep != trep-1)
                //     intersection.clear();        
            }
            if (out.is_open()) {
                out << QTries.size() << "," << (float)time_10/trep<< "," << intersection.size() << std::endl;
            }
            // std::cout << nq <<"|Time execution: " << (double)(time_10*1e-3)/(trep) << "[ms] " << intersection.size() << std::endl;
            ++nq;
            if (nq % 1000 == 0) {
                std::cout << nq << " queries processed" << std::endl;
            }
        }
    }
    out.close();

    for (auto T: sequences)
        delete T;

    std::cout << "Number of queries: " << nq << std::endl;
    std::cout <<"Avg time execution: " << (double)(total_time*1e-3)/(nq*trep) << "[ms]" << std::endl;
}


int main(int argc, char const *argv[]) {
    int mandatory = 2;
    // (*) mandatory parameters
    if (argc < mandatory){
        std::cout   << "collection filename " // (*)
                        "query log "          // (*)
                        "[--runs r] "         // (*)
                        "[--rank v] "         // (*)
                        "[--out results_filename]"
                    <<
        std::endl;
        return 1;
    }

    uint64_t min_size;
    std::string sequences_filename = std::string(argv[1]);
    std::string querylog_filename   = std::string(argv[2]);
    std::string output_filename = "";

    ifstream in_sequences;
    in_sequences.open(sequences_filename, std::ios::binary | std::ios::in);
    
    for (int i = 1; i < argc; ++i){
        if (std::string(argv[i]) == "--out") {
            ++i;
            output_filename = std::string(argv[i]);
        }
    }

    in_sequences.read(reinterpret_cast<char *>(&rankType) ,sizeof(rankType));
    if (rankType == 1) 
        in_sequences.read(reinterpret_cast<char *>(&block_size), sizeof(block_size));
    in_sequences.read(reinterpret_cast<char *>(&runs), sizeof(runs));
    in_sequences.read(reinterpret_cast<char *>(&levelwise), sizeof(levelwise));

    std::cout << "Rank: ";
    if (rankType == 0) std::cout << "rank v" << std::endl;
    else if (rankType == 1) {
        std::cout << "rank il" << std::endl;
        std::cout << "Block size: " << block_size << std::endl;
    }
    else std::cout << "rank v5" << std::endl;
    std::cout << "Runs:" << (runs == 1 ? "true" : "false") << std::endl;
    std::cout << "Level-wise: " << (levelwise == 1 ? "true" : "false") << std::endl;
    std::cout << "outfilename: " << output_filename << endl;

    performIntersections(in_sequences, querylog_filename, output_filename, runs);
        
    return 0;
}
