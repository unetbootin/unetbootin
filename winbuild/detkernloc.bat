echo baseinitkernurl| c:\unetbootin\tr.exe -d "\r\n" > c:\unetbootin\kernurl.txt
c:\unetbootin\wget.exe -q baseinitkernurl -O - | c:\unetbootin\tr.exe "<" "\n" | c:\unetbootin\tr.exe ">" "\n" | find "vmlinuz" | find /v "a href" | find "basedeselarch" | c:\unetbootin\tr.exe -d "\r\n" >> c:\unetbootin\kernurl.txt
