if not exist c:\unetbtin\config.sys exit
attrib -s -r -h c:\unetbtin\config.sys
attrib -s -r -h c:\config.sys
xcopy /y c:\unetbtin\config.sys c:\
attrib +h +r +s c:\config.sys
cls
