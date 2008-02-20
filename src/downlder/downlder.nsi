!define PRODUCT_NAME "UNetbootin"

RequestExecutionLevel admin

!addplugindir ".\plugins"

var /GLOBAL vardldurl
var /GLOBAL varoutloc

Name "${PRODUCT_NAME}"
OutFile "downlder.exe"
InstallDir "C:\."
ShowInstDetails show
ShowUnInstDetails show

AutoCloseWindow true

Section "MainSection" SEC01

FileOpen $4 "$EXEDIR\dlurl.txt" r
FileRead $4 $vardldurl
FileClose $4

FileOpen $4 "$EXEDIR\outfile.txt" r
FileRead $4 $varoutloc
FileClose $4

InetLoad::load /RESUME "Download failed" "$vardldurl" "$varoutloc"

SectionEnd
