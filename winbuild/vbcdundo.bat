set /p vbcdid= < c:\unetbtin\bcdid
bcdedit /delete {%vbcdid%}
cls
