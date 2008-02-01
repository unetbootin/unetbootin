bcdedit /create /d "UNetbootin-replacewithubnversion" /application bootsector | c:\unetbootin\tr.exe "{" "\n" | c:\unetbootin\tr.exe "}" "\n" | find "-" >> c:\unetbootin\bcdid
set /p vbcdid= < c:\unetbootin\bcdid
bcdedit /set {%vbcdid%} device boot
bcdedit /set {%vbcdid%} path \ubnldr.mbr
bcdedit /displayorder {%vbcdid%} /addlast
bcdedit /timeout 30
if not exist c:\config.sys echo rem >> config.sys
attrib -s -r -h c:\config.sys
copy /y c:\config.sys c:\unetbootin\config.bku
copy /y c:\unetbootin\config.sup + c:\unetbootin\config.bku c:\config.sys
attrib +h +r +s c:\config.sysrem cdtu echo baseinitkernurl| c:\unetbootin\tr.exe -d "\r\n" > c:\unetbootin\kernurl.txt
rem cdtu c:\unetbootin\wget.exe -q baseinitkernurl -O - | c:\unetbootin\tr.exe "<" "\n" | c:\unetbootin\tr.exe ">" "\n" | find "vmlinuz" | find /v "a href" | find "basedeselarch" | c:\unetbootin\tr.exe -d "\r\n" >> c:\unetbootin\kernurl.txtrem cdtu echo baseinitkernurl| c:\unetbootin\tr.exe -d "\r\n" > c:\unetbootin\initurl.txt
rem cdtu c:\unetbootin\wget.exe -q baseinitkernurl -O - | c:\unetbootin\tr.exe "<" "\n" | c:\unetbootin\tr.exe ">" "\n" | find "initrd" | find /v "a href" | find "basedeselarch" | c:\unetbootin\tr.exe -d "\r\n" >> c:\unetbootin\initurl.txtrem isdl c:\unetbootin\7z.exe x c:\unetbootin\ubniso.iso -o c:\unetbootin\isotemp
