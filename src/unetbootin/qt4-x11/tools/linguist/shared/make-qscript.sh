#!/bin/sh

me=$(dirname $0)
mkdir -p $me/out
(cd $me/out && ${QLALR-qlalr} --no-debug --troll --no-lines ../qscript.g)

for f in $me/out/*.{h,cpp}; do
    n=$(basename $f)
    p4 open $me/../$n
    cp $f $me/../$n
done

p4 revert -a $me/../...
p4 diff -du $me/../...
