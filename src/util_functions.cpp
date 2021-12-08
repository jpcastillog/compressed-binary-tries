#include <iostream>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <math.h>
#include <fstream>
#include <queue>


bool compareVectors(vector<uint64_t> &v1, vector<uint64_t> &v2) {
    uint64_t size1 = v1.size();
    uint64_t size2 = v2.size();
    if (size1 != size2) {
        cout << "Size of vectors dont match" << endl;
        return false;
    } 

    for (uint64_t i = 0; i < size1; ++i) {
        if (v1[i] != v2[i]) {
            cout << "Values of i: " << i << " element not match" << endl;
            return false;
        }
    }

    cout << "All elements are equal" << endl;
    return true;
}


// void decodeBinTrie(binTrie &b, vector<uint64_t> &decoded, bit_vector partial_result,
//                    uint64_t node_id, uint16_t curr_level, uint16_t max_level) {
   
//     if (curr_level == max_level) {
//         uint64_t number = 0;
//         for (uint16_t i = 0; i <  b.getHeight(); ++i) {
//             number += partial_result[i]*pow(2, i);  
//         }
//         decoded.push_back(number);
//         return;
//     }

//     bit_vector node = b.getNode(node_id, curr_level);
//     uint16_t next_level = curr_level + 1;

//     bit_vector leftResult  = partial_result;
//     bit_vector rightResult = partial_result;

//     if (node[0] == 1) {
//         leftResult[b.getHeight() - curr_level - 1] = 0; 
//         uint64_t left_child = b.getLeftChild(node_id, curr_level);
//         decodeBinTrie(b, decoded, leftResult, left_child, next_level, max_level);
//     }

//     if (node[1] == 1) {
//         rightResult[b.getHeight() - curr_level - 1] = 1;
//         uint64_t right_child = b.getRightChild(node_id, curr_level);
//         decodeBinTrie(b, decoded, rightResult, right_child, next_level, max_level);
//     }
// }


// template <class rankType>
// void decodeBinTrie(flatBinTrie<rankType> &b, vector<uint64_t> &decoded, bit_vector partial_result,
//                    uint64_t node_id, uint16_t curr_level, uint16_t max_level) {
//     if (curr_level == max_level) {
//         uint64_t number = 0;
//         for (uint16_t i = 0; i <  b.getHeight(); ++i) {
//             number += partial_result[i]*pow(2, i);  
//         }
//         decoded.push_back(number);
//         return;
//     }

//     bit_vector node = b.getNode(node_id);
//     uint16_t next_level = curr_level + 1;

//     bit_vector leftResult  = partial_result;
//     bit_vector rightResult = partial_result;

//     if (node[0] == 1) {
//         leftResult[b.getHeight() - curr_level - 1] = 0;
//         uint64_t left_child = b.getLeftChild(node_id);
//         decodeBinTrie(b, decoded, leftResult, left_child, next_level, max_level);
//     }

//     if (node[1] == 1) {
//         rightResult[b.getHeight() - curr_level - 1] = 1;
//         uint64_t right_child = b.getRightChild(node_id);
//         decodeBinTrie(b, decoded, rightResult, right_child, next_level, max_level);
//     }
// }
// template void decodeBinTrie<rank_support_v5<1>>(flatBinTrie<rank_support_v5<1>> &b, vector<uint64_t> &decoded, bit_vector partial_result,
//                                                 uint64_t node_id, uint16_t curr_level, uint16_t max_level);
// template void decodeBinTrie<rank_support_v<1>>(flatBinTrie<rank_support_v<1>> &b, vector<uint64_t> &decoded, bit_vector partial_result,
//                                                 uint64_t node_id, uint16_t curr_level, uint16_t max_level);


std::vector<uint64_t>* read_inverted_list(std::ifstream &input_stream, uint64_t n){
    uint64_t x;
    uint64_t f;
    uint64_t i;

    // int_vector<>* inverted_list  = new int_vector<>(n);
    vector<uint64_t>* inverted_list = new vector<uint64_t>;
    for (i = 0; i < n; i++){
        input_stream >> x;
        input_stream >> f;
        // (*inverted_list)[i] = x;
        inverted_list -> push_back(x);
    }

    return inverted_list;
}


void read_inverted_index(string file_path) {
    std::ifstream input_stream(file_path);
    if (!input_stream.is_open()) {
        cout << "No se pudo abrir el archivo: " << file_path << endl;
        return;
    }
    uint64_t u;
    // input_stream >> u;
    // cout << "Universe: " << u << endl;
    uint64_t total_size_tries = 0;
    uint64_t total_elements = 0;
    uint64_t number_inverted_list = 0;

    uint64_t total_size_tries_compress = 0;

    
    // for (uint64_t i = 0; i < u; ++i) {
    while ( !input_stream.eof() ){
    // for (uint64_t i = 0; i < 2000; ++i){
        uint64_t set_size;
        uint64_t termId;

        input_stream >> termId;
        input_stream >> set_size;
        
        if (set_size >= 4096) {
        // if (set_size >= 100000 && set_size != 11242476) {
        // if (set_size != 50564) {    
            vector<uint64_t> *il = read_inverted_list(input_stream, set_size);
            uint64_t max_value = (*il)[ set_size - 2];
            // cout << "max value: "<< (*il)[ set_size - 1] << endl;
            flatBinTrie<rank_support_v5<1>> trie = flatBinTrie<rank_support_v5<1>>(*il, max_value);
            uint64_t uncompress_size = trie.size_in_bytes();
            trie.compress();
            vector<uint64_t> decoded;
            trie.decode(decoded);
            
            // bit_vector p_result(trie.getHeight(), 0);
            // decodeBinTrie(trie, decoded, p_result, 0, 0, trie.getHeight());
            // cout << "paso decode" << endl;
            compareVectors(*il, decoded);
            // cout << "paso compare" << endl;
            uint64_t compress_size_trie = trie.size_in_bytes();
            cout << "height: " << trie.getHeight() << endl;
            cout << "n° elements: " << set_size << endl;
            cout << "uncompress size in bits: " << uncompress_size*8 << endl;
            cout << "compress size in bits: " << compress_size_trie*8 << endl;
            cout << "avg size uncompress: " << (float)(uncompress_size*8)/set_size << endl;
            cout << "avg size compress: " << (float)(compress_size_trie*8)/set_size << endl;
            cout << "--------------------------------------" << endl;
            total_size_tries_compress += compress_size_trie;
            total_size_tries += uncompress_size;
            total_elements += set_size;
            number_inverted_list ++;
            delete il;
        }
        else {
            input_stream.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cout << "Total elements: " << total_elements << endl;
    cout << "Total size uncompress: " << total_size_tries << endl;
    cout << "Avg size uncompress: " << (float)(total_size_tries*8)/total_elements << endl;
    cout << "Avg size compress: " << (float)(total_size_tries_compress*8)/total_elements << endl;
    cout << "Total number of inverted list: " << number_inverted_list << endl;
}


void force_brute_intersection(vector<uint64_t> Sets[], uint16_t k, vector<uint64_t> &intersection) {
    queue<vector<uint64_t>> q;
    for (uint64_t i = 0; i < k; ++i) {
        q.push(Sets[i]);
    }
    vector<uint64_t> s1;
    vector<uint64_t> s2;
    s1 = q.front();
    q.pop();
    while (!q.empty()) {
        s2 = q.front();
        q.pop();
        vector<uint64_t> aux_intersection;
        for (uint64_t i = 0; i < s1.size(); ++i) {
            for (uint64_t j = 0; j < s2.size(); ++j){
                if (s1[i] == s2[j]) {
                    aux_intersection.push_back(s1[i]);
                }
            }
        }
        s1 = aux_intersection;
    }
    for (uint64_t i = 0; i < s1.size(); ++i) {
        intersection.push_back(s1[i]);
    }
    
}


void performQueryLog(string query_log_path, string ii_path) {
    std::ifstream query_stream(query_log_path);
    std::ifstream ii_stream(ii_path);

    if (!query_stream.is_open()) {
        cout << "Can't open queries file: " << query_log_path << endl;
        return;
    }

    if (!ii_stream.is_open()) {
        cout << "Can't open inverted index file: " << ii_path << endl;
        return;
    }

    std::string line;
    uint64_t max_number_of_sets = 0;
    uint64_t number_of_queries = 0;
    while ( getline( query_stream, line ) ) {
        std::istringstream is( line );
        vector <uint64_t> termsId = std::vector<uint64_t>( std::istream_iterator<int>(is),
                                                      std::istream_iterator<int>() );
        if (termsId.size() > max_number_of_sets) {
            max_number_of_sets = termsId.size();
        }                                              
        cout << "Number of sets: " << termsId.size() << " ";
        for (uint16_t i= 0; i < termsId.size(); ++i) {
            cout << termsId[i] << " ";
        }
        cout << endl;
        number_of_queries++;
    }
    cout << "---------------------------------------" << endl;
    cout << "Número maximo de conjuntos por query: " << max_number_of_sets << endl;

}