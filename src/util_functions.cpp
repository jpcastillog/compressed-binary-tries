#include <iostream>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <math.h>
#include <fstream>
#include <limits>
#include <queue>
#include "intersection.hpp"
#include <chrono>
#include "barbay_and_kenyon.hpp"



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
    uint64_t total_size_tries_v = 0;
    uint64_t total_size_tries_v5 = 0;
    uint64_t total_elements = 0;
    uint64_t number_inverted_list = 0;

    uint64_t total_size_tries_compress_v = 0;
    uint64_t total_size_tries_compress_v5 = 0;

    
    // for (uint64_t i = 0; i < u; ++i) {
    while ( !input_stream.eof() ){
    // for (uint64_t i = 0; i < 2000; ++i){
        uint64_t set_size;
        uint64_t termId;

        input_stream >> termId;
        input_stream >> set_size;
        
        if (set_size >= 100000) {
            cout << "termId: " << termId << endl;
        // if (set_size >= 100000 && set_size != 11242476) {
        // if (set_size != 50564) {    
            vector<uint64_t> *il = read_inverted_list(input_stream, set_size);
            uint64_t max_value = (*il)[ set_size - 2];
            
            flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(*il, max_value);
            flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(*il, max_value);
            
            
            uint64_t uncompress_size_v = trie_v.size_in_bytes();
            uint64_t uncompress_size_v5 = trie_v5.size_in_bytes();
            
            trie_v.compress();
            trie_v5.compress();
            uint64_t compress_size_trie_v = trie_v.size_in_bytes();
            uint64_t compress_size_trie_v5 = trie_v5.size_in_bytes();
            

            // vector<uint64_t> decoded;
            // trie_v.decode(decoded);
            // compareVectors(*il, decoded);

            
            cout << "height: " << trie_v.getHeight() << endl;
            cout << "n° elements: " << set_size << endl;
            cout << "uncompress size in bits, V rank: " << uncompress_size_v*8 << endl;
            cout << "uncompress size in bits, V5 rank: " << uncompress_size_v5*8 << endl;
            cout << "compress size in bits, V rank: " << compress_size_trie_v*8 << endl;
            cout << "compress size in bits, V5 rank: " << compress_size_trie_v5*8 << endl;
            cout << "avg size uncompress, V rank: " << (float)(uncompress_size_v*8)/set_size << endl;
            cout << "avg size uncompress, V5 rank: " << (float)(uncompress_size_v5*8)/set_size << endl;
            cout << "avg size compress, V rank: " << (float)(compress_size_trie_v*8)/set_size << endl;
            cout << "avg size compress, V5 rank: " << (float)(compress_size_trie_v5*8)/set_size << endl;
            cout << "--------------------------------------" << endl;
            
            total_size_tries_v += uncompress_size_v;
            total_size_tries_v5 += uncompress_size_v5;

            total_size_tries_compress_v += compress_size_trie_v;
            total_size_tries_compress_v5 += compress_size_trie_v5;
            total_elements += set_size;
            number_inverted_list ++;
            delete il;
        }
        else {
            input_stream.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cout << "Total elements: " << total_elements << endl;
    cout << "Avg size uncompress, rank V: " << (float)(total_size_tries_v*8)/total_elements << endl;
    cout << "Avg size uncompress, rank V5: " << (float)(total_size_tries_v5*8)/total_elements << endl;

    cout << "Avg size compress, rank V: " << (float)(total_size_tries_compress_v*8)/total_elements << endl;
    cout << "Avg size compress, rank V5: " << (float)(total_size_tries_compress_v5*8)/total_elements << endl;
    
    cout << "Total number of inverted list: " << number_inverted_list << endl;
}


void randomQueries(string file_path) {
    std::ifstream input_stream(file_path);
    if (!input_stream.is_open()) {
        cout << "No se pudo abrir el archivo: " << file_path << endl;
        return;
    }
    vector<uint64_t> termsId;
    while ( !input_stream.eof() ){
        uint64_t set_size;
        uint64_t termId;

        input_stream >> termId;
        input_stream >> set_size;
        
        if (set_size >= 4096) {
            termsId.push_back(termId);    
            input_stream.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            input_stream.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    input_stream.close();
    cout << "-> Get all terms >= 4096" << endl;

    vector<tuple<uint64_t, uint64_t>> queries;
    vector<uint64_t> allTermsId;
    for (uint64_t i = 0; i < 1000; ++i){
        
        uint64_t termId1 = termsId[rand() % termsId.size()];
        uint64_t termId2 = termsId[rand() % termsId.size()];
        
        while (termId1 == termId2) {
            termId2 = termsId[rand() % termsId.size()];
        }
        allTermsId.push_back(termId1);
        allTermsId.push_back(termId2);

        tuple<uint64_t, uint64_t> query(termId1, termId2);
        queries.push_back(query);
    }
    cout << "-> Generate all queries" << endl;
    allTermsId.erase( unique( allTermsId.begin(), allTermsId.end() ), allTermsId.end() );
    std::sort(allTermsId.begin(), allTermsId.end());

    map<uint64_t, flatBinTrie<rank_support_v<1>>> tries_v;
    map<uint64_t, flatBinTrie<rank_support_v5<1>>> tries_v5;

    map<uint64_t, flatBinTrie<rank_support_v<1>>> run_tries_v;
    map<uint64_t, flatBinTrie<rank_support_v5<1>>> run_tries_v5;

    std::ifstream ii_stream(file_path);
    uint64_t nil = 0;
    while ( !ii_stream.eof() && nil < allTermsId.size() ){
        uint64_t set_size;
        uint64_t termId;

        ii_stream >> termId;
        ii_stream >> set_size;
        
        if (allTermsId[nil] == termId) {    
            vector<uint64_t> *il = read_inverted_list(ii_stream, set_size);
            uint64_t max_value = (*il)[ set_size - 2];
            
            flatBinTrie<rank_support_v<1>> trie_v = flatBinTrie<rank_support_v<1>>(*il, max_value);
            flatBinTrie<rank_support_v5<1>> trie_v5 = flatBinTrie<rank_support_v5<1>>(*il, max_value);

            tries_v.insert(std::pair<uint64_t, flatBinTrie<rank_support_v<1>>>(termId, trie_v));
            tries_v5.insert(std::pair<uint64_t, flatBinTrie<rank_support_v5<1>>>(termId, trie_v5));
            
            trie_v.compress();
            trie_v5.compress();

            run_tries_v.insert(std::pair<uint64_t, flatBinTrie<rank_support_v<1>>>(termId, trie_v));
            run_tries_v5.insert(std::pair<uint64_t, flatBinTrie<rank_support_v5<1>>>(termId, trie_v5));
            
            delete il;
            nil++;
        }
        else {
            ii_stream.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cout << "-> Indexed all terms" << endl;

    uint64_t total_time_v = 0;
    uint64_t total_time_v5 = 0;
    for (uint64_t i = 0; i < queries.size(); ++i) {
        uint64_t termId1, termId2;
        tuple<uint64_t, uint64_t> q = queries[i];
        std::tie(termId1, termId2) = q;
        cout << "Desacople de tuplas ok" << endl;
        flatBinTrie <rank_support_v<1>> trie1_v = run_tries_v[termId1];
        flatBinTrie <rank_support_v<1>> trie2_v = run_tries_v[termId2];

        flatBinTrie <rank_support_v5<1>> trie1_v5 = run_tries_v5[termId1];
        flatBinTrie <rank_support_v5<1>> trie2_v5 = run_tries_v5[termId2];
        cout << "Creación de tries OK" << endl;
        vector <flatBinTrie<rank_support_v<1>>> Bs_v {trie1_v, trie2_v};
        vector <flatBinTrie<rank_support_v5<1>>> Bs_v5 {trie1_v5, trie2_v5};
        cout << "Comienzo de intersección" << Bs_v.size() << endl;
        flatBinTrie <rank_support_v<1>>* result_v;
        uint64_t time_v;
        result_v = joinTries<rank_support_v<1>>(Bs_v, false, time_v);
        cout << "Time execution, rank V: " << (float) time_v*10e-6 << endl;

        flatBinTrie <rank_support_v5<1>>* result_v5;
        uint64_t time_v5;
        result_v5 = joinTries<rank_support_v5<1>>(Bs_v5, false, time_v5);
        cout << "Time execution, rank V5: " << (float) time_v5*10e-6 << endl;
        cout << "--------------------------------------------------"<< endl;

        total_time_v += time_v;
        total_time_v5 += time_v;
    }
    cout <<  "Avg time, rank V: " << (float) (total_time_v*10e-6)/1000 << "[ms]"<< endl;
    cout <<  "Avg time, rank V5: " << (float) (total_time_v5*10e-6)/1000 << "[ms]"<< endl;
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


std::fstream& GotoLine(std::fstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
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
    // Get all terms of queries
    std::vector<uint64_t> all_termsId = vector<uint64_t>(std::istream_iterator<uint64_t>(query_stream), 
                                        std::istream_iterator<uint64_t>() );
    query_stream.close();

    cout << "-> Total de terms id en querys (con duplicados): " << all_termsId.size() << endl;
    std::sort(all_termsId.begin(), all_termsId.end());
    all_termsId.erase( unique( all_termsId.begin(), all_termsId.end() ), all_termsId.end() );
    cout << "-> Numero total de terms id (sin duplicar): " << all_termsId.size() << endl;

    // Indexing inverted lists
    // map<uint64_t, flatBinTrie<rank_support_v5<1>>> tries;
    map<uint64_t, flatBinTrie<rank_support_v<1>>> tries;
    map<uint64_t, vector<uint64_t>> il_vectors;
    uint64_t n_il = 0;
    while (!ii_stream.eof() && n_il < all_termsId.size()) {
        uint64_t termId;
        uint64_t n;

        ii_stream >> termId;
        ii_stream >> n;
        if (all_termsId[n_il] == termId) {
            vector<uint64_t> *il = read_inverted_list(ii_stream, n);
            uint64_t max_value = (*il)[ n - 2];
            // flatBinTrie<rank_support_v5<1>> trie = flatBinTrie<rank_support_v5<1>>(*il, max_value);
            flatBinTrie<rank_support_v<1>> trie = flatBinTrie<rank_support_v<1>>(*il, max_value);
            // trie.compress();
            // tries.insert(std::pair<uint64_t, flatBinTrie<rank_support_v5<1>>>(termId, trie));
            tries.insert(std::pair<uint64_t, flatBinTrie<rank_support_v<1>>>(termId, trie));
            il_vectors.insert(std::pair<uint64_t, vector<uint64_t>>(termId, *il));
            // delete il;
            n_il++;
        }
        else{
            ii_stream.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    cout << "-> End indexing inverted lists" << endl;

    cout << "-> Start procesing queries" << endl;

    // Procesing queries
    std::ifstream query_log_stream(query_log_path);
    if (!query_log_stream.is_open()) {
        cout << "Can't open queries file: " << query_log_path << endl;
        return;
    }

    std::string line;
    uint64_t max_number_of_sets = 0;
    uint64_t number_of_queries = 0;
    uint64_t total_time = 0;
    uint64_t total_time_bk = 0;
    while ( getline( query_log_stream, line ) ) {
        // vector <flatBinTrie<rank_support_v5<1>>> Bs;
        vector <flatBinTrie<rank_support_v<1>>> Bs;
        vector <vector<uint64_t>> sets;

        std::istringstream is( line );
        vector <uint64_t> termsId = std::vector<uint64_t>( std::istream_iterator<int>(is),
                                                        std::istream_iterator<int>()
                                                        );
        // if (termsId.size() <= 16 && termsId.size() > 1) {
        if (termsId.size() <= 16 && termsId.size() > 1) {
            for (uint16_t i = 0; i < termsId.size(); ++i){
                Bs.push_back(tries[termsId[i]]);
                sets.push_back(il_vectors[termsId[i]]);
            }

            // flatBinTrie<rank_support_v5<1>>* result;
            flatBinTrie<rank_support_v<1>>* result;
            // auto start = std::chrono::high_resolution_clock::now();
            
            uint64_t time;
            // result = joinTries<rank_support_v5<1>>(Bs, true, time);
            result = joinTries<rank_support_v<1>>(Bs, true, time);
            // auto end = std::chrono::high_resolution_clock::now();
            // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            // total_time += elapsed.count();
            total_time += time;
            cout << "i: " << number_of_queries << " |n: " << termsId.size() << " |Time execution: " << (float)time*10e-6 << "[ms]" << endl;

            // Barbay and Kenyon
            vector<uint64_t> intersection_bk;
            auto start = std::chrono::high_resolution_clock::now();
            barbayKenyon(sets, termsId.size(), intersection_bk);
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            total_time_bk += elapsed.count();
            cout << "i: " << number_of_queries << " |n: " << termsId.size() << " |Time execution B&K: " << (float)elapsed.count()*10e-6 << "[ms]" << endl;
            cout << "----------------------------------------------------------"<< endl;
             
            number_of_queries++;
        }
        // if (termsId.size() > max_number_of_sets) {
        //     max_number_of_sets = termsId.size();
        // }                                              
        // cout << "Number of sets: " << termsId.size() << " ";
        // for (uint16_t i= 0; i < termsId.size(); ++i) {
        //     cout << termsId[i] << " ";
        // }
        // cout << endl;
        // number_of_queries++;
    }
    cout << "---------------------------------------" << endl;
    // cout << "Número maximo de conjuntos por query: " << max_number_of_sets << endl;
    cout << "Número total de queries: " << number_of_queries << endl;
    cout << "Tiempo promedio:" << (float)(total_time*10e-6)/number_of_queries << "[ms]" << endl;
    cout << "Tiempo promedio B&K: " << (float)(total_time_bk*10e-6)/number_of_queries << "[ms]" << endl;


    query_log_stream.close();
    ii_stream.close();

}