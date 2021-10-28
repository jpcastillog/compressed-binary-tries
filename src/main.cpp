#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include "binTrie.hpp"
#include "flatBinTrie.hpp"
#include <ctime>
#include <math.h>
#include "utils_functions.hpp"
#include "intersection.hpp"

void foo(int arr[], int n) {
    arr[0] = 1;

    cout << "In foo function" << endl;
    for (int i = 0; i < n; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;

};

int main() {
    string file_path = "./../../../../data/bitvectors/ii/gov2/url/gov2_ii_nofreq_url_dif.txt.B";
    read_inverted_index(file_path);
    
    return 0;
}

// int main(){
//     // int_vector<> s = {1, 2, 3, 5};
//     vector<uint64_t> s =  {1, 3, 7, 8, 9, 11, 12};
//     int_vector<> s1 = {1, 3, 7, 8, 9, 11, 12};
//     int_vector<> s2 = {2, 3, 5, 7, 12, 15};

//     flatBinTrie ts = flatBinTrie(s, 12);
//     ts.print();

//     flatBinTrie pb1 = flatBinTrie(s1);
//     flatBinTrie pb2 = flatBinTrie(s2);

//     cout << "Size in bits s1: " << pb1.size_in_bytes() << endl;
    
//     cout << "-----------Flat binary trie b1-----------" << endl;
//     pb1.print();
//     cout << "-----------Flat binary trie b2-----------" << endl;
//     pb2.print();

//     vector<flatBinTrie> pBs = {pb1, pb2};
//     flatBinTrie fresult = intersectFlatTries(pBs);
//     cout << "-----------result flat binary trie-----------" << endl;
//     fresult.print();
    
//     binTrie b1 = binTrie(s1);
//     binTrie b2 = binTrie(s2);
//     cout << "-----------b1-----------" << endl;
//     for (int i = 0; i < b1.bTrie.size(); ++i) {
//         // cout << b.bTrie[i]<< endl;
//         for (int j = 0; j < b1.bTrie[i].size(); ++(++j)) {
//             cout << b1.bTrie[i][j] << b1.bTrie[i][j+1] << " ";
//         }
//         cout << endl;
//     }
//     cout << "-----------b2-----------" << endl;
//     for (int i = 0; i < b2.bTrie.size(); ++i) {
//         // cout << b.bTrie[i]<< endl;
//         for (int j = 0; j < b2.bTrie[i].size(); ++(++j)) {
//             cout << b2.bTrie[i][j] << b2.bTrie[i][j+1] << " ";
//         }
//         cout << endl;
//     }
//     // cout << b1.bv_rank[3](5) << endl;

//     vector<binTrie> Bs = {b1, b2};
//     binTrie result = intersectTries(Bs);
//     cout << "-----------resultV1-----------" << endl;
//     for (int i = 0; i < result.bTrie.size(); ++i) {
//         // result.bTrie[i].resize(last_pos[i]);
//         for (int j = 0; j < result.bTrie[i].size(); ++(++j)) {
//             cout << result.bTrie[i][j] << result.bTrie[i][j+1] << " ";
//         }
//         cout << endl;
//     }

//     binTrie result2 = intersectTriesV2(Bs);
//     cout << "-----------resultV2-----------" << endl;
//     for (int i = 0; i < result2.bTrie.size(); ++i) {
//         // result.bTrie[i].resize(last_pos[i]);
//         for (int j = 0; j < result2.bTrie[i].size(); ++(++j)) {
//             cout << result2.bTrie[i][j] << result2.bTrie[i][j+1] << " ";
//         }
//         cout << endl;
//     }

//     cout << "-----------decoded b1-----------" << endl;
//     vector<uint64_t> v1;
//     bit_vector partial_result = bit_vector(b1.getHeight(), 0);
//     decodeBinTrie(b1, v1, partial_result, 0, 0, b1.getHeight());
//     for (uint64_t i = 0; i < v1.size(); ++i) {
//         cout << v1[i] << " ";
//     }
//     cout << endl;
    
//     cout << "-----------decoded b2-----------" << endl;
//     vector<uint64_t> v2;
//     bit_vector partial_result2 = bit_vector(b1.getHeight(), 0);
//     decodeBinTrie(b2, v2, partial_result2, 0, 0, b2.getHeight());
//     for (uint64_t i = 0; i < v2.size(); ++i) {
//         cout << v2[i] << " ";
//     }
//     cout << endl;

//     cout << "-----------decoded result-----------" << endl;
//     vector<uint64_t> r;
//     bit_vector partial_result3 = bit_vector(fresult.getHeight(), 0);
//     decodeBinTrie(fresult, r, partial_result3, 0, 0, fresult.getHeight());
//     for (uint64_t i = 0; i < r.size(); ++i) {
//         cout << r[i] << " ";
//     }
//     cout << endl;

//     cout << "-----------Test Code-----------" << endl;
//     int arr[] = {0, 1, 2, 3};
//     int n = 4;
//     cout << "Before foo function" << endl;
//     for (int i = 0; i < n; ++i) {
//         cout << arr[i] << " ";
//     }
//     cout << endl;
//     foo(arr, n);
//     cout << "After foo function" << endl;
//     for (int i = 0; i < n; ++i) {
//         cout << arr[i] << " ";
//     }
//     cout << endl;
//     return 0;
// }