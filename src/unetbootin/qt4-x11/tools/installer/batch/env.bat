:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
:: Contact: Qt Software Information (qt-info@nokia.com)
::
:: This file is part of the Windows installer of the Qt Toolkit.
::
:: $QT_BEGIN_LICENSE:LGPL$
:: Commercial Usage
:: Licensees holding valid Qt Commercial licenses may use this file in
:: accordance with the Qt Commercial License Agreement provided with the
:: Software or, alternatively, in accordance with the terms contained in
:: a written agreement between you and Nokia.
::
:: GNU Lesser General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU Lesser
:: General Public License version 2.1 as published by the Free Software
:: Foundation and appearing in the file LICENSE.LGPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU Lesser General Public License version 2.1 requirements
:: will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
::
:: In addition, as a special exception, Nokia gives you certain
:: additional rights. These rights are described in the Nokia Qt LGPL
:: Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
:: package.
::
:: GNU General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU
:: General Public License version 3.0 as published by the Free Software
:: Foundation and appearing in the file LICENSE.GPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU General Public License version 3.0 requirements will be
:: met: http://www.gnu.org/copyleft/gpl.html.
::
:: If you are unsure which license is appropriate for your use, please
:: contact the sales department at qt-sales@nokia.com.
:: $QT_END_LICENSE$
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
