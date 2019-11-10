#include"densetrie.h"

#include<algorithm>
#include<fstream>

DenseTrie::DenseTrie(){
    // The length of unsigned int should be 4 to ensure expected behavior
    static_assert(sizeof(unsigned int)==4,"sizeof(unsigned int)!=4");

    trieSize=0;
}

// Deep copy constructor
DenseTrie::DenseTrie(const DenseTrie &t){
    trieSize=t.trieSize;

    // If object exists, use the old-school way to copy them over
    if(t.trie!=nullptr){
        trie.reset(new unsigned int[trieSize]);
        std::copy(t.trie.get(),t.trie.get()+t.trieSize,trie.get());
    }

    if(t.alphabets!=nullptr){
        alphabets.reset(new std::vector<char>());
        std::copy(t.alphabets->begin(),t.alphabets->end(),alphabets->begin());
    }

    if(t.isWords!=nullptr){
        isWords.reset(new std::vector<bool>());
        std::copy(t.isWords->begin(),t.isWords->end(),isWords->begin());
    }

    if(t.childs!=nullptr){
        childs.reset(new std::vector<std::map<const char,const size_t>>());
        for(auto &e:*t.childs){
            childs->push_back(e);
        }
    }
}

DenseTrie::~DenseTrie(){
    // Auto release memory with unique_ptr
}

void DenseTrie::insert(const char *str){
    // Allocate memory only for the first time
    if(alphabets==nullptr||isWords==nullptr||childs==nullptr){
        alphabets.reset(new std::vector<char>(1,0));
        isWords.reset(new std::vector<bool>(1,false));
        childs.reset(new std::vector<std::map<const char,const size_t>>());
        childs->emplace_back();
    }

    // Skip if it is an empty string
    if(str[0]==0){
        return;
    }

    size_t nodeNum=0;
    bool isNewSubstring=false;

    // Folow the tree and insert the string
    for(size_t strIndex=0;str[strIndex];strIndex++){
        const char currentChar=str[strIndex];

        // If node has been insert previously, or current node does not exist, insert a new node
        if(isNewSubstring||childs->at(nodeNum).count(currentChar)==0){
            isNewSubstring=true;

            childs->at(nodeNum).insert(std::make_pair(currentChar,childs->size()));
            alphabets->push_back(currentChar);
            isWords->push_back(false);
            childs->emplace_back();
        }

        nodeNum=childs->at(nodeNum).at(currentChar);
    }

    isWords->at(nodeNum)=true;
}

void DenseTrie::insert(const std::string str){
    insert(str.c_str());
}

void DenseTrie::consolidate(){
    // It's complicated...
    std::vector<size_t> nodeNum2Idx(1,1);
    for(size_t nodeNum=1;nodeNum<alphabets->size();nodeNum++){
        nodeNum2Idx.push_back(
                    nodeNum2Idx.back()
                    +childs->at(nodeNum-1).size()
                    +(childs->at(nodeNum).size()>1)
        );
    }
    nodeNum2Idx.at(0)=0;

    std::vector<unsigned int> trie;
    // Always push node0's size
    // If node0's size >1, it will be pushed later
    if(childs->at(0).size()==1){
        trie.push_back(1);
    }

    for(size_t nodeNum=0;nodeNum<alphabets->size();nodeNum++){
        // Skip empty node
        if(childs->at(nodeNum).empty()){
            continue;
        }

        // Push the size if >1
        if(childs->at(nodeNum).size()>1){
            trie.push_back(static_cast<unsigned int>(childs->at(nodeNum).size()));
        }

        // Push all childs
        for(auto nextNode:childs->at(nodeNum)){
            const char nextNodeChar=nextNode.first;
            const size_t nextNodeNum=nextNode.second;

            trie.push_back(
                        static_cast<unsigned int>(nextNodeChar)<<24
                        |static_cast<unsigned int>(isWords->at(nextNodeNum))<<23
                        |static_cast<unsigned int>(childs->at(nextNodeNum).empty()?0:nodeNum2Idx.at(nextNodeNum))
            );
        }
    }

    // Copy processed data
    trieSize=trie.size();
    this->trie.reset(new unsigned int[trieSize]);
    copy(trie.begin(),trie.end(),this->trie.get());

    // Cleanup used objects
    alphabets.reset();
    isWords.reset();
    childs.reset();
}

bool DenseTrie::disable(const char *str){
    size_t index=findIndex(str);

    if(index){
        setIsWord(index,false);
        return true;
    }

    return false;
}

bool DenseTrie::disable(const std::string str){
    return disable(str.c_str());
}

bool DenseTrie::enable(const char *str){
    size_t index=findIndex(str);

    if(index){
        setIsWord(index,true);
        return true;
    }

    return false;
}

bool DenseTrie::enable(const std::string str){
    return enable(str.c_str());
}

bool DenseTrie::contains(const char *str)const{
    size_t index=findIndex(str);

    if(index){
        return getIsWord(index);
    }

    return false;
}

bool DenseTrie::contains(const std::string str)const{
    return contains(str.c_str());
}

bool DenseTrie::containsSlot(const char *str)const{
    // findIndex() returns 0 if slot is not found
    return findIndex(str);
}

bool DenseTrie::containsSlot(const std::string str)const{
    return containsSlot(str.c_str());
}

bool DenseTrie::serialize(const char *fileName)const{
    std::fstream f(fileName,std::fstream::out|std::fstream::binary);

    if(!f.is_open()){
        return false;
    }

    // Dump trie content to file
    f.write(reinterpret_cast<const char *>(trie.get()),static_cast<long long>(trieSize*sizeof(unsigned int)));
    f.close();

    return true;
}

bool DenseTrie::serialize(const std::string str)const{
    return serialize(str.c_str());
}

bool DenseTrie::deserialize(const char *fileName){
    std::fstream f(fileName,std::fstream::in|std::fstream::binary);

    if(!f.is_open()){
        return false;
    }

    // Get trie size and allocate memory
    f.seekg(0,std::ios_base::end);
    trieSize=static_cast<size_t>(f.tellg())/sizeof(unsigned int);
    trie.reset(new unsigned int[trieSize]);
    f.seekg(0,std::ios_base::beg);

    // Dump file content to trie
    f.read(reinterpret_cast<char *>(trie.get()),static_cast<long long>(trieSize*sizeof(unsigned int)));
    f.close();

    return true;
}

bool DenseTrie::deserialize(const std::string str){
    return deserialize(str.c_str());
}

void DenseTrie::dump(){
    // Check for trie==nullptr is omitted

    for(size_t i=0;i<trieSize;i++){
        // If it is a size node, pnly print the size
        if(trie[i]<0x100){
            printf("trie[%lu]=%d\n",i,trie[i]);
            continue;
        }

        // Otherwise decompose the node and print its contents
        printf(
                    "trie[%lu]={char=%c,isWord=%d,index=%u}\n",
                    i,
                    getChar(i),
                    getIsWord(i),
                    getIndex(i)
        );
    }
}

char DenseTrie::getChar(const size_t index) const{
    // Bit 31 to 24
    return static_cast<char>(trie[index]>>24);
}

bool DenseTrie::getIsWord(const size_t index) const{
    // Bit 23
    return static_cast<bool>(trie[index]&0x800000);
}

bool DenseTrie::checkIfIndex(const size_t index) const{
    // Check if only contains an index. With this datastructure,
    // it is gaurenteed to have a valid number after bit 22
    // Since '\0' is not a readable character
    return static_cast<bool>(!(trie[index] >> 23));
}

unsigned int DenseTrie::getIndex(const size_t index) const{
    // Bit 22 to 0
    return static_cast<unsigned int>(trie[index]&0x7fffff);
}

void DenseTrie::setIsWord(const size_t index,const bool isWord){
    if(isWord){
        // Set bit 23
        trie[index]|=static_cast<unsigned int>(0x800000);
    }else{
        // Clear bit 23
        trie[index]&=~static_cast<unsigned int>(0x800000);
    }
}

size_t DenseTrie::findIndex(const char *str) const{
    // Skip if empty string or no trie yet
    if(!str[0]||!trieSize){
        return 0;
    }

    size_t idx=1,size;

    for(size_t strIdx=0;str[strIdx];strIdx++){
        bool foundNextIdx=false;
        
        // Assume there are multiple starting characters, establish size for loop
        size = checkIfIndex(idx - 1) ? trie[idx - 1] : 1;
        
        for (size_t nextIdx = idx;nextIdx < idx + size;nextIdx++) {
            // Skip to next index if char is different
            if (getChar(nextIdx) != str[strIdx]) {
                continue;
            }

            // If this is the last char, return answer
            if (!str[strIdx + 1]) {
                return nextIdx;
            }

            idx = getIndex(nextIdx);
            foundNextIdx = true;
            break;
        }

        // Did not find index for next char, return not found
        if(!foundNextIdx){
            return 0;
        }
    }

    // This case should not happen
    return 0;
}
