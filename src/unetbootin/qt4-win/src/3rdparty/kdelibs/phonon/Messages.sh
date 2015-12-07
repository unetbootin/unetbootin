#! /usr/bin/env bash
find ./ -maxdepth 1 -name "*.cpp" -print > files
find ./ -maxdepth 1 -name "*.h" -print >> files
#$EXTRACTRC `find $dirs -maxdepth 1 \( -name \*.rc -o -name \*.ui -o -name \*.ui3 -o -name \*.ui4 -o -name \*.kcfg \) ` >> rc.cpp || exit 11
$XGETTEXT -ktr:1,1t -ktr:1,2c,2t -kQT_TRANSLATE_NOOP:1c,2,2t -kQT_TR_NOOP:1,1t -ktranslate:1c,2,2t -ktranslate:2,3c,3t --files-from=files -o $podir/libphonon.pot
rm files
