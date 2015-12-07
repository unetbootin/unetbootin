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
!ifndef SYSTEM_INCLUDE
!define SYSTEM_INCLUDE

!define QTVSIP2003_GUID "{789202F4-94F5-4f0a-AA00-73295FEBFD68}"
!define QTVSIP2005_GUID "{789202F4-94F5-4f0a-AA00-73295FEBFD69}"

!define QMSNET2002_GUID "{C174ACCD-D856-4B60-9887-0FF9E841E0EC}"
!define QMSNET2003_GUID "{C174ACCE-D857-4B61-9888-0FF9E841E0ED}"
!define QMSNET2005_GUID "{14E98DB4-A232-49a4-8EC1-8CE4F6985C73}"

!macro GetVSInstallationDir UN
; Usage:
;
;   push "7.0"
;   call GetVSInstallationDir
;   pop $0
;
; If the requested VS version can be found, its
; installation directory is returned.
Function ${UN}GetVSInstallationDir
  Exch $0
  Push $1
  ReadRegStr $1 HKLM "Software\Microsoft\VisualStudio\$0" "InstallDir"
  StrCpy $0 $1
  StrCmp $0 "" 0 +2
    SetErrors
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro GetVSInstallationDir ""
!insertmacro GetVSInstallationDir "un."


!macro IsDotNETInstalled UN
; Usage:
;
;  push "8.0"
;  call IsDotNETInstalled
;  pop $0
;
;  $0 contains the path where the .NET framework is installed.
;  If not installation can be found $0 is empty.
Function ${UN}IsDotNETInstalled
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5
  
  StrCpy $5 $0

  ReadRegStr $4 HKEY_LOCAL_MACHINE "Software\Microsoft\.NETFramework" "InstallRoot"
  Push $4
  Exch $EXEDIR
  Exch $EXEDIR
  Pop $4

  IfFileExists $4 0 noDotNET
  StrCpy $0 0

  EnumStart:
  EnumRegKey $2 HKEY_LOCAL_MACHINE "Software\Microsoft\.NETFramework\Policy"  $0
  IntOp $0 $0 + 1
  StrCmp $2 "" noDotNET
  StrCpy $1 0

  EnumPolicy:
  EnumRegValue $3 HKEY_LOCAL_MACHINE "Software\Microsoft\.NETFramework\Policy\$2" $1
  IntOp $1 $1 + 1
  StrCmp $3 "" EnumStart

  StrCmp $5 "8.0" 0 +2
    StrCmp $2 "v2.0" 0 EnumPolicy

  IfFileExists "$4\$2.$3" foundDotNET EnumPolicy

  noDotNET:
  StrCpy $0 0
  Goto done

  foundDotNET:
  StrCpy $0 "$4\$2.$3"

  done:
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsDotNETInstalled ""
!insertmacro IsDotNETInstalled "un."

!macro IsQMsNetInstalled UN
; Usage:
;
;  push "8.0"
;  call IsQMsNetInstalled
;  pop $0
Function ${UN}IsQMsNetInstalled
  Exch $0
  Push $1
  Push $2
  Push $3
  
  StrCmp $0 "7.0" 0 +2
    StrCpy $2 "${QMSNET2002_GUID}"
  StrCmp $0 "7.1" 0 +2
    StrCpy $2 "${QMSNET2003_GUID}"
  StrCmp $0 "8.0" 0 +2
    StrCpy $2 "${QMSNET2005_GUID}"

  StrCpy $3 0

  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\$0\NewProjectTemplates\TemplateDirs\$2\/2" "TemplatesDir"
  StrCmp $1 "" +3
    StrCpy $3 1
    goto done

  ReadRegStr $1 HKCU "SOFTWARE\Microsoft\VisualStudio\$0\NewProjectTemplates\TemplateDirs\$2\/2" "TemplatesDir"
  StrCmp $1 "" +2
    StrCpy $3 1

done:
  StrCpy $0 $3

  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsQMsNetInstalled ""
!insertmacro IsQMsNetInstalled "un."

!macro IsQMsDevInstalled UN
; Usage:
;
;  call IsQMsDevInstalled
;  pop $0
Function ${UN}IsQMsDevInstalled
  Push $0
  Push $1
  Push $2

  StrCpy $0 0

  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "Filename"
  StrCmp $1 "" +3
    StrCpy $0 1
    goto done

  ReadRegStr $1 HKCU "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "Filename"
  StrCmp $1 "" +2
    StrCpy $0 1

done:
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsQMsDevInstalled ""
!insertmacro IsQMsDevInstalled "un."

!macro IsIntegrationInstalled UN
; Usage:
;
;  push "8.0"
;  call IsIntegrationInstalled
;  pop $0
Function ${UN}IsIntegrationInstalled
  Exch $0
  Push $1
  Push $2

  StrCmp $0 "7.1" 0 +2
    StrCpy $2 "${QTVSIP2003_GUID}"
  StrCmp $0 "8.0" 0 +2
    StrCpy $2 "${QTVSIP2005_GUID}"

  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\$0\Packages\$2" "ProductName"
  
  StrCpy $0 0
  StrCmp $1 "" done
    StrCpy $0 1

done:
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsIntegrationInstalled ""
!insertmacro IsIntegrationInstalled "un."

!macro AdministratorRights UN
Function ${UN}HasAdminRights
  push $0
  ClearErrors
  UserInfo::GetAccountType
  IfErrors Yes ;It's probably Win95
  pop $0
  StrCmp $0 "Admin" Yes
  StrCmp $0 "Power" Yes

  StrCpy $0 "false"
  goto Done

  Yes:
  StrCpy $0 "true"

  Done:
  exch $0
FunctionEnd
!macroend
!insertmacro AdministratorRights ""
!insertmacro AdministratorRights "un."

!endif ;SYSTEM_INCLUDE