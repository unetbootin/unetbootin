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

:begin
for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_COMPILER=%%~m& set IWMAKE_TMP=%%~n

if "%IWMAKE_TMP%"=="" set IWMAKE_TMP=build_%IWMAKE_COMPILER%
set IWMAKE_BUILD=%IWMAKE_ROOT%\%IWMAKE_TMP%
if not exist %IWMAKE_BUILD% mkdir %IWMAKE_BUILD%

if not "%PATH%"=="" set IWMAKE_OLD_PATH=%PATH%
if not "%QMAKESPEC%"=="" set IWMAKE_OLD_QMAKESPEC=%QMAKESPEC%
if not "%QTDIR%"=="" set IWMAKE_OLD_QTDIR=%QTDIR%
if not "%INCLUDE%"=="" set IWMAKE_OLD_INCLUDE=%INCLUDE%
if not "%LIB%"=="" set IWMAKE_OLD_LIB=%LIB%

set PATH=%IWMAKE_BUILD%\bin;%PATH%
set QTDIR=%IWMAKE_BUILD%

if "%IWMAKE_COMPILER%"=="vs2003" goto VS2003Env
if "%IWMAKE_COMPILER%"=="vs2002" goto VS2002Env
if "%IWMAKE_COMPILER%"=="vs2005" goto VS2005Env
if "%IWMAKE_COMPILER%"=="vc60" goto VC60Env
if "%IWMAKE_COMPILER%"=="mingw" goto MinGWEnv
goto :eof

:VS2003Env
set QMAKESPEC=win32-msvc.net
if not exist "%VS71COMNTOOLS%vsvars32.bat" echo VS2003 not found >> %IWMAKE_LOGFILE% & exit /b 1
call "%VS71COMNTOOLS%vsvars32.bat" >> %IWMAKE_LOGFILE%
set IWMAKE_MAKE=nmake
goto :eof

:VS2002Env
set QMAKESPEC=win32-msvc.net
if not exist "%VSCOMNTOOLS%vsvars32.bat" echo VS2002 not found >> %IWMAKE_LOGFILE% & exit /b 1
call "%VSCOMNTOOLS%vsvars32.bat" >> %IWMAKE_LOGFILE%
set IWMAKE_MAKE=nmake
goto :eof

:VS2005Env
set QMAKESPEC=win32-msvc2005
if not exist "%VS80COMNTOOLS%vsvars32.bat" echo VS2005 not found >> %IWMAKE_LOGFILE% & exit /b 1
call "%VS80COMNTOOLS%vsvars32.bat" >> %IWMAKE_LOGFILE%
set IWMAKE_MAKE=nmake
goto :eof

:VC60Env
set QMAKESPEC=win32-msvc
if not exist "%ProgramFiles%\Microsoft Visual Studio\VC98\Bin\vcvars32.bat" echo VC60 not found >> %IWMAKE_LOGFILE% & exit /b 1
call "%ProgramFiles%\Microsoft Visual Studio\VC98\Bin\vcvars32.bat" >> %IWMAKE_LOGFILE%
set IWMAKE_MAKE=nmake
goto :eof

:MinGWEnv
set QMAKESPEC=win32-g++
if not exist %IWMAKE_MINGWPATH%\bin\gcc.exe echo MinGW not found in %IWMAKE_MINGWPATH% >> %IWMAKE_LOGFILE% & exit /b 1
set PATH=%IWMAKE_MINGWPATH%\bin;%PATH%
set IWMAKE_MAKE=mingw32-make
goto :eof

:finish
  if not "%IWMAKE_OLD_PATH%"=="" set PATH=%IWMAKE_OLD_PATH%& set IWMAKE_OLD_PATH=
  if not "%IWMAKE_OLD_QMAKESPEC%"=="" set QMAKESPEC=%IWMAKE_OLD_QMAKESPEC%& set IWMAKE_OLD_QMAKESPEC=
  if not "%IWMAKE_OLD_QTDIR%"=="" set QTDIR=%IWMAKE_OLD_QTDIR%& set IWMAKE_OLD_QTDIR=
  if not "%IWMAKE_OLD_INCLUDE%"=="" set INCLUDE=%IWMAKE_OLD_INCLUDE%& set IWMAKE_OLD_INCLUDE=
  if not "%IWMAKE_OLD_LIB%"=="" set LIB=%IWMAKE_OLD_LIB%& set IWMAKE_OLD_LIB=
goto :eof

:configure
  pushd %IWMAKE_BUILD%
  configure %~1 >> %IWMAKE_LOGFILE% 2>&1
  popd
goto :eof

:bin
  pushd %IWMAKE_BUILD%
  %IWMAKE_MAKE% %~1 >>%IWMAKE_LOGFILE% 2>&1
  popd
goto :eof

:binInDir
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  pushd %IWMAKE_BUILD%\%IWMAKE_TMP%
  %IWMAKE_MAKE% %IWMAKE_TMP2% >> %IWMAKE_LOGFILE% 2>&1
  popd
goto :eof

:DBPlugins
  call "%IWMAKE_SCRIPTDIR%\batch\copy.bat" extsync sql
  set IWMAKE_SQL_OLD_LIB=%LIB%
  pushd %IWMAKE_BUILD%\src\plugins\sqldrivers\mysql
  set LIB=%IWMAKE_ROOT%\sql\mysql\lib\debug;%IWMAKE_SQL_OLD_LIB%
  qmake "INCLUDEPATH+=%IWMAKE_ROOT%\sql\mysql\include" "LIBS+=libmysql.lib ws2_32.lib advapi32.lib user32.lib" >> %IWMAKE_LOGFILE% 2>&1
  %IWMAKE_MAKE% debug >> %IWMAKE_LOGFILE% 2>&1
  set LIB=%IWMAKE_ROOT%\sql\mysql\lib\opt;%IWMAKE_SQL_OLD_LIB%
  qmake "INCLUDEPATH+=%IWMAKE_ROOT%\sql\mysql\include" "LIBS+=libmysql.lib ws2_32.lib advapi32.lib" >> %IWMAKE_LOGFILE% 2>&1
  %IWMAKE_MAKE% release >> %IWMAKE_LOGFILE% 2>&1
  popd

  set LIB=%IWMAKE_ROOT%\sql\%IWMAKE_COMPILER%;%IWMAKE_SQL_OLD_LIB%
  pushd %IWMAKE_BUILD%\src\plugins\sqldrivers\psql
  qmake "INCLUDEPATH+=%IWMAKE_ROOT%\sql\include\psql" "LIBS+=libpqd.lib ws2_32.lib advapi32.lib shfolder.lib shell32.lib" >> %IWMAKE_LOGFILE% 2>&1
  %IWMAKE_MAKE% debug >> %IWMAKE_LOGFILE% 2>&1
  qmake "INCLUDEPATH+=%IWMAKE_ROOT%\sql\include\psql" "LIBS+=libpq.lib ws2_32.lib advapi32.lib shfolder.lib shell32.lib" >> %IWMAKE_LOGFILE% 2>&1
  %IWMAKE_MAKE% release >> %IWMAKE_LOGFILE% 2>&1
  popd
  set LIB=%IWMAKE_SQL_OLD_LIB%
  set IWMAKE_SQL_OLD_LIB=
goto :eof

:root
  set IWMAKE_BUILD=%~1
  if not exist %IWMAKE_BUILD% mkdir %IWMAKE_BUILD%
goto :eof

:END
