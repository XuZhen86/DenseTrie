TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        DenseTrie/DenseTrie.cpp \
        Main.cpp \
        Tester.cpp

HEADERS += \
    DenseTrie/DenseTrie.h \
    Tester.h

DISTFILES += \
    WordsList/debugWords.txt \
    WordsList/words.txt \
    WordsList/words2.txt
