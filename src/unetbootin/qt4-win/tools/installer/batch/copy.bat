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

:dest
  set IWMAKE_OUTDIR=%IWMAKE_ROOT%\%~1
  if not exist %IWMAKE_OUTDIR% mkdir %IWMAKE_OUTDIR%
goto :eof

:src
  set IWMAKE_SRCDIR=%IWMAKE_ROOT%\%~1
  if not exist %IWMAKE_SRCDIR% mkdir %IWMAKE_SRCDIR%
goto :eof

:destAbs
  set IWMAKE_OUTDIR=%1
  if not exist %IWMAKE_OUTDIR% mkdir %IWMAKE_OUTDIR%
goto :eof

:srcAbs
  set IWMAKE_SRCDIR=%1
  if not exist %IWMAKE_SRCDIR% mkdir %IWMAKE_SRCDIR%
goto :eof

:extsync
  if exist %IWMAKE_ROOT%\%~1 rd /S /Q %IWMAKE_ROOT%\%~1 >> %IWMAKE_LOGFILE%
  xcopy /H /Y /Q /I /R /E %IWMAKE_EXTERNAL%\%~1 %IWMAKE_ROOT%\%~1 >> %IWMAKE_LOGFILE%
goto :eof

:all
  xcopy /H /Y /Q /I /R /S %IWMAKE_SRCDIR%\%~1 %IWMAKE_OUTDIR%\ >> %IWMAKE_LOGFILE%
goto :eof

:filesEx
  for /F "tokens=1,2*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n& set IWMAKE_TMP3=%%~o
  echo %IWMAKE_TMP3% > %IWMAKE_ROOT%\iwmake_exclude
  xcopy /H /Y /Q /I /R /S /EXCLUDE:%IWMAKE_ROOT%\iwmake_exclude %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
goto :eof

:file
  set IWMAKE_TMP=%~1
  set IWMAKE_TMP2=%~nx1
  echo set IWMAKE_TMP3=%%IWMAKE_TMP:%IWMAKE_TMP2%=%%>"%IWMAKE_ROOT%\iwmake_tmp.bat"
  call %IWMAKE_ROOT%\iwmake_tmp.bat
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%"
  xcopy /H /Y /Q /I /R %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP3% >> %IWMAKE_LOGFILE%
goto :eof

:fileAndRename
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n& set IWMAKE_TMP4=%%~nxn
  echo set IWMAKE_TMP3=%%IWMAKE_TMP2:%IWMAKE_TMP4%=%%>"%IWMAKE_ROOT%\iwmake_tmp.bat"
  call %IWMAKE_ROOT%\iwmake_tmp.bat
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP3%"
  echo > %IWMAKE_OUTDIR%\%IWMAKE_TMP2%
  xcopy /H /Y /Q /R %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
  set IWMAKE_TMP4=
goto :eof

:files
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if not exist "%IWMAKE_OUTDIR%\%IWMAKE_TMP2%" mkdir "%IWMAKE_OUTDIR%\%IWMAKE_TMP2%"
  xcopy /H /Y /Q /I /R /S %IWMAKE_SRCDIR%\%IWMAKE_TMP% %IWMAKE_OUTDIR%\%IWMAKE_TMP2% >> %IWMAKE_LOGFILE%
goto :eof

:runtime
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if "%IWMAKE_TMP2%" == "" set IWMAKE_TMP2=bin
  xcopy /H /Y /Q /I /R %SystemRoot%\system32\msvcr%IWMAKE_TMP%.dll %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
  xcopy /H /Y /Q /I /R %SystemRoot%\system32\msvcp%IWMAKE_TMP%.dll %IWMAKE_OUTDIR%\%IWMAKE_TMP2%\ >> %IWMAKE_LOGFILE%
goto :eof

:syncqt
  pushd %IWMAKE_OUTDIR%
  if exist "include" rd /S /Q include
  if not "%QTDIR%"=="" set IWMAKE_OLD_QTDIR=%QTDIR%
  set QTDIR=%IWMAKE_OUTDIR%
  "%IWMAKE_PERLPATH%\perl" %QTDIR%\bin\syncqt -copy >> %IWMAKE_LOGFILE% 2>&1
  if not "%IWMAKE_OLD_QTDIR%"=="" set QTDIR=%IWMAKE_OLD_QTDIR%
  popd
goto :eof

:END
