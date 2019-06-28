#include"Tester/Tester.h"

#include<DenseTrie/DenseTrie.h>
#include<assert.h>
#include<algorithm>
#include<chrono>
#include<fstream>
#include<map>
#include<random>
#include<ctime>
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

void Tester::testInitPerformance(const char *fileName,unsigned long long trialCount){
    clock_t totalClock=0;

    // auto t=new DenseTrie();
    for(unsigned long long trial=0;trial<trialCount;trial++){
        clock_t clockStamp=clock();

        auto t=new DenseTrie();
        fstream fs(fileName,fstream::in);
        string str;
        while(fs>>str){
            t->insert(str.c_str());
        }
        t->consolidate();

        totalClock+=clock()-clockStamp;
        delete t;
    }
    printf("%llu,",totalClock/trialCount);

    // auto t=new map<string,bool>();
    for(unsigned long long trial=0;trial<trialCount;trial++){
        clock_t clockStamp=clock();

        auto t=new map<string,bool>();
        fstream fs(fileName,fstream::in);
        string str;
        while(fs>>str){
            t->insert(make_pair(str,true));
        }

        totalClock+=clock()-clockStamp;
        delete t;
    }
    printf("%llu,",totalClock/trialCount);

    // auto t=new vector<pair<string,bool>>();
    for(unsigned long long trial=0;trial<trialCount;trial++){
        clock_t clockStamp=clock();

        auto t=new vector<pair<string,bool>>();
        fstream fs(fileName,fstream::in);
        string str;
        while(fs>>str){
            t->push_back(make_pair(str,true));
        }
        sort(t->begin(),t->end());

        totalClock+=clock()-clockStamp;
        delete t;
    }
    printf("%llu\n",totalClock/trialCount);
}

void Tester::testQueryPerformance(const char *fileName,unsigned long long trialCount,unsigned long long queryCount){
    clock_t totalClock=0;
    random_device random;

    vector<string> strings;
    {
        fstream fs(fileName,fstream::in);
        string str;
        while(fs>>str){
            strings.push_back(str);
        }
    }

    // auto t=new DenseTrie();
    for(unsigned long long trial=0;trial<trialCount;trial++){
        auto t=new DenseTrie();
        fstream fs(fileName,fstream::in);
        string str;
        while(fs>>str){
            t->insert(str.c_str());
        }
        t->consolidate();

        clock_t clockStamp=clock();

        for(unsigned long long query=0;query<queryCount;query++){
            size_t index=random()%strings.size();
            t->contains(strings.at(index).c_str());
        }

        totalClock+=clock()-clockStamp;
        delete t;
    }
    printf("%llu,",totalClock/trialCount);

    // auto t=new map<string,bool>();
    for(unsigned long long trial=0;trial<trialCount;trial++){
        auto t=new map<string,bool>();
        fstream fs(fileName,fstream::in);
        string str;
        while(fs>>str){
            t->insert(make_pair(str,true));
        }

        clock_t clockStamp=clock();

        for(unsigned long long query=0;query<queryCount;query++){
            size_t index=random()%strings.size();
            t->find(strings.at(index));
        }

        totalClock+=clock()-clockStamp;
        delete t;
    }
    printf("%llu,",totalClock/trialCount);

    // auto t=new vector<pair<string,bool>>();
    for(unsigned long long trial=0;trial<trialCount;trial++){
        auto t=new vector<pair<string,bool>>();
        fstream fs(fileName,fstream::in);
        string str;
        while(fs>>str){
            t->push_back(make_pair(str,true));
        }
        sort(t->begin(),t->end());

        clock_t clockStamp=clock();

        for(unsigned long long query=0;query<queryCount;query++){
            size_t index=random()%strings.size();
            binary_search(t->begin(),t->end(),make_pair(strings.at(index),true));
        }

        totalClock+=clock()-clockStamp;
        delete t;
    }
    printf("%llu\n",totalClock/trialCount);
}
