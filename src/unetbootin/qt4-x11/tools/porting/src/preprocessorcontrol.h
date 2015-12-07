/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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
