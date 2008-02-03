echo baseinitkernurl| c:\unetbtin\tr.exe -d "\r\n" > c:\unetbtin\kernurl.txt
c:\unetbtin\wget.exe -q baseinitkernurl -O - | c:\unetbtin\tr.exe "<" "\n" | c:\unetbtin\tr.exe ">" "\n" | find "vmlinuz" | find /v "a href" | find "basedeselarch" | c:\unetbtin\tr.exe -d "\r\n" >> c:\unetbtin\kernurl.txt
echo baseinitkernurl| c:\unetbtin\tr.exe -d "\r\n" > c:\unetbtin\initurl.txt
c:\unetbtin\wget.exe -q baseinitkernurl -O - | c:\unetbtin\tr.exe "<" "\n" | c:\unetbtin\tr.exe ">" "\n" | find "initrd" | find /v "a href" | find "basedeselarch" | c:\unetbtin\tr.exe -d "\r\n" >> c:\unetbtin\initurl.txtrem isdl c:\unetbtin\7z.exe x c:\unetbtin\ubniso.iso -o c:\unetbtin\isotemp
cls
