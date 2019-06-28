#include"DenseTrie/DenseTrie.h"
#include"Tester.h"
#include<fstream>
#include<iostream>
#include<string>
using namespace std;

void basicUsageExample();
void advancedUsageExample_HostSide();
void advancedUsageExample_TargetSide();

int main(){
//    basicUsageExample();

//    advancedUsageExample_HostSide();
//    advancedUsageExample_TargetSide();

    Tester().testInitPerformance("WordsList/words.txt",10);
    Tester().testQueryPerformance("WordsList/words.txt",1,1000000);
}

void basicUsageExample(){
    DenseTrie *dt=new DenseTrie();

    clog<<"Reading words from WordsList/words.txt"<<endl;
    fstream fs("WordsList/words.txt",fstream::in);
    string str;
    while(fs>>str){
        if(!str.empty())dt->insert(str.c_str());
    }

    clog<<"Consolidating"<<endl;
    dt->consolidate();
    dt->dumpData();

    clog<<"Please input strings: (use Ctrl-D to quit)"<<endl;
    while(cin>>str){
        cout<<str<<" => "<<dt->contains(str.c_str())<<endl;
    }

    clog<<"Finished"<<endl;
    delete dt;
}

void advancedUsageExample_HostSide(){
    DenseTrie *dt=new DenseTrie();

    clog<<"Host: Reading words from WordsList/words.txt"<<endl;
    fstream fs("WordsList/words.txt",fstream::in);
    string str;
    while(fs>>str){
        dt->insert(str.c_str());
    }

    clog<<"Host: Consolidating"<<endl;
    dt->consolidate();

    clog<<"Host: Serializing to WordsList/words.dat"<<endl;
    dt->serialize("WordsList/words.dat");

    clog<<"Host: Finished"<<endl;
    delete dt;
}

void advancedUsageExample_TargetSide(){
    DenseTrie *dt=new DenseTrie();

    clog<<"Target: Deserializing from WordsList/words.dat"<<endl;
    dt->deserialize("WordsList/words.dat");

    clog<<"Target: Please input strings: (use Ctrl-D to quit)"<<endl;
    string str;
    while(cin>>str){
        cout<<str<<" => "<<dt->contains(str.c_str())<<endl;
    }

    clog<<"Target: Finished"<<endl;
    delete dt;
}
