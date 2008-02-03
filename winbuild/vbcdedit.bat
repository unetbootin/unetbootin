bcdedit /create /d "UNetbootin-replacewithubnversion" /application bootsector | c:\unetbtin\tr.exe "{" "\n" | c:\unetbtin\tr.exe "}" "\n" | find "-" >> c:\unetbtin\bcdid
set /p vbcdid= < c:\unetbtin\bcdid
bcdedit /set {%vbcdid%} device boot
bcdedit /set {%vbcdid%} path \ubnldr.mbr
bcdedit /displayorder {%vbcdid%} /addlast
bcdedit /timeout 30
cls
