if not exist c:\bootmgr exit
del c:\unetbootin\bcdid
bcdedit /create /d "UNetbootin-replacewithubnversion" /application bootsector | c:\unetbootin\tr.exe "{" "\n" | c:\unetbootin\tr.exe "}" "\n" | findstr \- >> c:\unetbootin\bcdid
set /p vbcdid= < c:\unetbootin\bcdid
bcdedit /set {%vbcdid%} device boot
bcdedit /set {%vbcdid%} path \grldr.mbr
bcdedit /displayorder {%vbcdid%} /addlast
