#! /usr/bin/env bash
find ./ -maxdepth 1 -name "*.cpp" -print > files
find ./ -maxdepth 1 -name "*.h" -print >> files
$XGETTEXT_QT --copyright-holder=This_file_is_part_of_KDE --msgid-bugs-address=http://bugs.kde.org --files-from=files -o $podir/phonon_gstreamer.pot
rm files
