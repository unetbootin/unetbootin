;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
;;
;; This file is part of the Windows installer of the Qt Toolkit.
;;
;; This file may be used under the terms of the GNU General Public
;; License versions 2.0 or 3.0 as published by the Free Software
;; Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
;; included in the packaging of this file.  Alternatively you may (at
;; your option) use any later version of the GNU General Public
;; License if such license has been publicly approved by Trolltech ASA
;; (or its successors, if any) and the KDE Free Qt Foundation. In
;; addition, as a special exception, Trolltech gives you certain
;; additional rights. These rights are described in the Trolltech GPL
;; Exception version 1.2, which can be found at
;; http://www.trolltech.com/products/qt/gplexception/ and in the file
;; GPL_EXCEPTION.txt in this package.
;;
;; Please review the following information to ensure GNU General
;; Public Licensing requirements will be met:
;; http://trolltech.com/products/qt/licenses/licensing/opensource/. If
;; you are unsure which license is appropriate for your use, please
;; review the following information:
;; http://trolltech.com/products/qt/licenses/licensing/licensingoverview
;; or contact the sales department at sales@trolltech.com.
;;
;; In addition, as a special exception, Trolltech, as the sole
;; copyright holder for Qt Designer, grants users of the Qt/Eclipse
;; Integration plug-in the right for the Qt/Eclipse Integration to
;; link to functionality provided by Qt Designer and its related
;; libraries.
;;
;; This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
;; INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
;; A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
;; granted herein.
;;
;; This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
;; WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!ifdef MODULE_ENVIRONMENT
!macro ENVIRONMENT_INITIALIZE
  !include "includes\writeEnvStr.nsh"
  !include "includes\writePathStr.nsh"
  
  !ifndef MODULE_ENVIRONMENT_QTDIR
    !ifdef MODULE_MINGW
      !define MODULE_ENVIRONMENT_QTDIR $MINGW_INSTDIR
    !endif

    !ifdef MODULE_MSVC
      !define MODULE_ENVIRONMENT_QTDIR $MSVC_INSTDIR
    !endif
  !endif

  !define MODULE_ENVIRONMENT_PAGE "envpage.ini"
  var MODULE_ENVIRONMENT_SET
  var MODULE_ENVIRONMENT_OLD
  LangString ModuleEnvironmentTitle ${LANG_ENGLISH} "Configure Environment"
  LangString ModuleEnvironmentDescription ${LANG_ENGLISH} "Configure Qt environment variables"
  
  Page custom ModuleEnvironmentPageEnter ModuleEnvironmentPageExit
!macroend
!macro ENVIRONMENT_SECTIONS
  Section -ModuleEnvironmentRegister
    push "${MODULE_ENVIRONMENT_QTDIR}"
    call RegisterQtEnvVariables
  SectionEnd

  Function ModuleEnvironmentPageEnter
    push $0
    Call IsNT
    pop $0
    strcmp "$0" "1" +2
      abort
    pop $0
    
    !insertmacro MUI_HEADER_TEXT "$(ModuleEnvironmentTitle)" "$(ModuleEnvironmentDescription)"

    strcmp $MODULE_ENVIRONMENT_SET "1" 0 envCheckNo
      !insertmacro MUI_INSTALLOPTIONS_WRITE "${MODULE_ENVIRONMENT_PAGE}" "Field 2" "State" "1"
      goto showEnvPage
    envCheckNo:
      !insertmacro MUI_INSTALLOPTIONS_WRITE "${MODULE_ENVIRONMENT_PAGE}" "Field 2" "State" "0"

    showEnvPage:
      !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_ENVIRONMENT_PAGE}"
  FunctionEnd

  Function ModuleEnvironmentPageExit
    !insertmacro MUI_INSTALLOPTIONS_READ $MODULE_ENVIRONMENT_SET "${MODULE_ENVIRONMENT_PAGE}" "Field 2" "State"
  FunctionEnd

  Function RegisterQtEnvVariables
    exch $2 ; the installation path = QTDIR
    push $0 ; I think WriteEnvStr mixes up $0 and $1
    push $1

    WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QtEnvSet" $MODULE_ENVIRONMENT_SET

    strcmp $MODULE_ENVIRONMENT_SET "1" 0 noenv

    StrCmp $MODULE_ENVIRONMENT_OLD "0" +4
    DetailPrint "Removing $MODULE_ENVIRONMENT_OLD\bin from PATH"
    push "$MODULE_ENVIRONMENT_OLD\bin"
    Call RemoveFromPath ; remove old qtdir

    DetailPrint "Setting QTDIR to $2"
    push "QTDIR"
    push $2
    Call WriteEnvStr ; set the QTDIR

    DetailPrint "Adding $2\bin to PATH"
    push "$2\bin"
    Call AddToPath ; set the PATH


    push "QMAKESPEC"
    push ${INSTALL_COMPILER}
    Call GetMkSpec
    pop $0
    DetailPrint "Setting QMAKESPEC to $0"
    push $0
    Call WriteEnvStr ; set the QMAKESPEC

    noenv:
      pop $1
      pop $0
      pop $2
  FunctionEnd

  Function un.RegisterQtEnvVariables
    exch $0 ; QTDIR
    push $1

    ClearErrors
    ReadRegDWORD $MODULE_ENVIRONMENT_SET SHCTX "$PRODUCT_UNIQUE_KEY" "QtEnvSet"
    intcmp $MODULE_ENVIRONMENT_SET 0 noenv

    DetailPrint "Removing $0\bin from the PATH"
    push "$0\bin"
    Call un.RemoveFromPath ; removes qt from the path

    ;Check if QTDIR is equal to installdir
    ExpandEnvStrings $1 "%QTDIR%"

    StrCmp "$0" "$1" removeenv
    StrCmp "$0\" "$1" removeenv
    StrCmp "$0" "$1\" removeenv
    Goto noenv

    removeenv:
    DetailPrint "Removing QTDIR"
    push "QTDIR"
    Call un.DeleteEnvStr ; removes QTDIR

    DetailPrint "Removing QMAKESPEC"
    push "QMAKESPEC"
    Call un.DeleteEnvStr ; removes QMAKESPEC

    noenv:
      pop $1
       pop $0
  FunctionEnd
!macroend
!macro ENVIRONMENT_DESCRIPTION
!macroend
!macro ENVIRONMENT_STARTUP
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_ENVIRONMENT_PAGE}"
  push $0
  ExpandEnvStrings $0 "%QTDIR%"

  StrCmp $0 "%QTDIR%" +4
  strcpy $MODULE_ENVIRONMENT_SET "0" ;QTDIR exists
  strcpy $MODULE_ENVIRONMENT_OLD $0
  Goto +3
  strcpy $MODULE_ENVIRONMENT_SET "1" ;no QTDIR
  strcpy $MODULE_ENVIRONMENT_OLD "0"
  
  Call IsNT
  pop $0
  strcmp "$0" "1" +2
    strcpy $MODULE_ENVIRONMENT_SET "0"
  pop $0
!macroend
!macro ENVIRONMENT_FINISH
!macroend
!macro ENVIRONMENT_UNSTARTUP
!macroend
!macro ENVIRONMENT_UNINSTALL
  Section -un.ModuleEnvironmentRegister
    push "${MODULE_ENVIRONMENT_QTDIR}"
    call un.RegisterQtEnvVariables
  SectionEnd
!macroend
!macro ENVIRONMENT_UNFINISH
!macroend
!else ;MODULE_ENVIRONMENT
!macro ENVIRONMENT_INITIALIZE
!macroend
!macro ENVIRONMENT_SECTIONS
!macroend
!macro ENVIRONMENT_DESCRIPTION
!macroend
!macro ENVIRONMENT_STARTUP
!macroend
!macro ENVIRONMENT_FINISH
!macroend
!macro ENVIRONMENT_UNSTARTUP
!macroend
!macro ENVIRONMENT_UNINSTALL
!macroend
!macro ENVIRONMENT_UNFINISH
!macroend
!endif ;MODULE_ENVIRONMENT

