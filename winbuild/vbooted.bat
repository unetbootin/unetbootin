if not exist c:\bootmgr exit
del c:\unetbootin\bcdid
bcdedit /create /d "UNetbootin-replacewithubnversion" /application bootsector | c:\unetbootin\tr.exe "{" "\n" | c:\unetbootin\tr.exe "}" "\n" | find "-" >> c:\unetbootin\bcdid
set /p vbcdid= < c:\unetbootin\bcdid
bcdedit /set {%vbcdid%} device boot
bcdedit /set {%vbcdid%} path \ubnldr.mbr
bcdedit /displayorder {%vbcdid%} /addlast
bcdedit /timeout 30
