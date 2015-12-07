#! /usr/bin/env bash
find ./ -maxdepth 1 -name "*.cpp" -print > files
find ./ -maxdepth 1 -name "*.h" -print >> files
#$EXTRACTRC `find $dirs -maxdepth 1 \( -name \*.rc -o -name \*.ui -o -name \*.ui3 -o -name \*.ui4 -o -name \*.kcfg \) ` >> rc.cpp || exit 11
$XGETTEXT_QT --copyright-holder=This_file_is_part_of_KDE --msgid-bugs-address=http://bugs.kde.org --files-from=files -o $podir/libphonon.pot
rm files
