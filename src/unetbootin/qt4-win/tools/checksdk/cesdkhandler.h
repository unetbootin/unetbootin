/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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
#ifndef CE_SDK_HANDLER_INCL
#define CE_SDK_HANDLER_INCL

#include <QStringList>
#include <QDir>

QT_USE_NAMESPACE

#define VCINSTALL_MACRO "$(VCInstallDir)"
#define VSINSTALL_MACRO "$(VSInstallDir)"

class CeSdkInfo
{
public:
    CeSdkInfo();
    inline QString                  name();
    inline QString                  binPath();
    inline QString                  includePath();
    inline QString                  libPath();
    QStringList                     environment();
    inline bool                     isValid();
    inline int                      majorVersion();
    inline int                      minorVersion();
    inline bool                     isSupported();
private:
    friend class                    CeSdkHandler;
    QString                         m_name;
    QString                         m_bin;
    QString                         m_include;
    QString                         m_lib;
    int                             m_major;
    int                             m_minor;
};

inline QString CeSdkInfo::name(){ return m_name; }
inline QString CeSdkInfo::binPath(){ return m_bin; }
inline QString CeSdkInfo::includePath(){ return m_include; }
inline QString CeSdkInfo::libPath(){ return m_lib; }
inline bool CeSdkInfo::isValid(){ return !m_name.isEmpty() && !m_bin.isEmpty() && !m_include.isEmpty() && !m_lib.isEmpty(); }
inline int CeSdkInfo::majorVersion(){ return m_major; }
inline int CeSdkInfo::minorVersion(){ return m_minor; }
inline bool CeSdkInfo::isSupported() { return m_major >= 5; }

class CeSdkHandler
{
public:
                                    CeSdkHandler();
    bool                            parse();
    inline QList<CeSdkInfo>         listAll() const;
    CeSdkInfo                       find(const QString &name);
private:
    inline QString                  fixPaths(QString path) const;
    QList<CeSdkInfo>                m_list;
    QString                         VCInstallDir;
    QString                         VSInstallDir;
};

inline QList<CeSdkInfo> CeSdkHandler::listAll() const
{
    return m_list;
}

inline QString CeSdkHandler::fixPaths(QString path) const
{
    return QDir::toNativeSeparators(QDir::cleanPath(path.replace(VCINSTALL_MACRO, VCInstallDir).replace(VSINSTALL_MACRO, VSInstallDir).replace(QLatin1String("$(PATH)"), QLatin1String("%PATH%"))));
}

#endif