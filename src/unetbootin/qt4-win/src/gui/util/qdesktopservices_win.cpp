/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <qsettings.h>
#include <qdir.h>
#include <qurl.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <qtemporaryfile.h>
#include <qcoreapplication.h>

#include <windows.h>
#include <shlobj.h>
#if !defined(Q_OS_WINCE)
#  include <intshcut.h>
#else
#  include <qguifunctions_wince.h>
#  if !defined(STANDARDSHELL_UI_MODEL)
#    include <winx.h>
#  endif
#endif

#ifndef QT_NO_DESKTOPSERVICES

QT_BEGIN_NAMESPACE

//#undef UNICODE

static bool openDocument(const QUrl &file)
{
    if (!file.isValid())
        return false;

    quintptr returnValue;
    QT_WA({
                returnValue = (quintptr)ShellExecute(0, 0, (TCHAR *)file.toString().utf16(), 0, 0, SW_SHOWNORMAL);
            } , {
                returnValue = (quintptr)ShellExecuteA(0, 0, file.toString().toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);
            });
    return (returnValue > 32); //ShellExecute returns a value greater than 32 if successful
}

static bool launchWebBrowser(const QUrl &url)
{
    if (url.scheme() == QLatin1String("mailto")) {
        //Retrieve the commandline for the default mail client
        //the key used below is the command line for the mailto: shell command
        DWORD  bufferSize = 2 * MAX_PATH;
        long  returnValue =  -1;
        QString command;

        HKEY handle;
        LONG res;
        QT_WA ({
            res = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"mailto\\Shell\\Open\\Command", 0, KEY_READ, &handle);
            if (res != ERROR_SUCCESS)
                return false;

            wchar_t keyValue[2 * MAX_PATH];
            returnValue = RegQueryValueExW(handle, L"", 0, 0, reinterpret_cast<unsigned char*>(keyValue), &bufferSize);
            if (!returnValue)
                command = QString::fromRawData((QChar*)keyValue, bufferSize);
        }, {
            res = RegOpenKeyExA(HKEY_CLASSES_ROOT, "mailto\\Shell\\Open\\Command", 0, KEY_READ, &handle);
            if (res != ERROR_SUCCESS)
                return false;

            char keyValue[2 * MAX_PATH];
            returnValue = RegQueryValueExA(handle, "", 0, 0, reinterpret_cast<unsigned char*>(keyValue), &bufferSize);
            if (!returnValue)
                command = QString::fromLocal8Bit(keyValue);
        });
        RegCloseKey(handle);

        if(returnValue)
            return false;
        command = command.trimmed();
        //Make sure the path for the process is in quotes
        int index = -1 ;
        if (command[0]!= QLatin1Char('\"')) {
            index = command.indexOf(QLatin1String(".exe "), 0, Qt::CaseInsensitive);
            command.insert(index+4, QLatin1Char('\"'));
            command.insert(0, QLatin1Char('\"'));
        }
        //pass the url as the parameter
        index =  command.lastIndexOf(QLatin1String("%1"));
        if (index != -1){
            command.replace(index, 2, url.toString());
        }
        //start the process
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));
        QT_WA ({
            STARTUPINFO si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);

            returnValue = CreateProcess(NULL, (TCHAR*)command.utf16(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        }, {
            STARTUPINFOA si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);

            returnValue = CreateProcessA(NULL, command.toLocal8Bit().data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        });

        if (!returnValue)
            return false;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }

    if (!url.isValid())
        return false;

    quintptr returnValue;
     QT_WA ({
         returnValue = (quintptr)ShellExecute(0, 0, (TCHAR *) QString::fromUtf8(url.toEncoded().constData()).utf16(), 0, 0, SW_SHOWNORMAL);
            } , {
                returnValue = (quintptr)ShellExecuteA(0, 0, url.toEncoded().constData(), 0, 0, SW_SHOWNORMAL);
            });
    return (returnValue > 32);
}

QString QDesktopServices::storageLocation(StandardLocation type)
{
    QSettings settings(QSettings::UserScope, QLatin1String("Microsoft"), QLatin1String("Windows"));
    settings.beginGroup(QLatin1String("CurrentVersion/Explorer/Shell Folders"));
    switch (type) {
    case DataLocation:
        if (!settings.contains(QLatin1String("Local AppData")))
            break;
        return settings.value(QLatin1String("Local AppData")).toString()
            + QLatin1String("\\") + QCoreApplication::organizationName()
            + QLatin1String("\\") + QCoreApplication::applicationName();
        break;
    case DesktopLocation:
        return settings.value(QLatin1String("Desktop")).toString();
        break;

    case DocumentsLocation:
        return settings.value(QLatin1String("Personal")).toString();
        break;

    case FontsLocation:
        return settings.value(QLatin1String("Fonts")).toString();
        break;

    case ApplicationsLocation:
        return settings.value(QLatin1String("Programs")).toString();
        break;

    case MusicLocation:
        return settings.value(QLatin1String("My Music")).toString();
        break;

    case MoviesLocation:
        return settings.value(QLatin1String("My Video")).toString();
        break;

    case PicturesLocation:
        return settings.value(QLatin1String("My Pictures")).toString();
        break;

    case QDesktopServices::HomeLocation:
        return QDir::homePath(); break;

    case QDesktopServices::TempLocation:
        return QDir::tempPath(); break;

    default:
        break;
    }

    return QString();
}

QString QDesktopServices::displayName(StandardLocation type)
{
    Q_UNUSED(type);
    return QString();
}

QT_END_NAMESPACE

#endif // QT_NO_DESKTOPSERVICES
