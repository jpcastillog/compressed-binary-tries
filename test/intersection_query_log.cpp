#include <iostream>
#include <vector>
#include "../src/flatBinTrie.hpp"
#include "../src/intersection.hpp"

using namespace std;
using namespace sdsl;

template <class rankType>
vector<flatBinTrie<rankType>>* loadSequences(std::string input_path){

    vector<flatBinTrie<rankType>>* sequences = new vector<flatBinTrie<rankType>>();
    std::ifstream in;
    in.open(input_path, std::ios::binary | std::ios::in);
    if (!in.is_open()) {
        cout << "Cant't open file: " << input_path << endl;
        return sequences;
    }

    uint32_t _1, u;
    // Read universe
    in.read(reinterpret_cast<char*>(&_1), 4);
    in.read(reinterpret_cast<char*>(&u), 4);
    cout << "universe: "<< u << endl;
    // vector<flatBinTrie<rankType>>* sequences = new vector<flatBinTrie<rankType>>();
    while (true) {
        if (in.eof()){
            break;
        }
        flatBinTrie <rankType> b;
        b.load(in);
        sequences -> push_back(b);
    }

    return sequences;
}
template vector<flatBinTrie<sdsl::rank_support_v<1>>>* loadSequences<sdsl::rank_support_v<1>>(std::string input_path);
template vector<flatBinTrie<sdsl::rank_support_v5<1>>>* loadSequences<sdsl::rank_support_v5<1>>(std::string input_path);



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
            // cout << (uint32_t)id << " ";
            // cout << id << " ";

        }
        // cout << endl;
        queries->push_back(query);
        // break;
    }
    // cout << "# queries: " << queries->size() << endl;
    return queries;
}

template <class rankType>
void performIntersections( std::string sequences_path, 
                           std::string query_path, bool runs_encoded) {

    vector<vector<uint32_t>>* queries;
    vector<flatBinTrie<rankType>>* sequences;

    sequences = loadSequences<rankType>(sequences_path);
    cout << "Sequences loaded succefully" << endl;
    queries   = loadQueryLog(query_path);
    cout << "Queries loaded succefully" << endl;

    uint64_t nq = 0;
    auto total_time = 0;
    for (int i = 0; i < queries -> size(); ++i) {
        vector<flatBinTrie<rankType>> Bs;
        for(int j = 0; j < (*queries)[i].size(); ++j) {
            Bs.push_back((*sequences)[(*queries)[i][j]]);
        }
        if (Bs.size() <= 16){
            cout << Bs[0].getHeight() << " " << Bs[1].getHeight() << endl;
            flatBinTrie<rankType>* intersection;
            cout << "Query size: " << Bs.size() << endl;
            uint64_t time2;
            auto start = std::chrono::high_resolution_clock::now();
            intersection = joinTries<rankType>(Bs, runs_encoded, time2);
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            // auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            auto time = elapsed.count();
            total_time += time;
            vector<uint64_t> decode_r;
            intersection -> decode(decode_r);
            // cout << nq << " |Time execution: " << (float)time*10e-6 << "[ms]" << endl;
            cout << nq << " |Time execution: " << time*10e-6 << "[ms] |" << decode_r.size()<< endl;
            
            intersection -> free();
            delete intersection;
            ++nq;
        }
    }
    delete sequences;
    delete queries;

    cout <<"|Avg time execution: " << (float)(total_time*10e-6)/nq << "[ms]" << endl;
    // cout <<"|Avg time execution: " << (float)(total_time)/nq << "[ms]" << endl;
}
template void performIntersections<sdsl::rank_support_v<1>> ( std::string sequences_path, std::string query_path, bool runs_encoded);
template void performIntersections<sdsl::rank_support_v5<1>> ( std::string sequences_path, std::string query_path, bool runs_encoded);


int main(int argc, char const *argv[]) {
    vector<vector<uint32_t>>* queries;
    // queries = loadQueryLog("/mnt/d/Data/IntegerCompression2014.3april2014/Gov2Flat/aol.txt");

    int mandatory = 3;

    if (argc < mandatory){
        std::cout   << "collection filename"
                        "query log"
                        "[--runs r]"
                        "[--rank v]"
                    <<
        std::endl;
        return 1;
    }

    int rank;
    uint64_t min_size;
    bool runs;
    std::string sequences_filename = std::string(argv[1]);
    std::string querylog_filename   = std::string(argv[2]);
    std::string output_filename = "";
    for (int i = 1; i < argc; ++i){
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
        if (std::string(argv[i]) == "--out") {
            ++i;
            output_filename = std::string(argv[i]);
        }
    }
    cout << "rank: " << rank << endl;
    cout << "runs: " << runs << endl;

    if (rank == 0) {
        performIntersections<sdsl::rank_support_v<1>>(sequences_filename, querylog_filename, runs);
    }
    else {
        performIntersections<sdsl::rank_support_v5<1>>(sequences_filename, querylog_filename, runs);
    }
    
    return 0;
}
