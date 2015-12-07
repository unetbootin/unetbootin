/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef QHELPSEARCHINDEXDEFAULT_H
#define QHELPSEARCHINDEXDEFAULT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience
// of the help generator tools. This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QDataStream>

QT_BEGIN_NAMESPACE

namespace QtHelpInternal {

struct Document {
    Document(qint16 d, qint16 f) 
        : docNumber(d), frequency(f) {}
    
    Document() 
        : docNumber(-1), frequency(0) {}
    
    bool operator==(const Document &doc) const {
	    return docNumber == doc.docNumber;
    }
    bool operator<(const Document &doc) const {
	    return frequency > doc.frequency;
    }
    bool operator<=(const Document &doc) const {
	    return frequency >= doc.frequency;
    }
    bool operator>(const Document &doc) const {
	    return frequency < doc.frequency;
    }
    
    qint16 docNumber;
    qint16 frequency;
};

struct DocumentInfo : public Document {
    DocumentInfo() 
        : Document(-1, 0), documentTitle(QString()), documentUrl(QString()) {}
    
    DocumentInfo(qint16 d, qint16 f, const QString &title, const QString &url) 
        : Document(d, f), documentTitle(title), documentUrl(url) {}

    DocumentInfo(const Document &document, const QString &title, const QString &url)
        : Document(document.docNumber, document.frequency), documentTitle(title), documentUrl(url) {}
    
    QString documentTitle;
    QString documentUrl;
};

struct Entry {
    Entry(qint16 d) { documents.append(Document(d, 1)); }
    Entry(QVector<Document> l) : documents(l) {}
    
    QVector<Document> documents;
};

struct PosEntry {
    PosEntry(int p) { positions.append(p); }
    QList<uint> positions;
};

struct Term {
    Term() : frequency(-1) {}
    Term(const QString &t, int f, QVector<Document> l) : term(t), frequency(f), documents(l) {}
    QString term;
    int frequency;
    QVector<Document>documents;
    bool operator<(const Term &i2) const { return frequency < i2.frequency; }
};

struct TermInfo {
    TermInfo() : frequency(-1) {}
    TermInfo(const QString &t, int f, QVector<DocumentInfo> l) 
        : term(t), frequency(f), documents(l) {}
    
    bool operator<(const TermInfo &i2) const { return frequency < i2.frequency; }

    QString term;
    int frequency;
    QVector<DocumentInfo>documents;
};

} // namespace QtHelpInternal

using QtHelpInternal::Document;
using QtHelpInternal::DocumentInfo;
using QtHelpInternal::Entry;
using QtHelpInternal::PosEntry;
using QtHelpInternal::Term;
using QtHelpInternal::TermInfo;

QDataStream &operator>>(QDataStream &s, Document &l);
QDataStream &operator<<(QDataStream &s, const Document &l);

QT_END_NAMESPACE

#endif  // QHELPSEARCHINDEXDEFAULT_H
