#ifndef BINARYTRIE
#define BINARYTRIE

#include<stdint.h>
#include<vector>

template<class rankType>
class binaryTrie{
private:
    uint16_t height;
    bool runsEncoded;
    bool emptyTrie;
public:
    binaryTrie()=default;
    ~binaryTrie();
    
    virtual uint64_t getNode(uint64_t &nodeId, uint16_t level);
    virtual uint64_t getLeftChild(uint64_t &nodeId, uint16_t level);
    virtual uint64_t getRightChild(uint64_t &nodeId, uint64_t level);
    virtual uint64_t size_in_bytes();
    virtual void serialize(std::ostrean &out);
    virtual void load(std::istream &in);
    virtual void encodeRuns();
    virtual void decode(std::vector<uint64_t> &decoded);
};
#endif