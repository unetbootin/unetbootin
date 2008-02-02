@echo off

set /p vbcdid= < c:\unetbootin\bcdid
bcdedit /delete {%vbcdid%}
del c:\unetbootin\bcdid
if not exist c:\unetbootin\config.bku exit
attrib -s -r -h c:\unetbootin\config.bku
attrib -s -r -h c:\config.sys
copy /y c:\unetbootin\config.bku c:\config.sys
del c:\unetbootin\config.bku
attrib +h +r +s c:\config.sys

cls
