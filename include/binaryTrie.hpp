#ifndef BINARYTRIE
#define BINARYTRIE

#include<stdint.h>
#include<vector>

class binaryTrie {
protected:
    uint16_t height_with_runs;
    bool runs_encoded;
    bool empty_trie;
public:
    binaryTrie(){
        height_with_runs = 0;
        runs_encoded = false;
        empty_trie = false;
    };
    virtual ~binaryTrie()=default;
    
    virtual uint16_t getHeight()= 0;
    virtual uint64_t getNode(uint64_t &nodeId, uint16_t level)= 0;
    virtual uint64_t getLeftChild(uint64_t &nodeId, uint16_t level)= 0;
    virtual uint64_t getRightChild(uint64_t &nodeId, uint16_t level)= 0;
    virtual uint64_t size_in_bytes()= 0;
    virtual uint64_t serialize(std::ostream &out)= 0;
    virtual void load(std::istream &in)= 0;
    virtual void encodeRuns()= 0;
    virtual void decode(std::vector<uint64_t> &decoded)= 0;
};
#endif