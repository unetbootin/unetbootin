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
