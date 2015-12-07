/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QTOKENIZER_P_H
#define QTOKENIZER_P_H

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

#include "qtoken_p.h"
#include "qreader_p.h"
#include "qtokenstream_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QChar>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

class QHELP_EXPORT QCLuceneTokenizer : public QCLuceneTokenStream
{
public:
    QCLuceneTokenizer(const QCLuceneReader &reader);
    virtual ~QCLuceneTokenizer();

    void close();
    bool next(QCLuceneToken &token);

protected:
    friend class QCLuceneStandardTokenizer;

private:
    QCLuceneTokenizer();
    QCLuceneReader reader;
};

class QHELP_EXPORT QCLuceneStandardTokenizer : public QCLuceneTokenizer
{
public:
    QCLuceneStandardTokenizer(const QCLuceneReader &reader);
    ~QCLuceneStandardTokenizer();

    bool readApostrophe(const QString &string, QCLuceneToken &token);
    bool readAt(const QString &string, QCLuceneToken &token);
    bool readCompany(const QString &string, QCLuceneToken &token);
};

class QCLuceneCharTokenizer : public QCLuceneTokenizer
{

};

class QCLuceneLetterTokenizer : public QCLuceneCharTokenizer
{

};

class QCLuceneLowerCaseTokenizer : public QCLuceneLetterTokenizer
{

};

class QCLuceneWhitespaceTokenizer : public QCLuceneCharTokenizer
{

};

class QCLuceneKeywordTokenizer : public QCLuceneTokenizer
{

};

QT_END_NAMESPACE

#endif  // QTOKENIZER_P_H
