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
!ifndef QTENV_INCLUDE
!define QTENV_INCLUDE
#
# creates a qtvars.bat file in $QTDIR\bin
# push "vs2003" #compiler
# push "c:\qt"  #QTDIR
# call MakeQtVarsFile
#
Function MakeQtVarsFile
  exch $1 ; QTDIR
  exch
  exch $3 ; vs version
  push $0 ; file handle
  push $2
  push $4

  push $3
  call GetMkSpec
  pop $2

  ClearErrors
  FileOpen $0 "$1\bin\qtvars.bat" w
  IfErrors done
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "rem$\r$\n"
  FileWrite $0 "rem This file is generated$\r$\n"
  FileWrite $0 "rem$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "echo Setting up a Qt environment...$\r$\n"
  FileWrite $0 "echo -- QTDIR set to $1$\r$\n"
  FileWrite $0 "echo -- Added $1\bin to PATH$\r$\n"
  FileWrite $0 "echo -- QMAKESPEC set to $2$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "set QTDIR=$1$\r$\n"
  FileWrite $0 "set PATH=$1\bin;%PATH%$\r$\n"
  FileWrite $0 "set QMAKESPEC=$2$\r$\n"
  
  call IsExpressVersion
  pop $4
  strcmp $4 "" noExpressVersion
  FileWrite $0 "$\r$\n"
  FileWrite $0 'regedit /e S$$D$$K$$ "HKEY_LOCAL_MACHINE\Software\Microsoft\MicrosoftSDK"$\r$\n'
  Filewrite $0 'if not exist S$$D$$K$$ goto ENDSDK\r$\n'
  FileWrite $0 'find "Install Dir" < S$$D$$K$$ > D$$I$$R$$$\r$\n'
  FileWrite $0 'del S$$D$$K$$$\r$\n'
  FileWrite $0 'for /f "tokens=2 delims==" %%i in (D$$I$$R$$) do call %%i\setenv$\r$\n'
  FileWrite $0 'del D$$I$$R$$$\r$\n'
  Filewrite $0 ':ENDSDK\r$\n'
  noExpressVersion:

  push $3
  call GetVSVarsFile
  pop $2
  strcmp $2 "" novsvars
    FileWrite $0 "$\r$\n"
    FileWrite $0 'if not "%1"=="vsvars" goto END$\r$\n'
    FileWrite $0 'call "$2"$\r$\n'
    FileWrite $0 ":END$\r$\n"

    FileWrite $0 "$\r$\n"
    FileWrite $0 'if not "%1"=="vsstart" goto ENDSTARTVS$\r$\n'
    FileWrite $0 'call "$2"$\r$\n'

    strcmp $3 "vc60" vc60startup
      FileWrite $0 "devenv /useenv$\r$\n"
      Goto donevsstartup
    vc60startup:
      FileWrite $0 "msdev /useenv$\r$\n"
    donevsstartup:
    
    FileWrite $0 ":ENDSTARTVS$\r$\n"
    
  novsvars:
  FileWrite $0 "$\r$\n"
  FileClose $0
  done:
  pop $4
  pop $2
  pop $0
  pop $3
  pop $1
FunctionEnd

Function GetMkSpec
  exch $0
  StrCmp $0 "mingw" MINGW
  StrCmp $0 "vs2005" VS2005
  StrCmp $0 "vs2003" VS2003
  StrCmp $0 "vs2002" VS2002
  StrCmp $0 "vc60" VS60
  StrCmp $0 "icc" ICC

  MINGW:
  pop $0
  push "win32-g++"
  goto done

  VS2005:
  pop $0
  push "win32-msvc2005"
  goto done

  VS2003:
  pop $0
  push "win32-msvc.net"
  goto done

  VS2002:
  pop $0
  push "win32-msvc.net"
  goto done

  VS60:
  pop $0
  push "win32-msvc"
  goto done
  
  ICC:
  pop $0
  push "win32-icc"
  goto done

  done:
FunctionEnd

!define AD_COMPILER_NAME_VS2005 "Visual Studio .NET 2005"
!define AD_COMPILER_NAME_VS2005_EXPRESS "Visual C++ 2005 Express Edition"
!define AD_COMPILER_NAME_VS2003 "Visual Studio .NET 2003"
!define AD_COMPILER_NAME_VS2002 "Visual Studio .NET 2002"
!define AD_COMPILER_NAME_VC60 "Visual Studio 6.0"
!define AD_COMPILER_NAME_ICC "Intel C++ Compiler"
!define AD_COMPILER_NAME_MINGW "MinGW (Must be in PATH!)"

Function GetShortCompilerName
  exch $0

  strcmp "$0" "${AD_COMPILER_NAME_VS2005}" 0 +3
  strcpy $0 "vs2005"
  goto done

  strcmp "$0" "${AD_COMPILER_NAME_VS2005_EXPRESS}" 0 +3
  strcpy $0 "vs2005"
  goto done
  
  strcmp "$0" "${AD_COMPILER_NAME_VS2003}" 0 +3
  strcpy $0 "vs2003"
  goto done

  strcmp "$0" "${AD_COMPILER_NAME_VS2002}" 0 +3
  strcpy $0 "vs2002"
  goto done

  strcmp "$0" "${AD_COMPILER_NAME_VC60}" 0 +3
  strcpy $0 "vc60"
  goto done

  strcmp "$0" "${AD_COMPILER_NAME_ICC}" 0 +3
  strcpy $0 "icc"
  goto done

  strcmp "$0" "${AD_COMPILER_NAME_MINGW}" 0 +3
  strcpy $0 "mingw"
  goto done

  strcpy $0 "" ;this is bad!

  done:
  exch $0
FunctionEnd

Function IsExpressVersion
  push $0
  ReadRegStr $0 HKLM "Software\Microsoft\VCExpress\8.0" "InstallDir"
  ClearErrors
  exch $0
FunctionEnd

Function AutoDetectCompilers
  push $0
  push $1
  
  strcpy $1 ""
  
  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\8.0" "InstallDir"
  strcmp $0 "" +2
  strcpy $1 "$1${AD_COMPILER_NAME_VS2005}|"

  ReadRegStr $0 HKLM "Software\Microsoft\VCExpress\8.0" "InstallDir"
  strcmp $0 "" +2
  strcpy $1 "$1${AD_COMPILER_NAME_VS2005_EXPRESS}|"

  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\7.1" "InstallDir"
  strcmp $0 "" +2
  strcpy $1 "$1${AD_COMPILER_NAME_VS2003}|"

  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\7.0" "InstallDir"
  strcmp $0 "" +2
  strcpy $1 "$1${AD_COMPILER_NAME_VS2002}|"

  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  strcmp $0 "" +2
  strcpy $1 "$1${AD_COMPILER_NAME_VC60}|"

  ReadRegStr $0 HKLM "Software\Intel\Compilers\C++\80" "Major Version"
  strcmp $0 "" +2
  strcpy $1 "$1${AD_COMPILER_NAME_ICC}|"
  
  strcpy $1 "$1${AD_COMPILER_NAME_MINGW}"
  
  exch
  pop $0
  exch $1
FunctionEnd

Function GetVSVarsFile
  exch $1
  push $0

  StrCmp $1 "vs2005" VS2005
  StrCmp $1 "vs2003" VS2003
  StrCmp $1 "vs2002" VS2002
  StrCmp $1 "vc60" VS60

  push "" ;empty string if not found
  goto done

  VS2005:
  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\8.0\Setup\VS" "ProductDir"
  StrCmp $0 "" +1 foundVSDir ; found msvc.net 2005

  ReadRegStr $0 HKLM "Software\Microsoft\VCExpress\8.0\Setup\VS" "ProductDir"
  StrCmp $0 "" +1 foundVSDir ; found msvc.net 2005 epress

  VS2003:
  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\7.1\Setup\VS" "ProductDir"
  StrCmp $0 "" +1 foundVSDir ; found msvc.net 2003

  VS2002:
  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\7.0\Setup\VS" "ProductDir"
  StrCmp $0 "" +1 foundVSDir ; found msvc.net 2002

  VS60:
  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\6.0\Setup\Microsoft Visual C++" "ProductDir"
  StrCmp $0 "" +1 foundVCDir ; found msvc 6.0

  push "" ;empty string if not found
  goto done

  foundVSDir:
    push "$0\Common7\Tools\vsvars32.bat"
    goto done

  foundVCDir:
    push "$0\bin\vcvars32.bat"

  done:
    exch
    pop $0
    exch
    pop $1
FunctionEnd

!endif ;QTENV_INCLUDE
