#!/bin/bash

files="qquerytransformparser_p.h qquerytransformparser.cpp"

#p4 revert $files
#p4 edit $files
bison --defines=qquerytransformparser_p.h querytransformparser.ypp

addHeaderScript="1{h; r trolltechHeader.txt
  D; }
2{x; G; }"
sed -i -e "$addHeaderScript" $files

sed -i -f winCEWorkaround.sed qquerytransformparser_p.h
#p4 revert -a $files
