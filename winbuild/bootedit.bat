if not exist c:\config.sys exit
attrib -s -r -h c:\config.sys
xcopy /y c:\config.sys c:\unetbtin\
copy /y c:\unetbtin\config.sup + c:\unetbtin\config.sys c:\config.sys
attrib +h +r +s c:\config.sys
cls
