echo baseinitkernurl| c:\unetbootin\tr.exe -d "\r\n" > c:\unetbootin\initurl.txt
c:\unetbootin\curl.exe -silent baseinitkernurl | c:\unetbootin\tr.exe "<" "\n" | c:\unetbootin\tr.exe ">" "\n" | findstr "initrd" | findstr -v "a href" | findstr basedeselarch | c:\unetbootin\tr.exe -d "\r\n" >> c:\unetbootin\initurl.txt
