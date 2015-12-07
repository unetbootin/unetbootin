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

#include "qfileiconprovider.h"

#ifndef QT_NO_FILEICONPROVIDER
#include <qstyle.h>
#include <qapplication.h>
#include <qdir.h>
#if defined(Q_WS_WIN)
#define _WIN32_IE 0x0500
#include <objbase.h>
#include <private/qpixmapdata_p.h>
#include <qpixmapcache.h>
#elif defined(Q_WS_MAC)
#include <private/qt_mac_p.h>
#endif
#ifdef Q_OS_WINCE
#include <qfunctions_wince.h>
#include <Commctrl.h>
#endif

#ifndef SHGFI_ADDOVERLAYS
#define SHGFI_ADDOVERLAYS 0x000000020
#endif

QT_BEGIN_NAMESPACE

/*!
  \class QFileIconProvider

  \brief The QFileIconProvider class provides file icon for the QDirModel class.
*/

/*!
  \enum QFileIconProvider::IconType
  \value Computer
  \value Desktop
  \value Trashcan
  \value Network
  \value Drive
  \value Folder
  \value File
*/

class QFileIconProviderPrivate
{
    Q_DECLARE_PUBLIC(QFileIconProvider)

public:
    QFileIconProviderPrivate();
    QIcon getIcon(QStyle::StandardPixmap name) const;
#ifdef Q_WS_WIN
    QIcon getWinIcon(const QFileInfo &fi) const;
#elif defined(Q_WS_MAC)
    QIcon getMacIcon(const QFileInfo &fi) const;
#endif
    QFileIconProvider *q_ptr;
    QString homePath;

private:
    QIcon file;
    QIcon fileLink;
    QIcon directory;
    QIcon directoryLink;
    QIcon harddisk;
    QIcon floppy;
    QIcon cdrom;
    QIcon ram;
    QIcon network;
    QIcon computer;
    QIcon desktop;
    QIcon trashcan;
    QIcon generic;
    QIcon home;
};

QFileIconProviderPrivate::QFileIconProviderPrivate()
{
    QStyle *style = QApplication::style();
    file = style->standardIcon(QStyle::SP_FileIcon);
    directory = style->standardIcon(QStyle::SP_DirIcon);
    fileLink = style->standardIcon(QStyle::SP_FileLinkIcon);
    directoryLink = style->standardIcon(QStyle::SP_DirLinkIcon);
    harddisk = style->standardIcon(QStyle::SP_DriveHDIcon);
    floppy = style->standardIcon(QStyle::SP_DriveFDIcon);
    cdrom = style->standardIcon(QStyle::SP_DriveCDIcon);
    network = style->standardIcon(QStyle::SP_DriveNetIcon);
    computer = style->standardIcon(QStyle::SP_ComputerIcon);
    desktop = style->standardIcon(QStyle::SP_DesktopIcon);
    trashcan = style->standardIcon(QStyle::SP_TrashIcon);
    home = style->standardIcon(QStyle::SP_DirHomeIcon);
    homePath = QDir::home().absolutePath();
}

QIcon QFileIconProviderPrivate::getIcon(QStyle::StandardPixmap name) const
{
    switch(name) {
    case QStyle::SP_FileIcon:
        return file;
    case QStyle::SP_FileLinkIcon:
        return fileLink;
    case QStyle::SP_DirIcon:
        return directory;
    case QStyle::SP_DirLinkIcon:
        return directoryLink;
    case QStyle::SP_DriveHDIcon:
        return harddisk;
    case QStyle::SP_DriveFDIcon:
        return floppy;
    case QStyle::SP_DriveCDIcon:
        return cdrom;
    case QStyle::SP_DriveNetIcon:
        return network;
    case QStyle::SP_ComputerIcon:
        return computer;
    case QStyle::SP_DesktopIcon:
        return desktop;
    case QStyle::SP_TrashIcon:
        return trashcan;
    case QStyle::SP_DirHomeIcon:
        return home;
    default:
        return QIcon();
    }
    return QIcon();
}

/*!
  Constructs a file icon provider.
*/

QFileIconProvider::QFileIconProvider()
    : d_ptr(new QFileIconProviderPrivate)
{
}

/*!
  Destroys the file icon provider.

*/

QFileIconProvider::~QFileIconProvider()
{
    delete d_ptr;
}

/*!
  Returns an icon set for the given \a type.
*/

QIcon QFileIconProvider::icon(IconType type) const
{
    Q_D(const QFileIconProvider);
    switch (type) {
    case Computer:
        return d->getIcon(QStyle::SP_ComputerIcon);
    case Desktop:
        return d->getIcon(QStyle::SP_DesktopIcon);
    case Trashcan:
        return d->getIcon(QStyle::SP_TrashIcon);
    case Network:
        return d->getIcon(QStyle::SP_DriveNetIcon);
    case Drive:
        return d->getIcon(QStyle::SP_DriveHDIcon);
    case Folder:
        return d->getIcon(QStyle::SP_DirIcon);
    case File:
        return d->getIcon(QStyle::SP_FileIcon);
    default:
        break;
    };
    return QIcon();
}

#ifdef Q_WS_WIN
QIcon QFileIconProviderPrivate::getWinIcon(const QFileInfo &fileInfo) const
{
    QIcon retIcon;
    QString fileExtension = fileInfo.suffix().toUpper();
    fileExtension.prepend( QLatin1String(".") );

    QString key;
    if (fileInfo.isFile() && !fileInfo.isExecutable() && !fileInfo.isSymLink())
        key = QLatin1String("qt_") + fileExtension;

    QPixmap pixmap;
    QPixmapCache::find(key, pixmap);

    if (!pixmap.isNull()) {
        retIcon.addPixmap(pixmap);
        if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
            retIcon.addPixmap(pixmap);
        return retIcon;
    }

    /* We don't use the variable, but by storing it statically, we
     * ensure CoInitialize is only called once. */
    static HRESULT comInit = CoInitialize(NULL);
    Q_UNUSED(comInit);

    SHFILEINFO info;
    unsigned long val = 0;

    //Get the small icon
#ifndef Q_OS_WINCE
    val = SHGetFileInfo((const WCHAR *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS);
    if (val) {
        pixmap = convertHIconToPixmap(info.hIcon);
        if (!pixmap.isNull()) {
            retIcon.addPixmap(pixmap);
            if (!key.isEmpty())
                QPixmapCache::insert(key, pixmap);
        }
        DestroyIcon(info.hIcon);
    }
#else
      val = SHGetFileInfo((const WCHAR *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
      if (val) {
          pixmap = convertHIconToPixmap(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL));
          if (!pixmap.isNull()) {
              retIcon.addPixmap(pixmap);
              if (!key.isEmpty())
                  QPixmapCache::insert(key, pixmap);
          }
          else {
              qWarning("QFileIconProviderPrivate::getWinIcon() no small icon found");
          }
          DestroyIcon(info.hIcon);
      }
      else {
          qWarning("QFileIconProviderPrivate::getWinIcon() SHGetFileInfo failed");
      }
#endif

    //Get the big icon
#ifndef Q_OS_WINCE
    val = SHGetFileInfo((const WCHAR *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS);
    if (val) {
        pixmap = convertHIconToPixmap(info.hIcon);
        if (!pixmap.isNull()) {
            retIcon.addPixmap(pixmap);
            if (!key.isEmpty())
                QPixmapCache::insert(key, pixmap);
        }
        DestroyIcon(info.hIcon);
    }
#else
    val = SHGetFileInfo((const WCHAR *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
    if (val) {
        pixmap = convertHIconToPixmap(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL), true);
        if (!pixmap.isNull()) {
            retIcon.addPixmap(pixmap);
            if (!key.isEmpty())
                QPixmapCache::insert(key + QLatin1Char('l'), pixmap);
        }
        else {
            qWarning("QFileIconProviderPrivate::getWinIcon() no large icon found");
        }
        DestroyIcon(info.hIcon);
    }
    else {
        qWarning("QFileIconProviderPrivate::getWinIcon() SHGetFileInfo failed");
    }
#endif
    return retIcon;
}

#elif defined(Q_WS_MAC)
QIcon QFileIconProviderPrivate::getMacIcon(const QFileInfo &fi) const
{
    QIcon retIcon;
    FSRef macRef;
    OSStatus status = FSPathMakeRef(reinterpret_cast<const UInt8*>(fi.canonicalFilePath().toUtf8().constData()),
                                    &macRef, 0);
    if (status != noErr)
        return retIcon;
    FSCatalogInfo info;
    HFSUniStr255 macName;
    status = FSGetCatalogInfo(&macRef, kIconServicesCatalogInfoMask, &info, &macName, 0, 0);
    if (status != noErr)
        return retIcon;
    IconRef iconRef;
    SInt16 iconLabel;
    status = GetIconRefFromFileInfo(&macRef, macName.length, macName.unicode, kIconServicesCatalogInfoMask, &info, kIconServicesNormalUsageFlag, &iconRef, &iconLabel);
    if (status != noErr)
        return retIcon;
    extern void qt_mac_constructQIconFromIconRef(const IconRef, const IconRef, QIcon*, QStyle::StandardPixmap = QStyle::SP_CustomBase); // qmacstyle_mac.cpp
    qt_mac_constructQIconFromIconRef(iconRef, 0, &retIcon);
    ReleaseIconRef(iconRef);
    return retIcon;
}
#endif


/*!
  Returns an icon for the file described by \a info.
*/

QIcon QFileIconProvider::icon(const QFileInfo &info) const
{
    Q_D(const QFileIconProvider);
#ifdef Q_WS_MAC
    QIcon retIcon = d->getMacIcon(info);
    if (!retIcon.isNull())
        return retIcon;
#elif defined Q_WS_WIN
    QIcon icon = d->getWinIcon(info);
    if (!icon.isNull())
        return icon;
#endif
    if (info.isRoot())
#if defined (Q_WS_WIN) && !defined(Q_OS_WINCE)
    {
        uint type = DRIVE_UNKNOWN;
        QT_WA({ type = GetDriveTypeW((wchar_t *)info.absoluteFilePath().utf16()); },
        { type = GetDriveTypeA(info.absoluteFilePath().toLocal8Bit()); });

        switch (type) {
        case DRIVE_REMOVABLE:
            return d->getIcon(QStyle::SP_DriveFDIcon);
        case DRIVE_FIXED:
            return d->getIcon(QStyle::SP_DriveHDIcon);
        case DRIVE_REMOTE:
            return d->getIcon(QStyle::SP_DriveNetIcon);
        case DRIVE_CDROM:
            return d->getIcon(QStyle::SP_DriveCDIcon);
        case DRIVE_RAMDISK:
        case DRIVE_UNKNOWN:
        case DRIVE_NO_ROOT_DIR:
        default:
            return d->getIcon(QStyle::SP_DriveHDIcon);
        }
    }
#else
    return d->getIcon(QStyle::SP_DriveHDIcon);
#endif
    if (info.isFile())
        if (info.isSymLink())
            return d->getIcon(QStyle::SP_FileLinkIcon);
        else {
            return d->getIcon(QStyle::SP_FileIcon);
  }
  if (info.isDir()) {
    if (info.isSymLink()) {
      return d->getIcon(QStyle::SP_DirLinkIcon);
    } else {
      if (info.absoluteFilePath() == d->homePath) {
        return d->getIcon(QStyle::SP_DirHomeIcon);
      } else {
        return d->getIcon(QStyle::SP_DirIcon);
      }
    }
  }
  return QIcon();
}

/*!
  Returns the type of the file described by \a info.
*/

QString QFileIconProvider::type(const QFileInfo &info) const
{
    if (info.isRoot())
        return QApplication::translate("QFileDialog", "Drive");
    if (info.isFile()) {
        if (!info.suffix().isEmpty())
            return info.suffix() + QLatin1Char(' ') + QApplication::translate("QFileDialog", "File");
        return QApplication::translate("QFileDialog", "File");
    }

    if (info.isDir())
        return QApplication::translate("QFileDialog",
#ifdef Q_WS_WIN
                                       "File Folder", "Match Windows Explorer"
#else
                                       "Folder", "All other platforms"
#endif
            );
    // Windows   - "File Folder"
    // OS X      - "Folder"
    // Konqueror - "Folder"
    // Nautilus  - "folder"

    if (info.isSymLink())
        return QApplication::translate("QFileDialog",
#ifdef Q_OS_MAC
                                       "Alias", "Mac OS X Finder"
#else
                                       "Shortcut", "All other platforms"
#endif
            );
    // OS X      - "Alias"
    // Windows   - "Shortcut"
    // Konqueror - "Folder" or "TXT File" i.e. what it is pointing to
    // Nautilus  - "link to folder" or "link to object file", same as Konqueror

    return QApplication::translate("QFileDialog", "Unknown");
}

QT_END_NAMESPACE

#endif
