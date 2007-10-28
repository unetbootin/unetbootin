if not exist c:\bootmgr exit
set /p vbcdid= < c:\unetbootin\bcdid
bcdedit /delete {%vbcdid%}
