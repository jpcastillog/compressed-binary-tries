#include <iostream>
#include <vector>
#include "../src/flatBinTrie.hpp"

using namespace std;
using namespace sdsl;

template <class rankType>
vector<flatBinTrie<rankType>>* loadIndex(std::string input_path){

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
    // vector<flatBinTrie<rankType>>* sequences = new vector<flatBinTrie<rankType>>();
    while (true) {
        
        if (in.eof()){
            break;
        }

        flatBinTrie <rankType>  b;
        b.load(in);
        sequences -> push_back(b);
    }

    return sequences;
}

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
        std::string line;

        std::istringstream iss(line);
        for (std::string s; iss >> s;) {
            cout << "entra al for " << endl;
            uint32_t id = (uint32_t)stoi(s);
            query.push_back(id);
            // cout << (uint32_t)id << " ";
            cout << s << " ";

        }
        cout << endl;
        queries->push_back(query);
        break;
    }
    return queries;
}


int main(int argc, char const *argv[]) {
    loadQueryLog("/mnt/d/Data/IntegerCompression2014.3april2014/Gov2Flat/aol.txt");
    return 0;
}
