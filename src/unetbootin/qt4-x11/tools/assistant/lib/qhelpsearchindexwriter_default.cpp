/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qhelpsearchindexwriter_default_p.h"
#include "qhelp_global.h"
#include "qhelpenginecore.h"

#include <QtCore/QDir>
#include <QtCore/QSet>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QVariant>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

namespace qt {
    namespace fulltextsearch {
        namespace std {

Writer::Writer(const QString &path)
    : indexPath(path)
    , indexFile(QString())
    , documentFile(QString())
{
    // nothing todo
}

Writer::~Writer()
{
    reset();
}

void Writer::reset()
{
    for(QHash<QString, Entry*>::ConstIterator it =
        index.begin(); it != index.end(); ++it) {
            delete it.value();
    }

    index.clear();
    documentList.clear();
}

bool Writer::writeIndex() const
{
    bool status;
    QFile idxFile(indexFile);
    if (!(status = idxFile.open(QFile::WriteOnly)))
        return status;

    QDataStream indexStream(&idxFile);
    for(QHash<QString, Entry*>::ConstIterator it =
        index.begin(); it != index.end(); ++it) {
        indexStream << it.key();
        indexStream << it.value()->documents.count();
        indexStream << it.value()->documents;
    }
    idxFile.close();

    QFile docFile(documentFile);
    if (!(status = docFile.open(QFile::WriteOnly)))
        return status;

    QDataStream docStream(&docFile);
    foreach(const QStringList list, documentList) {
        docStream << list.at(0);
        docStream << list.at(1);
    }
    docFile.close();

    return status;
}

void Writer::removeIndex() const
{
    QFile idxFile(indexFile);
    if (idxFile.exists())
        idxFile.remove();

    QFile docFile(documentFile);
    if (docFile.exists())
        docFile.remove();
}

void Writer::setIndexFile(const QString &namespaceName, const QString &attributes)
{
    QString extention = namespaceName + QLatin1String("@") + attributes;
    indexFile = indexPath + QLatin1String("/indexdb40.") + extention;
    documentFile = indexPath + QLatin1String("/indexdoc40.") + extention;
}

void Writer::insertInIndex(const QString &string, int docNum)
{
    if (string == QLatin1String("amp") || string == QLatin1String("nbsp"))
        return;

    Entry *entry = 0;
    if (index.count())
        entry = index[string];

    if (entry) {
        if (entry->documents.last().docNumber != docNum)
            entry->documents.append(Document(docNum, 1));
        else
            entry->documents.last().frequency++;
    } else {
        index.insert(string, new Entry(docNum));
    }
}

void Writer::insertInDocumentList(const QString &title, const QString &url)
{
    documentList.append(QStringList(title) << url);
}


QHelpSearchIndexWriter::QHelpSearchIndexWriter()
    : QThread()
    , m_cancel(false)
{
    // nothing todo
}

QHelpSearchIndexWriter::~QHelpSearchIndexWriter()
{
    mutex.lock();
    this->m_cancel = true;
    waitCondition.wakeOne();
    mutex.unlock();

    wait();
}

void QHelpSearchIndexWriter::cancelIndexing()
{
    mutex.lock();
    this->m_cancel = true;
    mutex.unlock();
}

void QHelpSearchIndexWriter::updateIndex(const QString &collectionFile,
                                         const QString &indexFilesFolder,
                                         bool reindex)
{
    QMutexLocker lock(&mutex);

    this->m_cancel = false;
    this->m_reindex = reindex;
    this->m_collectionFile = collectionFile;
    this->m_indexFilesFolder = indexFilesFolder;

    start(QThread::NormalPriority);
}

void QHelpSearchIndexWriter::run()
{
    mutex.lock();

    if (m_cancel) {
        mutex.unlock();
        return;
    }

    const bool reindex(this->m_reindex);
    const QLatin1String key("DefaultSearchNamespaces");
    const QString collectionFile(this->m_collectionFile);
    const QString indexPath = m_indexFilesFolder;
    
    mutex.unlock();

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;

    if (reindex)
        engine.setCustomValue(key, QLatin1String(""));

    const QStringList registeredDocs = engine.registeredDocumentations();
    const QStringList indexedNamespaces = engine.customValue(key).toString().
        split(QLatin1String("|"), QString::SkipEmptyParts);

    emit indexingStarted();

    QStringList namespaces;
    Writer writer(indexPath);
    foreach(const QString namespaceName, registeredDocs) {
        mutex.lock();
        if (m_cancel) {
            mutex.unlock();
            return;
        }
        mutex.unlock();

        // if indexed, continue
        namespaces.append(namespaceName);
        if (indexedNamespaces.contains(namespaceName))
            continue;

        const QList<QStringList> attributeSets =
            engine.filterAttributeSets(namespaceName);

        foreach (QStringList attributes, attributeSets) {
            // cleanup maybe old or unfinished files
            writer.setIndexFile(namespaceName, attributes.join(QLatin1String("@")));
            writer.removeIndex();

            QSet<QString> documentsSet;
            const QList<QUrl> docFiles = engine.files(namespaceName, attributes);
            foreach(QUrl url, docFiles) {
                if (m_cancel)
                    return;

                // get rid of duplicated files
                if (url.hasFragment())
                    url.setFragment(QString());
                
                QString s = url.toString();
                if (s.endsWith(QLatin1String(".html"))
                    || s.endsWith(QLatin1String(".htm"))
                    || s.endsWith(QLatin1String(".txt")))
                    documentsSet.insert(s);
            }

            int docNum = 0;
            const QStringList documentsList(documentsSet.toList());
            foreach(const QString url, documentsList) {
                if (m_cancel)
                    return;

                QByteArray data(engine.fileData(url));
                if (data.isEmpty())
                    continue;

                QTextStream s(data);
                QString en = QHelpGlobal::charsetFromData(data);
                s.setCodec(QTextCodec::codecForName(en.toLatin1().constData()));

                QString text = s.readAll();
                if (text.isNull())
                    continue;

                QString title = QHelpGlobal::documentTitle(text);

                int j = 0;
                int i = 0;
                bool valid = true;
                const QChar *buf = text.unicode();
                QChar str[64];
                QChar c = buf[0];

                while ( j < text.length() ) {
                    if (m_cancel)
                        return;

                    if ( c == QLatin1Char('<') || c == QLatin1Char('&') ) {
                        valid = false;
                        if ( i > 1 )
                            writer.insertInIndex(QString(str,i), docNum);
                        i = 0;
                        c = buf[++j];
                        continue;
                    }
                    if ( ( c == QLatin1Char('>') || c == QLatin1Char(';') ) && !valid ) {
                        valid = true;
                        c = buf[++j];
                        continue;
                    }
                    if ( !valid ) {
                        c = buf[++j];
                        continue;
                    }
                    if ( ( c.isLetterOrNumber() || c == QLatin1Char('_') ) && i < 63 ) {
                        str[i] = c.toLower();
                        ++i;
                    } else {
                        if ( i > 1 )
                            writer.insertInIndex(QString(str,i), docNum);
                        i = 0;
                    }
                    c = buf[++j];
                }
                if ( i > 1 )
                    writer.insertInIndex(QString(str,i), docNum);

                docNum++;
                writer.insertInDocumentList(title, url);
            }

            if (writer.writeIndex()) {
                engine.setCustomValue(key, addNamespace(
                    engine.customValue(key).toString(), namespaceName));
            }

            writer.reset();
        }
    }

    QStringListIterator qsli(indexedNamespaces);
    while (qsli.hasNext()) {
        const QString namespaceName = qsli.next();
        if (namespaces.contains(namespaceName))
            continue;

        const QList<QStringList> attributeSets =
            engine.filterAttributeSets(namespaceName);

        foreach (QStringList attributes, attributeSets) {
            writer.setIndexFile(namespaceName, attributes.join(QLatin1String("@")));
            writer.removeIndex();
        }

        engine.setCustomValue(key, removeNamespace(
            engine.customValue(key).toString(), namespaceName));
    }

    emit indexingFinished();
}

QString QHelpSearchIndexWriter::addNamespace(const QString namespaces,
                                             const QString &namespaceName)
{
    QString value = namespaces;
    if (!value.contains(namespaceName))
        value.append(namespaceName).append(QLatin1String("|"));

    return value;
}

QString QHelpSearchIndexWriter::removeNamespace(const QString namespaces,
                                                const QString &namespaceName)
{
    QString value = namespaces;
    if (value.contains(namespaceName))
        value.remove(namespaceName + QLatin1String("|"));

    return value;
}

        }   // namespace std
    }   // namespace fulltextsearch
}   // namespace qt

QT_END_NAMESPACE
