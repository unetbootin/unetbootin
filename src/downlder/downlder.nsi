!define PRODUCT_NAME "UNetbootin"

SetCompressor lzma
RequestExecutionLevel admin

!addplugindir ".\plugins"
!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"


!insertmacro MUI_LANGUAGE "English" ;first language is the default language

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH


; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS


!insertmacro MUI_UNPAGE_FINISH

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
