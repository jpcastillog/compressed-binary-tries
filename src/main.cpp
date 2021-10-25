#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include "binTrie.hpp"
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

int main(){
    // int_vector<> s = {1, 2, 3, 5};
    int_vector<> s1 = {1, 3, 7, 8, 9, 11, 12};
    int_vector<> s2 = {2, 5, 7, 12, 15};
    
    binTrie b1 = binTrie(s1);
    binTrie b2 = binTrie(s2);
    cout << "-----------b1-----------" << endl;
    for (int i = 0; i < b1.bTrie.size(); ++i) {
        // cout << b.bTrie[i]<< endl;
        for (int j = 0; j < b1.bTrie[i].size(); ++(++j)) {
            cout << b1.bTrie[i][j] << b1.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }
    cout << "-----------b2-----------" << endl;
    for (int i = 0; i < b2.bTrie.size(); ++i) {
        // cout << b.bTrie[i]<< endl;
        for (int j = 0; j < b2.bTrie[i].size(); ++(++j)) {
            cout << b2.bTrie[i][j] << b2.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }
    // cout << b1.bv_rank[3](5) << endl;

    vector<binTrie> Bs = {b1, b2};
    binTrie result = intersectTries(Bs);
    cout << "-----------resultV1-----------" << endl;
    for (int i = 0; i < result.bTrie.size(); ++i) {
        // result.bTrie[i].resize(last_pos[i]);
        for (int j = 0; j < result.bTrie[i].size(); ++(++j)) {
            cout << result.bTrie[i][j] << result.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }

    binTrie result2 = intersectTriesV2(Bs);
    cout << "-----------resultV2-----------" << endl;
    for (int i = 0; i < result2.bTrie.size(); ++i) {
        // result.bTrie[i].resize(last_pos[i]);
        for (int j = 0; j < result2.bTrie[i].size(); ++(++j)) {
            cout << result2.bTrie[i][j] << result2.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }

    cout << "-----------decoded b1-----------" << endl;
    vector<uint64_t> v1;
    bit_vector partial_result = bit_vector(b1.getHeight(), 0);
    decodeBinTrie(b1, v1, partial_result, 0, 0, b1.getHeight());
    for (uint64_t i = 0; i < v1.size(); ++i) {
        cout << v1[i] << " ";
    }
    cout << endl;
    
    cout << "-----------decoded b2-----------" << endl;
    vector<uint64_t> v2;
    bit_vector partial_result2 = bit_vector(b1.getHeight(), 0);
    decodeBinTrie(b2, v2, partial_result2, 0, 0, b2.getHeight());
    for (uint64_t i = 0; i < v2.size(); ++i) {
        cout << v2[i] << " ";
    }
    cout << endl;

    cout << "-----------Test Code-----------" << endl;
    int arr[] = {0, 1, 2, 3};
    int n = 4;
    cout << "Before foo function" << endl;
    for (int i = 0; i < n; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
    foo(arr, n);
    cout << "After foo function" << endl;
    for (int i = 0; i < n; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}