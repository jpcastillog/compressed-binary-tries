#include <iostream> 
#include <vector>

using namespace std;


uint64_t binarySearch(vector<uint64_t> &array, uint64_t low, uint64_t high, uint64_t x, uint64_t size) {
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (array[mid] == x)
			return mid;

		if (array[mid] < x)
			low = mid + 1;

		else
			high = mid - 1;
	}

    if (low > size - 1){
        return high;
    }
    if (high < 0){
        return low;
    }
    
    return array[low] < array[high]?high:low;
}


uint64_t exponentialSearch(vector<uint64_t> &arr, uint64_t n, uint64_t x, uint64_t initial_position) { 
    if (arr[initial_position] == x) 
        return initial_position; 
    if (arr[initial_position] > x)
        return initial_position + 1;
    if (arr[n-1] < x)
        return n-1;
  
    uint64_t i = initial_position + 1; 
    while (i < n && arr[i] <= x) 
        i = i*2; 
    return binarySearch(arr, i/2, min(i, n-1), x, n); 
}


void barbayKenyon(vector<uint64_t> sets[], uint16_t k, vector<uint64_t> &positions, vector<uint64_t> &intersection){
    // eliminator element in [0,0], first element of first set
    uint64_t e = sets[0][0];
    // Set index
    uint64_t i = 1;
    // ocurrences of e
    uint64_t occr = 1;
    // Init actual elements and size of initial set
    uint64_t size = sets[i].size();

    while ( e != -1 ){
        // position of e in ith-set
        uint64_t pos = exponentialSearch(sets[i], size, e, positions[i]);
        if (sets[i][pos] == e){
            occr ++;
            // sets[i]->pos = pos;
            positions[i] = pos;
            if(occr == k){
                intersection.push_back(e);
            }
        } 
        if(occr == k || sets[i][pos] != e){
            // Position remain and size of next set
            uint64_t next_set_pos = positions[i];
            uint64_t next_set_size = sets[i].size();

            // No elements remain in the smallest set
            if (next_set_pos == next_set_size-1){
            // if (pos + 1 > next_set_size - 1){
                e = -1;
                return;
            }

            // e it's part of intersection      
            if (occr == k){
                e = sets[i][pos+1];
                positions[i] = pos + 1;
            }
            // e is not found in actual set
            else{
                // pos it's a succesor index of e 
                e = sets[i][pos];
                positions[i] = pos;
                
            }
            // restart occurrences
            occr = 1;
        }
        // Cyclical index of sets
        i = (i+1)%k;
        size = sets[i].size();
    }
    return;
}