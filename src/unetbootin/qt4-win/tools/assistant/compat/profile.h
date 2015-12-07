/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef PROFILE_H
#define PROFILE_H

#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QMap>

QT_BEGIN_NAMESPACE

class DocuParser;

class Profile
{
public:
    enum ProfileType { DefaultProfile, UserProfile };
    Profile();

    bool isValid() const;

    void addDCF( const QString &docfile );
    void addDCFIcon( const QString title, const QString &icon );
    void addDCFIndexPage( const QString title, const QString &indexPage );
    void addDCFImageDir( const QString title, const QString &imgDir );
    void addDCFTitle( const QString &dcf, const QString &title );
    void addProperty( const QString &name, const QString &value );
    bool hasDocFile( const QString &docFile );
    void removeDocFileEntry( const QString &title );

    ProfileType profileType() const { return type; }
    void setProfileType( ProfileType t ) { type = t; }

    DocuParser *docuParser() const { return dparser; }
    void setDocuParser( DocuParser *dp ) { dparser = dp; }

    static Profile* createDefaultProfile(const QString &docPath = QString());
    static QString makeRelativePath(const QString &base, const QString &path);
    static QString storableFilePath(const QString &fileName);
    static QString loadableFilePath(const QString &fileName);

    uint valid:1;
    ProfileType type;
    DocuParser *dparser;
    QMap<QString,QString> props;
    QMap<QString,QString> icons;
    QMap<QString,QString> indexPages;
    QMap<QString,QString> imageDirs;
    QMap<QString,QString> dcfTitles;
    QStringList docs;
};

QT_END_NAMESPACE

#endif // PROFILE_H
