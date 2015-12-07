#!/bin/sh
p4 edit ../keywords.cpp
p4 edit ../ppkeywords.cpp
qmake
make
cat licenseheader.txt > ../keywords.cpp
cat licenseheader.txt > ../ppkeywords.cpp
./generate_keywords >> ../keywords.cpp
./generate_keywords preprocessor >> ../ppkeywords.cpp
p4 revert -a ../keywords.cpp
p4 revert -a ../ppkeywords.cpp
