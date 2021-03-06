#include <iostream>
// #include "binTrie.hpp"
// #include "flatBinTrie.hpp"
// #include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <vector>
#include <math.h>
#include <fstream>
#include <limits>
#include <queue>
#include <tuple>
// #include "intersection.hpp"
#include <chrono>
#include "barbay_and_kenyon.hpp"


// bool compareVectors(vector<uint64_t> &v1, vector<uint64_t> &v2) {
//     uint64_t size1 = v1.size();
//     uint64_t size2 = v2.size();
//     if (size1 != size2) {
//         cout << "Size of vectors dont match" << endl;
//         return false;
//     } 

//     for (uint64_t i = 0; i < size1; ++i) {
//         if (v1[i] != v2[i]) {
//             cout << "Values of i: " << i << " element not match" << endl;
//             return false;
//         }
//     }

//     cout << "All elements are equal" << endl;
//     return true;
// }


// void writeVector(vector<uint64_t> &v, string path) {
//     ofstream o;
//     o.open(path);

//     vector<uint64_t>::iterator it;
//     for (it = v.begin(); it != v.end(); ++it) {
//         o << *it << endl;
//     }

//     o.close();
// }


// void force_brute_intersection(vector<uint64_t> Sets[], uint16_t k, vector<uint64_t> &intersection) {
//     queue<vector<uint64_t>> q;
//     for (uint64_t i = 0; i < k; ++i) {
//         q.push(Sets[i]);
//     }
//     vector<uint64_t> s1;
//     vector<uint64_t> s2;
//     s1 = q.front();
//     q.pop();
//     while (!q.empty()) {
//         s2 = q.front();
//         q.pop();
//         vector<uint64_t> aux_intersection;
//         for (uint64_t i = 0; i < s1.size(); ++i) {
//             for (uint64_t j = 0; j < s2.size(); ++j){
//                 if (s1[i] == s2[j]) {
//                     aux_intersection.push_back(s1[i]);
//                 }
//             }
//         }
//         s1 = aux_intersection;
//     }
//     for (uint64_t i = 0; i < s1.size(); ++i) {
//         intersection.push_back(s1[i]);
//     }
    
// }


// std::vector<uint64_t>* read_inverted_list(std::ifstream &input_stream, uint64_t n){
//     uint64_t x;
//     uint64_t f;
//     uint64_t i;

//     // int_vector<>* inverted_list  = new int_vector<>(n);
//     vector<uint64_t>* inverted_list = new vector<uint64_t>;
//     for (i = 0; i < n; i++){
//         input_stream >> x;
//         input_stream >> f;
//         // (*inverted_list)[i] = x;
//         inverted_list -> push_back(x);
//     }

//     return inverted_list;
// }


// vector<uint64_t>* read_inverted_list_B(std::ifstream &input_stream, uint32_t n) {

//     vector <uint64_t>* il = new vector<uint64_t>();
//     il -> reserve(n);
//     for (uint32_t i = 0; i < n; ++i) {
//         uint32_t x;
//         input_stream.read(reinterpret_cast<char *>(&x), 4);
//         il -> push_back((uint64_t)x);
//     }
//     return il;
// }


// void testSpaceInvertedIndex(std::string ii_path) {
//     std::ifstream input_stream;
//     input_stream.open(ii_path, std::ios::binary | std::ios::in);
//     if (!input_stream.is_open()) {
//         cout << "No se pudo abrir el archivo: " << ii_path << endl;
//         return;
//     }

//     // std::ifstream sizes(sizes_path, std::ios::binary);
//     // if (!input_stream.is_open()) {
//     //     cout << "No se pudo abrir el archivo: " << sizes_path << endl;
//     //     return;
//     // }

//     uint32_t a, u;

//     input_stream.read(reinterpret_cast<char *>(&a), sizeof(a));
//     input_stream.read(reinterpret_cast<char *>(&u), sizeof(u));
    
//     cout << "Universe: " << u << endl;

//     uint64_t total_size = 0;
//     uint64_t total_elements = 0;
//     uint64_t n_il = 0;
//     for (uint32_t i = 0; i < u; ++i) {
        
//         uint32_t n;
//         input_stream.read(reinterpret_cast<char *>(&n), 4);
//         if (n > 4096){
//             vector <uint64_t> *il;
//             il = read_inverted_list_B(input_stream, n);
//             uint64_t max_value = (*il)[ n - 1];
//             flatBinTrie<rank_support_v5<1>> trie_v (*il, max_value);
//             trie_v.encodeRuns();

//             uint64_t trie_bytes_size = trie_v.size_in_bytes();
//             total_size += trie_bytes_size;
//             total_elements += n;
//             n_il++;

//             cout << "#Elements: " << n << " | Bpi: " << (float)(trie_bytes_size*8)/n << endl;
            
//             delete il;
//         }

//         else {
//             input_stream.seekg(4*n, ios::cur);
//         }
        
//     }
//     input_stream.close();

//     cout << "Total inverted lists: " << n_il << "| Bpi: " << (float)(total_size*8)/total_elements << endl;
    
//     return;
// }


// void testSpaceII(string file_path) {
//     std::ifstream input_stream(file_path);
//     if (!input_stream.is_open()) {
//         cout << "No se pudo abrir el archivo: " << file_path << endl;
//         return;
//     }
//     uint64_t u;
//     // input_stream >> u;
//     // cout << "Universe: " << u << endl;
//     uint64_t total_size_tries_v = 0;
//     uint64_t total_size_tries_v5 = 0;
//     uint64_t total_elements = 0;
//     uint64_t number_inverted_list = 0;

//     uint64_t total_size_tries_compress_v = 0;
//     uint64_t total_size_tries_compress_v5 = 0;

    
//     // for (uint64_t i = 0; i < u; ++i) {
//     while ( !input_stream.eof() ){
//     // for (uint64_t i = 0; i < 2000; ++i){
//         uint64_t set_size;
//         uint64_t termId;

//         input_stream >> termId;
//         input_stream >> set_size;
        
//         if (set_size >= 100000) {
//             cout << "termId: " << termId << endl;
//         // if (set_size >= 100000 && set_size != 11242476) {
//         // if (set_size != 50564) {    
//             vector<uint64_t> *il = read_inverted_list(input_stream, set_size);
//             uint64_t max_value = (*il)[ set_size - 2];
            
//             flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(*il, max_value);
//             flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(*il, max_value);
            
            
//             uint64_t uncompress_size_v = trie_v.size_in_bytes();
//             uint64_t uncompress_size_v5 = trie_v5.size_in_bytes();
            
//             trie_v.encodeRuns();
//             trie_v5.encodeRuns();
//             uint64_t compress_size_trie_v = trie_v.size_in_bytes();
//             uint64_t compress_size_trie_v5 = trie_v5.size_in_bytes();
            

//             // vector<uint64_t> decoded;
//             // trie_v.decode(decoded);
//             // compareVectors(*il, decoded);

            
//             cout << "height: " << trie_v.getHeight() << endl;
//             cout << "n?? elements: " << set_size << endl;
//             cout << "uncompress size in bits, V rank: " << uncompress_size_v*8 << endl;
//             cout << "uncompress size in bits, V5 rank: " << uncompress_size_v5*8 << endl;
//             cout << "compress size in bits, V rank: " << compress_size_trie_v*8 << endl;
//             cout << "compress size in bits, V5 rank: " << compress_size_trie_v5*8 << endl;
//             cout << "avg size uncompress, V rank: " << (float)(uncompress_size_v*8)/set_size << endl;
//             cout << "avg size uncompress, V5 rank: " << (float)(uncompress_size_v5*8)/set_size << endl;
//             cout << "avg size compress, V rank: " << (float)(compress_size_trie_v*8)/set_size << endl;
//             cout << "avg size compress, V5 rank: " << (float)(compress_size_trie_v5*8)/set_size << endl;
//             cout << "--------------------------------------" << endl;
            
//             total_size_tries_v += uncompress_size_v;
//             total_size_tries_v5 += uncompress_size_v5;

//             total_size_tries_compress_v += compress_size_trie_v;
//             total_size_tries_compress_v5 += compress_size_trie_v5;
//             total_elements += set_size;
//             number_inverted_list ++;
//             delete il;
//         }
//         else {
//             input_stream.ignore(numeric_limits<streamsize>::max(), '\n');
//         }
//     }
//     cout << "Total elements: " << total_elements << endl;
//     cout << "Avg size uncompress, rank V: " << (float)(total_size_tries_v*8)/total_elements << endl;
//     cout << "Avg size uncompress, rank V5: " << (float)(total_size_tries_v5*8)/total_elements << endl;

//     cout << "Avg size compress, rank V: " << (float)(total_size_tries_compress_v*8)/total_elements << endl;
//     cout << "Avg size compress, rank V5: " << (float)(total_size_tries_compress_v5*8)/total_elements << endl;
    
//     cout << "Total number of inverted list: " << number_inverted_list << endl;
// }


// void randomQueries(string file_path) {
//     srand(time(0));
//     std::ifstream input_stream(file_path);
//     if (!input_stream.is_open()) {
//         cout << "No se pudo abrir el archivo: " << file_path << endl;
//         return;
//     }
//     vector<uint64_t> termsId;
//     while ( !input_stream.eof() ){
//         uint64_t set_size;
//         uint64_t termId;

//         input_stream >> termId;
//         input_stream >> set_size;
        
//         if (set_size >= 4096) {
//             termsId.push_back(termId);    
//             input_stream.ignore(numeric_limits<streamsize>::max(), '\n');
//         }
//         else {
//             input_stream.ignore(numeric_limits<streamsize>::max(), '\n');
//         }
//     }
//     input_stream.close();
//     cout << "-> Get all terms >= 4096" << endl;

//     vector<tuple<uint64_t, uint64_t>> queries;
//     vector<uint64_t> allTermsId;
//     for (uint64_t i = 0; i < 1000; ++i){
        
//         uint64_t termId1 = termsId[rand() % termsId.size()];
//         uint64_t termId2 = termsId[rand() % termsId.size()];
        
//         while (termId1 == termId2) {
//             termId2 = termsId[rand() % termsId.size()];
//         }
//         allTermsId.push_back(termId1);
//         allTermsId.push_back(termId2);

//         tuple<uint64_t, uint64_t> query(termId1, termId2);
//         queries.push_back(query);
//     }
//     cout << "-> Generate all queries" << endl;
//     std::sort(allTermsId.begin(), allTermsId.end());
//     allTermsId.erase( unique( allTermsId.begin(), allTermsId.end() ), allTermsId.end() );
    
//     map<uint64_t, flatBinTrie<rank_support_v<1>>> tries_v;
//     map<uint64_t, flatBinTrie<rank_support_v5<1>>> tries_v5;

//     map<uint64_t, flatBinTrie<rank_support_v<1>>> run_tries_v;
//     map<uint64_t, flatBinTrie<rank_support_v5<1>>> run_tries_v5;

//     std::ifstream ii_stream(file_path);
//     uint64_t nil = 0;
//     cout << "Number of distinct inverted lists: " << allTermsId.size() << endl;
//     int count = 0;
//     while ( !ii_stream.eof() && nil < allTermsId.size() ){
//         uint64_t set_size;
//         uint64_t termId;

//         ii_stream >> termId;
//         ii_stream >> set_size;
        
//         if (allTermsId[nil] == termId) {    
//             vector<uint64_t> *il = read_inverted_list(ii_stream, set_size);
//             uint64_t max_value = (*il)[ set_size - 2];
            
//             flatBinTrie<rank_support_v<1>> trie_v   = flatBinTrie<rank_support_v<1>>(*il, 24622347);
//             flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(*il, 24622347);

//             tries_v.insert(std::pair<uint64_t, flatBinTrie<rank_support_v<1>>>(termId, trie_v));
//             tries_v5.insert(std::pair<uint64_t, flatBinTrie<rank_support_v5<1>>>(termId, trie_v5));
            
//             trie_v.encodeRuns();
//             trie_v5.encodeRuns();

//             run_tries_v.insert(std::pair<uint64_t, flatBinTrie<rank_support_v<1>>>(termId, trie_v));
//             run_tries_v5.insert(std::pair<uint64_t, flatBinTrie<rank_support_v5<1>>>(termId, trie_v5));
            
//             delete il;
//             nil++;
//             count ++;
//         }
//         else {
//             ii_stream.ignore(numeric_limits<streamsize>::max(), '\n');
//         }
//     }
//     cout << "-> Indexed all terms, #il: "<< count << endl;

//     uint64_t total_time_v = 0;
//     uint64_t total_time_v5 = 0;
//     for (uint64_t i = 0; i < queries.size(); ++i) {
//         uint64_t termId1, termId2;
//         tuple<uint64_t, uint64_t> q = queries[i];
//         std::tie(termId1, termId2) = q;
//         // cout << "Desacople de tuplas ok" << endl;
//         flatBinTrie <rank_support_v<1>> trie1_v = run_tries_v[termId1];
//         flatBinTrie <rank_support_v<1>> trie2_v = run_tries_v[termId2];
//         // cout << "Tries V Load  OK" << endl;
//         flatBinTrie <rank_support_v5<1>> trie1_v5 = run_tries_v5[termId1];
//         flatBinTrie <rank_support_v5<1>> trie2_v5 = run_tries_v5[termId2];
//         // cout << "Tries V5 Load  OK" << endl;
//         vector <flatBinTrie<rank_support_v<1>>> Bs_v {trie1_v, trie2_v};
//         vector <flatBinTrie<rank_support_v5<1>>> Bs_v5 {trie1_v5, trie2_v5};
//         // cout << "Comienzo de intersecci??n " << Bs_v.size() << endl;
        
//         uint64_t avg_time_v = 0;
//         for (uint16_t i = 0; i < 10; ++i) {
//             flatBinTrie <rank_support_v<1>>* result_v;
//             uint64_t time_v;
//             result_v = joinTries<flatBinTrie<rank_support_v<1>>>(Bs_v, true);
//             avg_time_v += time_v;
//         }
//         cout << "Time execution, rank V: " << (float) (avg_time_v/10)*10e-6 << endl;
        
//         uint64_t avg_time_v5 = 0;
//         for (uint16_t i = 0; i < 10; ++i) {
//             flatBinTrie <rank_support_v5<1>>* result_v5;
//             uint64_t time_v5;
//             result_v5 = joinTries<flatBinTrie<rank_support_v5<1>>>(Bs_v5, true);
//             avg_time_v5 += time_v5;
//         }
        
//         cout << "Time execution, rank V5: " << (float) (avg_time_v5/10)*10e-6 << endl;
//         cout << "--------------------------------------------------"<< endl;

//         total_time_v += avg_time_v/10;
//         total_time_v5 += avg_time_v5/10;
//     }
//     cout <<  "Avg time, rank V: " << (float) (total_time_v*10e-6)/1000 << "[ms]"<< endl;
//     cout <<  "Avg time, rank V5: " << (float) (total_time_v5*10e-6)/1000 << "[ms]"<< endl;
// }


// void performQueryLog(string query_log_path, string ii_path) {
//     std::ifstream query_stream(query_log_path);
//     std::ifstream ii_stream(ii_path);

//     if (!query_stream.is_open()) {
//         cout << "Can't open queries file: " << query_log_path << endl;
//         return;
//     }

//     if (!ii_stream.is_open()) {
//         cout << "Can't open inverted index file: " << ii_path << endl;
//         return;
//     }
//     // Get all terms of queries
//     std::vector<uint64_t> all_termsId = vector<uint64_t>(std::istream_iterator<uint64_t>(query_stream), 
//                                         std::istream_iterator<uint64_t>() );
//     query_stream.close();

//     cout << "-> Total de terms id en querys (con duplicados): " << all_termsId.size() << endl;
//     std::sort(all_termsId.begin(), all_termsId.end());
//     all_termsId.erase( unique( all_termsId.begin(), all_termsId.end() ), all_termsId.end() );
//     cout << "-> Numero total de terms id (sin duplicar): " << all_termsId.size() << endl;

//     // Indexing inverted lists
//     // map<uint64_t, flatBinTrie<rank_support_v5<1>>> tries;
//     map<uint64_t, flatBinTrie<rank_support_v<1>>> tries_v;
//     map<uint64_t, flatBinTrie<rank_support_v5<1>>> tries_v5;
//     map<uint64_t, vector<uint64_t>> il_vectors;
//     uint64_t n_il = 0;
//     while (!ii_stream.eof() && n_il < all_termsId.size()) {
//         uint64_t termId;
//         uint64_t n;

//         ii_stream >> termId;
//         ii_stream >> n;
//         if (all_termsId[n_il] == termId) {
//             vector<uint64_t> *il = read_inverted_list(ii_stream, n);
//             // uint64_t max_value = (*il)[ n - 2];
            
//             flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(*il, 24622347);
//             flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(*il, 24622347);

//             trie_v.encodeRuns();
//             trie_v5.encodeRuns();
            
//             tries_v.insert(std::pair<uint64_t, flatBinTrie<rank_support_v<1>>>(termId, trie_v));
//             tries_v5.insert(std::pair<uint64_t, flatBinTrie<rank_support_v5<1>>>(termId, trie_v5));
//             il_vectors.insert(std::pair<uint64_t, vector<uint64_t>>(termId, *il));
//             // delete il;
//             n_il++;
//         }
//         else{
//             ii_stream.ignore(numeric_limits<streamsize>::max(), '\n');
//         }
//     }

//     cout << "-> End indexing inverted lists" << endl;

//     cout << "-> Start procesing queries" << endl;

//     // Procesing queries
//     std::ifstream query_log_stream(query_log_path);
//     if (!query_log_stream.is_open()) {
//         cout << "Can't open queries file: " << query_log_path << endl;
//         return;
//     }

//     std::string line;
//     uint64_t max_number_of_sets = 0;
//     uint64_t number_of_queries = 0;
//     uint64_t total_time_v = 0;
//     uint64_t total_time_v5 = 0;
//     uint64_t total_time_bk = 0;
//     uint64_t total_elem = 0;
//     vector<uint64_t> intersections_elements;
//     while ( getline( query_log_stream, line ) ) {
//         vector <flatBinTrie<rank_support_v<1>>> Bs_v;
//         vector <flatBinTrie<rank_support_v5<1>>> Bs_v5;
        
//         vector <vector<uint64_t>> sets;

//         std::istringstream is( line );
//         vector <uint64_t> termsId = std::vector<uint64_t>( std::istream_iterator<int>(is),
//                                                         std::istream_iterator<int>()
//                                                         );
//         // if (termsId.size() <= 16 && termsId.size() > 1) {
//         if (termsId.size() <= 16 && termsId.size() > 1) {
//             for (uint16_t i = 0; i < termsId.size(); ++i){
//                 Bs_v.push_back(tries_v[termsId[i]]);
//                 Bs_v5.push_back(tries_v5[termsId[i]]);
//                 sets.push_back(il_vectors[termsId[i]]);
//             }
            
//             flatBinTrie<rank_support_v<1>>* result_v;
//             flatBinTrie<rank_support_v5<1>>* result_v5;
            
//             uint64_t time_v;
//             uint64_t time_v5;
//             auto start_v = std::chrono::high_resolution_clock::now();
//             result_v = joinTries<flatBinTrie<rank_support_v<1>>>(Bs_v, true);
//             auto end_v = std::chrono::high_resolution_clock::now();
//             auto elapsed_v = std::chrono::duration_cast<std::chrono::nanoseconds>(end_v - start_v);
//             time_v = elapsed_v.count();
            
//             auto start_v5 = std::chrono::high_resolution_clock::now();
//             result_v5 = joinTries<flatBinTrie<rank_support_v5<1>>>(Bs_v5, true);
//             auto end_v5 = std::chrono::high_resolution_clock::now();
//             auto elapsed_v5 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_v5 - start_v5);
//             time_v5 = elapsed_v5.count();
            
//             vector<uint64_t> intersect;
//             // force_brute_intersection(sets.data(), 2, intersect);
//             result_v->decode(intersect);
//             total_elem += intersect.size();
//             cout << "Intersection Size: " << intersect.size() << endl;
            
//             intersections_elements.push_back(intersect.size());

//             total_time_v += time_v;
//             total_time_v5 += time_v5;
            
//             cout << "i: " << number_of_queries << " |n: " << termsId.size() << " |Time execution rank v: " << (float)time_v*10e-6 << "[ms]" << endl;
//             cout << "i: " << number_of_queries << " |n: " << termsId.size() << " |Time execution rank v5: " << (float)time_v5*10e-6 << "[ms]" << endl;

//             // Barbay and Kenyon
//             // vector<uint64_t> intersection_bk;
//             // auto start = std::chrono::high_resolution_clock::now();
//             // barbayKenyon(sets, termsId.size(), intersection_bk);
//             // auto end = std::chrono::high_resolution_clock::now();
//             // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//             // total_time_bk += elapsed.count();
//             // cout << "i: " << number_of_queries << " |n: " << termsId.size() << " |Time execution B&K: " << (float)elapsed.count()*10e-6 << "[ms]" << endl;
//             cout << "----------------------------------------------------------"<< endl;
             
//             number_of_queries++;
//         }
//         // if (termsId.size() > max_number_of_sets) {
//         //     max_number_of_sets = termsId.size();
//         // }                                              
//         // cout << "Number of sets: " << termsId.size() << " ";
//         // for (uint16_t i= 0; i < termsId.size(); ++i) {
//         //     cout << termsId[i] << " ";
//         // }
//         // cout << endl;
//         // number_of_queries++;
//     }
//     writeVector(intersections_elements, "./elements_compress_structure.txt");
//     cout << "---------------------------------------" << endl;
//     // cout << "N??mero maximo de conjuntos por query: " << max_number_of_sets << endl;
//     cout << "N??mero total de queries: " << number_of_queries << endl;
//     cout << "N??mero total elementos: " << total_elem << endl;
//     cout << "Tiempo promedio rank v:" << (float)(total_time_v*10e-6)/number_of_queries << "[ms]" << endl;
//     cout << "Tiempo promedio rank v5:" << (float)(total_time_v5*10e-6)/number_of_queries << "[ms]" << endl;
//     // cout << "Tiempo promedio B&K: " << (float)(total_time_bk*10e-6)/number_of_queries << "[ms]" << endl;


//     query_log_stream.close();
//     ii_stream.close();

// }

// void splitUniverse(vector<uint64_t> &set, queue<tuple<uint64_t, uint64_t, uint64_t>> &q,  
//                    sdsl::bit_vector *bTrie, sdsl::bit_vector *lastLevel, 
//                    uint64_t *level_pos, uint16_t height, uint16_t level_of_cut) {
    
//             uint16_t level            = 0;
//             uint64_t nodes_curr_level = 1; 
//             uint64_t count_nodes      = 0;
//             uint64_t nodes_next_level = 0;
//             uint64_t index            = 0;
//             uint64_t total_nodes      = 0;
//             uint64_t nodes_last_level = 0;

//             while (!q.empty()) {
//                 count_nodes++; // count node visited
//                 // Get node to write
//                 tuple<uint64_t, uint64_t, uint64_t> s = q.front();
//                 q.pop(); 
                
//                 uint64_t l, r, n;
//                 std::tie(l, r, n) = s;
//                 uint64_t left_elements  = 0;
//                 uint64_t right_elements = 0;

//                 // j-th most significative bit
//                 uint8_t j = height - level;
//                 uint64_t ll, lr, rl, rr;
//                 for (uint64_t i = l; i < r+1; ++i) {
//                     if ((set[i] >> j-1) & 1) {                        
//                         right_elements = r-i+1;
//                         rl = i;
//                         rr = r;
//                         break;
//                     }
//                     else {
//                         if (i == l){
//                             ll = l;
//                         }
//                         lr = i;    
//                         left_elements++;
//                     }
//                 }
//                 // Add to queue split sets and write nodes
//                 tuple<uint64_t,uint64_t,uint64_t> left_split;
//                 tuple<uint64_t,uint64_t,uint64_t> right_split;
//                 // left child
//                 if (left_elements > 0) {
//                     // write 1
//                     if (level == height-1)
//                         (*lastLevel)[index] = 1;
//                     else
//                         (*bTrie)[index] = 1;
//                     tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
//                     q.push(left_split);
//                     nodes_next_level++;
//                     index++;
//                     total_nodes++;
//                 }
//                 else {
//                     // write 0
//                     index++;
//                     total_nodes++;
//                 }
//                 // right child
//                 if (right_elements > 0) {
//                     // write 1
//                     if (level == height-1)
//                         (*lastLevel)[index] = 1;
//                     else
//                         (*bTrie)[index] = 1;
//                     tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
//                     q.push(right_split);
//                     nodes_next_level++;
//                     index++;
//                     total_nodes++;
//                 }
//                 else {
//                     // write 0
//                     index++;
//                     total_nodes++;
//                 }

//                 if (count_nodes == nodes_curr_level) {
//                     level_pos[level] = index;
//                     if (level == height-2){
//                         nodes_last_level = nodes_next_level;
//                         index = 0;
//                     }
                    
//                     nodes_curr_level = nodes_next_level;
//                     nodes_next_level = 0;
//                     count_nodes = 0;
//                     level++;
                    
//                 }

//                 if (level == flatBinTrie::height || level == level_of_cut) {
//                     break;
//                 }
//             }
// }


void splitUniverse(vector<uint64_t> &set, queue<tuple<uint64_t, uint64_t, uint64_t>> &q,  
                //    sdsl::bit_vector *bTrie, sdsl::bit_vector *lastLevel,
                   std::vector<uint64_t> &ones, std::vector<uint64_t> &ones_last_lvl, 
                   uint64_t *level_pos, uint16_t init_level, uint16_t height, uint16_t level_of_cut) {
    
    uint16_t level            = init_level;
    uint64_t nodes_curr_level = 1; 
    uint64_t count_nodes      = 0;
    uint64_t nodes_next_level = 0;
    uint64_t index            = 0;
    uint64_t total_nodes      = 0;
    uint64_t nodes_last_level = 0;

    while (!q.empty()) {
        count_nodes++; // count node visited
        // Get node to write
        tuple<uint64_t, uint64_t, uint64_t> s = q.front();
        q.pop(); 
        
        uint64_t l, r, n;
        std::tie(l, r, n) = s;
        uint64_t left_elements  = 0;
        uint64_t right_elements = 0;

        // j-th most significative bit
        uint8_t j = height - level;
        uint64_t ll, lr, rl, rr;
        for (uint64_t i = l; i < r+1; ++i) {
            if ((set[i] >> j-1) & 1) {                        
                right_elements = r-i+1;
                rl = i;
                rr = r;
                break;
            }
            else {
                if (i == l){
                    ll = l;
                }
                lr = i;    
                left_elements++;
            }
        }
        // Add to queue split sets and write nodes
        tuple<uint64_t,uint64_t,uint64_t> left_split;
        tuple<uint64_t,uint64_t,uint64_t> right_split;
        // left child
        if (left_elements > 0) {
            // write 1
            if (level == height-1)
                ones_last_lvl.push_back(index);
                // (*lastLevel)[index] = 1;
            else
                ones.push_back(index);
                // (*bTrie)[index] = 1;
            tuple<uint64_t,uint64_t,uint64_t> left_split(ll, lr, left_elements);
            q.push(left_split);
            nodes_next_level++;
            index++;
            total_nodes++;
        }
        else {
            // write 0
            index++;
            total_nodes++;
        }
        // right child
        if (right_elements > 0) {
            // write 1
            if (level == height-1)
                ones_last_lvl.push_back(index);
                // (*lastLevel)[index] = 1; 
            else
                ones.push_back(index);
                // (*bTrie)[index] = 1;
            tuple<uint64_t,uint64_t,uint64_t> right_split(rl, rr, right_elements);
            q.push(right_split);
            nodes_next_level++;
            index++;
            total_nodes++;
        }
        else {
            // write 0
            index++;
            total_nodes++;
        }

        if (count_nodes == nodes_curr_level) {
            level_pos[level] = index;
            if (level == height-2){
                nodes_last_level = nodes_next_level;
                index = 0;
            }
            
            nodes_curr_level = nodes_next_level;
            nodes_next_level = 0;
            count_nodes = 0;
            level++;
            
        }

        if (level == height || level == level_of_cut) {
            break;
        }
    }
}

    // void joinSolutions(sdsl::bit_vector* bTrie, sdsl::bit_vector* lastLevel, 
    //                    uint64_t* level_pos, uint16_t level_of_cut, uint16_t height,
    //                    std::vector<sdsl::bit_vector> &bvs, std::vector<sdsl::bit_vector> &bvs_last,
    //                    std::vector<uint64_t*> &level_positions) {

    //         uint16_t nThreads = bvs.size();
    //         std::vector<uint64_t> positions(nThreads, 0);
    //         uint64_t curr = level_pos[level_of_cut -1];
    //         uint64_y curr_last_level = 0;
    //         for (uint16_t level = level_of_cut; i < height; ++level) {
    //             for (uint16_t t = 0; t < nThreads; ++t) {
    //                 if (level < height - 1) {
    //                     uint64_t nbits = level == level_of_cut ?
    //                                  level_positions[t][level] :
    //                                  level_positions[t][level] - level_positions[t][level - 1];
    //                     for(uint64_t j = 0; j < nbits; ++j) {
    //                         (*bTrie)[curr] = bvs[t][j];
    //                         curr++;
    //                     }
    //                 } else {
    //                     for(uint64_t j = 0; j < nbits; ++j) {
    //                         (*lastLevel)[curr_last_level] = bvs_last[t][j];
    //                         curr_last_level++;
    //                     }
    //                 }
    //                 level_pos[level] = curr;
    //             }
    //         }

    //     return;
    // }


    void joinSolutions(sdsl::bit_vector* bTrie, sdsl::bit_vector* lastLevel, 
                       uint64_t* level_pos, uint16_t level_of_cut, uint16_t height,
                       std::vector<std::vector<uint64_t>> &ones, std::vector<std::vector<uint64_t>> &ones_last_lvl, 
                       std::vector<uint64_t*> &level_positions) {

    uint16_t nThreads = ones.size();
    uint64_t pos, nBits, shift = 0;
    std::vector<uint64_t> positions(nThreads, 0);
    // Write first levels of Trie
    for(uint16_t level = 0; level < level_of_cut; ++level) {
        for(uint64_t i = 0; i < ones[0].size(); ++i) {
            pos = ones[0][i];
            (*bTrie)[pos] = 1;
        }
        shift = level_positions[0][level];
    }
    // cout << "Join Solutions: Ok before level of cut" << endl;
    for (uint16_t level = level_of_cut; level < height; ++level) {
        for (uint16_t thread = 1; thread < nThreads; ++thread) {
            // Number of bits
            nBits = ((level == level_of_cut || level == height - 1) ?
                    level_positions[thread][level] :
                    level_positions[thread][level] - level_positions[thread][level-1] - 1);
            // cout << "Join Solutions: nBits -> " << nBits << "|level " << level << endl;
            // cout << level_positions[thread][level] << endl;
            if (level < height - 1){
                // uint64_t i = 0;
                uint64_t i = (level == level_of_cut ?
                              0 :
                              level_positions[thread][level-1]
                             );
                while(ones[thread][i] < level_positions[thread][level] && i < ones[thread].size()) {
                    pos = ones[thread][i] + shift;
                    (*bTrie)[pos] = 1;
                    ++i;
                }
            } 
            else {
                shift = 0;
                uint64_t i = 0;
                while(ones_last_lvl[thread][i] < level_positions[thread][level] && i < ones_last_lvl[thread].size()) {
                    pos = ones_last_lvl[thread][i] + shift;
                    (*lastLevel)[pos] = 1;
                    ++i;
                }
            }
            shift += nBits;       
        }
        level_pos[level] = shift;
    }
    return;
}