!define PRODUCT_NAME "UNetbootin"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\ubnldr.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma
RequestExecutionLevel admin

!addplugindir ".\plugins"
!include LogicLib.nsh

var /GLOBAL varwinvers

Name "${PRODUCT_NAME}"
OutFile "booteder.exe"
InstallDir "$EXEDIR\.."
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
 
  ReadRegStr $R0 HKLM \
  "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
  "UninstallString"
  StrCmp $R0 "" done
 
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "${PRODUCT_NAME} is already installed. $\n$\nClick `OK` to remove the \
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

WriteRegStr HKEY_LOCAL_MACHINE SOFTWARE\Microsoft\WIndows\CurrentVersion\RunOnce "UNetbootin Uninstaller" "c:\unetbtin\uninst.exe"

  ReadRegStr $varwinvers HKLM \
  "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

  ${If} $varwinvers >= 6.0
     ExecWait '"$INSTDIR\unetbtin\emtxfile.exe" "$INSTDIR\unetbtin\vbcdedit.bat" runas'
     IfFileExists "$INSTDIR\unetbtin\bcdid" vbtedfin
     ExecWait '"$INSTDIR\unetbtin\runxfile.exe" "$INSTDIR\unetbtin\vbcdedit.bat" runas'
     vbtedfin:
  ${Else}
     ExecWait '"$INSTDIR\unetbtin\runxfile.exe" "$INSTDIR\unetbtin\bootedit.bat"'
  ${EndIf}

  SetFileAttributes "$INSTDIR\config.sys" NORMAL

  SetFileAttributes "$INSTDIR\..\boot.ini" NORMAL
  WriteIniStr "$INSTDIR\..\boot.ini" "operating systems" "$INSTDIR\ubnldr.mbr" '"UNetbootin"'
  WriteIniStr "$INSTDIR\..\boot.ini" "boot loader" "timeout" 15 
  
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\unetbtin\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\ubnldr.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "UNetbootin"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\unetbtin\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\ubnldr.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "1"

      MessageBox MB_YESNO|MB_ICONQUESTION "You need to reboot to complete the installation. Select the UNetbootin boot menu entry on the next bootup. Do you want to reboot now?" IDNO +2
        Reboot


SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "UNetbootin was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove UNetbootin and all of its components?" IDYES +2
  Abort
FunctionEnd


Section Uninstall

StrCpy $R7 "$INSTDIR\.."

  ReadRegStr $varwinvers HKLM \
  "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

  ${If} $varwinvers >= 6.0
     ExecWait '"$R7\unetbtin\emtxfile.exe" "$R7\unetbtin\vbcdundo.bat" runas'
     ExecWait '"$R7\unetbtin\runxfile.exe" "$R7\unetbtin\vbcdundo.bat" runas'
  ${Else}
     ExecWait '"$INSTDIR\runxfile.exe" "$INSTDIR\unetbtin\bootundo.bat"'
  ${EndIf}

  Delete "$R7\ubnldr"
  Delete "$R7\ubnldr.mbr"
  Delete "$R7\ubnldr.exe"

  RMDir /R "$R7\unetbtin"

      SetFileAttributes "$R7\boot.ini" NORMAL
    DeleteINIStr "$R7\boot.ini" "operating systems" "$R7\ubnldr.mbr"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UNetbootin"


  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
