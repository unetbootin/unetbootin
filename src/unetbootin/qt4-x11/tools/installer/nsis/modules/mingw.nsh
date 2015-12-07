;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
;; Contact: Qt Software Information (qt-info@nokia.com)
;;
;; This file is part of the Windows installer of the Qt Toolkit.
;;
;; $QT_BEGIN_LICENSE:LGPL$
;; Commercial Usage
;; Licensees holding valid Qt Commercial licenses may use this file in
;; accordance with the Qt Commercial License Agreement provided with the
;; Software or, alternatively, in accordance with the terms contained in
;; a written agreement between you and Nokia.
;;
;; GNU Lesser General Public License Usage
;; Alternatively, this file may be used under the terms of the GNU Lesser
;; General Public License version 2.1 as published by the Free Software
;; Foundation and appearing in the file LICENSE.LGPL included in the
;; packaging of this file.  Please review the following information to
;; ensure the GNU Lesser General Public License version 2.1 requirements
;; will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
;;
;; In addition, as a special exception, Nokia gives you certain
;; additional rights. These rights are described in the Nokia Qt LGPL
;; Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
;; package.
;;
;; GNU General Public License Usage
;; Alternatively, this file may be used under the terms of the GNU
;; General Public License version 3.0 as published by the Free Software
;; Foundation and appearing in the file LICENSE.GPL included in the
;; packaging of this file.  Please review the following information to
;; ensure the GNU General Public License version 3.0 requirements will be
;; met: http://www.gnu.org/copyleft/gpl.html.
;;
;; If you are unsure which license is appropriate for your use, please
;; contact the sales department at qt-sales@nokia.com.
;; $QT_END_LICENSE$
;;
;; This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
;; WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!ifdef MODULE_MINGW
!macro MINGW_INITIALIZE
!include "includes\qtcommon.nsh"
!ifndef MODULE_MINGW_NAME
  !define MODULE_MINGW_NAME "Qt"
!endif
!ifndef MODULE_MINGW_VERSION
  !define MODULE_MINGW_VERSION "${PRODUCT_VERSION}"
!endif
!ifndef MODULE_MINGW_BUILDDIR
  !error "MODULE_MINGW_BUILDDIR not defined!"
!endif
!ifndef MODULE_MINGW_ROOT
  !error "MODULE_MINGW_ROOT not defined!"
!endif
!ifndef MODULE_MINGW_URL
  !define MODULE_MINGW_URL "ftp://ftp.trolltech.com/misc"
!endif
!ifndef MODULE_MINGW_COMPILERVERSION
  !define MODULE_MINGW_COMPILERVERSION "3.4.2"
!endif
!ifndef MODULE_MINGW_LICENSE
  !define MODULE_MINGW_LICENSE "C:\MinGW\COPYING"
!endif

!define MODULE_MINGW_DOWNLOADPAGE "gwdownload.ini"
!define MODULE_MINGW_MIRRORPAGE "gwmirror.ini"
!define MODULE_MINGW_RUNTIME_LIB "mingw*.dll"
!define MODULE_MINGW_DOWNLOADFILE "MinGW-${MODULE_MINGW_COMPILERVERSION}"

var MODULE_MINGW_DOWNLOAD
var MODULE_MINGW_SOURCEDOWNLOAD
var MODULE_MINGW_MIRRORS
var MODULE_MINGW_INSTOK
var MODULE_MINGW_COMPILERINSTDIR

LangString ModuleMinGWTitle ${LANG_ENGLISH} "MinGW Installation"
LangString ModuleMinGWDescription ${LANG_ENGLISH} "You need MinGW to be able to compile Qt applications."
LangString ModuleMinGWMirrorTitle ${LANG_ENGLISH} "MinGW Download Mirror"
LangString ModuleMinGWMirrorDescription ${LANG_ENGLISH} "Select a download mirror."

Page custom ModuleMinGWDownloadPageEnter ModuleMinGWDownloadPageExit
!define MUI_PAGE_CUSTOMFUNCTION_PRE ModuleMinGWLicensePageEnter
!define MUI_PAGE_HEADER_TEXT "MinGW License Agreement"
!define MUI_PAGE_HEADER_SUBTEXT "Please review the license terms before installing MinGW."
!define MUI_LICENSEPAGE_TEXT_TOP "MinGW License Information"
!insertmacro MUI_PAGE_LICENSE "${MODULE_MINGW_LICENSE}"
Page custom ModuleMinGWMirrorPageEnter ModuleMinGWMirrorPageExit

!include "includes\qtenv.nsh"
!macroend

!macro MINGW_SECTIONS
Section "${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION}" MINGW_SEC01
  strcmp "$MINGW_INSTDIR" "" 0 +5
    StrCpy $MINGW_INSTDIR "$INSTDIR\${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION}"
    push $MINGW_INSTDIR
    call MakeQtDirectory
    pop $MINGW_INSTDIR

  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "MINGWInstalled" 1
  
  SetOutPath "$MINGW_INSTDIR"
  SetOverwrite ifnewer
  !insertmacro MODULE_MINGW_INSTALLFILES
  
  push "$MINGW_INSTDIR"
  call DeleteFloatingLicenseProgram

  push "$MINGW_INSTDIR\bin"
  call AddStartmenuApplication
  
  push ${MODULE_MINGW_BUILDDIR}
  push "$MINGW_INSTDIR"
  call PatchPrlFiles
  
  IfFileExists "$MINGW_INSTDIR\.qmake.cache" 0 +5
  push "$MINGW_INSTDIR\.qmake.cache"
  push ${MODULE_MINGW_BUILDDIR}
  push $MINGW_INSTDIR
  call PatchPath

  IfFileExists "$MINGW_INSTDIR\mkspecs\default\qmake.conf" 0 +5
  push "$MINGW_INSTDIR\mkspecs\default\qmake.conf"
  push ${MODULE_MINGW_BUILDDIR}
  push $MINGW_INSTDIR
  call PatchPath

  push $MINGW_INSTDIR
  call PatchCommonBinaryFiles
  
  push $MINGW_INSTDIR
  call PatchLicenseInformation

  WriteRegStr SHCTX "SOFTWARE\Trolltech\Common\${MODULE_MINGW_VERSION}\$LICENSE_PRODUCT" "Key" "$LICENSE_KEY"

  push $MINGW_INSTDIR
  call ModuleMinGWMakeEnvFile
  CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION} Command Prompt.lnk" "%COMSPEC%" '/k "$MINGW_INSTDIR\bin\qtvars.bat"'
  CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION} (Build Debug Libraries).lnk" "%COMSPEC%" '/k "$MINGW_INSTDIR\bin\qtvars.bat compile_debug"'

  push $0
  strcmp $MODULE_MINGW_DOWNLOAD "no" DoneMinGWInstall
    DetailPrint "Installing MinGW into $MODULE_MINGW_COMPILERINSTDIR"
    WriteRegStr SHCTX "$PRODUCT_UNIQUE_KEY" "MinGWInstDir" "$MODULE_MINGW_COMPILERINSTDIR"
    nsExec::ExecToLog '"$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}.exe" /S /D=$MODULE_MINGW_COMPILERINSTDIR'
    pop $0
  strcmp $MODULE_MINGW_SOURCEDOWNLOAD "no" DoneMinGWInstall
    DetailPrint "Installing MinGW sources into $MODULE_MINGW_COMPILERINSTDIR\src"
    WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "MinGWSources" 1
    nsExec::ExecToLog '"$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}-src.exe" /S /D=$MODULE_MINGW_COMPILERINSTDIR\src'
    pop $0
  DoneMinGWInstall:
  pop $0

  DetailPrint "Copying MinGW runtime..."
  SetDetailsPrint none
  CopyFiles /SILENT "$MODULE_MINGW_COMPILERINSTDIR\bin\${MODULE_MINGW_RUNTIME_LIB}" "$MINGW_INSTDIR\bin"
  SetDetailsPrint both
SectionEnd

Function EnableButtons
  Push $0
  GetDlgItem $0 $HWNDPARENT 3
  EnableWindow $0 1
  GetDlgItem $0 $HWNDPARENT 1
  EnableWindow $0 1
  GetDlgItem $0 $HWNDPARENT 2
  EnableWindow $0 1
  Pop $0
FunctionEnd

Function DisableButtons
  Push $0
  GetDlgItem $0 $HWNDPARENT 3
  EnableWindow $0 0
  GetDlgItem $0 $HWNDPARENT 1
  EnableWindow $0 0
  GetDlgItem $0 $HWNDPARENT 2
  EnableWindow $0 0
  Pop $0
FunctionEnd

Function ModuleMinGWDownloadPageEnter
  strcmp $MODULE_MINGW_INSTOK "yes" 0 +2
  Abort

  !insertmacro MUI_HEADER_TEXT "$(ModuleMinGWTitle)" "$(ModuleMinGWTitleDescription)"
  Call UpdateCtrlStates
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_MINGW_DOWNLOADPAGE}"
  !insertmacro MUI_INSTALLOPTIONS_WRITE "${MODULE_MINGW_DOWNLOADPAGE}" "Field 8" "State" "0"
FunctionEnd

Function ModuleMinGWMirrorPageEnter
  strcmp $MODULE_MINGW_DOWNLOAD "yes" +2
  Abort

  !insertmacro MUI_HEADER_TEXT "$(ModuleMinGWMirrorTitle)" "$(ModuleMinGWMirrorDescription)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_MINGW_MIRRORPAGE}"
FunctionEnd

Function ModuleMinGWLicensePageEnter
  strcmp $MODULE_MINGW_DOWNLOAD "yes" +2
  Abort
FunctionEnd

Function UpdateCtrlStates
  push $0
  push $1
  push $2

  !insertmacro MUI_INSTALLOPTIONS_READ $0 "${MODULE_MINGW_DOWNLOADPAGE}" "Field 8" "State"
  intop $0 $0 !
  FindWindow $2 "#32770" "" $HWNDPARENT
  GetDlgItem $1 $2 1205
  EnableWindow $1 $0
  GetDlgItem $1 $2 1202
  EnableWindow $1 $0
  GetDlgItem $1 $2 1203
  EnableWindow $1 $0

  intop $0 $0 !
  GetDlgItem $1 $2 1206
  EnableWindow $1 $0
  GetDlgItem $1 $2 1207
  EnableWindow $1 $0
  GetDlgItem $1 $2 1208
  EnableWindow $1 $0

  GetDlgItem $1 $HWNDPARENT 1
  IntCmp $0 0 +3
    SendMessage $1 ${WM_SETTEXT} 0 "STR:Next >"
  Goto +2
    SendMessage $1 ${WM_SETTEXT} 0 "STR:Install"
      
  pop $2
  pop $1
  pop $0
FunctionEnd

Function ModuleMinGWDownloadPageExit
  push $0
  push $1
  
  !insertmacro MUI_INSTALLOPTIONS_READ $0 "${MODULE_MINGW_DOWNLOADPAGE}" "Settings" "State"
  strcmp "$0" "8" 0 NoNotify
    Call UpdateCtrlStates
    abort
  NoNotify:

  !insertmacro MUI_INSTALLOPTIONS_READ $0 "${MODULE_MINGW_DOWNLOADPAGE}" "Field 8" "State"
  strcmp "$0" "0" noDownload doDownload

doDownload:
  !insertmacro MUI_INSTALLOPTIONS_READ $0 "${MODULE_MINGW_DOWNLOADPAGE}" "Field 6" "State"
  strcmp $0 "" 0 +3
    MessageBox MB_ICONEXCLAMATION|MB_OK "You need to specify an installation directory!"
    goto tryAgain

  strcpy $MODULE_MINGW_COMPILERINSTDIR $0
  strcpy $MODULE_MINGW_DOWNLOAD "yes"
  CreateDirectory "$MINGW_INSTDIR\downloads"
  
  Call DisableButtons
  InetLoad::load /BANNER "Mirror Download" "Downloading mirrors from server..." "${MODULE_MINGW_URL}/${MODULE_MINGW_DOWNLOADFILE}.mirrors" "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}.mirrors" /END
  Pop $1 ;Get the return value
  Call EnableButtons

  StrCmp $1 "OK" +3
    MessageBox MB_ICONEXCLAMATION|MB_RETRYCANCEL "Was not able to download mirror list ($1)." IDRETRY tryAgain 0
    Quit

  call ModuleMinGWReadMirrors
  !insertmacro MUI_INSTALLOPTIONS_WRITE ${MODULE_MINGW_MIRRORPAGE} "Field 3" "ListItems" "$MODULE_MINGW_MIRRORS"
  goto done

noDownload:
  strcpy $MODULE_MINGW_DOWNLOAD "no"
  strcpy $MODULE_MINGW_SOURCEDOWNLOAD "no"
  call ModuleMinGWChecking
  strcmp $MODULE_MINGW_INSTOK "yes" done
  MessageBox MB_ICONEXCLAMATION|MB_YESNO "There is a problem with your MinGW installation:$\r$\n$MODULE_MINGW_INSTOK$\r$\nDo you still want to continue? (Your installation may not work)" IDNO tryAgain
  goto done

tryAgain:
  pop $1
  pop $0
  Abort

done:
  pop $1
  pop $0
FunctionEnd

Function ModuleMinGWMirrorPageExit
  push $0
  push $2
  push $1

  !insertmacro MUI_INSTALLOPTIONS_READ $0 "${MODULE_MINGW_MIRRORPAGE}" "Field 3" "State"
  strcmp "$0" "" 0 +3
    MessageBox MB_ICONEXCLAMATION|MB_OK "You must select a mirror to download from!"
    goto tryAgain

  push $0
  call ModuleMinGWGetMirror
  pop $0

  Call DisableButtons
  InetLoad::load /BANNER "MinGW Download" "Downloading MinGW from server..." "$0/${MODULE_MINGW_DOWNLOADFILE}.exe" "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}.exe" /END
  Pop $2 ;get the return value
  Call EnableButtons

  StrCmp $2 "OK" +3
    MessageBox MB_ICONEXCLAMATION|MB_OK "Was not able to download MinGW ($2). Please try another mirror."
    Goto tryAgain

  !insertmacro MUI_INSTALLOPTIONS_READ $1 "${MODULE_MINGW_MIRRORPAGE}" "Field 2" "State"
  strcmp "$1" "0" done

  Call DisableButtons
  InetLoad::load /BANNER "MinGW Sources Download" "Downloading MinGW Sources from server..." "$0/${MODULE_MINGW_DOWNLOADFILE}-src.exe" "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}-src.exe" /END
  Pop $2
  Call EnableButtons

  strcpy $MODULE_MINGW_SOURCEDOWNLOAD "yes"

  StrCmp $2 "OK" +3
    MessageBox MB_ICONEXCLAMATION|MB_RETRYCANCEL "Was not able to download MinGW sources ($2). Please try another mirror?" IDRETRY tryAgain 0
    Quit

  goto done

tryAgain:
  pop $1
  pop $2
  pop $0
  Abort

done:
  pop $1
  pop $2
  pop $0
FunctionEnd

Function ModuleMinGWReadMirrors
  push $0 ;file handle
  push $1 ;line

  ClearErrors
  FileOpen $0 "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}.mirrors" r
  IfErrors done

  strcpy $MODULE_MINGW_MIRRORS ""

nextline:
  FileRead $0 $1
  IfErrors done
  push $1
  call ModuleMinGWRemoveNewLine
  pop $1
  strcpy $MODULE_MINGW_MIRRORS "$MODULE_MINGW_MIRRORS|$1"
  FileRead $0 $1 ;Jump over next line
  IfErrors done
  goto nextline

done:
  FileClose $0
  strlen $1 $MODULE_MINGW_MIRRORS
  intcmp $1 0 failed failed cleanup

failed:
  MessageBox MB_ICONSTOP|MB_OK "Unable to parse mirror list, exiting!"
  Quit

cleanup:
  pop $1
  pop $0
FunctionEnd

#this just removes the last two chars
Function ModuleMinGWRemoveNewLine
exch $0
push $1
push $2

strlen $1 $0
intop $1 $1 - 1
strcpy $2 $0 1 $1 ;get last char

strcmp "$2" "$\n" 0 +2
intop $1 $1 - 1

strcpy $2 $0 1 $1 ;get last char
strcmp "$2" "$\r" 0 +2
intop $1 $1 - 1

intop $1 $1 + 1
strcpy $0 $0 $1

pop $2
pop $1
exch $0
FunctionEnd

#push serverid
#call GetMirror
#pop server
Function ModuleMinGWGetMirror
  exch $1 ;id
  push $0 ;file handle
  push $2 ;line
  push $3 ;tmp

  strcpy $3 ""

  ClearErrors
  FileOpen $0 "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}.mirrors" r
  IfErrors done

nextline:
  FileRead $0 $2
  IfErrors done
  push $2
  call ModuleMinGWRemoveNewLine
  pop $2
  strcmp $1 $2 0 nextline
  FileRead $0 $3
  IfErrors done
  push $3
  call ModuleMinGWRemoveNewLine
  pop $3

done:
  strcpy $1 $3
  FileClose $0
  strlen $2 $1
  intcmp $2 0 failed failed cleanup

failed:
  MessageBox MB_ICONSTOP|MB_OK "Unable to parse mirror list, exiting!"
  Quit

cleanup:
  pop $3
  pop $2
  pop $0
  exch $1
FunctionEnd

Function ModuleMinGWChecking
  push $0

  ### update with plugin
  strcpy $MODULE_MINGW_INSTOK "yes"
  strcpy $MODULE_MINGW_COMPILERINSTDIR "C:\MinGW" ;fallback dir

  !insertmacro MUI_INSTALLOPTIONS_READ $0 "${MODULE_MINGW_DOWNLOADPAGE}" "Field 3" "State"
  strcmp "$0" "" +2
    strcpy $MODULE_MINGW_COMPILERINSTDIR $0

  IfFileExists "$MODULE_MINGW_COMPILERINSTDIR\bin\g++.exe" +3 0
    strcpy $MODULE_MINGW_INSTOK "g++ not found in $MODULE_MINGW_COMPILERINSTDIR\bin\"
    goto DoneChecking

!ifndef OPENSOURCE_BUILD
  ; check w32api.h
  push $MODULE_MINGW_COMPILERINSTDIR
  qtnsisext::HasValidWin32Library
  pop $0
  strcmp "$0" "1" +3 0
    strcpy $MODULE_MINGW_INSTOK "The installer could not find a valid $MODULE_MINGW_COMPILERINSTDIR\include\w32api.h$\r$\n(The supported version is 3.2)"
    goto DoneChecking

  ; check version
  push $MODULE_MINGW_COMPILERINSTDIR
  qtnsisext::GetMinGWVersion
  pop $0
  strcmp "$0" "${MODULE_MINGW_COMPILERVERSION}" +3 0
    strcpy $MODULE_MINGW_INSTOK "g++ version found does not match ${MODULE_MINGW_COMPILERVERSION} (Found version $0)."
    goto DoneChecking
!endif    

DoneChecking:
  pop $0
FunctionEnd

#
# creates a qtvars.bat file in $QTDIR\bin
# push "c:\qt"  #QTDIR
# call MakeQtVarsFile
#
Function ModuleMinGWMakeEnvFile
  push $0 ; file handle

  ClearErrors
  FileOpen $0 "$MINGW_INSTDIR\bin\qtvars.bat" w
  IfErrors WriteMakeFile
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "rem$\r$\n"
  FileWrite $0 "rem This file is generated$\r$\n"
  FileWrite $0 "rem$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "echo Setting up a MinGW/Qt only environment...$\r$\n"
  FileWrite $0 "echo -- QTDIR set to $MINGW_INSTDIR$\r$\n"
  FileWrite $0 "echo -- PATH set to $MINGW_INSTDIR\bin$\r$\n"
  FileWrite $0 "echo -- Adding $MODULE_MINGW_COMPILERINSTDIR\bin to PATH$\r$\n"
  FileWrite $0 "echo -- Adding %SystemRoot%\System32 to PATH$\r$\n"
  FileWrite $0 "echo -- QMAKESPEC set to win32-g++$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "set QTDIR=$MINGW_INSTDIR$\r$\n"
  FileWrite $0 "set PATH=$MINGW_INSTDIR\bin$\r$\n"
  FileWrite $0 "set PATH=%PATH%;$MODULE_MINGW_COMPILERINSTDIR\bin$\r$\n"
  FileWrite $0 "set PATH=%PATH%;%SystemRoot%\System32$\r$\n"
  FileWrite $0 "set QMAKESPEC=win32-g++$\r$\n"
  FileWrite $0 "$\r$\n"
  
  FileWrite $0 'if not "%1"=="compile_debug" goto END$\r$\n'
  FileWrite $0 "cd %QTDIR%$\r$\n"
  FileWrite $0 "echo This will configure and compile qt in debug.$\r$\n"
  FileWrite $0 "echo The release libraries will not be recompiled.$\r$\n"
  FileWrite $0 "pause$\r$\n"
  FileWrite $0 "configure -plugin-sql-sqlite -plugin-sql-odbc -qt-libpng -qt-libjpeg$\r$\n"
  FileWrite $0 "cd %QTDIR%\src$\r$\n"
  FileWrite $0 "qmake$\r$\n"
  FileWrite $0 "mingw32-make debug$\r$\n"
  FileWrite $0 ":END$\r$\n"
  FileClose $0

WriteMakeFile:
  ClearErrors
  FileOpen $0 "$MINGW_INSTDIR\bin\make.bat" w
  IfErrors done
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "mingw32-make %*$\r$\n"
  FileClose $0

done:
;  pop $1
  pop $0
FunctionEnd

Function MINGW_ValidateDirectoryFunc
  push "${MODULE_MINGW_BUILDDIR}"
  push $MINGW_INSTDIR
  call CommonCheckDirectory
FunctionEnd
!macroend

!macro MINGW_DESCRIPTION
  !insertmacro MUI_DESCRIPTION_TEXT ${MINGW_SEC01} "This installs ${MODULE_MINGW_NAME} version ${MODULE_MINGW_VERSION} on your system."
!macroend

!macro MINGW_STARTUP
  !ifndef MODULE_MINGW_NODEFAULT
    SectionSetFlags ${MINGW_SEC01} 17
  !endif
  strcpy $MINGW_INSTDIR "C:\Qt\${MODULE_MINGW_VERSION}"
  push $MINGW_INSTDIR
  call MakeQtDirectory
  pop $MINGW_INSTDIR
  
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_MINGW_DOWNLOADPAGE}"
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_MINGW_MIRRORPAGE}"

  !insertmacro MUI_INSTALLOPTIONS_WRITE "${MODULE_MINGW_DOWNLOADPAGE}" "Field 3" "State" "C:\MinGW"
  !insertmacro MUI_INSTALLOPTIONS_WRITE "${MODULE_MINGW_DOWNLOADPAGE}" "Field 6" "State" "C:\MinGW"

  strcpy $MODULE_MINGW_DOWNLOAD "no"
  strcpy $MODULE_MINGW_SOURCEDOWNLOAD "no"
!macroend

!macro MINGW_FINISH
!macroend

!macro MINGW_RUN_FUNCTION
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MINGWInstalled"
  intcmp $0 1 0 DoneRunFunctionMINGW

  IfFileExists "$MINGW_INSTDIR\bin\qtdemo.exe" 0 +2
    Exec '$MINGW_INSTDIR\bin\qtdemo.exe'
  goto DoneRunFunction ;don't run more applications
  
  DoneRunFunctionMINGW:
!macroend

!macro MINGW_README_FUNCTION
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MINGWInstalled"
  intcmp $0 1 0 DoneReadmeFunctionMINGW

  IfFileExists "$MINGW_INSTDIR\bin\assistant.exe" 0 +2
    Exec '$MINGW_INSTDIR\bin\assistant.exe'
  goto DoneReadmeFunction ;don't run more applications

  DoneReadmeFunctionMINGW:
!macroend

!macro MINGW_UNSTARTUP
  strcmp "$MINGW_INSTDIR" "" 0 +5
    StrCpy $MINGW_INSTDIR "$INSTDIR\${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION}"
    push $MINGW_INSTDIR
    call un.MakeQtDirectory
    pop $MINGW_INSTDIR

  !insertmacro ConfirmOnRemove "MINGWInstalled" "- ${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION} in $MINGW_INSTDIR"
!macroend

!macro MINGW_UNINSTALL
Section un.ModuleMinGW
  push $0
  push $1
  
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MINGWInstalled"
  intcmp $0 1 0 DoneUnInstallMINGW

  Delete "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}.mirrors"
  
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MinGWSources"
  strcmp $0 "" MinGWSourcesUninstallDone ;not installed
  Delete "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}-src.exe"
  nsExec::ExecToLog '"$0\src\uninst.exe"'
  pop $1
  MinGWSourcesUninstallDone:

  ReadRegStr $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MinGWInstDir"
  strcmp $0 "" MinGWUninstallDone ;not installed
  Delete "$MINGW_INSTDIR\downloads\${MODULE_MINGW_DOWNLOADFILE}.exe"
  nsExec::ExecToLog '"$0\uninst.exe"'
  pop $1
  MinGWUninstallDone:

  DetailPrint "Removing start menu shortcuts"
  call un.RemoveStartmenuApplication
  Delete "$SMPROGRAMS\$STARTMENU_STRING\${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION} Command Prompt.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_STRING\${MODULE_MINGW_NAME} ${MODULE_MINGW_VERSION} (Build Debug Libraries).lnk"

  Delete "$MINGW_INSTDIR\bin\${MODULE_MINGW_RUNTIME_LIB}"
  Delete "$MINGW_INSTDIR\bin\make.bat"
  Delete "$MINGW_INSTDIR\bin\qtvars.bat"

  !insertmacro MODULE_MINGW_REMOVE "$MINGW_INSTDIR"
  RMDir $MINGW_INSTDIR ;removes it if empty

  DoneUnInstallMINGW:
  pop $1
  pop $0
SectionEnd
!macroend
!macro MINGW_UNFINISH
!macroend
!else ;MODULE_MINGW
!macro MINGW_INITIALIZE
!macroend
!macro MINGW_SECTIONS
!macroend
!macro MINGW_DESCRIPTION
!macroend
!macro MINGW_STARTUP
!macroend
!macro MINGW_FINISH
!macroend
!macro MINGW_RUN_FUNCTION
!macroend
!macro MINGW_README_FUNCTION
!macroend
!macro MINGW_UNSTARTUP
!macroend
!macro MINGW_UNINSTALL
!macroend
!macro MINGW_UNFINISH
!macroend
!endif ;MODULE_MINGW

