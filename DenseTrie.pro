TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        DenseTrie/DenseTrie.cpp \
        Main.cpp

HEADERS += \
    DenseTrie/DenseTrie.h

DISTFILES += \
    WordsList/debugWords.txt \
    WordsList/words.txt \
    WordsList/words2.txt
