/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#ifndef METAINFO_H
#define METAINFO_H

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

struct MetaInfoSingleFile
{
    qint64 length;
    QByteArray md5sum;
    QString name;
    int pieceLength;
    QList<QByteArray> sha1Sums;
};

struct MetaInfoMultiFile
{
    qint64 length;
    QByteArray md5sum;
    QString path;
};

class MetaInfo
{
public:
    enum FileForm {
        SingleFileForm,
        MultiFileForm
    };

    MetaInfo();
    void clear();

    bool parse(const QByteArray &data);
    QString errorString() const;

    QByteArray infoValue() const;

    FileForm fileForm() const;
    QString announceUrl() const;
    QStringList announceList() const;
    QDateTime creationDate() const;
    QString comment() const;
    QString createdBy() const;

    // For single file form
    MetaInfoSingleFile singleFile() const;

    // For multifile form
    QList<MetaInfoMultiFile> multiFiles() const;
    QString name() const;
    int pieceLength() const;
    QList<QByteArray> sha1Sums() const;

    // Total size
    qint64 totalSize() const;

private: 
    QString errString;
    QByteArray content;
    QByteArray infoData;

    FileForm metaInfoFileForm;
    MetaInfoSingleFile metaInfoSingleFile;
    QList<MetaInfoMultiFile> metaInfoMultiFiles;
    QString metaInfoAnnounce;
    QStringList metaInfoAnnounceList;
    QDateTime metaInfoCreationDate;
    QString metaInfoComment;
    QString metaInfoCreatedBy;
    QString metaInfoName;
    int metaInfoPieceLength;
    QList<QByteArray> metaInfoSha1Sums;
};

#endif
