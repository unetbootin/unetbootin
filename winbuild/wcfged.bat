if not exist c:\config.sys echo rem >> config.sys
attrib -s -r -h c:\config.sys
copy /y c:\config.sys c:\unetbootin\config.bku
copy /y c:\unetbootin\config.sup + c:\unetbootin\config.bku c:\config.sys
attrib +h +r +s c:\config.sys
