attrib -s -r -h c:\unetbtin\config.sys
attrib -s -r -h c:\config.sys
xcopy /y c:\unetbtin\config.sys c:\
del c:\unetbtin\config.sys
attrib +h +r +s c:\config.sys
cls
