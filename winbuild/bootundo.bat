@echo off

set /p vbcdid= < c:\unetbtin\bcdid
bcdedit /delete {%vbcdid%}
del c:\unetbtin\bcdid
if not exist c:\unetbtin\config.bku exit
attrib -s -r -h c:\unetbtin\config.bku
attrib -s -r -h c:\config.sys
copy /y c:\unetbtin\config.bku c:\config.sys
del c:\unetbtin\config.bku
attrib +h +r +s c:\config.sys

cls
