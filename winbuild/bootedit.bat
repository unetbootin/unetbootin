@echo off

bcdedit /create /d "UNetbootin-replacewithubnversion" /application bootsector | c:\unetbtin\tr.exe "{" "\n" | c:\unetbtin\tr.exe "}" "\n" | find "-" >> c:\unetbtin\bcdid
set /p vbcdid= < c:\unetbtin\bcdid
bcdedit /set {%vbcdid%} device boot
bcdedit /set {%vbcdid%} path \ubnldr.mbr
bcdedit /displayorder {%vbcdid%} /addlast
bcdedit /timeout 30
if not exist c:\config.sys echo rem >> config.sys
attrib -s -r -h c:\config.sys
copy /y c:\config.sys c:\unetbtin\config.bku
copy /y c:\unetbtin\config.sup + c:\unetbtin\config.bku c:\config.sys
attrib +h +r +s c:\config.sys
rem cdtu echo baseinitkernurl| c:\unetbtin\tr.exe -d "\r\n" > c:\unetbtin\kernurl.txt
rem cdtu c:\unetbtin\wget.exe -q baseinitkernurl -O - | c:\unetbtin\tr.exe "<" "\n" | c:\unetbtin\tr.exe ">" "\n" | find "vmlinuz" | find /v "a href" | find "basedeselarch" | c:\unetbtin\tr.exe -d "\r\n" >> c:\unetbtin\kernurl.txtrem cdtu echo baseinitkernurl| c:\unetbtin\tr.exe -d "\r\n" > c:\unetbtin\initurl.txt
rem cdtu c:\unetbtin\wget.exe -q baseinitkernurl -O - | c:\unetbtin\tr.exe "<" "\n" | c:\unetbtin\tr.exe ">" "\n" | find "initrd" | find /v "a href" | find "basedeselarch" | c:\unetbtin\tr.exe -d "\r\n" >> c:\unetbtin\initurl.txtrem isdl c:\unetbtin\7z.exe x c:\unetbtin\ubniso.iso -o c:\unetbtin\isotemp

cls
