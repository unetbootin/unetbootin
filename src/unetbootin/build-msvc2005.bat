qmake "DEFINES += STATICWINDOWS" "RESOURCES += unetbootin-windows.qrc unetbootin-sevnz.qrc" "RC_FILE += ubnembed.rc"
nmake
upx --lzma release\unetbootin.exe