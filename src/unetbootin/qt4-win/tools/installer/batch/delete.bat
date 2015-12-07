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

:destDir
  if exist "%IWMAKE_OUTDIR%\%~1" rd /S /Q %IWMAKE_OUTDIR%\%~1
goto :eof

:dir
  if exist "%IWMAKE_ROOT%\%~1" rd /S /Q %IWMAKE_ROOT%\%~1
goto :eof

:dirAbs
  if exist "%~1"  rd /S /Q %~1
goto :eof

:file
  del /Q /F %IWMAKE_OUTDIR%\%~1 >> %IWMAKE_LOGFILE% 2>&1
  exit /b 0
goto :eof

:files
  del /S /Q /F %IWMAKE_OUTDIR%\%~1 >> %IWMAKE_LOGFILE% 2>&1
  exit /b 0
goto :eof

:line
  for /F "tokens=1*" %%m in ("%~1") do set IWMAKE_TMP=%%~m& set IWMAKE_TMP2=%%~n
  if exist "%IWMAKE_ROOT%\tmp_line.txt" del /Q /F "%IWMAKE_ROOT%\tmp_line.txt" >> %IWMAKE_LOGFILE%
  type "%IWMAKE_ROOT%\%IWMAKE_TMP%" | find /V "%IWMAKE_TMP2%" >> "%IWMAKE_ROOT%\tmp_line.txt"
  xcopy /Y /Q /R %IWMAKE_ROOT%\tmp_line.txt %IWMAKE_ROOT%\%IWMAKE_TMP% >> %IWMAKE_LOGFILE%
goto :eof

:END
