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

using namespace std;
using namespace sdsl;

template <class trieType>
vector<trieType>* loadSequences(std::ifstream &in){

    vector<trieType>* sequences = new vector<trieType>();
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
        trieType b = trieType();
        b.load(in);
        sequences -> push_back(b);
        count++;
    }
    return sequences;
}
template 
vector<binTrie<sdsl::rank_support_v<1>>>* loadSequences<binTrie<sdsl::rank_support_v<1>>>(ifstream &in);
template 
vector<flatBinTrie<sdsl::rank_support_v<1>>>* loadSequences<flatBinTrie<sdsl::rank_support_v<1>>>(ifstream &in);
template 
vector<binTrie<sdsl::rank_support_v5<1>>>* loadSequences<binTrie<sdsl::rank_support_v5<1>>>(ifstream &in);
template 
vector<flatBinTrie<sdsl::rank_support_v5<1>>>* loadSequences<flatBinTrie<sdsl::rank_support_v5<1>>>(ifstream &in);
template 
vector<binTrie_il<512>>* loadSequences<binTrie_il<512>>(ifstream &in);
template 
vector<flatBinTrie_il<512>>* loadSequences<flatBinTrie_il<512>>(ifstream &in);
template 
vector< binTrie_il<256>>* loadSequences<binTrie_il<256>>(ifstream &in);
template 
vector< flatBinTrie_il<256>>* loadSequences<flatBinTrie_il<256>>(ifstream &in);
template 
vector<binTrie_il<128>>* loadSequences<binTrie_il<128>>(ifstream &in);
template 
vector<flatBinTrie_il<128>>* loadSequences<flatBinTrie_il<128>>(ifstream &in);
template 
vector<binTrie_il<64>>* loadSequences<binTrie_il<64>>(ifstream &in);
template 
vector<flatBinTrie_il<64>>* loadSequences<flatBinTrie_il<64>>(ifstream &in);


vector<vector<uint32_t>>* loadQueryLog(std::string path) {
    vector<vector<uint32_t>>* queries = new vector<vector<uint32_t>>();
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
        queries->push_back(query);
    }
    
    return queries;
}


template <class trieType>
void performIntersections( std::ifstream &in_sequences, std::string query_path,
                           std::string out_path, bool runs_encoded
                           ) {
    uint64_t trep = 10;
    std::ofstream out;
    if (out_path != "") {
        out.open(out_path, std::ios::out);
        out << "elements_per_query,time execution,size_intersection" << std::endl; 
    }

    vector<vector<uint32_t>>* queries;
    vector<trieType>* sequences;

    sequences = loadSequences<trieType>(in_sequences);
    cout << "Sequences loaded succefully" << endl;
    queries   = loadQueryLog(query_path);
    cout << "Queries loaded succefully" << endl;

    uint64_t n_ranks = 0;

    uint64_t nq = 0;
    long unsigned int total_time = 0;
    uint64_t total_height = 0;
    uint64_t time_of_ranks = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < queries -> size(); ++i) {
        vector<trieType> Bs((*queries)[i].size());
        // Bs.reserve(16);
        for(int j = 0; j < (*queries)[i].size(); ++j) {
            // Bs.push_back((*sequences)[(*queries)[i][j]]);
            Bs[j] = (*sequences)[(*queries)[i][j]];
        }
        vector<uint64_t> intersection;
        if (Bs.size() <= 16){
            uint64_t time_10 = 0;
            for(int rep = 0; rep < trep; ++rep) {
                // cout << "before intersection" << endl;
                auto start = std::chrono::high_resolution_clock::now();
                Intersect(Bs, intersection, runs_encoded);
                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                // cout << "ok intersection" << endl;
                auto time = elapsed.count();
                total_time += time;
                time_10 += time;
                if (rep != trep-1)
                    intersection.clear();        
            }
            if (out.is_open()) {
                out << Bs.size() << "," << (float)time_10/trep<< "," << intersection.size() << std::endl;
            }
            // std::cout << nq <<"|Time execution: " << (double)(time_10*1e-3)/(trep) << "[ms] " << intersection.size() << std::endl;
            ++nq;
            if (nq % 1000 == 0) {
                std::cout << nq << " queries processed" << std::endl;
            }
        }
    }
    out.close();

    delete sequences;
    delete queries;

    std::cout << "Number of queries: " << nq << std::endl;
    std::cout <<"Avg time execution: " << (double)(total_time*1e-3)/(nq*trep) << "[ms]" << std::endl;
}
template
void performIntersections<binTrie<sdsl::rank_support_v<1>>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template
void performIntersections<flatBinTrie<sdsl::rank_support_v<1>>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template
void performIntersections<binTrie<sdsl::rank_support_v5<1>>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template
void performIntersections<flatBinTrie<sdsl::rank_support_v5<1>>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<binTrie_il<512>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<flatBinTrie_il<512>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<binTrie_il<256>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<flatBinTrie_il<256>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<binTrie_il<128>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<flatBinTrie_il<128>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<binTrie_il<64>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);
template 
void performIntersections<flatBinTrie_il<64>>(ifstream &in_sequences, std::string query_path, std::string out_path,bool runs_encoded);


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

    int rank;
    uint64_t min_size;
    bool runs, levelwise;
    uint32_t block_size = 512;
    std::string sequences_filename = std::string(argv[1]);
    std::string querylog_filename   = std::string(argv[2]);
    std::string output_filename = "";

    ifstream in_sequences;
    in_sequences.open(sequences_filename, std::ios::binary | std::ios::in);
    
    for (int i = 1; i < argc; ++i){
        // if (std::string(argv[i]) == "--rank") {
        //     ++i;
        //     if (std::string(argv[i]) == "v") {
        //         rank = 0;
        //     }
        //     else if (std::string(argv[i]) == "il") {
        //         rank = 1;
        //         ++i;
        //         block_size = std::atoi(argv[i]);
        //     }
        //     else {
        //         rank = 2;
        //     }
        // }
        // if (std::string(argv[i]) == "--runs") {
        //     ++i;
        //     if (std::string(argv[i]) == "t")
        //         runs = true;
        //     else
        //         runs = false;
        // }
        if (std::string(argv[i]) == "--out") {
            ++i;
            output_filename = std::string(argv[i]);
        }
    }

    in_sequences.read(reinterpret_cast<char *>(&rank) ,sizeof(rank));
    if (rank == 1) 
        in_sequences.read(reinterpret_cast<char *>(&block_size), sizeof(block_size));
    in_sequences.read(reinterpret_cast<char *>(&runs), sizeof(runs));
    in_sequences.read(reinterpret_cast<char *>(&levelwise), sizeof(levelwise));

    std::cout << "Rank: ";
    if (rank == 0) std::cout << "rank v" << std::endl;
    else if (rank == 1) {
        std::cout << "rank il" << std::endl;
        std::cout << "Block size: " << block_size << std::endl;
    }
    else std::cout << "rank v5" << std::endl;
    std::cout << "Runs:" << (runs == 1 ? "true" : "false") << std::endl;
    std::cout << "Level-wise: " << (levelwise == 1 ? "true" : "false") << std::endl;
    std::cout << "outfilename: " << output_filename << endl;


    if (rank == 0) {
        if (levelwise)
            performIntersections<binTrie<sdsl::rank_support_v<1>>>(in_sequences, querylog_filename, output_filename, runs);
        else
            performIntersections<flatBinTrie<sdsl::rank_support_v<1>>>(in_sequences, querylog_filename, output_filename, runs);
    }
    else if (rank == 1) {
        // For some reason block size = 64 dont works
        // if (rank == 64){
        //     performIntersections<flatBinTrie<sdsl::rank_support_v<1>>>(in_sequences, querylog_filename, output_filename, runs);
        //     performIntersections<binTrie_il<64>>(in_sequences, querylog_filename, output_filename, runs);
        // } 
        if (block_size == 128){
            if (levelwise)
                performIntersections<binTrie_il<128>>(in_sequences, querylog_filename, output_filename, runs);
            else
                performIntersections<flatBinTrie_il<128>>(in_sequences, querylog_filename, output_filename, runs);
        }
        else if (block_size == 256){
            if (levelwise)
                performIntersections<binTrie_il<256>>(in_sequences, querylog_filename, output_filename, runs);
            else
                performIntersections<flatBinTrie_il<256>>(in_sequences, querylog_filename, output_filename, runs);
        }
        else{ 
            if (levelwise)
                performIntersections<binTrie_il<512>>(in_sequences, querylog_filename, output_filename, runs);
            else 
                performIntersections<flatBinTrie_il<512>>(in_sequences, querylog_filename, output_filename, runs);
        }
    }
    // rank == 2 rank_v5
    else {
        if (levelwise)
            performIntersections<binTrie<sdsl::rank_support_v5<1>>>(in_sequences, querylog_filename, output_filename, runs);
        else
            performIntersections<flatBinTrie<sdsl::rank_support_v5<1>>>(in_sequences, querylog_filename, output_filename, runs);
    }
    
    return 0;
}
