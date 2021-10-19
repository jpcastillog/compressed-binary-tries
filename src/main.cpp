#include <iostream>
#include <sdsl/int_vector.hpp>
#include "binTrie.hpp"
#include <ctime>


int main(){
    // int_vector<> s = {1, 2, 3, 5};
    int_vector<> s = {1, 3, 7, 8, 9, 11, 12};
    binTrie b = binTrie(s);
    
    for (int i = 0; i < b.bTrie.size(); ++i) {
        // cout << b.bTrie[i]<< endl;
        for (int j = 0; j < b[i].size(); ++(++j)) {
            cout << b.bTrie[i][j] << b.bTrie[i][j+1] << " ";
        }
        cout << endl;
    }
    return 0;

}