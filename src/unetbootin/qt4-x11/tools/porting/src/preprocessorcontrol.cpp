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

#include "preprocessorcontrol.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>

QT_BEGIN_NAMESPACE
using namespace TokenEngine;
using namespace Rpp;

IncludeFiles::IncludeFiles(const QString &basePath, const QStringList &searchPaths)
:m_basePath(basePath)
{
    //prepend basePath to all relative paths in searchPaths
    foreach (QString path, searchPaths) {
        QString finalPath;
        if (QDir::isAbsolutePath(path))
            finalPath = QDir::cleanPath(path);
        else
            finalPath = QDir::cleanPath(m_basePath + QLatin1String("/") + path);

        if(QFile::exists(finalPath))
            m_searchPaths.append(finalPath);
    }
}

/*
    Performs an #include "..." style file lookup.
    Aboslute filenames are checked directly. Relative filenames are first
    looked for relative to the current file path, then the includepaths
    are searched if not found.
*/
QString IncludeFiles::quoteLookup(const QString &currentFile,
                                  const QString &includeFile) const
{
    //if includeFile is absolute, check if it exists
    if (QDir::isAbsolutePath(includeFile)) {
        if(QFile::exists(includeFile))
            return includeFile;
        else
            return QString();
    }

    //If currentFile is not an absolute file path, make it one by
    //prepending m_baspath
    QString currentFilePath;
    if(QDir::isAbsolutePath(currentFile))
        currentFilePath = currentFile;
    else
        currentFilePath = QDir::cleanPath(m_basePath + QLatin1String("/") + currentFile);

    //Check if it includeFile exists in the same dir as currentFilePath
    const QString currentPath = QFileInfo(currentFilePath).path();
    QString localFile = QDir::cleanPath(currentPath + QLatin1String("/") + includeFile);
    if(QFile::exists(localFile))
        return localFile;

    return searchIncludePaths(includeFile);
}

/*
    Performs an #include <...> style file lookup.
    Aboslute filenames are checked directly.
    Relative paths are searched for in the includepaths.
*/
QString IncludeFiles::angleBracketLookup(const QString &includeFile) const
{
    //if includeFile is absolute, check if it exists
    if (QDir::isAbsolutePath(includeFile)) {
        if(QFile::exists(includeFile))
            return includeFile;
        else
            return QString();
    }

    return searchIncludePaths(includeFile);
}

QString IncludeFiles::resolve(const QString &filename) const
{
    if(QDir::isAbsolutePath(filename))
        return filename;

    QString prepended = QDir::cleanPath(m_basePath + QLatin1String("/") + filename);
    if(QFile::exists(prepended))
        return prepended;
    else
        return QString();
}


/*
    Searches for includeFile paths by appending it to all includePaths
    and checking if the file exists. Returns QString() if the file is not
    found.
*/
QString IncludeFiles::searchIncludePaths(const QString &includeFile) const
{
    QString foundFile;
    foreach(QString includePath, m_searchPaths) {
        QString testFile = includePath + QLatin1String("/") + includeFile;
        if(QFile::exists(testFile)){
            foundFile = testFile;
            break;
        }
    }
    return foundFile;
}

QByteArray PreprocessorCache::readFile(const QString &filename) const
{
    // If anybody is connected to the readFile signal we tell them to
    // read the file for us.
    if (receivers(SIGNAL(readFile(QByteArray&,QString))) > 0) {
        QByteArray array;
        // Workaround for "not beeing able to emit from const function"
        PreprocessorCache *cache = const_cast<PreprocessorCache *>(this);
        emit cache->readFile(array, filename);
        return array;
    }

    QFile f(filename);
    if (!f.exists())
        return QByteArray();
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen())
        return QByteArray();
    return f.readAll();
}

PreprocessorCache::PreprocessorCache()
{
    connect(&m_preprocessor, SIGNAL(error(QString,QString)),
            this, SIGNAL(error(QString,QString)));
}


/*
    Return a TokenSequence with the contents of filname.
    Assumens filename exists and is readable, returns a empty
    TokenSequence if not.

    The result is cached.
*/
TokenContainer PreprocessorCache::sourceTokens(const QString &filename)
{
    // Check if the source tokens are already in the cache.
    if(m_sourceTokens.contains(filename))
        return m_sourceTokens.value(filename);

    // Read and tokenize file.
    QByteArray fileContents = readFile(filename);
    if(fileContents == QByteArray())
        return TokenContainer();

    QVector<TokenEngine::Token> tokenList = m_tokenizer.tokenize(fileContents);

    // Create a FileInfo object that holds the filename for this container.
    FileInfo *containterFileInfo = new FileInfo;
    containterFileInfo->filename = filename;

    // Create container.
    TokenContainer tokenContainer(fileContents, tokenList, containterFileInfo);

    // Insert into cache.
    m_sourceTokens.insert(filename, tokenContainer);
    return tokenContainer;
}

/*
    Return a Source* tree representing the contents of filename.
    Assumens filename exists and is readable, returns a empty
    Source object if not.

    The result is cached.
*/
Source *PreprocessorCache::sourceTree(const QString &filename)
{
    // Check if the Rpp tree for this file is already in the cache.
    if(m_sourceTrees.contains(filename))
        return m_sourceTrees.value(filename);

    // Get the tokens for the contents of this file.
    TokenContainer tokenContainer = sourceTokens(filename);

    // Run lexer and the preprocessor-parser.
    QVector<Type> tokenTypes = m_lexer.lex(tokenContainer);
    Source *source = m_preprocessor.parse(tokenContainer, tokenTypes, &m_memoryPool);
    source->setFileName(filename);

    // Insert into cache.
    if(tokenContainer.count() > 0) //don't cache empty files.
        m_sourceTrees.insert(filename, source);

    return source;
}


/*
    Returns whether the cache contains a TokenContainer for the given filename.
*/
bool PreprocessorCache::containsSourceTokens(const QString &filename)
{
    return m_sourceTokens.contains(filename);
}

/*
    Returns whether the cache contains a Preprocessor tree for the given filename.
*/
bool PreprocessorCache::containsSourceTree(const QString &filename)
{
    return m_sourceTrees.contains(filename);
}

PreprocessorController::PreprocessorController(IncludeFiles includeFiles,
        PreprocessorCache &preprocessorCache,
        QStringList preLoadFilesFilenames)
:m_includeFiles(includeFiles),
 m_preprocessorCache(preprocessorCache)
 {
    // Load qt3 headers from resources. The headers are stored as
    // QHash<QString, QByteArray>, serialized using QDataStream. The hash
    // maps filename -> contents.
    if (preLoadFilesFilenames != QStringList()) {
        foreach (QString filename,  preLoadFilesFilenames) {
            QFile f(filename);
            if (f.open(QIODevice::ReadOnly)) {
                QByteArray buffer = f.readAll();
                f.close();
                QDataStream stream(buffer);
                QHash<QString, QByteArray> files;
                stream >> files;
                m_preLoadFiles.unite(files);
            }
        }
    }
    
    //connect include callback
    connect(&m_rppTreeEvaluator,
        SIGNAL(includeCallback(Rpp::Source *&, const Rpp::Source *,
        const QString &, Rpp::RppTreeEvaluator::IncludeType)),
        SLOT(includeSlot(Rpp::Source *&, const Rpp::Source *,
        const QString &, Rpp::RppTreeEvaluator::IncludeType)));

    // connect readFile callback
    connect(&m_preprocessorCache, SIGNAL(readFile(QByteArray&,QString)),
        SLOT(readFile(QByteArray&,QString)));

    //connect error handlers
    connect(&m_preprocessorCache , SIGNAL(error(QString,QString)),
            this, SIGNAL(error(QString,QString)));
}

/*
    Callback from RppTreeEvaluator, called when we evaluate an #include
    directive. We do a filename lookup based on the type of include, and then ask
    the cache to give us the source tree for that file.
*/
void PreprocessorController::includeSlot(Source *&includee,
                                         const Source *includer,
                                         const QString &filename,
                                         RppTreeEvaluator::IncludeType includeType)
{
    QString newFilename;
    if(includeType == RppTreeEvaluator::QuoteInclude)
        newFilename = m_includeFiles.quoteLookup(includer->fileName(), filename);
    else //AngleBracketInclude
        newFilename = m_includeFiles.angleBracketLookup(filename);

    if (QFile::exists(newFilename)) {
        includee = m_preprocessorCache.sourceTree(newFilename);
        return;
    }

    if (m_preLoadFiles.contains(filename)) {
        includee = m_preprocessorCache.sourceTree(filename);
        return;
    }

    includee = m_preprocessorCache.sourceTree(newFilename);
    emit error(QLatin1String("Error"), QLatin1String("Could not find file ") + filename);
}

/*
    Callback connected to preprocessorCache. Tries to load a file from
    m_preLoadFiles before going to disk.
*/
void PreprocessorController::readFile(QByteArray &contents, QString filename)
{
    if (m_preLoadFiles.contains(filename)) {
        contents = m_preLoadFiles.value(filename);
        return;
    }

    QFile f(filename);
    if (!f.exists())
        return;
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen())
        return;
    contents = f.readAll();
}

/*
    Preprocess file give by filename. Filename is resloved agains the basepath
    set in IncludeFiles.
*/
TokenSectionSequence PreprocessorController::evaluate(const QString &filename, Rpp::DefineMap *activedefinitions)
{
    QString resolvedFilename = m_includeFiles.resolve(filename);
    if(!QFile::exists(resolvedFilename))
        emit error(QLatin1String("Error"), QLatin1String("Could not find file: ") + filename);
    Source *source  = m_preprocessorCache.sourceTree(resolvedFilename);

    return m_rppTreeEvaluator.evaluate(source, activedefinitions);
}

QByteArray defaultDefines =
    "#define __attribute__(a...)  \n \
         #define __attribute__ \n \
         #define __extension \n \
         #define __extension__ \n \
         #define __restrict \n \
         #define __restrict__      \n \
         #define __volatile         volatile\n \
         #define __volatile__       volatile\n \
         #define __inline             inline\n \
         #define __inline__           inline\n \
         #define __const               const\n \
         #define __const__             const\n \
         #define __asm               asm\n \
         #define __asm__             asm\n \
         #define __GNUC__                 2\n \
         #define __GNUC_MINOR__          95\n  \
         #define __cplusplus \n \
         #define __linux__ \n";


/*
    Returns a DefineMap containing the above macro definitions. The DefineMap
    will contain pointers to data stored in the provided cache object.
*/
Rpp::DefineMap *defaultMacros(PreprocessorCache &cache)
{
    DefineMap *defineMap = new DefineMap();
    //write out default macros to a temp file
    QTemporaryFile tempfile;
    tempfile.open();
    tempfile.write(defaultDefines);
    tempfile.flush();

    IncludeFiles *includeFiles = new IncludeFiles(QString(), QStringList());
    PreprocessorController preprocessorController(*includeFiles, cache);
    //evaluate default macro file.
    preprocessorController.evaluate(tempfile.fileName(), defineMap);
    delete includeFiles;
    return defineMap;
}

void StandardOutErrorHandler::error(QString type, QString text)
{
    Q_UNUSED(type);
    puts(qPrintable(text));
}

/*
    RppPreprocessor is a convenience class that contains all the components
    needed to preprocess files. Error messages are printed to standard out.
*/
RppPreprocessor::RppPreprocessor(QString basePath, QStringList includePaths, QStringList preLoadFilesFilenames)
:m_includeFiles(basePath, includePaths)
,m_activeDefinitions(defaultMacros(m_cache))
,m_controller(m_includeFiles, m_cache, preLoadFilesFilenames)
{
    QObject::connect(&m_controller, SIGNAL(error(QString,QString)), &m_errorHandler, SLOT(error(QString,QString)));
}

RppPreprocessor::~RppPreprocessor()
{
    delete m_activeDefinitions;
}

TokenEngine::TokenSectionSequence RppPreprocessor::evaluate(const QString &filename)
{
    DefineMap defMap = *m_activeDefinitions;
    return m_controller.evaluate(filename, &defMap);
}

QT_END_NAMESPACE
