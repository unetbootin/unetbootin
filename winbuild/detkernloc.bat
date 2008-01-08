echo baseinitkernurl| c:\unetbootin\tr.exe -d "\r\n" > c:\unetbootin\kernurl.txt
c:\unetbootin\curl.exe -silent baseinitkernurl | c:\unetbootin\tr.exe "<" "\n" | c:\unetbootin\tr.exe ">" "\n" | findstr "vmlinuz" | findstr -v "a href" | findstr basedeselarch | c:\unetbootin\tr.exe -d "\r\n" >> c:\unetbootin\kernurl.txt
