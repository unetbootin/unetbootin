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

#include "qfiledialog.h"

#ifndef QT_NO_FILEDIALOG

#include <private/qfiledialog_p.h>
#include <qapplication.h>
#include <private/qapplication_p.h>
#include <qt_windows.h>
#include <qglobal.h>
#include <qregexp.h>
#include <qbuffer.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qlibrary.h>

#ifndef QT_NO_THREAD
#  include <private/qmutexpool_p.h>
#endif

#include <shlobj.h>

#ifdef Q_OS_WINCE
#include <commdlg.h>
#  ifndef BFFM_SETSELECTION
#    define BFFM_SETSELECTION (WM_USER + 102)
#  endif
// Windows Mobile has a broken definition for BROWSEINFO
// Only compile fix
typedef struct qt_priv_browseinfo {
    HWND          hwndOwner;
    LPCITEMIDLIST pidlRoot;
    LPTSTR        pszDisplayName;
    LPCTSTR       lpszTitle;
    UINT          ulFlags;
    BFFCALLBACK   lpfn;
    LPARAM        lParam;
    int           iImage;
} qt_BROWSEINFO;
bool qt_priv_ptr_valid = false;
#endif


// Don't remove the lines below!
//
// resolving the W methods manually is needed, because Windows 95 doesn't include
// these methods in Shell32.lib (not even stubs!), so you'd get an unresolved symbol
// when Qt calls getExistingDirectory(), etc.
typedef LPITEMIDLIST (WINAPI *PtrSHBrowseForFolder)(BROWSEINFO*);
static PtrSHBrowseForFolder ptrSHBrowseForFolder = 0;
typedef BOOL (WINAPI *PtrSHGetPathFromIDList)(LPITEMIDLIST,LPWSTR);
static PtrSHGetPathFromIDList ptrSHGetPathFromIDList = 0;

QT_BEGIN_NAMESPACE

static void qt_win_resolve_libs()
{
    static bool triedResolve = false;

    if (!triedResolve) {
#ifndef QT_NO_THREAD
        // protect initialization
        QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
        // check triedResolve again, since another thread may have already
        // done the initialization
        if (triedResolve) {
            // another thread did initialize the security function pointers,
            // so we shouldn't do it again.
            return;
        }
#endif

        triedResolve = true;
        if (!(QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based)) {
#if !defined(Q_OS_WINCE)
            QLibrary lib(QLatin1String("shell32"));
            ptrSHBrowseForFolder = (PtrSHBrowseForFolder) lib.resolve("SHBrowseForFolderW");
            ptrSHGetPathFromIDList = (PtrSHGetPathFromIDList) lib.resolve("SHGetPathFromIDListW");
#else
            // CE stores them in a different lib and does not use unicode version
    	    HINSTANCE handle = LoadLibraryW(L"Ceshell");
            ptrSHBrowseForFolder = (PtrSHBrowseForFolder)GetProcAddress(handle, L"SHBrowseForFolder");
            ptrSHGetPathFromIDList = (PtrSHGetPathFromIDList)GetProcAddress(handle, L"SHGetPathFromIDList");
            if (ptrSHBrowseForFolder && ptrSHGetPathFromIDList)
                qt_priv_ptr_valid = true;
#endif
        }
    }
}

extern const char* qt_file_dialog_filter_reg_exp; // defined in qfiledialog.cpp
extern QStringList qt_make_filter_list(const QString &filter);

const int maxNameLen = 1023;
const int maxMultiLen = 65535;

// Returns the wildcard part of a filter.
static QString qt_win_extract_filter(const QString &rawFilter)
{
    QString result = rawFilter;
    QRegExp r(QString::fromLatin1(qt_file_dialog_filter_reg_exp));
    int index = r.indexIn(result);
    if (index >= 0)
        result = r.cap(2);
    return result.replace(QLatin1Char(' '), QLatin1Char(';'));
}

static QStringList qt_win_make_filters_list(const QString &filter)
{
    QString f(filter);

    if (f.isEmpty())
#ifndef Q_OS_WINCE
        f = QFileDialog::tr("All Files (*)");
#else
        f = QFileDialog::tr("All Files (*.*)");
#endif

    return qt_make_filter_list(f);
}

// Makes a NUL-oriented Windows filter from a Qt filter.
static QString qt_win_filter(const QString &filter)
{
    QStringList filterLst = qt_win_make_filters_list(filter);
    QStringList::Iterator it = filterLst.begin();
    QString winfilters;
    for (; it != filterLst.end(); ++it) {
        QString subfilter = *it;
        if (!subfilter.isEmpty()) {
            winfilters += subfilter;
            winfilters += QChar();
            winfilters += qt_win_extract_filter(subfilter);
            winfilters += QChar();
        }
    }
    winfilters += QChar();
    return winfilters;
}

static QString qt_win_selected_filter(const QString &filter, DWORD idx)
{
    return qt_win_make_filters_list(filter).at((int)idx - 1);
}

#ifndef Q_OS_WINCE
// Static vars for OFNA funcs:
static QByteArray aInitDir;
static QByteArray aInitSel;
static QByteArray aTitle;
static QByteArray aFilter;
// Use ANSI strings and API

// If you change this, then make sure you change qt_win_make_OFN (below) too
static OPENFILENAMEA *qt_win_make_OFNA(QWidget *parent,
				       const QString &initialSelection,
				       const QString &initialDirectory,
				       const QString &title,
				       const QString &filters,
				       QFileDialog::FileMode mode,
				       QFileDialog::Options options)
{
    if (parent)
        parent = parent->window();
    else
        parent = qApp->activeWindow();

    aTitle = title.toLocal8Bit();
    aInitDir = QDir::toNativeSeparators(initialDirectory).toLocal8Bit();
    if (initialSelection.isEmpty()) {
        aInitSel = "";
    } else {
        aInitSel = QDir::toNativeSeparators(initialSelection).toLocal8Bit();
	aInitSel.replace("<", "");
	aInitSel.replace(">", "");
	aInitSel.replace("\"", "");
	aInitSel.replace("|", "");
    }
    int maxLen = mode == QFileDialog::ExistingFiles ? maxMultiLen : maxNameLen;
    aInitSel.resize(maxLen + 1);                // make room for return value
    aFilter = filters.toLocal8Bit();

    OPENFILENAMEA* ofn = new OPENFILENAMEA;
    memset(ofn, 0, sizeof(OPENFILENAMEA));

#if defined(Q_CC_BOR) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    // according to the MSDN, this should also be necessary for MSVC, but
    // OPENFILENAME_SIZE_VERSION_400A is in not Microsoft header, as it seems
    if (QApplication::winVersion()==Qt::WV_NT || QApplication::winVersion()&Qt::WV_DOS_based) {
        ofn->lStructSize = OPENFILENAME_SIZE_VERSION_400A;
    } else {
        ofn->lStructSize = sizeof(OPENFILENAMEA);
    }
#else
    ofn->lStructSize = sizeof(OPENFILENAMEA);
#endif
    Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));
    ofn->hwndOwner = parent ? parent->winId() : 0;
    ofn->lpstrFilter = aFilter;
    ofn->lpstrFile = aInitSel.data();
    ofn->nMaxFile = maxLen;
    ofn->lpstrInitialDir = aInitDir.data();
    ofn->lpstrTitle = aTitle.data();
    ofn->Flags = (OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_EXPLORER);

    if (mode == QFileDialog::ExistingFile ||
         mode == QFileDialog::ExistingFiles)
        ofn->Flags |= (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);
    if (mode == QFileDialog::ExistingFiles)
        ofn->Flags |= (OFN_ALLOWMULTISELECT);
    if (!(options & QFileDialog::DontConfirmOverwrite))
        ofn->Flags |= OFN_OVERWRITEPROMPT;

    return ofn;
}

static void qt_win_clean_up_OFNA(OPENFILENAMEA **ofn)
{
    delete *ofn;
    *ofn = 0;
}
#endif

static QString tFilters, tTitle, tInitDir;

#ifdef UNICODE
// If you change this, then make sure you change qt_win_make_OFNA (above) too
static OPENFILENAME* qt_win_make_OFN(QWidget *parent,
                                     const QString& initialSelection,
                                     const QString& initialDirectory,
                                     const QString& title,
                                     const QString& filters,
                                     QFileDialog::FileMode mode,
				     QFileDialog::Options options)
{
    if (parent)
        parent = parent->window();
    else
        parent = qApp->activeWindow();

    tInitDir = QDir::toNativeSeparators(initialDirectory);
    tFilters = filters;
    tTitle = title;
    QString initSel = QDir::toNativeSeparators(initialSelection);
    if (!initSel.isEmpty()) {
	initSel.replace(QLatin1String("<"), QLatin1String(""));
	initSel.replace(QLatin1String(">"), QLatin1String(""));
	initSel.replace(QLatin1String("\""), QLatin1String(""));
	initSel.replace(QLatin1String("|"), QLatin1String(""));
    }

    int maxLen = mode == QFileDialog::ExistingFiles ? maxMultiLen : maxNameLen;
    TCHAR *tInitSel = new TCHAR[maxLen+1];
    if (initSel.length() > 0 && initSel.length() <= maxLen)
        memcpy(tInitSel, initSel.utf16(), (initSel.length()+1)*sizeof(QChar));
    else
        tInitSel[0] = 0;

    OPENFILENAME* ofn = new OPENFILENAME;
    memset(ofn, 0, sizeof(OPENFILENAME));

#if defined(Q_CC_BOR) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    // according to the MSDN, this should also be necessary for MSVC, but
    // OPENFILENAME_SIZE_VERSION_400 is in not Microsoft header, as it seems
    if (QApplication::winVersion()==Qt::WV_NT || QApplication::winVersion()&Qt::WV_DOS_based) {
        ofn->lStructSize= OPENFILENAME_SIZE_VERSION_400;
    } else {
        ofn->lStructSize = sizeof(OPENFILENAME);
    }
#else
    ofn->lStructSize = sizeof(OPENFILENAME);
#endif
    Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));
    ofn->hwndOwner = parent ? parent->winId() : 0;
    ofn->lpstrFilter = (TCHAR *)tFilters.utf16();
    ofn->lpstrFile = tInitSel;
    ofn->nMaxFile = maxLen;
    ofn->lpstrInitialDir = (TCHAR *)tInitDir.utf16();
    ofn->lpstrTitle = (TCHAR *)tTitle.utf16();
    ofn->Flags = (OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_EXPLORER);
    if (mode == QFileDialog::ExistingFile ||
         mode == QFileDialog::ExistingFiles)
        ofn->Flags |= (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);
    if (mode == QFileDialog::ExistingFiles)
        ofn->Flags |= (OFN_ALLOWMULTISELECT);
    if (!(options & QFileDialog::DontConfirmOverwrite))
        ofn->Flags |= OFN_OVERWRITEPROMPT;

    return ofn;
}


static void qt_win_clean_up_OFN(OPENFILENAME **ofn)
{
    delete [] (*ofn)->lpstrFile;
    delete *ofn;
    *ofn = 0;
}

#endif // UNICODE

extern void qt_win_eatMouseMove();

QString qt_win_get_open_file_name(const QFileDialogArgs &args,
                                  QString *initialDirectory,
                                  QString *selectedFilter)
{
    QString result;

    QString isel = args.selection;

    if (initialDirectory && initialDirectory->left(5) == QLatin1String("file:"))
        initialDirectory->remove(0, 5);
    QFileInfo fi(*initialDirectory);

    if (initialDirectory && !fi.isDir()) {
        *initialDirectory = fi.absolutePath();
        if (isel.isEmpty())
            isel = fi.fileName();
    }

    if (!fi.exists())
        *initialDirectory = QDir::homePath();

    DWORD selFilIdx = 0;

    int idx = 0;
    if (selectedFilter) {
        QStringList filterLst = qt_win_make_filters_list(args.filter);
        idx = filterLst.indexOf(*selectedFilter);
    }

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(args.parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);
    QT_WA({
        // Use Unicode strings and API
        OPENFILENAME* ofn = qt_win_make_OFN(args.parent, args.selection,
                                            args.directory, args.caption,
                                            qt_win_filter(args.filter),
					    QFileDialog::ExistingFile,
					    args.options);
        if (idx)
            ofn->nFilterIndex = idx + 1;
        if (GetOpenFileName(ofn)) {
            result = QString::fromUtf16((ushort*)ofn->lpstrFile);
            selFilIdx = ofn->nFilterIndex;
        }
        qt_win_clean_up_OFN(&ofn);
    } , {
        // Use ANSI strings and API
        OPENFILENAMEA* ofn = qt_win_make_OFNA(args.parent, args.selection,
                                              args.directory, args.caption,
                                              qt_win_filter(args.filter),
					      QFileDialog::ExistingFile,
					      args.options);
        if (idx)
            ofn->nFilterIndex = idx + 1;
        if (GetOpenFileNameA(ofn)) {
            result = QString::fromLocal8Bit(ofn->lpstrFile);
            selFilIdx = ofn->nFilterIndex;
        }
        qt_win_clean_up_OFNA(&ofn);
    });
    QApplicationPrivate::leaveModal(&modal_widget);

    qt_win_eatMouseMove();

    if (result.isEmpty())
        return result;

    fi = result;
    *initialDirectory = fi.path();
    if (selectedFilter)
        *selectedFilter = qt_win_selected_filter(args.filter, selFilIdx);
    return fi.absoluteFilePath();
}

QString qt_win_get_save_file_name(const QFileDialogArgs &args,
                                  QString *initialDirectory,
				  QString *selectedFilter)
{
    QString result;

    QString isel = args.selection;
    if (initialDirectory && initialDirectory->left(5) == QLatin1String("file:"))
        initialDirectory->remove(0, 5);
    QFileInfo fi(*initialDirectory);

    if (initialDirectory && !fi.isDir()) {
        *initialDirectory = fi.absolutePath();
        if (isel.isEmpty())
            isel = fi.fileName();
    }

    if (!fi.exists())
        *initialDirectory = QDir::homePath();

    DWORD selFilIdx = 0;

    int idx = 0;
    if (selectedFilter) {
        QStringList filterLst = qt_win_make_filters_list(args.filter);
        idx = filterLst.indexOf(*selectedFilter);
    }

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(args.parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);

    // This block is used below for the lpstrDefExt member.
    // Note that the current MSDN docs document this member wrong.
    // It should rather be documented as "the default extension if no extension was given and if the
    // current filter does not have a extension (e.g (*)). If the current filter have an extension, use
    // the extension of the current filter"
    QString defaultSaveExt;
    if (selectedFilter && !selectedFilter->isEmpty()) {
        defaultSaveExt = qt_win_extract_filter(*selectedFilter);
        // make sure we only have the extension
        int firstDot = defaultSaveExt.indexOf(QLatin1Char('.'));
        if (firstDot != -1) {
            defaultSaveExt.remove(0, firstDot + 1);
        } else {
            defaultSaveExt.clear();
        }
    }

    QT_WA({
        // Use Unicode strings and API
        OPENFILENAME *ofn = qt_win_make_OFN(args.parent, args.selection,
                                            args.directory, args.caption,
                                            qt_win_filter(args.filter),
					    QFileDialog::AnyFile,
					    args.options);

        ofn->lpstrDefExt = (TCHAR *)defaultSaveExt.utf16();

        if (idx)
            ofn->nFilterIndex = idx + 1;
        if (GetSaveFileName(ofn)) {
            result = QString::fromUtf16((ushort*)ofn->lpstrFile);
            selFilIdx = ofn->nFilterIndex;
        }
        qt_win_clean_up_OFN(&ofn);
    } , {
        // Use ANSI strings and API
        OPENFILENAMEA *ofn = qt_win_make_OFNA(args.parent, args.selection,
                                              args.directory, args.caption,
                                              qt_win_filter(args.filter),
					      QFileDialog::AnyFile,
					      args.options);
        QByteArray asciiExt = defaultSaveExt.toAscii();
        ofn->lpstrDefExt = asciiExt.data();

        if (idx)
            ofn->nFilterIndex = idx + 1;
        if (GetSaveFileNameA(ofn)) {
            result = QString::fromLocal8Bit(ofn->lpstrFile);
            selFilIdx = ofn->nFilterIndex;
        }
        qt_win_clean_up_OFNA(&ofn);
    });
#if defined(Q_OS_WINCE)
    int semIndex = result.indexOf(QLatin1Char(';'));
    if (semIndex >= 0)
        result = result.left(semIndex);
#endif

    QApplicationPrivate::leaveModal(&modal_widget);

    qt_win_eatMouseMove();

    if (result.isEmpty())
        return result;

    fi = result;
    *initialDirectory = fi.path();
    if (selectedFilter)
        *selectedFilter = qt_win_selected_filter(args.filter, selFilIdx);
    return fi.absoluteFilePath();
}

QStringList qt_win_get_open_file_names(const QFileDialogArgs &args,
                                       QString *initialDirectory,
                                       QString *selectedFilter)
{
    QStringList result;
    QFileInfo fi;
    QDir dir;
    QString isel;

    if (initialDirectory && initialDirectory->left(5) == QLatin1String("file:"))
        initialDirectory->remove(0, 5);
    fi = QFileInfo(*initialDirectory);

    if (initialDirectory && !fi.isDir()) {
        *initialDirectory = fi.absolutePath();
        isel = fi.fileName();
    }

    if (!fi.exists())
        *initialDirectory = QDir::homePath();

    DWORD selFilIdx = 0;

    int idx = 0;
    if (selectedFilter) {
        QStringList filterLst = qt_win_make_filters_list(args.filter);
        idx = filterLst.indexOf(*selectedFilter);
    }

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(args.parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);
    QT_WA({
        OPENFILENAME* ofn = qt_win_make_OFN(args.parent, args.selection,
                                            args.directory, args.caption,
                                            qt_win_filter(args.filter),
					    QFileDialog::ExistingFiles,
					    args.options);
        if (idx)
            ofn->nFilterIndex = idx + 1;
        if (GetOpenFileName(ofn)) {
            QString fileOrDir = QString::fromUtf16((ushort*)ofn->lpstrFile);
            selFilIdx = ofn->nFilterIndex;
            int offset = fileOrDir.length() + 1;
            if (ofn->lpstrFile[offset] == 0) {
                // Only one file selected; has full path
                fi.setFile(fileOrDir);
                QString res = fi.absoluteFilePath();
                if (!res.isEmpty())
                    result.append(res);
            }
            else {
                // Several files selected; first string is path
                dir.setPath(fileOrDir);
                QString f;
                while(!(f = QString::fromUtf16((ushort*)ofn->lpstrFile+offset)).isEmpty()) {
                    fi.setFile(dir, f);
                    QString res = fi.absoluteFilePath();
                    if (!res.isEmpty())
                        result.append(res);
                    offset += f.length() + 1;
                }
            }
        }
        qt_win_clean_up_OFN(&ofn);
    } , {
        OPENFILENAMEA* ofn = qt_win_make_OFNA(args.parent, args.selection,
                                              args.directory, args.caption,
                                              qt_win_filter(args.filter),
					      QFileDialog::ExistingFiles,
					      args.options);
        if (idx)
            ofn->nFilterIndex = idx + 1;
        if (GetOpenFileNameA(ofn)) {
            QByteArray fileOrDir(ofn->lpstrFile);
            selFilIdx = ofn->nFilterIndex;
            int offset = fileOrDir.length() + 1;
            if (ofn->lpstrFile[offset] == '\0') {
                // Only one file selected; has full path
                fi.setFile(QString::fromLocal8Bit(fileOrDir));
                QString res = fi.absoluteFilePath();
                if (!res.isEmpty())
                    result.append(res);
            }
            else {
                // Several files selected; first string is path
                dir.setPath(QString::fromLocal8Bit(fileOrDir));
                QByteArray f;
                while (!(f = QByteArray(ofn->lpstrFile + offset)).isEmpty()) {
                    fi.setFile(dir, QString::fromLocal8Bit(f));
                    QString res = fi.absoluteFilePath();
                    if (!res.isEmpty())
                        result.append(res);
                    offset += f.length() + 1;
                }
            }
            qt_win_clean_up_OFNA(&ofn);
        }
    });
    QApplicationPrivate::leaveModal(&modal_widget);

    qt_win_eatMouseMove();

    if (!result.isEmpty()) {
        *initialDirectory = fi.path();    // only save the path if there is a result
        if (selectedFilter)
            *selectedFilter = qt_win_selected_filter(args.filter, selFilIdx);
    }
    return result;
}

// MFC Directory Dialog. Contrib: Steve Williams (minor parts from Scott Powers)

static int __stdcall winGetExistDirCallbackProc(HWND hwnd,
                                                UINT uMsg,
                                                LPARAM lParam,
                                                LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED && lpData != 0) {
        QString *initDir = (QString *)(lpData);
        if (!initDir->isEmpty()) {
            // ### Lars asks: is this correct for the A version????
            QT_WA({
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, LPARAM(initDir->utf16()));
            } , {
                SendMessageA(hwnd, BFFM_SETSELECTION, TRUE, LPARAM(initDir->utf16()));
            });
        }
    } else if (uMsg == BFFM_SELCHANGED) {
        QT_WA({
            qt_win_resolve_libs();
            TCHAR path[MAX_PATH];
            ptrSHGetPathFromIDList(LPITEMIDLIST(lParam), path);
            QString tmpStr = QString::fromUtf16((ushort*)path);
            if (!tmpStr.isEmpty())
                SendMessage(hwnd, BFFM_ENABLEOK, 1, 1);
            else
                SendMessage(hwnd, BFFM_ENABLEOK, 0, 0);
            SendMessage(hwnd, BFFM_SETSTATUSTEXT, 1, LPARAM(path));
        } , {
            char path[MAX_PATH];
            SHGetPathFromIDListA(LPITEMIDLIST(lParam), path);
            QString tmpStr = QString::fromLocal8Bit(path);
            if (!tmpStr.isEmpty())
                SendMessageA(hwnd, BFFM_ENABLEOK, 1, 1);
            else
                SendMessageA(hwnd, BFFM_ENABLEOK, 0, 0);
            SendMessageA(hwnd, BFFM_SETSTATUSTEXT, 1, LPARAM(path));
        });
    }
    return 0;
}

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
#endif


QString qt_win_get_existing_directory(const QFileDialogArgs &args)
{
    QString currentDir = QDir::currentPath();
    QString result;
    QWidget *parent = args.parent;
    if (parent)
        parent = parent->window();
    else
        parent = qApp->activeWindow();
    if (parent)
        parent->createWinId();

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);
#if !defined(Q_OS_WINCE)
    QT_WA({
        qt_win_resolve_libs();
        QString initDir = QDir::toNativeSeparators(args.directory);
        TCHAR path[MAX_PATH];
        TCHAR initPath[MAX_PATH];
        initPath[0] = 0;
        path[0] = 0;
        tTitle = args.caption;
        BROWSEINFO bi;
        Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));
        bi.hwndOwner = (parent ? parent->winId() : 0);
        bi.pidlRoot = NULL;
        //### This does not seem to be respected? - the dialog always displays "Browse for folder"
        bi.lpszTitle = (TCHAR*)tTitle.utf16();
        bi.pszDisplayName = initPath;
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;
        bi.lpfn = winGetExistDirCallbackProc;
        bi.lParam = LPARAM(&initDir);
        if (ptrSHBrowseForFolder) {
            LPITEMIDLIST pItemIDList = ptrSHBrowseForFolder(&bi);
            if (pItemIDList && ptrSHGetPathFromIDList) {
                ptrSHGetPathFromIDList(pItemIDList, path);
                IMalloc *pMalloc;
                if (SHGetMalloc(&pMalloc) != NOERROR)
                    result = QString();
                else {
                    pMalloc->Free(pItemIDList);
                    pMalloc->Release();
                    result = QString::fromUtf16((ushort*)path);
                }
            } else
                result = QString();
        }
        tTitle = QString();
    } , {
        QString initDir = QDir::toNativeSeparators(args.directory);
        char path[MAX_PATH];
        char initPath[MAX_PATH];
        QByteArray ctitle = args.caption.toLocal8Bit();
        initPath[0]=0;
        path[0]=0;
        BROWSEINFOA bi;
        Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));
        bi.hwndOwner = (parent ? parent->winId() : 0);
        bi.pidlRoot = NULL;
        bi.lpszTitle = ctitle;
        bi.pszDisplayName = initPath;
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;
        bi.lpfn = winGetExistDirCallbackProc;
        bi.lParam = LPARAM(&initDir);
        LPITEMIDLIST pItemIDList = SHBrowseForFolderA(&bi);
        if (pItemIDList) {
            SHGetPathFromIDListA(pItemIDList, path);
            IMalloc *pMalloc;
            if (SHGetMalloc(&pMalloc) != NOERROR)
                result = QString();
            else {
                pMalloc->Free(pItemIDList);
                pMalloc->Release();
                result = QString::fromLocal8Bit(path);
            }
        } else
            result = QString();
    });
#else
    qt_win_resolve_libs();
    QString initDir = QDir::toNativeSeparators(args.directory);
    TCHAR path[MAX_PATH];
    TCHAR initPath[MAX_PATH];
    memset(initPath, 0 , MAX_PATH*sizeof(TCHAR));
    memset(path, 0, MAX_PATH*sizeof(TCHAR));
    tTitle = args.caption;
    qt_BROWSEINFO bi;
    Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));
    bi.hwndOwner = (parent ? parent->winId() : 0);
    bi.pidlRoot = NULL;
    bi.lpszTitle = (TCHAR*)tTitle.utf16();
    bi.pszDisplayName = initPath;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;
    bi.lpfn = winGetExistDirCallbackProc;
    bi.lParam = LPARAM(&initDir);
    if (ptrSHBrowseForFolder) {
        LPITEMIDLIST pItemIDList = ptrSHBrowseForFolder((BROWSEINFO*)&bi);
        if (pItemIDList && ptrSHGetPathFromIDList) {
            ptrSHGetPathFromIDList(pItemIDList, path);
            IMalloc *pMalloc;
            if (SHGetMalloc(&pMalloc) != NOERROR)
                result = QString();
            else {
                pMalloc->Free(pItemIDList);
                pMalloc->Release();
                result = QString::fromUtf16((ushort*)path);
            }
        } else
            result = QString();
    }
    tTitle = QString();

#endif
    QApplicationPrivate::leaveModal(&modal_widget);

    qt_win_eatMouseMove();

    // Due to a bug on Windows Me, we need to reset the current
    // directory
    if ((QSysInfo::WindowsVersion == QSysInfo::WV_98 || QSysInfo::WindowsVersion == QSysInfo::WV_Me)
	&& QDir::currentPath() != currentDir)
        QDir::setCurrent(currentDir);

    if (!result.isEmpty())
        result.replace(QLatin1String("\\"), QLatin1String("/"));
    return result;
}


QT_END_NAMESPACE

#endif
