#ifndef TESTER_H
#define TESTER_H

class Tester{
    public:
    Tester();

    void testEnableDisable(const char *fileName,unsigned long long trialCount);

    void testInitPerformance(const char *fileName,unsigned long long trialCount);
    void testQueryPerformance(const char *fileName,unsigned long long trialCount,unsigned long long queryCount);
};

#endif // TESTER_H
