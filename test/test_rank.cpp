#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sdsl/bit_vectors.hpp>

using namespace std;

vector<sdsl::bit_vector>* read_inverted_lists(uint32_t n, const std::string path){
    
    vector<sdsl::bit_vector>* bvs= new vector<sdsl::bit_vector>();
    
    std::ifstream in;
    in.open(path, std::ios::binary | std::ios::in);
    if (!in.is_open()){
        cout << "Can't open file " << path << endl;
        return bvs;
    }

    uint32_t _1, u;
    // Read universe
    in.read(reinterpret_cast<char*>(&_1), 4);
    in.read(reinterpret_cast<char*>(&u), 4);
    cout << "universe: "<< u << endl;

    for (uint32_t i = 0; i < n; ++i) {
        uint32_t bytes_to_skip = 2*sizeof(uint16_t) + 2*sizeof(bool);
        in.seekg(bytes_to_skip, std::ios::cur);
        sdsl::bit_vector bv;
        sdsl::bit_vector _bv;
        sdsl::rank_support_v<1> rnk_s;

        bv.load(in);
        _bv.load(in);
        rnk_s.load(in, &bv);

        bvs -> push_back(bv);
    }
    return bvs;
}


vector<vector<uint64_t>>* generateRandomRanks(vector<sdsl::bit_vector> &bvs){
    vector<vector<uint64_t>>* positions_to_rank = new vector<vector<uint64_t>>();
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (uint32_t i = 0; i < bvs.size(); ++i) {
        uint64_t size_bv = bvs[i].size();
        uint64_t n_ranks = size_bv/50;
        vector<uint64_t> ranks;
        ranks.reserve(n_ranks);
        for (uint64_t j = 0; j < n_ranks; ++j) {
            uint64_t pos_to_rank = std::rand()%size_bv;
            ranks.push_back(pos_to_rank);
        }
        positions_to_rank -> push_back(ranks);
    }
    return positions_to_rank;
}


template <uint32_t block_size=512>
float testRankOp(vector<sdsl::bit_vector> &bvs, vector<vector<uint64_t>> &positions_to_rank, 
                 int rankType){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    uint64_t total_ranks = 0;
    uint64_t total_ranks_r = 0;
    uint64_t total_time = 0;
    for (uint32_t i = 0; i < bvs.size(); ++i) {
        uint64_t size_bv = bvs[i].size();
        uint64_t n_ranks = positions_to_rank[i].size();
        total_ranks += n_ranks;
        // Rank v
        if (rankType == 0){
            sdsl::rank_support_v<1>rank_v(&bvs[i]);
            auto start = std::chrono::high_resolution_clock::now();
            for (uint32_t j = 0; j < n_ranks; ++j) {
                int r = rank_v(positions_to_rank[i][j]);
                total_ranks_r += r;
                // total_time += time;
                // total_ranks_r += r;
            }
            auto end     = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            auto time    = elapsed.count();
            total_time   += time; 
        }
        // Rank v5
        else if (rankType == 1) {
            sdsl::rank_support_v5<1>rank_v5(&bvs[i]);
            auto start = std::chrono::high_resolution_clock::now();
            for (uint32_t j = 0; j < n_ranks; ++j) {
                int r = rank_v5(positions_to_rank[i][j]);
                total_ranks_r += r;
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            auto time = elapsed.count();
            total_time += time;
        }
        // Rank il
        else {
            sdsl::bit_vector_il<block_size> bv_il(bvs[i]);
            sdsl::rank_support_il<1, block_size> rank_il(&bv_il);
            uint64_t pos_to_rank = std::rand()%size_bv;
            auto start = std::chrono::high_resolution_clock::now();
            for (uint32_t j = 0; j < n_ranks; ++j) {
                int r = rank_il(positions_to_rank[i][j]);
                total_ranks_r += r;
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            auto time = elapsed.count();
            total_time += time;
        }        
    }
    cout << "Total ranks: " << total_ranks_r << endl;
    return (float) total_time/total_ranks;
}


int main(int argc, const char** argv) {
    // std::string path = "/mnt/d/data/Gov2Flat/gov2_rank_v_runs_t.bin";
    std::string path = "/media/jpcastillog/Nuevo vol/data/Gov2Flat/gov2_rank_v_runs_t.bin";
    vector<sdsl::bit_vector>* bvs;
    bvs = read_inverted_lists(10000, path);
    vector<vector<uint64_t>>* positions_to_rank = generateRandomRanks(*bvs);
    // cout << "ok"<< endl;
    float time1 = testRankOp<512>(*bvs, *positions_to_rank, 0);
    // cout << "ok1"<< endl; 
    float time2 = testRankOp<512>(*bvs, *positions_to_rank, 1);
    // cout << "ok2"<< endl; 
    float time3 = testRankOp<512>(*bvs, *positions_to_rank, 2);
    float time4 = testRankOp<256>(*bvs, *positions_to_rank, 2);
    float time5 = testRankOp<128>(*bvs, *positions_to_rank, 2);
    float time6 = testRankOp<64>(*bvs, *positions_to_rank, 2);
    cout << "Tiempo rank v: " << time1 << endl;
    cout << "Tiempo rank v5: " << time2 << endl;
    cout << "Tiempo rank il block size 512: " << time3 << endl;
    cout << "Tiempo rank il block size 256: " << time4 << endl;
    cout << "Tiempo rank il block size 128: " << time5 << endl;
    cout << "Tiempo rank il block size 64: " << time6 << endl;
    delete bvs;
    delete positions_to_rank;
    return 0;
}