#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <binaryTrie.hpp>
#include <binTrie.hpp>
#include <flatBinTrie.hpp>
#include <binTrie_il.hpp>
#include <flatBinTrie_il.hpp>
#include <sdsl/rank_support_v.hpp>
#include <sdsl/rank_support_v5.hpp>
#include "intersection.hpp"

using namespace std;
using namespace sdsl;

int rankType = 0;
bool runs = false;
bool levelwise = false;
uint32_t block_size = 512; //Only needed on binTrie_il


vector<vector<uint64_t>> loadQueryLog(string queryPath){
    vector<vector<uint64_t>> queries;
    ifstream in(queryPath);
    if (!in.is_open()){
        cerr << "Can't open file: " << queryPath << "\n";
        return queries;
    }
    string line;
    while(getline(in, line)) {
        vector<uint64_t> query;
        istringstream iss(line);
        for (string s; iss >> s;) {
            uint64_t id = (uint64_t)stoi(s);
            query.push_back(id);
        }
        queries.push_back(query);
    }
    in.close();
    return queries;
}

map<uint64_t, binaryTrie*> loadTries(ifstream &in, vector<vector<uint64_t>> &queries){
    vector<uint64_t> setIndexes;
    for(auto q: queries)
        setIndexes.insert(setIndexes.end(), q.begin(), q.end());
    sort(setIndexes.begin(), setIndexes.end());
    setIndexes.erase(unique(setIndexes.begin(), setIndexes.end()), setIndexes.end());

    map<uint64_t, binaryTrie*> tries; 
    uint64_t nIl = 0;
    uint32_t _1, u, n;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    in.read(reinterpret_cast<char*>(&_1), sizeof(_1));
    in.read(reinterpret_cast<char*>(&u), sizeof(u));
    std::cout << "Num. of sets: " << n << std::endl;
    std::cout << "Universe: "<< u << std::endl;
    uint32_t np = 0;
    for(uint32_t i = 0; i < n; ++i) {
        binaryTrie* trie;
        if (in.eof()) return tries;
        if (rankType == 0){
            if (levelwise)
                trie = new binTrie<sdsl::rank_support_v<1>>();
            else 
                trie = new flatBinTrie<rank_support_v<1>>();
        }
        else if(rankType == 1){
            if(block_size == 128){
                if (levelwise)
                    trie = new binTrie_il<128>;
                else 
                    trie = new flatBinTrie_il<128>;
            }
            if (block_size == 256){
                if (levelwise)
                    trie = new binTrie_il<256>;
                else 
                    trie = new flatBinTrie_il<256>;
            }
            if (block_size == 512){
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
        if (i == setIndexes[np]){
            tries.insert({i, trie});
            np++;
            if (np == setIndexes.size()) break;
        }
    }
    return tries;
}


void performIntersections( std::ifstream &in_sequences, std::string query_path,
                         bool runs_encoded) {
    
    vector<vector<uint64_t>> queries;
    map<uint64_t, binaryTrie*> tries;

    queries = loadQueryLog(query_path);
    cout << "Queries loaded succefully, Total: " << queries.size() << "" << endl;
    tries = loadTries(in_sequences, queries);
    cout << "Sequences loaded succefully, Total: " << tries.size() << endl;

    cout << "Computing queries...\n";
    uint64_t nq = 0;
    long unsigned int total_time = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto q: queries) {
        vector<binaryTrie*> QTries;
        vector<vector<uint64_t>> Qsets;
        for (auto i: q){
            vector<uint64_t> decodedTrie;
            tries[i]->decode(decodedTrie);
            Qsets.push_back(decodedTrie);
            QTries.push_back(tries[i]);    
        }
        vector<uint64_t> intersection;
        vector<uint64_t> FBintersection;
        if (QTries.size() <= 16){
            intersection = Intersect(QTries, runs_encoded, true);
            FBintersection = forceBruteIntersection(Qsets);
            // Check equality of intersection (forcebrute and trie-intersection)
            if (intersection != FBintersection){
                cout << "Trie intersection size: " << intersection.size() << "\n";
                cout << "Set intersection size: " << FBintersection.size() << "\n";
                uint64_t c = 0;
                for (uint64_t i = 0; i<intersection.size(); ++i){
                    if (intersection[i] != FBintersection[i]){
                        cout << "i: " << i << " " << intersection[i] << " " << FBintersection[i] << "\n";
                        c++;
                        if (c == 10) break;
                    }
                }
                cout << "Query " << nq << " not match with force brute intersection" << endl;
                break;
            }
            ++nq;
            if (nq % 1000 == 0) {
                std::cout << nq << " correct queries processed" << std::endl;
            }
        }
    }
    for (auto T: tries)
        delete T.second;

    std::cout << "Number of queries: " << nq << std::endl;
}


int main(int argc, char const *argv[]) {
    int mandatory = 3;
    // (*) mandatory parameters
    if (argc < mandatory){
        std::cout   << "collection filename " // (*)
                        "query log" // (*)
                    << "\n";
        return 1;
    }

    uint64_t min_size;
    std::string sequences_filename = std::string(argv[1]);
    std::string querylog_filename   = std::string(argv[2]);

    ifstream in_sequences;
    in_sequences.open(sequences_filename, std::ios::binary | std::ios::in);

    in_sequences.read(reinterpret_cast<char *>(&rankType) ,sizeof(rankType));
    if (rankType == 1) 
        in_sequences.read(reinterpret_cast<char *>(&block_size), sizeof(block_size));
    in_sequences.read(reinterpret_cast<char *>(&runs), sizeof(runs));
    in_sequences.read(reinterpret_cast<char *>(&levelwise), sizeof(levelwise));

    std::cout << "Type of trie\n";
    std::cout << "* Rank: ";
    if (rankType == 0) std::cout << "rank v" << std::endl;
    else if (rankType == 1) {
        std::cout << "rank il" << std::endl;
        std::cout << "** Block size: " << block_size << std::endl;
    }
    else std::cout << "rank v5" << std::endl;
    std::cout << "* Runs:" << (runs == 1 ? "true" : "false") << std::endl;
    std::cout << "* Level-wise: " << (levelwise == 1 ? "true" : "false") << std::endl;

    performIntersections(in_sequences, querylog_filename, runs);
        
    return 0;
}
