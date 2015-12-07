/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef PREPROCESSORCONTROL_H
#define PREPROCESSORCONTROL_H

#include "tokenengine.h"
#include "tokenizer.h"
#include "rpplexer.h"
#include "rpptreeevaluator.h"
#include "rpp.h"
#include <QString>
#include <QStringList>
#include <QHash>

QT_BEGIN_NAMESPACE

class IncludeFiles
{
public:
    IncludeFiles(const QString &basePath, const QStringList &searchPaths);
    QString quoteLookup(const QString &currentFile,
                        const QString &includeFile)const;
    QString angleBracketLookup(const QString &includeFile) const;
    QString resolve(const QString &filename) const;
private:
    QString searchIncludePaths(const QString &includeFile)const;
    QStringList m_searchPaths;
    QString m_basePath;
};

class PreprocessorCache: public QObject
{
Q_OBJECT
public:
    PreprocessorCache();
    TokenEngine::TokenContainer sourceTokens(const QString &filename);
    Rpp::Source *sourceTree(const QString &filename);
    bool containsSourceTokens(const QString &filename);
    bool containsSourceTree(const QString &filename);
signals:
    void error(QString type, QString text);
    void readFile(QByteArray &contents, QString filename);
private:
    QByteArray readFile(const QString & filename) const;
    Tokenizer m_tokenizer;
    Rpp::RppLexer m_lexer;
    Rpp::Preprocessor m_preprocessor;
    TypedPool<Rpp::Item> m_memoryPool;
    QHash<QString, Rpp::Source *> m_sourceTrees;
    QHash<QString, TokenEngine::TokenContainer> m_sourceTokens;
};

class PreprocessorController: public QObject
{
Q_OBJECT
public:
    PreprocessorController(IncludeFiles includefiles,
                           PreprocessorCache &preprocessorCache,
                           QStringList preLoadFilesFilenames = QStringList());

    TokenEngine::TokenSectionSequence evaluate(const QString &filename, Rpp::DefineMap *activedefinitions);
public slots:
    void includeSlot(Rpp::Source *&includee, const Rpp::Source *includer,
         const QString &filename, Rpp::RppTreeEvaluator::IncludeType includeType);
    void readFile(QByteArray &contents, QString filename);
signals:
    void error(QString type, QString text);
private:
    IncludeFiles m_includeFiles;
    Rpp::RppTreeEvaluator m_rppTreeEvaluator;
    PreprocessorCache &m_preprocessorCache;
    QHash<QString, QByteArray> m_preLoadFiles;
};

Rpp::DefineMap *defaultMacros(PreprocessorCache &preprocessorCache);

class StandardOutErrorHandler : public QObject
{
Q_OBJECT
public slots:
    void error(QString type, QString text);
};

class RppPreprocessor
{
public:
    RppPreprocessor(QString basePath, QStringList includePaths, QStringList preLoadFilesFilename = QStringList());
    ~RppPreprocessor();
    TokenEngine::TokenSectionSequence evaluate(const QString &filename);
private:
    IncludeFiles m_includeFiles;
    PreprocessorCache m_cache;
    Rpp::DefineMap *m_activeDefinitions;
    PreprocessorController m_controller;
    StandardOutErrorHandler m_errorHandler;
};

QT_END_NAMESPACE

#endif
