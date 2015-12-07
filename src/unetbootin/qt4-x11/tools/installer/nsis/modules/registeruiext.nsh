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
; UI Extension Module

!ifdef MODULE_REGISTERUIEXT

;------------------------------------------------------------------------------------------------
!macro REGISTERUIEXT_INITIALIZE
  !include "includes\system.nsh"

  !ifndef MODULE_REGISTERUIEXT_QTDIR
    !ifdef MODULE_MINGW
      !define MODULE_REGISTERUIEXT_QTDIR $MINGW_INSTDIR
    !endif

    !ifdef MODULE_MSVC
      !define MODULE_REGISTERUIEXT_QTDIR $MSVC_INSTDIR
    !endif
  !endif
  
  !define MODULE_REGISTERUIEXT_INTERNAL_DESC "Trolltech.DesignerForm"
  !define MODULE_REGISTERUIEXT_DESC_DESIGNER "Open with Qt Designer"
  !define MODULE_REGISTERUIEXT_DESC_DEVENV "Open with Visual Studio .NET"
  !define MODULE_REGISTERUIEXT_FILE_DESC "Qt Designer File"
!macroend

;------------------------------------------------------------------------------------------------

!macro REGISTERUIEXT_SECTIONS

Function GetSelectedVSIP
  Push $0
  Push $1

  StrCpy $0 ""
!ifdef MODULE_VSIP
!ifdef VSIP_SEC01
  SectionGetFlags ${VSIP_SEC01} $1
  IntOp $1 $1 & 1
  IntCmp $1 0 +2
    StrCpy $0 "7.1"
!endif
!ifdef VSIP_SEC02
  SectionGetFlags ${VSIP_SEC02} $1
  IntOp $1 $1 & 1
  IntCmp $1 0 +2
    StrCpy $0 "8.0"
!endif
!endif
  
  Pop $1
  Exch $0
FunctionEnd

SectionGroup "File Associations"
Section "UI Files (*.ui)" REGISTERUIEXT_SEC01
  call ModuleRegisterUI
SectionEnd
SectionGroupEnd

Function ModuleRegisterUI
  push $0
  push $1
  
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "UIExtRegistered" 1
  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}" "" "${MODULE_REGISTERUIEXT_FILE_DESC}"
  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}\shell" "" "open"

  Call GetSelectedVSIP
  Pop $1
    
  StrCmp "$1" "" 0 RegisterVSIP
  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}\shell\open" "" "${MODULE_REGISTERUIEXT_DESC_DESIGNER}"
  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}\shell\open\command" "" "${MODULE_REGISTERUIEXT_QTDIR}\bin\designer.exe $\"%1$\""
  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}\DefaultIcon" "" "${MODULE_REGISTERUIEXT_QTDIR}\bin\designer.exe,0"
  goto RegisterFinished

  RegisterVSIP:
  Push $1
  Call GetVSInstallationDir
  Pop $0

  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}\shell\open" "" "${MODULE_REGISTERUIEXT_DESC_DEVENV}"
  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}\shell\${MODULE_REGISTERUIEXT_DESC_DEVENV}\command" "" "$0\devenv.exe $\"%1$\""
  WriteRegStr HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}\DefaultIcon" "" "$VSIP_INSTDIR\ui.ico"
  RegisterFinished:
  WriteRegStr HKCR ".ui" "" "${MODULE_REGISTERUIEXT_INTERNAL_DESC}"

  pop $1
  pop $0
FunctionEnd

!macroend

;------------------------------------------------------------------------------------------------

!macro REGISTERUIEXT_DESCRIPTION
  !insertmacro MUI_DESCRIPTION_TEXT ${REGISTERUIEXT_SEC01} "This will associate the file extention .ui with the Qt GUI editor."
!macroend

;------------------------------------------------------------------------------------------------

!macro REGISTERUIEXT_STARTUP
  StrCmp $RUNNING_AS_ADMIN "true" +2
    SectionSetFlags ${REGISTERUIEXT_SEC01} 16
!macroend

;------------------------------------------------------------------------------------------------

!macro REGISTERUIEXT_FINISH
!macroend

;------------------------------------------------------------------------------------------------

!macro REGISTERUIEXT_UNSTARTUP
!macroend

;------------------------------------------------------------------------------------------------

!macro REGISTERUIEXT_UNINSTALL
Function un.ModuleRegisterUI
  push $1
  ReadRegStr $1 HKCR ".ui" ""
  strcmp $1 "${MODULE_REGISTERUIEXT_INTERNAL_DESC}" 0 continue
    ; do not delete this key since a subkey openwithlist
    ; or open withprogid may exist
    WriteRegStr HKCR ".ui" "" ""
  continue:
  ; just delete it since nobody else is supposed to use it
  DeleteRegKey HKCR "${MODULE_REGISTERUIEXT_INTERNAL_DESC}"

  pop $1
FunctionEnd

Section -un.ModuleRegisterUIExtSection
  push $0
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "UIExtRegistered"
  intcmp $0 1 0 DoneUnRegister
  call un.ModuleRegisterUI
  DoneUnRegister:
  pop $0
SectionEnd
!macroend

;------------------------------------------------------------------------------------------------

!macro REGISTERUIEXT_UNFINISH
!macroend

;------------------------------------------------------------------------------------------------

!else
!macro REGISTERUIEXT_INITIALIZE
!macroend
!macro REGISTERUIEXT_SECTIONS
!macroend
!macro REGISTERUIEXT_DESCRIPTION
!macroend
!macro REGISTERUIEXT_STARTUP
!macroend
!macro REGISTERUIEXT_FINISH
!macroend
!macro REGISTERUIEXT_UNSTARTUP
!macroend
!macro REGISTERUIEXT_UNINSTALL
!macroend
!macro REGISTERUIEXT_UNFINISH
!macroend
!endif
