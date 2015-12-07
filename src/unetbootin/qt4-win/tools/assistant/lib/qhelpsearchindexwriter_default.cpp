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
    , m_collectionFile(QString())
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

void QHelpSearchIndexWriter::updateIndex(const QString &collectionFile, bool reindex)
{
    QMutexLocker lock(&mutex);
    
    this->m_cancel = false;
    this->m_reindex = reindex;
    this->m_collectionFile = collectionFile;

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
    
    mutex.unlock();

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;

    if (reindex)
        engine.setCustomValue(key, QLatin1String(""));

    QString indexPath = engine.customValue(QLatin1String("indexFilesFolder")).toString();
    QDir dir(indexPath);
    if (!dir.isAbsolute()) {
        indexPath = QFileInfo(engine.collectionFile()).path() + dir.separator() + dir.path();
    }

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
            const QList<QUrl> docFiles = engine.files(namespaceName, attributes, QLatin1String("html"));
            foreach(QUrl url, docFiles) {
                if (m_cancel)
                    return;

                // get rid of duplicated files
                if (url.hasFragment())
                    url.setFragment(QString());
                   
                documentsSet.insert(url.toString());
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
                QString en = charsetFromData(data);
                s.setCodec(QTextCodec::codecForName(en.toLatin1().constData()));

                QString text = s.readAll();
                if (text.isNull())
                    continue;

                QString title = documentTitle(text);

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
