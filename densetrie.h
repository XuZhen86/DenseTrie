#ifndef DENSETRIE_H
#define DENSETRIE_H

#include<map>
#include<memory>
#include<string>
#include<vector>

class DenseTrie{
    public:
    DenseTrie();
    DenseTrie(const DenseTrie &t);
    ~DenseTrie();

    void insert(const char *str);
    void insert(const std::string str);

    void consolidate();

    bool disable(const char *str);
    bool disable(const std::string str);
    bool enable(const char *str);
    bool enable(const std::string str);

    bool contains(const char *str)const;
    bool contains(const std::string str)const;
    bool containsSlot(const char *str)const;
    bool containsSlot(const std::string str)const;

    bool serialize(const char *fileName)const;
    bool serialize(const std::string str)const;
    bool deserialize(const char *fileName);
    bool deserialize(const std::string str);

    void dump();

    private:
    std::unique_ptr<unsigned int[]> trie;
    size_t trieSize;

    std::unique_ptr<std::vector<char>> alphabets;
    std::unique_ptr<std::vector<bool>> isWords;
    std::unique_ptr<std::vector<std::map<const char,const size_t>>> childs;

    char getChar(const size_t index) const;
    bool getIsWord(const size_t index) const;
    bool getIsSize1(const size_t index) const;
    unsigned int getIndex(const size_t index) const;
    void setIsWord(const size_t index,const bool isWord);
    size_t findIndex(const char *str) const;
};

#endif // DENSETRIE_H
