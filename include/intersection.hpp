#ifndef INTERSECTION
#define INTERSECTION

#include <vector>
#include "binaryTrie.hpp"

using namespace std;

vector<uint64_t> forceBruteIntersection(vector<vector<uint64_t>> &S);
vector<uint64_t> Intersect(vector<binaryTrie*> &Ts, bool runs, bool parallel);

#endif