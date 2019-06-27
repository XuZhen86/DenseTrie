#include"DenseTrie.h"

#include<fstream>
#include<iostream>
using namespace std;

//#define PRINT_CONTENTS

DenseTrie::DenseTrie(){
    static_assert(sizeof(unsigned int)==4,"sizeof(unsigned int)!=4");

    this->trieSize=0;
    this->trie=nullptr;

    this->alphabets=nullptr;
    this->isWords=nullptr;
    this->childs=nullptr;
}

DenseTrie::DenseTrie(const DenseTrie &dt):DenseTrie(){
    this->trieSize=dt.trieSize;

    if(dt.trie!=nullptr){
        this->trie=new unsigned int[this->trieSize];
        copy(dt.trie,dt.trie+dt.trieSize,this->trie);
    }

    if(dt.alphabets!=nullptr){
        this->alphabets=new vector<char>();
        copy(dt.alphabets->begin(),dt.alphabets->end(),this->alphabets->begin());
    }

    if(dt.isWords!=nullptr){
        this->isWords=new vector<bool>();
        copy(dt.isWords->begin(),dt.isWords->end(),this->isWords->begin());
    }

    if(dt.childs!=nullptr){
        this->childs=new vector<map<const char,const size_t>>();
        for(auto &e:*dt.childs){
            this->childs->push_back(e);
        }
    }
}

DenseTrie::~DenseTrie(){
    delete this->trie;
    delete this->alphabets;
    delete this->isWords;
    delete this->childs;
}

void DenseTrie::insert(const char *str){
    if(this->alphabets==nullptr||this->isWords==nullptr||this->childs==nullptr){
        this->alphabets=new vector<char>(1,0);
        this->isWords=new vector<bool>(1,false);
        this->childs=new vector<map<const char,const size_t>>();
        this->childs->emplace_back();
    }

    if(str[0]==0){
        return;
    }

    size_t nodeNum=0;
    bool isNewSubstring=false;

    for(size_t strIndex=0;str[strIndex];strIndex++){
        const char currentChar=str[strIndex];

        if(isNewSubstring||this->childs->at(nodeNum).count(currentChar)==0){
            isNewSubstring=true;

            this->childs->at(nodeNum).insert(make_pair(currentChar,this->childs->size()));
            this->alphabets->push_back(currentChar);
            this->isWords->push_back(false);
            this->childs->emplace_back();
        }

        nodeNum=this->childs->at(nodeNum).at(currentChar);
    }

    this->isWords->at(nodeNum)=true;


}

void DenseTrie::consolidate(){
#ifdef PRINT_CONTENTS
    for(size_t i=0;i<alphabets->size();i++){
        fprintf(stderr,"[%lu]={%c,%d,",i,alphabets->at(i)==0?'_':alphabets->at(i),static_cast<bool>(isWords->at(i)));
        for(auto j:childs->at(i)){
            fprintf(stderr,"{%c,%lu},",j.first,j.second);
        }
        fprintf(stderr,"}\n");
    }
    fprintf(stderr,"\n");
#endif

    vector<size_t> nodeNum2Idx(1,1);
    for(size_t nodeNum=1;nodeNum<this->alphabets->size();nodeNum++){
        nodeNum2Idx.push_back(
                    nodeNum2Idx.back()
                    +this->childs->at(nodeNum-1).size()
                    +(this->childs->at(nodeNum).size()>1));
    }
    nodeNum2Idx.at(0)=0;

#ifdef PRINT_CONTENTS
    for(size_t i=0;i<nodeNum2Idx.size();i++){
        fprintf(stderr,"nodeNum2Idx[%lu]=%lu\n",i,nodeNum2Idx.at(i));
    }
    fprintf(stderr,"\n");
#endif

    vector<unsigned int> trie;
    if(this->childs->at(0).size()==1){
        trie.push_back(1);
    }

    for(size_t nodeNum=0;nodeNum<this->alphabets->size();nodeNum++){
        if(!this->childs->at(nodeNum).empty()){
            if(this->childs->at(nodeNum).size()>1){
                trie.push_back(static_cast<unsigned int>(this->childs->at(nodeNum).size()));
            }

            for(auto nextNode:this->childs->at(nodeNum)){
                const char nextNodeChar=nextNode.first;
                const size_t nextNodeNum=nextNode.second;

                trie.push_back(
                            static_cast<unsigned int>(nextNodeChar)<<24
                            |static_cast<unsigned int>(this->isWords->at(nextNodeNum))<<23
                            |static_cast<unsigned int>(this->childs->at(nextNodeNum).size()==1)<<22
                            |static_cast<unsigned int>(this->childs->at(nextNodeNum).empty()?0:nodeNum2Idx.at(nextNodeNum)));
            }
        }
    }

    this->trieSize=trie.size();
    this->trie=new unsigned int[this->trieSize];
    copy(trie.begin(),trie.end(),this->trie);

    delete this->alphabets;
    delete this->isWords;
    delete this->childs;
    this->alphabets=nullptr;
    this->isWords=nullptr;
    this->childs=nullptr;
}

bool DenseTrie::disable(const char *str){
    size_t index=this->findIndex(str);
    if(index){
        this->setIsWord(index,false);
        return true;
    }
    return false;
}

bool DenseTrie::enable(const char *str){
    size_t index=this->findIndex(str);
    if(index){
        this->setIsWord(index,true);
        return true;
    }
    return false;
}

bool DenseTrie::contains(const char *str){
    size_t index=this->findIndex(str);
    if(index){
        return this->getIsWord(index);
    }
    return false;
}

bool DenseTrie::containsSlot(const char *str){
    size_t index=this->findIndex(str);
    return index;
}

void DenseTrie::serialize(const char *fileName){
    fstream fs(fileName,fstream::out|fstream::binary);
    fs.write(reinterpret_cast<const char *>(this->trie),static_cast<long long>(this->trieSize*sizeof(unsigned int)));
    fs.close();
}

void DenseTrie::deserialize(const char *fileName){
    fstream fs(fileName,fstream::in|fstream::binary);

    fs.seekg(0,ios_base::end);
    this->trieSize=static_cast<size_t>(fs.tellg())/sizeof(unsigned int);
    fs.seekg(0,ios_base::beg);

    this->trie=new unsigned int[this->trieSize];
    fs.read(reinterpret_cast<char *>(this->trie),static_cast<long long>(this->trieSize*sizeof(unsigned int)));
    fs.close();
}

void DenseTrie::dumpData(){
    for(size_t i=0;i<this->trieSize;i++){
        if(this->trie[i]<0x100){
            printf("trie[%lu]=%d\n",i,this->trie[i]);
        }else{
            printf("trie[%lu]={char=%c,isWord=%d,isSize1=%d,index=%u}\n",
                   i,
                   this->getChar(i),
                   this->getIsWord(i),
                   this->getIsSize1(i),
                   this->getIndex(i));
        }
    }
}

char DenseTrie::getChar(const size_t index) const{
    return static_cast<char>(this->trie[index]>>24);
}

bool DenseTrie::getIsWord(const size_t index) const{
    return static_cast<bool>(this->trie[index]&0x800000);
}

bool DenseTrie::getIsSize1(const size_t index) const{
    return static_cast<bool>(this->trie[index]&0x400000);
}

unsigned int DenseTrie::getIndex(const size_t index) const{
    return static_cast<unsigned int>(this->trie[index]&0x3fffff);
}

void DenseTrie::setIsWord(const size_t index,const bool isWord){
    if(isWord){
        this->trie[index]|=static_cast<unsigned int>(0x800000);
    }else{
        this->trie[index]&=~static_cast<unsigned int>(0x800000);
    }
}

size_t DenseTrie::findIndex(const char *str) const{
    if(!str[0]||!this->trieSize){
        return 0;
    }

    size_t idx=1,size=this->trie[0],strIdx;

    for(strIdx=0;str[strIdx];strIdx++){
        bool foundNextIdx=false;

        for(size_t nextIdx=idx;nextIdx<idx+size;nextIdx++){
            if(this->getChar(nextIdx)==str[strIdx]){
                foundNextIdx=true;

                if(!str[strIdx+1]){
                    return nextIdx;
                }

                if(this->getIsSize1(nextIdx)){
                    idx=this->getIndex(nextIdx);
                    size=1;
                }else{
                    idx=this->getIndex(nextIdx);
                    if(!idx){
                        return 0;
                    }
                    size=this->trie[this->getIndex(nextIdx)-1];
                }

                break;
            }
        }

        if(!foundNextIdx){
            return 0;
        }
    }

    return 0;
}
