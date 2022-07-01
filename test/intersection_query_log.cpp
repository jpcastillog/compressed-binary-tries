#include <iostream>
#include <vector>
#include <fstream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/rank_support_v.hpp>
#include <sdsl/rank_support_v5.hpp>
#include "../src/intersection.hpp"
#include "../src/flatBinTrie.hpp"
#include "../src/binTrie_il.hpp"

using namespace std;
using namespace sdsl;

template <class trieType>
vector<trieType>* loadSequences(std::string input_path, uint64_t n){

    vector<trieType>* sequences = new vector<trieType>();
    // sequences -> reserve (1200000);
    std::ifstream in;
    in.open(input_path, std::ios::binary | std::ios::in);
    if (!in.is_open()) {
        cout << "Cant't open file: " << input_path << endl;
        return sequences;
    }

    uint32_t _1, u;
    // Read universe
    in.read(reinterpret_cast<char*>(&_1), sizeof(_1));
    in.read(reinterpret_cast<char*>(&u), sizeof(u));
    cout << "universe: "<< u << endl;
    
    uint64_t count = 0;
    while (true && count < n) {
    // while (true) {
        if (in.eof()){
            break;
        }
        trieType b = trieType();
        // cout << "Init load" << endl;
        b.load(in);
        // cout << "Finish load " << count << endl;
        sequences -> push_back(b);
        count++;
    }
    cout << "OK load sequences" << endl;
    return sequences;
}
template 
vector<flatBinTrie<sdsl::rank_support_v<1>>>* loadSequences<flatBinTrie<sdsl::rank_support_v<1>>>(std::string input_path, uint64_t n);
template 
vector<flatBinTrie<sdsl::rank_support_v5<1>>>* loadSequences<flatBinTrie<sdsl::rank_support_v5<1>>>(std::string input_path, uint64_t n);
template 
vector<binTrie_il<512>>* loadSequences<binTrie_il<512>>(std::string input_path, uint64_t n);
template 
vector< binTrie_il<256>>* loadSequences<binTrie_il<256>>(std::string input_path, uint64_t n);
template 
vector<binTrie_il<128>>* loadSequences<binTrie_il<128>>(std::string input_path, uint64_t n);
template 
vector<binTrie_il<64>>* loadSequences<binTrie_il<64>>(std::string input_path, uint64_t n);


// template <uint32_t block_size>
// vector<binTrie_il<block_size>>* loadSequences_il(std::string input_path, uint64_t n){

//     vector<binTrie_il<block_size>>* sequences = new vector<binTrie_il<block_size>>();
//     std::ifstream in;
//     in.open(input_path, std::ios::binary | std::ios::in);
//     if (!in.is_open()) {
//         cout << "Cant't open file: " << input_path << endl;
//         return sequences;
//     }

//     uint32_t _1, u;
//     // Read universe
//     in.read(reinterpret_cast<char*>(&_1), sizeof(_1));
//     in.read(reinterpret_cast<char*>(&u), sizeof(u));
//     cout << "universe: "<< u << endl;
    
//     uint64_t count = 0;
//     while (true && count < n) {
//         if (in.eof()){
//             break;
//         }
//         binTrie_il<block_size> b = binTrie_il<block_size>();
//         // cout << "Init load" << endl;
//         b.load(in);
//         // cout << "Finish load " << count << endl;
//         sequences -> push_back(b);
//         count++;
//     }
//     cout << "OK load sequences" << endl;
//     return sequences;
// }


vector<vector<uint32_t>>* loadQueryLog(std::string path) {
    vector<vector<uint32_t>>* queries = new vector<vector<uint32_t>>();
    std::ifstream in(path);
    // in.open(path, std::iostream || std::ios::in);
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
void performIntersections( std::string sequences_path, std::string query_path,
                           std::string out_path, bool runs_encoded, uint64_t n_sequences) {
    std::ofstream out;
    if (out_path != "") {
        out.open(out_path, std::ios::out);
        out << "elements_per_query,time execution,size_intersection" << std::endl; 
    }

    vector<vector<uint32_t>>* queries;
    vector<trieType>* sequences;

    sequences = loadSequences<trieType>(sequences_path, n_sequences);
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
    // for (int i = 0; i < 2000; ++i) {
        vector<trieType> Bs;
        Bs.reserve(16);
        // cout << "ids: " ;
        for(int j = 0; j < (*queries)[i].size(); ++j) {
            Bs.push_back((*sequences)[(*queries)[i][j]]);
            // cout << (*sequences)[(*queries)[i][j]].getHeight() << endl;
            // cout << (*queries)[i][j] << " ";
        }
        // cout << endl;
        // cout << "Query size: " << Bs.size() << endl;
        vector<uint64_t> intersection;
        if (Bs.size() <= 16){
            // cout << "i: " << nq << endl;
            // trieType* intersection;
            
            // intersection = joinTries<trieType>(Bs, runs_encoded, time_of_ranks);
            // intersection =
            uint64_t time_10 = 0;
            for(int rep = 0; rep < 10; ++rep) {
                auto start = std::chrono::high_resolution_clock::now();
                parJoin<trieType>(Bs, intersection);
                auto end = std::chrono::high_resolution_clock::now();
                // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                auto time = elapsed.count();
                total_time += time;
                time_10 += time;
                if (rep != 9)
                    intersection.clear();        
            }
            if (out.is_open()) {
                // out << Bs.size() << "," << (float)time_10*10e-6/10.0 << "," << intersection.size() << std::endl;
                out << Bs.size() << "," << (float)time_10/10.0 << "," << intersection.size() << std::endl;
            }
            // cout << nq <<"|Time execution: " << (double)(time_10)*10e-6/10.0 << "[ms]" << endl;
            cout << nq <<"|Time execution: " << (double)(time_10*1e-3)/(10.0) << "[ms]| " << intersection.size() << endl;

            // total_height += intersection -> getHeight();
            // cout << "Size of intersection: " << intersection -> elements_coded() << endl;
            // vector<uint64_t> decode_r;
            // intersection -> decode(decode_r);
            // cout << nq << " |Query size: "<< Bs.size() << " |Time execution: " << (float)time*10e-6 << "[ms]" 
                //  cout << "| intersection size: " << decode_r.size() << endl;
            
            // Write results in a out file
            // if (out.is_open()) {
            //     out << Bs.size() << "," << (float)time*10e-6 << "," << decode_r.size() << std::endl;
            // }
            // total_time += time;
            // intersection -> free();
            // delete intersection;
            ++nq;
        }
    }
    // auto end = std::chrono::high_resolution_clock::now();
    // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    // auto time = elapsed.count();
    out.close();

    delete sequences;
    delete queries;
    cout << "Number of queries: " << nq << endl;
    // cout <<"|Avg time execution: " << (double)(time)*10e-6/nq << "[ms]" << endl;
    // cout <<"|Avg time execution: " << (double)(total_time)*10e-6/(nq*10) << "[ms]" << endl;
    cout <<"|Avg time execution: " << (double)(total_time*1e-3)/(nq*10) << "[ms]" << endl;
    cout << total_height << endl;   
    cout << "Avg number of ranks: " << (float)n_ranks/nq << endl;
    cout << "Avg time of ranks: " << (double)(time_of_ranks*10e-6)/nq <<endl;
}
template
void performIntersections<flatBinTrie<sdsl::rank_support_v<1>>>(std::string sequences_path, std::string query_path, std::string out_path,bool runs_encoded, uint64_t n_sequences);
template
void performIntersections<flatBinTrie<sdsl::rank_support_v5<1>>>(std::string sequences_path, std::string query_path, std::string out_path,bool runs_encoded, uint64_t n_sequences);
template 
void performIntersections<binTrie_il<512>>(std::string sequences_path, std::string query_path, std::string out_path,bool runs_encoded, uint64_t n_sequences);
template 
void performIntersections<binTrie_il<256>>(std::string sequences_path, std::string query_path, std::string out_path,bool runs_encoded, uint64_t n_sequences);
template 
void performIntersections<binTrie_il<128>>(std::string sequences_path, std::string query_path, std::string out_path,bool runs_encoded, uint64_t n_sequences);
template 
void performIntersections<binTrie_il<64>>(std::string sequences_path, std::string query_path, std::string out_path,bool runs_encoded, uint64_t n_sequences);


int main(int argc, char const *argv[]) {
    // int mandatory = 5;
    int mandatory = 0;
    // (*) mandatory parameters
    if (argc < mandatory){
        std::cout   << "collection filename" // (*)
                        "query log"          // (*)
                        "[--runs r]"         // (*)
                        "[--rank v]"         // (*)
                        "[--out results_filename]"
                    <<
        std::endl;
        return 1;
    }

    int rank = 0;
    uint64_t min_size;
    bool runs = true;
    uint32_t block_size = 512;
    // uint64_t n_sequences = 0xffffffffffffffff;
    // uint64_t n_sequences = 1000000;
    uint64_t n_sequences = 1107204;
    std::string sequences_filename = std::string(argv[1]);
    std::string querylog_filename   = std::string(argv[2]);
    // std::string sequences_filename =  "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_il_512_runs_t.bin";
    // std::string sequences_filename =  "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_v_runs_t.bin";
    // std::string sequences_filename =  "/media/jpcastillog/Nuevo vol/data/ClueWeb09Flat/clueweb09_rank_v_runs_t.bin";
    // std::string sequences_filename =  "/home/jpcastillog/Escritorio/data/clueweb09_rank_v_runs_t_2.bin";
    // std::string sequences_filename =  "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_v5_runs_t.bin";
    // std::string querylog_filename   =  "/media/jpcastillog/Nuevo vol/data/Gov2Flat/1mq.txt";
    // std::string querylog_filename   =  "/media/jpcastillog/Nuevo vol/data/ClueWeb09Flat/1mq.txt";
    // std::string querylog_filename   =  "/media/jpcastillog/Nuevo vol/data/Gov2Flat/aol.txt";
    // std::string querylog_filename   = "./../../s_indexes/1000_pairwise_first_3000.txt";
    // std::string     querylog_filename   =  "/media/jpcastillog/Nuevo vol/data/Gov2Flat/random_pairwise_queries_1000.txt";
    std::string output_filename = "";
    for (int i = 1; i < argc; ++i){
        if (std::string(argv[i]) == "--rank") {
            ++i;
            if (std::string(argv[i]) == "v") {
                rank = 0;
            }
            else if (std::string(argv[i]) == "il") {
                rank = 1;
                ++i;
                block_size = std::atoi(argv[i]);
            }
            else {
                rank = 2;
            }
        }
        if (std::string(argv[i]) == "--#sequences"){
            ++i; 
            n_sequences = std::atoll(argv[i]);
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

    cout << "Rank: " << rank << endl;
    cout << "Runs:" << runs << endl;
    cout << "#sequences: " << n_sequences << endl;
    cout << "outfilename: " << output_filename << endl;


    if (rank == 0) {
        // vector<flatBinTrie<sdsl::rank_support_v<1>>>* sequences = loadSequences<flatBinTrie<sdsl::rank_support_v<1>>>(sequences_filename, n_sequences);
        // performIntersections<flatBinTrie<sdsl::rank_support_v<1>>>(sequences, querylog_filename, output_filename,runs);
        performIntersections<flatBinTrie<sdsl::rank_support_v<1>>>(sequences_filename, querylog_filename, output_filename, runs, n_sequences);
    }
    // 1107205
    else if (rank == 1) {
        if (rank == 64){
            // vector<binTrie_il<64>>* sequences = loadSequences_il<64>(sequences_filename, n_sequences);
            // performIntersections<binTrie_il<64>>(sequences, querylog_filename, output_filename, runs);
            performIntersections<flatBinTrie<sdsl::rank_support_v<1>>>(sequences_filename, querylog_filename, output_filename, runs, n_sequences);
        } 
        else if (block_size == 128){
            // vector<binTrie_il<128>>* sequences = loadSequences_il<128>(sequences_filename, n_sequences);
            // performIntersections<binTrie_il<128>>(sequences, querylog_filename, output_filename, runs); 
            performIntersections<binTrie_il<128>>(sequences_filename, querylog_filename, output_filename, runs, n_sequences);
            }
        else if (block_size == 256){
            // vector<binTrie_il<256>>* sequences = loadSequences_il<256>(sequences_filename, n_sequences);
            // performIntersections<binTrie_il<256>>(sequences, querylog_filename, output_filename, runs); 
            performIntersections<binTrie_il<256>>(sequences_filename, querylog_filename, output_filename, runs, n_sequences);
        }
        else{ 
            // vector<binTrie_il<512>>* sequences = loadSequences_il<512>(sequences_filename, n_sequences);
            // performIntersections<binTrie_il<512>>(sequences, querylog_filename, output_filename, runs); 
            performIntersections<binTrie_il<512>>(sequences_filename, querylog_filename, output_filename, runs, n_sequences);
        }
    }
    else {
        cout << "rank v5" << endl;
        // vector<flatBinTrie<sdsl::rank_support_v5<1>>>* sequences = loadSequences<flatBinTrie<sdsl::rank_support_v5<1>>>(sequences_filename, n_sequences);
        // performIntersections<flatBinTrie<sdsl::rank_support_v5<1>>>(sequences, querylog_filename, output_filename,runs);
        performIntersections<flatBinTrie<sdsl::rank_support_v5<1>>>(sequences_filename, querylog_filename, output_filename, runs, n_sequences);
    }
    
    return 0;
}
