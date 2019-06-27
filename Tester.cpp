#include"Tester.h"

#include<DenseTrie/DenseTrie.h>
#include<assert.h>
#include<chrono>
#include<fstream>
#include<map>
#include<random>
using namespace std;

Tester::Tester(){
}

void Tester::testEnableDisable(const char *fileName,unsigned long long trialCount){
    map<string,bool> strToBool;
    vector<string> strings;
    auto t=new DenseTrie();

    fstream fs(fileName,fstream::in);
    string str;
    while(fs>>str){
        strToBool.insert(make_pair(str,true));
        strings.push_back(str);
        t->insert(str.c_str());
    }

    fs.close();
    t->consolidate();

    random_device randomDevice;

    while(trialCount--){
        switch(randomDevice()%3){
            case 0:{
                string str=strings.at(randomDevice()%strings.size());
                strToBool.at(str)=false;
                assert(t->disable(str.c_str()));

                printf("%llu:-: str=%-20s\n",trialCount,str.c_str());
                break;
            }case 1:{
                string str=strings.at(randomDevice()%strings.size());
                strToBool.at(str)=true;
                assert(t->enable(str.c_str()));

                printf("%llu:+: str=%-20s\n",trialCount,str.c_str());
                break;
            }case 2:{
                string str=strings.at(randomDevice()%strings.size());
                assert(t->contains(str.c_str())==strToBool.at(str));

                printf("%llu:?: str=%-20s %s\n",trialCount,str.c_str(),strToBool.at(str)?"true":"false");
                break;
            }
        }
    }
}
