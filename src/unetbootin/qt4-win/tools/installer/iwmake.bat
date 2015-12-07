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
rem @echo off
call :init
if "%IWMAKE_STATUS%"=="failed" goto FAILED
if not exist "%IWMAKE_SCRIPTDIR%\config\%1.conf" goto FAILED
if not "%~2"=="" set IWMAKE_SECTION=%~2
for /F "eol=# tokens=1,2*" %%i in (%IWMAKE_SCRIPTDIR%\config\config.default) do set IWMAKE_TMP=%%k& call :func_delegate %%i %%j
if "%IWMAKE_STATUS%"=="failed" goto FAILED
if exist "%IWMAKE_SCRIPTDIR%\config\config.%COMPUTERNAME%" for /F "eol=# tokens=1,2*" %%i in (%IWMAKE_SCRIPTDIR%\config\config.%COMPUTERNAME%) do set IWMAKE_TMP=%%k& call :func_delegate %%i %%j
if "%IWMAKE_STATUS%"=="failed" goto FAILED
call :checkrequirements
if "%IWMAKE_STATUS%"=="failed" goto FAILED
for /F "eol=# tokens=1,2*" %%i in (%IWMAKE_SCRIPTDIR%\config\%1.conf) do set IWMAKE_TMP=%%k& call :func_delegate %%i %%j
if "%IWMAKE_STATUS%"=="failed" goto FAILED
goto DONE

:func_delegate
if "%IWMAKE_STATUS%"=="failed" goto :eof
set IWMAKE_TMP="%IWMAKE_TMP:"=%"

if /i "%1"=="sectionend" echo Leaving Section& set IWMAKE_PARSESECTION=1& goto :eof
if /i not "%1"=="section" goto callScript
echo Entering Section %~2
for %%m in (%IWMAKE_SECTION%) do call :checkSection %%m %~2
goto :eof

:callScript
if "%IWMAKE_PARSESECTION%"=="0" goto :eof

call "%IWMAKE_SCRIPTDIR%\batch\%1.bat" %2 %IWMAKE_TMP%
if not "%errorlevel%"=="0" echo %1 %2 failed! >> %IWMAKE_LOGFILE%& set IWMAKE_STATUS=failed
goto :eof

:checkSection
  if /i "%1"=="%2" echo   Skipping Section& set IWMAKE_PARSESECTION=0
goto :eof

:checkrequirements
  if not exist %IWMAKE_ROOT% mkdir %IWMAKE_ROOT%
  if not "%IWMAKE_SIGNPATH%"=="" goto CheckSIGNOK
  call "%IWMAKE_SCRIPTDIR%\batch\copy.bat" extsync sign
  call "%IWMAKE_SCRIPTDIR%\batch\env.bat" signPath "%IWMAKE_ROOT%\sign"
:CheckSIGNOK
  if not "%IWMAKE_WGET%"=="" goto CheckWGETOK
  call "%IWMAKE_SCRIPTDIR%\batch\copy.bat" extsync wget
  call "%IWMAKE_SCRIPTDIR%\batch\env.bat" wgetDir wget
:CheckWGETOK
  if exist "%IWMAKE_PERLPATH%\perl.exe" goto CheckPerlOK
  set IWMAKE_STATUS=failed
  echo Perl not found in %IWMAKE_PERLPATH%! (check your config file)
:CheckPerlOK
  if not "%IWMAKE_UNZIPAPP%"=="" goto CheckUNZIPOK
  call "%IWMAKE_SCRIPTDIR%\batch\copy.bat" extsync unzip
  call "%IWMAKE_SCRIPTDIR%\batch\env.bat" unzipApp "%IWMAKE_ROOT%\unzip\unzip.exe"
:CheckUNZIPOK
  if exist "%IWMAKE_NSISPATH%\makensis.exe" goto CheckNSISOK
  set IWMAKE_STATUS=failed
  echo NSIS not found! (check your config file)
:CheckNSISOK
  call "%IWMAKE_SCRIPTDIR%\batch\installer.bat" updateplugins
goto :eof

:init
  set IWMAKE_SCRIPTDIR=%~dp0
  set IWMAKE_SCRIPTDIR=%IWMAKE_SCRIPTDIR:~0,-1%
  call "%IWMAKE_SCRIPTDIR%\batch\env.bat" setglobals
goto :eof

:cleanup
  pushd "%IWMAKE_STARTDIR%"
  call "%IWMAKE_SCRIPTDIR%\batch\env.bat" removeglobals
  popd
goto :eof

:FAILED
  call :cleanup
  echo Failed!
goto END

:DONE
  call :cleanup
  echo Done!
goto END

:END
