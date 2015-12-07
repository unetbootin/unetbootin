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
  set IWMAKE_EXTRACTDEST=%IWMAKE_ROOT%\%~1
goto :eof

:extUnpack
  set IWMAKE_EXTRACTSRC=%~n1
  pushd %IWMAKE_ROOT%
  if exist "%IWMAKE_EXTRACTSRC%.zip" del /Q /F "%IWMAKE_EXTRACTSRC%.zip"
  %IWMAKE_WGET%\wget %IWMAKE_WGETUSER% %IWMAKE_WGETPASS% %IWMAKE_RELEASELOCATION%/%IWMAKE_EXTRACTSRC%.zip >> %IWMAKE_LOGFILE% 2>&1
  popd
  call :unpack "%~1"
goto :eof

:unpack
  set IWMAKE_EXTRACTSRC=%~n1
  pushd %IWMAKE_ROOT%
  if exist "%IWMAKE_EXTRACTDEST%" rd /S /Q %IWMAKE_EXTRACTDEST% >> %IWMAKE_LOGFILE% 2>&1
  if exist "%IWMAKE_EXTRACTSRC%" rd /S /Q %IWMAKE_EXTRACTSRC% >> %IWMAKE_LOGFILE% 2>&1
  %IWMAKE_UNZIPAPP% %IWMAKE_EXTRACTSRC%.zip >> %IWMAKE_LOGFILE%
  popd
  move %IWMAKE_ROOT%\%IWMAKE_EXTRACTSRC% %IWMAKE_EXTRACTDEST% >> %IWMAKE_LOGFILE%
goto :eof

:extPatch
  pushd %IWMAKE_ROOT%
  if exist "%~1" del /Q /F "%~1"
  %IWMAKE_WGET%\wget %IWMAKE_WGETUSER% %IWMAKE_WGETPASS% %IWMAKE_RELEASELOCATION%/%~1 >> %IWMAKE_LOGFILE% 2>&1
  popd
  call :patch "%~1"
goto :eof

:patch
  pushd %IWMAKE_ROOT%
  %IWMAKE_UNZIPAPP% %~1 >> %IWMAKE_LOGFILE%
  popd
  xcopy /R /I /S /Q /Y %IWMAKE_ROOT%\%IWMAKE_EXTRACTSRC%\*.* %IWMAKE_EXTRACTDEST%\ >> %IWMAKE_LOGFILE%
  rd /S /Q %IWMAKE_ROOT%\%IWMAKE_EXTRACTSRC% >> %IWMAKE_LOGFILE%
goto :eof

:END
