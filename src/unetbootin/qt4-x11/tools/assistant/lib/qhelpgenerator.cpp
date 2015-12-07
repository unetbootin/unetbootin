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

#include "qhelpgenerator_p.h"
#include "qhelpdatainterface_p.h"

#include <math.h>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QTextCodec>
#include <QtSql/QSqlQuery>

QT_BEGIN_NAMESPACE

class QHelpGeneratorPrivate
{
public:
    QHelpGeneratorPrivate();
    ~QHelpGeneratorPrivate();

    QString error;
    QSqlQuery *query;

    int namespaceId;
    int virtualFolderId;

    QMap<QString, int> fileMap;
    QMap<int, QSet<int> > fileFilterMap;

    double progress;
    double oldProgress;
    double contentStep;
    double fileStep;
    double indexStep;
};

QHelpGeneratorPrivate::QHelpGeneratorPrivate()
{
    query = 0;
    namespaceId = -1;
    virtualFolderId = -1;
}

QHelpGeneratorPrivate::~QHelpGeneratorPrivate()
{
}



/*!
    \internal
    \class QHelpGenerator
    \since 4.4
    \brief The QHelpGenerator class generates a new
    Qt compressed help file (.qch).

    The help generator takes a help data structure as
    input for generating a new Qt compressed help files. Since
    the generation may takes some time, the generator emits
    various signals to inform about its current state.
*/

/*!
    \fn void QHelpGenerator::statusChanged(const QString &msg)

    This signal is emitted when the generation status changes.
    The status is basically a specific task like inserting
    files or building up the keyword index. The parameter
    \a msg contains the detailed status description.
*/

/*!
    \fn void QHelpGenerator::progressChanged(double progress)

    This signal is emitted when the progress changes. The
    \a progress ranges from 0 to 100.
*/

/*!
    \fn void QHelpGenerator::warning(const QString &msg)

    This signal is emitted when a non critical error occurs,
    e.g. when a referenced file cannot be found. \a msg
    contains the exact warning message.
*/

/*!
    Constructs a new help generator with the give \a parent.
*/
QHelpGenerator::QHelpGenerator(QObject *parent)
    : QObject(parent)
{
    d = new QHelpGeneratorPrivate;
}

/*!
    Destructs the help generator.
*/
QHelpGenerator::~QHelpGenerator()
{
    delete d;
}

/*!
    Takes the \a helpData and generates a new documentation
    set from it. The Qt compressed help file is written to \a
    outputFileName. Returns true on success, otherwise false.
*/
bool QHelpGenerator::generate(QHelpDataInterface *helpData,
                              const QString &outputFileName)
{
    emit progressChanged(0);
    d->error.clear();
    if (!helpData || helpData->namespaceName().isEmpty()) {
        d->error = tr("Invalid help data!");
        return false;
    }

    QString outFileName = outputFileName;
    if (outFileName.isEmpty()) {
        d->error = tr("No output file name specified!");
        return false;
    }

    QFileInfo fi(outFileName);
    if (fi.exists()) {
        if (!fi.dir().remove(fi.fileName())) {
            d->error = tr("The file %1 cannot be overwritten!").arg(outFileName);
            return false;
        }
    }

    setupProgress(helpData);

    emit statusChanged(tr("Building up file structure..."));
    bool openingOk = true;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("builder"));
        db.setDatabaseName(outFileName);
        openingOk = db.open();
        if (openingOk)
            d->query = new QSqlQuery(db);
    }

    if (!openingOk) {
        d->error = tr("Cannot open data base file %1!").arg(outFileName);
        cleanupDB();
        return false;
    }

    addProgress(1.0);
    createTables();
    insertFileNotFoundFile();
    insertMetaData(helpData->metaData());

    if (!registerVirtualFolder(helpData->virtualFolder(), helpData->namespaceName())) {
        d->error = tr("Cannot register namespace %1!").arg(helpData->namespaceName());
        cleanupDB();
        return false;
    }
    addProgress(1.0);

    emit statusChanged(tr("Insert custom filters..."));
    foreach (QHelpDataCustomFilter f, helpData->customFilters()) {
        if (!registerCustomFilter(f.name, f.filterAttributes, true)) {
            cleanupDB();
            return false;
        }
    }
    addProgress(1.0);

    int i = 1;
    QList<QHelpDataFilterSection>::const_iterator it = helpData->filterSections().constBegin();
    while (it != helpData->filterSections().constEnd()) {
        emit statusChanged(tr("Insert help data for filter section (%1 of %2)...")
            .arg(i++).arg(helpData->filterSections().count()));
        insertFilterAttributes((*it).filterAttributes());
        QByteArray ba;
        QDataStream s(&ba, QIODevice::WriteOnly);
        foreach (QHelpDataContentItem *itm, (*it).contents())
            writeTree(s, itm, 0);
        if (!insertFiles((*it).files(), helpData->rootPath(), (*it).filterAttributes())
            || !insertContents(ba, (*it).filterAttributes())
            || !insertKeywords((*it).indices(), (*it).filterAttributes())) {
            cleanupDB();
            return false;
        }
        ++it;
    }

    cleanupDB();
    emit progressChanged(100);
    emit statusChanged(tr("Documentation successfully generated."));
    return true;
}

void QHelpGenerator::setupProgress(QHelpDataInterface *helpData)
{
    d->progress = 0;
    d->oldProgress = 0;

    int numberOfFiles = 0;
    int numberOfIndices = 0;
    QList<QHelpDataFilterSection>::const_iterator it = helpData->filterSections().constBegin();
    while (it != helpData->filterSections().constEnd()) {
        numberOfFiles += (*it).files().count();
        numberOfIndices += (*it).indices().count();
        ++it;
    }
    // init      2%
    // filters   1%
    // contents 10%
    // files    60%
    // indices  27%
    d->contentStep = 10.0/(double)helpData->customFilters().count();
    d->fileStep = 60.0/(double)numberOfFiles;
    d->indexStep = 27.0/(double)numberOfIndices;
}

void QHelpGenerator::addProgress(double step)
{
    d->progress += step;
    if ((d->progress-d->oldProgress) >= 1.0 && d->progress <= 100.0) {
        d->oldProgress = d->progress;
        emit progressChanged(ceil(d->progress));
    }
}

void QHelpGenerator::cleanupDB()
{
    if (d->query) {
        d->query->clear();
        delete d->query;
        d->query = 0;
    }
    QSqlDatabase::removeDatabase(QLatin1String("builder"));
}

void QHelpGenerator::writeTree(QDataStream &s, QHelpDataContentItem *item, int depth)
{
    s << depth;
    s << item->reference();
    s << item->title();
    foreach (QHelpDataContentItem *i, item->children())
        writeTree(s, i, depth+1);
}

/*!
    Returns the last error message.
*/
QString QHelpGenerator::error() const
{
    return d->error;
}

bool QHelpGenerator::createTables()
{
    if (!d->query)
        return false;

    d->query->exec(QLatin1String("SELECT COUNT(*) FROM sqlite_master WHERE TYPE=\'table\'"
        "AND Name=\'NamespaceTable\'"));
    d->query->next();
    if (d->query->value(0).toInt() > 0) {
        d->error = tr("Some tables already exist!");
        return false;
    }

    QStringList tables;
    tables << QLatin1String("CREATE TABLE NamespaceTable ("
            "Id INTEGER PRIMARY KEY,"
            "Name TEXT )")
        << QLatin1String("CREATE TABLE FilterAttributeTable ("
            "Id INTEGER PRIMARY KEY, "
            "Name TEXT )")
        << QLatin1String("CREATE TABLE FilterNameTable ("
            "Id INTEGER PRIMARY KEY, "
            "Name TEXT )")
        << QLatin1String("CREATE TABLE FilterTable ("
            "NameId INTEGER, "
            "FilterAttributeId INTEGER )")
        << QLatin1String("CREATE TABLE IndexTable ("
            "Id INTEGER PRIMARY KEY, "
            "Name TEXT, "
            "Identifier TEXT, "
            "NamespaceId INTEGER, "
            "FileId INTEGER, "
            "Anchor TEXT )")
        << QLatin1String("CREATE TABLE IndexItemTable ("
            "Id INTEGER, "
            "IndexId INTEGER )")
        << QLatin1String("CREATE TABLE IndexFilterTable ("
            "FilterAttributeId INTEGER, "
            "IndexId INTEGER )")
        << QLatin1String("CREATE TABLE ContentsTable ("
            "Id INTEGER PRIMARY KEY, "
            "NamespaceId INTEGER, "
            "Data BLOB )")
        << QLatin1String("CREATE TABLE ContentsFilterTable ("
            "FilterAttributeId INTEGER, "
            "ContentsId INTEGER )")
        << QLatin1String("CREATE TABLE FileAttributeSetTable ("
            "Id INTEGER, "
            "FilterAttributeId INTEGER )")
        << QLatin1String("CREATE TABLE FileDataTable ("
            "Id INTEGER PRIMARY KEY, "
            "Data BLOB )")
        << QLatin1String("CREATE TABLE FileFilterTable ("
            "FilterAttributeId INTEGER, "
            "FileId INTEGER )")
        << QLatin1String("CREATE TABLE FileNameTable ("
            "FolderId INTEGER, "
            "Name TEXT, "
            "FileId INTEGER, "
            "Title TEXT )")
        << QLatin1String("CREATE TABLE FolderTable("
            "Id INTEGER PRIMARY KEY, "
            "Name Text, "
            "NamespaceID INTEGER )")
        << QLatin1String("CREATE TABLE MetaDataTable("
            "Name Text, "
            "Value BLOB )");

    foreach (QString q, tables) {
        if (!d->query->exec(q)) {
            d->error = tr("Cannot create tables!");
            return false;
        }
    }

    d->query->exec(QLatin1String("INSERT INTO MetaDataTable VALUES('qchVersion', '1.0')"));

    d->query->prepare(QLatin1String("INSERT INTO MetaDataTable VALUES('CreationDate', ?)"));
    d->query->bindValue(0, QDateTime::currentDateTime().toString(Qt::ISODate));
    d->query->exec();

    return true;
}

bool QHelpGenerator::insertFileNotFoundFile()
{
    if (!d->query)
        return false;

    d->query->exec(QLatin1String("SELECT id FROM FileNameTable WHERE Name=\'\'"));
    if (d->query->next() && d->query->isValid())
        return true;

    d->query->prepare(QLatin1String("INSERT INTO FileDataTable VALUES (Null, ?)"));
    d->query->bindValue(0, QByteArray());
    if (!d->query->exec())
        return false;

    int fileId = d->query->lastInsertId().toInt();
    d->query->prepare(QLatin1String("INSERT INTO FileNameTable (FolderId, Name, FileId, Title) "
        " VALUES (0, '', ?, '')"));
    d->query->bindValue(0, fileId);
    if (fileId > -1 && d->query->exec()) {
        d->fileMap.insert(QString(), fileId);
        return true;
    }
    return false;
}

bool QHelpGenerator::registerVirtualFolder(const QString &folderName, const QString &ns)
{
    if (!d->query || folderName.isEmpty() || ns.isEmpty())
        return false;

    d->query->prepare(QLatin1String("SELECT Id FROM FolderTable WHERE Name=?"));
    d->query->bindValue(0, folderName);
    d->query->exec();
    d->query->next();
    if (d->query->isValid() && d->query->value(0).toInt() > 0)
        return true;

    d->namespaceId = -1;
    d->query->prepare(QLatin1String("SELECT Id FROM NamespaceTable WHERE Name=?"));
    d->query->bindValue(0, ns);
    d->query->exec();
    while (d->query->next()) {
        d->namespaceId = d->query->value(0).toInt();
        break;
    }

    if (d->namespaceId < 0) {
        d->query->prepare(QLatin1String("INSERT INTO NamespaceTable VALUES(NULL, ?)"));
        d->query->bindValue(0, ns);
        if (d->query->exec())
            d->namespaceId = d->query->lastInsertId().toInt();
    }

    if (d->namespaceId > 0) {
        d->query->prepare(QLatin1String("SELECT Id FROM FolderTable WHERE Name=?"));
        d->query->bindValue(0, folderName);
        d->query->exec();
        while (d->query->next())
            d->virtualFolderId = d->query->value(0).toInt();

        if (d->virtualFolderId > 0)
            return true;

        d->query->prepare(QLatin1String("INSERT INTO FolderTable (NamespaceId, Name) "
            "VALUES (?, ?)"));
        d->query->bindValue(0, d->namespaceId);
        d->query->bindValue(1, folderName);
        if (d->query->exec()) {
            d->virtualFolderId = d->query->lastInsertId().toInt();
            return d->virtualFolderId > 0;
        }
    }
    d->error = tr("Cannot register virtual folder!");
    return false;
}

bool QHelpGenerator::insertFiles(const QStringList &files, const QString &rootPath,
                                 const QStringList &filterAttributes)
{
    if (!d->query)
        return false;

    emit statusChanged(tr("Insert files..."));
    QList<int> filterAtts;
    foreach (QString filterAtt, filterAttributes) {
        d->query->prepare(QLatin1String("SELECT Id FROM FilterAttributeTable WHERE Name=?"));
        d->query->bindValue(0, filterAtt);
        d->query->exec();
        if (d->query->next())
            filterAtts.append(d->query->value(0).toInt());
    }

    int filterSetId = -1;
    d->query->exec(QLatin1String("SELECT MAX(Id) FROM FileAttributeSetTable"));
    if (d->query->next())
        filterSetId = d->query->value(0).toInt();
    if (filterSetId < 0)
        return false;
    ++filterSetId;
    foreach (int attId, filterAtts) {
        d->query->prepare(QLatin1String("INSERT INTO FileAttributeSetTable VALUES(?, ?)"));
        d->query->bindValue(0, filterSetId);
        d->query->bindValue(1, attId);
        d->query->exec();
    }

    QString title;
    QString charSet;
    QMap<int, QSet<int> > tmpFileFilterMap;
    QList<FileNameTableData> fileNameDataList;
    QList<QByteArray> fileDataList;

    int tableFileId = 1;
    d->query->exec(QLatin1String("SELECT MAX(Id) FROM FileDataTable"));
    if (d->query->next())
        tableFileId = d->query->value(0).toInt() + 1;

    FileNameTableData fileNameData;

    int i = 0;
    foreach (QString file, files) {
        QFileInfo fi(rootPath + QDir::separator() + file);
        if (!fi.exists()) {
            emit warning(tr("The file %1 does not exist! Skipping it.")
                .arg(fi.absoluteFilePath()));
            continue;
        }

        QFile f(fi.absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly)) {
            emit warning(tr("Cannot open file %1! Skipping it.")
                .arg(fi.absoluteFilePath()));
            continue;
        }

        title.clear();
        QByteArray data;
        data = f.readAll();

        if (fi.suffix() == QLatin1String("html") || fi.suffix() == QLatin1String("htm")) {            
            charSet = QHelpGlobal::charsetFromData(data);                
            QTextStream stream(&data);
            stream.setCodec(QTextCodec::codecForName(charSet.toLatin1().constData()));
            title = QHelpGlobal::documentTitle(stream.readAll());
        }        

        QString fName = QDir::cleanPath(file);
        if (fName.startsWith(QLatin1String("./")))
            fName = fName.mid(2);

        int fileId = -1;
        if (!d->fileMap.contains(fName)) {
            fileDataList.append(qCompress(data));

            fileNameData.name = fName;
            fileNameData.fileId = tableFileId;
            fileNameData.title = title;
            fileNameDataList.append(fileNameData);

            d->fileMap.insert(fName, tableFileId);
            d->fileFilterMap.insert(tableFileId, filterAtts.toSet());
            tmpFileFilterMap.insert(tableFileId, filterAtts.toSet());

            ++tableFileId;
        } else {
            fileId = d->fileMap.value(fName);
            foreach (int filter, filterAtts) {
                if (!d->fileFilterMap.value(fileId).contains(filter)
                    && !tmpFileFilterMap.value(fileId).contains(filter)) {
                        d->fileFilterMap[fileId].insert(filter);
                        tmpFileFilterMap[fileId].insert(filter);
                }
            }
        }
    }

    if (tmpFileFilterMap.count()) {
        d->query->exec(QLatin1String("BEGIN"));
        QMap<int, QSet<int> >::const_iterator it = tmpFileFilterMap.constBegin();
        while (it != tmpFileFilterMap.constEnd()) {
            QSet<int>::const_iterator i = it.value().constBegin();
            while (i != it.value().constEnd()) {
                d->query->prepare(QLatin1String("INSERT INTO FileFilterTable VALUES(?, ?)"));
                d->query->bindValue(0, *i);
                d->query->bindValue(1, it.key());
                d->query->exec();
                ++i;
            }
            ++it;
        }

        QList<QByteArray>::const_iterator fileIt = fileDataList.constBegin();
        while (fileIt != fileDataList.constEnd()) {
            d->query->prepare(QLatin1String("INSERT INTO FileDataTable VALUES (Null, ?)"));
            d->query->bindValue(0, *fileIt);
            d->query->exec();
            ++fileIt;
            if (++i%20 == 0)
                addProgress(d->fileStep*20.0);
        }

        QList<FileNameTableData>::const_iterator fileNameIt = fileNameDataList.constBegin();
        while (fileNameIt != fileNameDataList.constEnd()) {
            d->query->prepare(QLatin1String("INSERT INTO FileNameTable (FolderId, Name, FileId, Title) "
                " VALUES (?, ?, ?, ?)"));
            d->query->bindValue(0, 1);
            d->query->bindValue(1, (*fileNameIt).name);
            d->query->bindValue(2, (*fileNameIt).fileId);
            d->query->bindValue(3, (*fileNameIt).title);
            d->query->exec();
            ++fileNameIt;
        }
        d->query->exec(QLatin1String("COMMIT"));
    }

    d->query->exec(QLatin1String("SELECT MAX(Id) FROM FileDataTable"));
    if (d->query->next()
        && d->query->value(0).toInt() == tableFileId-1) {
        addProgress(d->fileStep*(i%20));
        return true;
    }
    return false;
}

bool QHelpGenerator::registerCustomFilter(const QString &filterName, const QStringList &filterAttribs,
                                          bool forceUpdate)
{
    if (!d->query)
        return false;

    d->query->exec(QLatin1String("SELECT Id, Name FROM FilterAttributeTable"));
    QStringList idsToInsert = filterAttribs;
    QMap<QString, int> attributeMap;
    while (d->query->next()) {
        attributeMap.insert(d->query->value(1).toString(),
            d->query->value(0).toInt());
        if (idsToInsert.contains(d->query->value(1).toString()))
            idsToInsert.removeAll(d->query->value(1).toString());
    }

    foreach (QString id, idsToInsert) {
        d->query->prepare(QLatin1String("INSERT INTO FilterAttributeTable VALUES(NULL, ?)"));
        d->query->bindValue(0, id);
        d->query->exec();
        attributeMap.insert(id, d->query->lastInsertId().toInt());
    }

    int nameId = -1;
    d->query->prepare(QLatin1String("SELECT Id FROM FilterNameTable WHERE Name=?"));
    d->query->bindValue(0, filterName);
    d->query->exec();
    while (d->query->next()) {
        nameId = d->query->value(0).toInt();
        break;
    }

    if (nameId < 0) {
        d->query->prepare(QLatin1String("INSERT INTO FilterNameTable VALUES(NULL, ?)"));
        d->query->bindValue(0, filterName);
        if (d->query->exec())
            nameId = d->query->lastInsertId().toInt();
    } else if (!forceUpdate) {
        d->error = tr("The filter %1 is already registered!").arg(filterName);
        return false;
    }

    if (nameId < 0) {
        d->error = tr("Cannot register filter %1!").arg(filterName);
        return false;
    }

    d->query->prepare(QLatin1String("DELETE FROM FilterTable WHERE NameId=?"));
    d->query->bindValue(0, nameId);
    d->query->exec();

    foreach (QString att, filterAttribs) {
        d->query->prepare(QLatin1String("INSERT INTO FilterTable VALUES(?, ?)"));
        d->query->bindValue(0, nameId);
        d->query->bindValue(1, attributeMap[att]);
        if (!d->query->exec())
            return false;
    }
    return true;
}

bool QHelpGenerator::insertKeywords(const QList<QHelpDataIndexItem> keywords,
                                    const QStringList &filterAttributes)
{
    if (!d->query)
        return false;

    emit statusChanged(tr("Insert indices..."));
    int indexId = 1;
    d->query->exec(QLatin1String("SELECT MAX(Id) FROM IndexTable"));
    if (d->query->next())
        indexId = d->query->value(0).toInt() + 1;

    QList<int> filterAtts;
    foreach (QString filterAtt, filterAttributes) {
        d->query->prepare(QLatin1String("SELECT Id FROM FilterAttributeTable WHERE Name=?"));
        d->query->bindValue(0, filterAtt);
        d->query->exec();
        if (d->query->next())
            filterAtts.append(d->query->value(0).toInt());
    }

    int pos = -1;
    QString fileName;
    QString anchor;
    QString fName;
    int fileId = 1;
    QList<int> indexFilterTable;

    int i = 0;
    d->query->exec(QLatin1String("BEGIN"));
    foreach (QHelpDataIndexItem itm, keywords) {
        pos = itm.reference.indexOf(QLatin1Char('#'));
        fileName = itm.reference.left(pos);
        if (pos > -1)
            anchor = itm.reference.mid(pos+1);
        else
            anchor.clear();

        fName = QDir::cleanPath(fileName);
        if (fName.startsWith(QLatin1String("./")))
            fName = fName.mid(2);

        if (d->fileMap.contains(fName))
            fileId = d->fileMap.value(fName);
        else
            fileId = 1;

        d->query->prepare(QLatin1String("INSERT INTO IndexTable (Name, Identifier, NamespaceId, FileId, Anchor) "
            "VALUES(?, ?, ?, ?, ?)"));
        d->query->bindValue(0, itm.name);
        d->query->bindValue(1, itm.identifier);
        d->query->bindValue(2, d->namespaceId);
        d->query->bindValue(3, fileId);
        d->query->bindValue(4, anchor);
        d->query->exec();

        indexFilterTable.append(indexId++);
        if (++i%100 == 0)
            addProgress(d->indexStep*100.0);
    }
    d->query->exec(QLatin1String("COMMIT"));

    d->query->exec(QLatin1String("BEGIN"));
    foreach (int idx, indexFilterTable) {
        foreach (int a, filterAtts) {
            d->query->prepare(QLatin1String("INSERT INTO IndexFilterTable (FilterAttributeId, IndexId) "
                "VALUES(?, ?)"));
            d->query->bindValue(0, a);
            d->query->bindValue(1, idx);
            d->query->exec();
        }
    }
    d->query->exec(QLatin1String("COMMIT"));

    d->query->exec(QLatin1String("SELECT COUNT(Id) FROM IndexTable"));
    if (d->query->next() && d->query->value(0).toInt() >= keywords.count())
        return true;
    return false;
}

bool QHelpGenerator::insertContents(const QByteArray &ba,
                                    const QStringList &filterAttributes)
{
    if (!d->query)
        return false;

    emit statusChanged(tr("Insert contents..."));
    d->query->prepare(QLatin1String("INSERT INTO ContentsTable (NamespaceId, Data) "
        "VALUES(?, ?)"));
    d->query->bindValue(0, d->namespaceId);
    d->query->bindValue(1, ba);
    d->query->exec();
    int contentId = d->query->lastInsertId().toInt();
    if (contentId < 1) {
        d->error = tr("Cannot insert contents!");
        return false;
    }

    // associate the filter attributes
    foreach (QString filterAtt, filterAttributes) {
        d->query->prepare(QLatin1String("INSERT INTO ContentsFilterTable (FilterAttributeId, ContentsId) "
            "SELECT Id, ? FROM FilterAttributeTable WHERE Name=?"));
        d->query->bindValue(0, contentId);
        d->query->bindValue(1, filterAtt);
        d->query->exec();
        if (!d->query->isActive()) {
            d->error = tr("Cannot register contents!");
            return false;
        }
    }
    addProgress(d->contentStep);
    return true;
}

bool QHelpGenerator::insertFilterAttributes(const QStringList &attributes)
{
    if (!d->query)
        return false;

    d->query->exec(QLatin1String("SELECT Name FROM FilterAttributeTable"));
    QSet<QString> atts;
    while (d->query->next())
        atts.insert(d->query->value(0).toString());

    foreach (QString s, attributes) {
        if (!atts.contains(s)) {
            d->query->prepare(QLatin1String("INSERT INTO FilterAttributeTable VALUES(NULL, ?)"));
            d->query->bindValue(0, s);
            d->query->exec();
        }
    }
    return true;
}

bool QHelpGenerator::insertMetaData(const QMap<QString, QVariant> &metaData)
{
    if (!d->query)
        return false;

    QMap<QString, QVariant>::const_iterator it = metaData.constBegin();
    while (it != metaData.constEnd()) {
        d->query->prepare(QLatin1String("INSERT INTO MetaDataTable VALUES(?, ?)"));
        d->query->bindValue(0, it.key());
        d->query->bindValue(1, it.value());
        d->query->exec();
        ++it;
    }
    return true;
}

QT_END_NAMESPACE
