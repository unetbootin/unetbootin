/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

/*
  doc.h
*/

#ifndef DOC_H
#define DOC_H

#include <QSet>
#include <QString>

#include "location.h"

QT_BEGIN_NAMESPACE

class Atom;
class CodeMarker;
class Config;
class DocPrivate;
class Quoter;
class Text;

class Doc
{
public:
    // the order is important
    enum SectioningUnit { Book = -2, Part, Chapter, Section1, Section2, Section3, Section4 };

    Doc() : priv(0) {}
    Doc(const Location &loc, const QString &source, const QSet<QString> &metaCommandSet);
    Doc(const Doc &doc);
    ~Doc();

    Doc& operator=( const Doc& doc );

    void renameParameters(const QStringList &oldNames, const QStringList &newNames);
    void simplifyEnumDoc();
    void setBody(const Text &body);

    const Location &location() const;
    bool isEmpty() const;
    const QString& source() const;
    const Text& body() const;
    Text briefText() const;
    Text trimmedBriefText(const QString &className) const;
    Text legaleseText() const;
    const QString& baseName() const;
    SectioningUnit granularity() const;
    const QSet<QString> &parameterNames() const;
    const QStringList &enumItemNames() const;
    const QStringList &omitEnumItemNames() const;
    const QSet<QString> &metaCommandsUsed() const;
    QStringList metaCommandArgs( const QString& metaCommand ) const;
    const QList<Text> &alsoList() const;
    bool hasTableOfContents() const;
    bool hasKeywords() const;
    bool hasTargets() const;
    const QList<Atom *> &tableOfContents() const;
    const QList<int> &tableOfContentsLevels() const;
    const QList<Atom *> &keywords() const;
    const QList<Atom *> &targets() const;
    const QMap<QString, QString> &metaTagMap() const;

    static void initialize( const Config &config );
    static void terminate();
    static QString alias( const QString &english );
    static void trimCStyleComment( Location& location, QString& str );
    static CodeMarker *quoteFromFile(const Location &location, Quoter &quoter,
                                     const QString &fileName);
    static QString canonicalTitle(const QString &title);

private:
    void detach();

    DocPrivate *priv;
};

QT_END_NAMESPACE

#endif
