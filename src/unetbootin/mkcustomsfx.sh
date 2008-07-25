#!/bin/sh

7z a unetbootin.7z unetbootin.exe unetbootin_custom.qm
cat 7zS.sfx config.txt unetbootin.7z > unetbootin-customsfx-windows.exe
