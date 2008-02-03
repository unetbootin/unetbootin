set /p vbcdid= < c:\unetbtin\bcdid
bcdedit /delete {%vbcdid%}
del c:\unetbtin\bcdid
