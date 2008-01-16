!define PRODUCT_NAME "UNetbootin"
!define PRODUCT_VERSION "replacewithubnversion"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\grub.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma
RequestExecutionLevel admin

; MUI 1.67 compatible ------
!include "MUI.nsh"

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

Section "MainSection" SEC01

  SetOutPath "$INSTDIR\unetbootin"
  SetOverwrite on
  File "ubnkern"
  File "ubninit"
  File "vbooted.bat"
  File "vbootun.bat"
  File "wcfged.bat"
  File "wcfgun.bat"
  File "config.sup"
  File "tr.exe"
  ; cdtu File "curl.exe"
  ; cdtu File "detkernloc.bat"
  ; cdtu File "detinitloc.bat"

    SetOutPath "$INSTDIR"
  SetOverwrite on
    File "grub.exe"
  File "grldr"
  File "grldr.mbr"
  File "menu.lst"
  
WriteRegStr HKEY_LOCAL_MACHINE SOFTWARE\Microsoft\WIndows\CurrentVersion\RunOnce "UNetbootin Uninstaller" "C:\unetbootin\uninst.exe"

  ReadEnvStr $0 COMSPEC
  nsExec::Exec  '$0 /c "c:\unetbootin\vbooted.bat"'
  SetFileAttributes "c:\config.sys" NORMAL
  ReadEnvStr $0 COMSPEC
  nsExec::Exec  '$0 /c "c:\unetbootin\wcfged.bat"'
  ; cdtu ReadEnvStr $0 COMSPEC
  ; cdtu nsExec::Exec '$0 /c "c:\unetbootin\detkernloc.bat"'
  ; cdtu FileOpen $4 "c:\unetbootin\kernurl.txt" r
  ; cdtu FileRead $4 $varkernurl
  ; cdtu FileClose $4
  ; cdtu NSISdl::download $varkernurl "$INSTDIR\unetbootin\ubnkern"
  ; cdtu ReadEnvStr $0 COMSPEC
  ; cdtu nsExec::Exec '$0 /c "c:\unetbootin\detinitloc.bat"'
  ; cdtu FileOpen $4 "c:\unetbootin\initurl.txt" r
  ; cdtu FileRead $4 $variniturl
  ; cdtu FileClose $4
  ; cdtu NSISdl::download $variniturl "$INSTDIR\unetbootin\ubninit"
  ; ltbe NSISdl::download rpubnkernurl "$INSTDIR\unetbootin\ubnkern"
  ; ltbe NSISdl::download rpubniniturl "$INSTDIR\unetbootin\ubninit"
  SetFileAttributes "$INSTDIR\..\boot.ini" NORMAL
  WriteIniStr "$INSTDIR\..\boot.ini" "operating systems" "c:\grldr.mbr" '"UNetbootin-replacewithubnversion"'
  WriteIniStr "$INSTDIR\..\boot.ini" "boot loader" "timeout" 15 
  
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\unetbootin\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\grub.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\unetbootin\uninst.exe"
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
  ReadEnvStr $0 COMSPEC
  nsExec::Exec  '$0 /c "c:\unetbootin\vbootun.bat"'
  ReadEnvStr $0 COMSPEC
  nsExec::Exec  '$0 /c "c:\unetbootin\wcfgun.bat"'
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\..\grldr"
  Delete "$INSTDIR\..\grldr.mbr"
  Delete "$INSTDIR\..\grub.exe"
  Delete "$INSTDIR\..\menu.lst"
  Delete "$INSTDIR\ubninit"
  Delete "$INSTDIR\ubnkern"
  Delete "$INSTDIR\vbooted.bat"
  Delete "$INSTDIR\vbootun.bat"
  Delete "$INSTDIR\wcfged.bat"
  Delete "$INSTDIR\wcfgun.bat"
  Delete "$INSTDIR\config.sup"
  Delete "$INSTDIR\tr.exe"
  ; cdtu Delete "$INSTDIR\curl.exe"
  ; cdtu Delete "$INSTDIR\detkernloc.bat"
  ; cdtu Delete "$INSTDIR\detinitloc.bat"
  ; cdtu Delete "$INSTDIR\kernurl.txt"
  ; cdtu Delete "$INSTDIR\initurl.txt"

  RMDir "$INSTDIR"

      SetFileAttributes "c:\boot.ini" NORMAL
    DeleteINIStr "c:\boot.ini" "operating systems" "c:\grldr.mbr"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UNetbootin"


  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
