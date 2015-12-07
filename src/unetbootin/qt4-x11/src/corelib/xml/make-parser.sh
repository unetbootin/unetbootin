#!/bin/sh

me=$(dirname $0)
mkdir -p $me/out
(cd $me/out && ../../../../util/qlalr/qlalr --troll --no-debug --no-lines ../qxmlstream.g)

for f in $me/out/*.h; do
    n=$(basename $f)
    p4 open $n
    cp $f $n
done

p4 revert -a ...
p4 diff -dub ...

