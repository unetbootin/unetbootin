c:\unetbtin\7z.exe x c:\unetbtin\ubniso.iso -o c:\unetbtin\isotemp
copy c:\unetbtin\isotemp\isolinux\vmlinuz c:\unetbtin\ubnkern
copy c:\unetbtin\isotemp\isolinux\initrd.gz c:\unetbtin\ubninit
copy c:\unetbtin\isotemp\livecd.sqfs c:\livecd.sqfs
deltree c:\unetbtin\isotemp /y
rmdir c:\unetbtin\isotemp /s
cls
