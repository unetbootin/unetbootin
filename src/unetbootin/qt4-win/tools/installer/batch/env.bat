:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
::
:: This file is part of the Windows installer of the Qt Toolkit.
::
:: This file may be used under the terms of the GNU General Public
:: License versions 2.0 or 3.0 as published by the Free Software
:: Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
:: included in the packaging of this file.  Alternatively you may (at
:: your option) use any later version of the GNU General Public
:: License if such license has been publicly approved by Trolltech ASA
:: (or its successors, if any) and the KDE Free Qt Foundation. In
:: addition, as a special exception, Trolltech gives you certain
:: additional rights. These rights are described in the Trolltech GPL
:: Exception version 1.2, which can be found at
:: http://www.trolltech.com/products/qt/gplexception/ and in the file
:: GPL_EXCEPTION.txt in this package.
::
:: Please review the following information to ensure GNU General
:: Public Licensing requirements will be met:
:: http://trolltech.com/products/qt/licenses/licensing/opensource/. If
:: you are unsure which license is appropriate for your use, please
:: review the following information:
:: http://trolltech.com/products/qt/licenses/licensing/licensingoverview
:: or contact the sales department at sales@trolltech.com.
::
:: In addition, as a special exception, Trolltech, as the sole
:: copyright holder for Qt Designer, grants users of the Qt/Eclipse
:: Integration plug-in the right for the Qt/Eclipse Integration to
:: link to functionality provided by Qt Designer and its related
:: libraries.
::
:: This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
:: INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
:: A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
:: granted herein.
::
:: This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
:: WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
call :%1 %2
goto END

:setglobals
set IWMAKE_PARSESECTION=1
set IWMAKE_MINGWPATH=c:\MinGW\bin
set IWMAKE_STARTDIR=%CD%
set IWMAKE_NSISCONF=%IWMAKE_SCRIPTDIR%\nsis\config.nsh
set IWMAKE_ROOT=c:\package
set IWMAKE_OUTDIR=%IWMAKE_ROOT%
set IWMAKE_SRCDIR=%IWMAKE_ROOT%
set IWMAKE_EXTRACTDEST=%IWMAKE_ROOT%
set IWMAKE_NSISPATH=%PROGRAMFILES%\NSIS
call %IWMAKE_SCRIPTDIR%\batch\log.bat fileAbs "%IWMAKE_STARTDIR%\log.txt"
goto :eof

:signPath
  set IWMAKE_SIGNPATH=%~1
goto :eof

:wgetPath
  set IWMAKE_WGET=%~1
goto :eof

:wgetDir
  set IWMAKE_WGET=%IWMAKE_ROOT%\%~1
goto :eof

:NSISPath
  set IWMAKE_NSISPATH=%~1
goto :eof

:PerlPath
  set IWMAKE_PERLPATH=%~1
goto :eof

:MinGWPath
  set IWMAKE_MINGWPATH=%~1
goto :eof

:unzipApp
  set IWMAKE_UNZIPAPP=%~1
goto :eof

:releaseLocation
  set IWMAKE_WGETUSER= 
  set IWMAKE_WGETPASS= 
  for /F "tokens=1,2*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& if not "%%~n"=="" set IWMAKE_TMP2=%%~n& if not "%%~o"=="" set IWMAKE_TMP3=%%~o
  if not "%IWMAKE_TMP2%"=="" set IWMAKE_WGETUSER=--http-user=%IWMAKE_TMP2%
  if not "%IWMAKE_TMP3%"=="" set IWMAKE_WGETPASS=--http-passwd=%IWMAKE_TMP3%
  set IWMAKE_RELEASELOCATION=%IWMAKE_TMP%
goto :eof

:removeglobals
if not "%IWMAKE_OLD_PATH%"=="" call %IWMAKE_SCRIPTDIR%\batch\build.bat finish
set IWMAKE_RELEASELOCATION=
set IWMAKE_NSISPATH=
set IWMAKE_SECTION=
set IWMAKE_WGET=
set IWMAKE_WGETUSER=
set IWMAKE_WGETPASS=
set IWMAKE_UNZIPAPP=
set IWMAKE_MINGWPATH=
set IWMAKE_MAKE=
set IWMAKE_PERLPATH=
set IWMAKE_STARTDIR=
set IWMAKE_SCRIPTDIR=
set IWMAKE_NSISCONF=
set IWMAKE_RESULT=
set IWMAKE_TMP=
set IWMAKE_TMP2=
set IWMAKE_TMP3=
set IWMAKE_STATUS=
set IWMAKE_LOGFILE=
set IWMAKE_BUILD=
set IWMAKE_ROOT=
set IWMAKE_OUTDIR=
set IWMAKE_EXTERNAL=
set IWMAKE_OLD_PATH=
set IWMAKE_OLD_QMAKESPEC=
set IWMAKE_OLD_QTDIR=
set IWMAKE_OLD_INCLUDE=
set IWMAKE_OLD_LIB=
set IWMAKE_COMPILER=
set IWMAKE_SRCDIR=
set IWMAKE_EXTRACTSRC=
set IWMAKE_EXTRACTDEST=
set IWMAKE_PARSESECTION=
set IWMAKE_OUTPUT_FILE=
set IWMAKE_SIGNPATH=
goto :eof

:root
set IWMAKE_ROOT=%~1
goto :eof

:extroot
set IWMAKE_EXTERNAL=%~1
goto :eof

:END
