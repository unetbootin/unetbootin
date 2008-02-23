!define PRODUCT_NAME "UNetbootin"
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
ShowInstDetails show
ShowUnInstDetails show

AutoCloseWindow true

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

Function GetParent
 
  Exch $R0
  Push $R1
  Push $R2
  Push $R3
  
  StrCpy $R1 0
  StrLen $R2 $R0
  
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
  Goto loop
  
  get:
    StrCpy $R0 $R0 -$R1
    
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0
    
FunctionEnd

Section "MainSection" SEC01

Push "$EXEDIR"
Call GetParent
Pop $R0

  ReadRegStr $varwinvers HKLM \
  "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

  ${If} $varwinvers >= 6.0
     ExecWait '"$R0\unetbtin\emtxfile.exe" "$R0\unetbtin\vbcdedit.bat" runas'
     IfFileExists "$INSTDIR\unetbtin\bcdid" vbtedfin
     ExecWait '"$R0\unetbtin\runxfile.exe" "$R0\unetbtin\vbcdedit.bat" runas'
     vbtedfin:
  ${Else}
     ExecWait '"$R0\unetbtin\runxfile.exe" "$R0\unetbtin\bootedit.bat"'
  ${EndIf}

  SetFileAttributes "$R0\config.sys" NORMAL

  SetFileAttributes "$R0\boot.ini" NORMAL
  WriteIniStr "$R0\boot.ini" "operating systems" "$R0\ubnldr.mbr" '"UNetbootin"'
  WriteIniStr "$R0\boot.ini" "boot loader" "timeout" 15 
  
SectionEnd

Section -Post
  WriteUninstaller "$R0\unetbtin\uninst.exe"

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

Push $INSTDIR

  Exch $R0
  Push $R1
  Push $R2
  Push $R3
  
  StrCpy $R1 0
  StrLen $R2 $R0
  
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
  Goto loop
  
  get:
    StrCpy $R0 $R0 -$R1
    
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0

Pop $R0

StrCpy "$R7" "$R0"

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
  SetAutoClose true
SectionEnd
