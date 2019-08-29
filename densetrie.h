#ifndef DENSETRIE_H
#define DENSETRIE_H

#include<vector>
#include<map>

class DenseTrie{
    public:
    DenseTrie();
    DenseTrie(const DenseTrie &t);
    ~DenseTrie();

    void insert(const char *str);
    void consolidate();

    bool disable(const char *str);
    bool enable(const char *str);
    bool contains(const char *str);
    bool containsSlot(const char *str);

    bool serialize(const char *fileName);
    bool deserialize(const char *fileName);

    void dump();

    private:
    unsigned int *trie;
    size_t trieSize;

    std::vector<char> *alphabets;
    std::vector<bool> *isWords;
    std::vector<std::map<const char,const size_t>> *childs;

    char getChar(const size_t index) const;
    bool getIsWord(const size_t index) const;
    bool getIsSize1(const size_t index) const;
    unsigned int getIndex(const size_t index) const;
    void setIsWord(const size_t index,const bool isWord);
    size_t findIndex(const char *str) const;
};

#endif // DENSETRIE_H
