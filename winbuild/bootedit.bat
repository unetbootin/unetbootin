if not exist c:\config.sys echo rem >> c:\config.sys
attrib -s -r -h c:\config.sys
copy /y c:\config.sys c:\unetbtin\config.bku
copy /y c:\unetbtin\config.sup + c:\unetbtin\config.bku c:\config.sys
attrib +h +r +s c:\config.sys
