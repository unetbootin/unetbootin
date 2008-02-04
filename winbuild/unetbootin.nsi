!define PRODUCT_NAME "UNetbootin"
!define PROGRAM_NAME "${PRODUCT_NAME}"
!define PRODUCT_VERSION "replacewithubnversion"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\grub.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma
RequestExecutionLevel admin

!addplugindir ".\plugins"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include LogicLib.nsh

; MUI Settings
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

; MUI end ------

; cdtu var /GLOBAL varkernurl
; cdtu var /GLOBAL variniturl
var /GLOBAL varwinvers

VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "Comments" "Licenced under the GNU General Public License version 2 or later. Website is at lubi.sourceforge.net"
VIAddVersionKey "LegalCopyright" "Geza Kovacs"
VIAddVersionKey "FileDescription" "UNetbootin, the Netboot-based Installer"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"
VIProductVersion "ubnbzrsubversionum.1.1.1"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "setup.exe"
InstallDir "C:\."
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
 
  ReadRegStr $R0 HKLM \
  "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}" \
  "UninstallString"
  StrCmp $R0 "" done
 
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "${PROGRAM_NAME} is already installed. $\n$\nClick `OK` to remove the \
  previous version, then run this installer again." \
  IDOK uninst
  Abort
  
;Run the uninstaller
uninst:
  ClearErrors
  ExecShell "" "$INSTDIR\unetbtin\uninst.exe"
  Abort

done:
 
FunctionEnd

Section "MainSection" SEC01

  SetOutPath "$INSTDIR\unetbtin"
  SetOverwrite on
  File "ubnkern"
  File "ubninit"
  File "bootedit.bat"
  File "bootedit.pif"
  File "bootundo.bat"
  File "bootundo.pif"
  File "vbcdedit.bat"
  File "vbcdedit.pif"
  File "vbcdundo.bat"
  File "vbcdundo.pif"
  File "config.sup"
  File "tr.exe"
  File "menu.lst"
  File "runxfile.exe"
  File "emtxfile.exe"
  ; cdtu File "wget.exe"
  ; cdtu File "dtkernlc.bat"
  ; cdtu File "dtkernlc.pif"
  ; isdl File "7z.dll"
  ; isdl File "7z.exe"
  ; isdl File "isoexrct.bat"
  ; isdl File "isoexrct.pif"

  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "ubnldr.exe"
  File "ubnldr"
  File "ubnldr.mbr"

WriteRegStr HKEY_LOCAL_MACHINE SOFTWARE\Microsoft\WIndows\CurrentVersion\RunOnce "UNetbootin Uninstaller" "c:\unetbtin\uninst.exe"

  ; ltbe IfFileExists "$INSTDIR\unetbtin\ubnkern" ltbekdlok
  ; ltbe InetLoad::load /RESUME "Download failed, would you like to resume" "rpubnkernurl" "$INSTDIR\unetbtin\ubnkern"
  ; ltbe   Pop $0
  ; ltbe   StrCmp $0 "OK" ltbekdlok
  ; ltbe   MessageBox MB_OK|MB_ICONEXCLAMATION "Download Error, click OK to abort installation" /SD IDOK
  ; ltbe   Abort
  ; ltbe ltbekdlok:
  ; ltbe InetLoad::load /RESUME "Download failed, would you like to resume" "rpubniniturl" "$INSTDIR\unetbtin\ubninit"
  ; ltbe   Pop $0
  ; ltbe   StrCmp $0 "OK" ltbeidlok
  ; ltbe   MessageBox MB_OK|MB_ICONEXCLAMATION "Download Error, click OK to abort installation" /SD IDOK
  ; ltbe   Abort
  ; ltbe ltbeidlok:

  ; isdl InetLoad::load /RESUME "Download failed, would you like to resume" "isourloc" "$INSTDIR\unetbtin\ubniso.iso"
  ; isdl   Pop $0
  ; isdl   StrCmp $0 "OK" isdldlok
  ; isdl   MessageBox MB_OK|MB_ICONEXCLAMATION "Download Error, click OK to abort installation" /SD IDOK
  ; isdl   Abort
  ; isdl isdldlok:

  ; cdtu IfFileExists "$INSTDIR\unetbtin\ubnkern" cdtukdlok
  ; cdtu ${If} $varwinvers >= 6.0
  ; cdtu    ExecWait '"c:\unetbtin\runxfile.exe" "c:\unetbtin\dtkernlc.bat" runas'
  ; cdtu ${Else}
  ; cdtu    ExecWait '"c:\unetbtin\runxfile.exe" "c:\unetbtin\dtkernlc.bat"'
  ; cdtu ${EndIf}
  ; cdtu FileOpen $4 "c:\unetbtin\kernurl.txt" r
  ; cdtu FileRead $4 $varkernurl
  ; cdtu FileClose $4
  ; cdtu InetLoad::load /RESUME "Download failed, would you like to resume" "$varkernurl" "$INSTDIR\unetbtin\ubnkern"
  ; cdtu   Pop $0
  ; cdtu   StrCmp $0 "OK" cdtukdlok
  ; cdtu   MessageBox MB_OK|MB_ICONEXCLAMATION "Download Error, click OK to abort installation" /SD IDOK
  ; cdtu   Abort
  ; cdtu cdtukdlok:
  ; cdtu FileOpen $4 "c:\unetbtin\initurl.txt" r
  ; cdtu FileRead $4 $variniturl
  ; cdtu FileClose $4
  ; cdtu InetLoad::load /RESUME "Download failed, would you like to resume" "$variniturl" "$INSTDIR\unetbtin\ubninit"
  ; cdtu   Pop $0
  ; cdtu   StrCmp $0 "OK" cdtuidlok
  ; cdtu   MessageBox MB_OK|MB_ICONEXCLAMATION "Download Error, click OK to abort installation" /SD IDOK
  ; cdtu   Abort
  ; cdtu cdtuidlok:

  ReadRegStr $varwinvers HKLM \
  "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

  ${If} $varwinvers >= 6.0
     ExecWait '"c:\unetbtin\emtxfile.exe" "c:\unetbtin\vbcdedit.bat" runas'
     IfFileExists "c:\unetbtin\bcdid" vbtedfin
     ExecWait '"c:\unetbtin\runxfile.exe" "c:\unetbtin\vbcdedit.bat" runas'
     vbtedfin:
  ${Else}
     ExecWait '"c:\unetbtin\runxfile.exe" "c:\unetbtin\bootedit.bat"'
  ${EndIf}

  SetFileAttributes "c:\config.sys" NORMAL

  SetFileAttributes "$INSTDIR\..\boot.ini" NORMAL
  WriteIniStr "$INSTDIR\..\boot.ini" "operating systems" "c:\ubnldr.mbr" '"UNetbootin-replacewithubnversion"'
  WriteIniStr "$INSTDIR\..\boot.ini" "boot loader" "timeout" 15 
  
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\unetbtin\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\grub.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\unetbtin\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\grub.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"

      MessageBox MB_YESNO|MB_ICONQUESTION "You need to reboot to complete the installation. Select the UNetbootin boot menu entry on the next bootup. Do you want to reboot now?" IDNO +2
        Reboot


SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd


Section Uninstall
  ReadRegStr $varwinvers HKLM \
  "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

  ${If} $varwinvers >= 6.0
     ExecWait '"c:\unetbtin\emtxfile.exe" "c:\unetbtin\vbcdundo.bat" runas'
     ExecWait '"c:\unetbtin\runxfile.exe" "c:\unetbtin\vbcdundo.bat" runas'
  ${Else}
     ExecWait '"c:\unetbtin\runxfile.exe" "c:\unetbtin\bootundo.bat"'
  ${EndIf}

  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\..\ubnldr"
  Delete "$INSTDIR\..\ubnldr.mbr"
  Delete "$INSTDIR\..\ubnldr.exe"
  Delete "$INSTDIR\menu.lst"
  Delete "$INSTDIR\ubninit"
  Delete "$INSTDIR\ubnkern"
  Delete "$INSTDIR\bootedit.bat"
  Delete "$INSTDIR\bootedit.pif"
  Delete "$INSTDIR\bootundo.bat"
  Delete "$INSTDIR\bootundo.pif"
  Delete "$INSTDIR\vbcdedit.bat"
  Delete "$INSTDIR\vbcdedit.pif"
  Delete "$INSTDIR\vbcdundo.bat"
  Delete "$INSTDIR\vbcdundo.pif"
  Delete "$INSTDIR\config.sup"
  Delete "$INSTDIR\config.sys"
  Delete "$INSTDIR\tr.exe"
  Delete "$INSTDIR\runxfile.exe"
  Delete "$INSTDIR\emtxfile.exe"
  Delete "$INSTDIR\bcdid"
  ; cdtu Delete "$INSTDIR\wget.exe"
  ; cdtu Delete "$INSTDIR\kernurl.txt"
  ; cdtu Delete "$INSTDIR\initurl.txt"
  ; cdtu Delete "$INSTDIR\dtkernlc.bat"
  ; cdtu Delete "$INSTDIR\dtkernlc.pif"
  ; isdl Delete "$INSTDIR\isoexrct.bat"
  ; isdl Delete "$INSTDIR\isoexrct.pif"
  ; isdl Delete "$INSTDIR\7z.dll"
  ; isdl Delete "$INSTDIR\7z.exe"
  ; isdl Delete "$INSTDIR\unetbtin\ubniso.iso"


  RMDir "$INSTDIR"

      SetFileAttributes "c:\boot.ini" NORMAL
    DeleteINIStr "c:\boot.ini" "operating systems" "c:\ubnldr.mbr"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UNetbootin"


  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
