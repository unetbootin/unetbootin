/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "qtresourcemodel_p.h"
#include <rcc.h>

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QResource>
#include <QtCore/QFileInfo>
#include <QtCore/QIODevice>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QBuffer>

QT_BEGIN_NAMESPACE

enum { debugResourceModel = 0 };

// ------------------- QtResourceSetPrivate
class QtResourceSetPrivate
{
    QtResourceSet *q_ptr;
    Q_DECLARE_PUBLIC(QtResourceSet)
public:
    QtResourceSetPrivate(QtResourceModel *model = 0);

    QtResourceModel *m_resourceModel;
};

QtResourceSetPrivate::QtResourceSetPrivate(QtResourceModel *model) :
   q_ptr(0),
   m_resourceModel(model)
{
}

// -------------------- QtResourceModelPrivate
class QtResourceModelPrivate
{
    QtResourceModel *q_ptr;
    Q_DECLARE_PUBLIC(QtResourceModel)
    Q_DISABLE_COPY(QtResourceModelPrivate)
public:
    QtResourceModelPrivate();
    void activate(QtResourceSet *resourceSet, const QStringList &newPaths, int *errorCount = 0, QString *errorMessages = 0);
    void removeOldPaths(QtResourceSet *resourceSet, const QStringList &newPaths);

    QMap<QString, bool>                     m_pathToModified;
    QMap<QtResourceSet *, QStringList>      m_resourceSetToPaths;
    QMap<QtResourceSet *, bool>             m_resourceSetToReload; // while path is recreated it needs to be reregistered
                                                                   // (it is - in the new current resource set, but when the path was used in
                                                                   // other resource set
                                                                   // then later when that resource set is activated it needs to be reregistered)
    QMap<QtResourceSet *, bool>             m_newlyCreated; // all created but not activated yet
                                                            // (if was active at some point and it's not now it will not be on that map)
    QMap<QString, QList<QtResourceSet *> >  m_pathToResourceSet;
    QtResourceSet                          *m_currentResourceSet;

    typedef QMap<QString, const QByteArray *> PathDataMap;
    PathDataMap m_pathToData;

    QMap<QString, QStringList> m_pathToContents; // qrc path to its contents.
    QMap<QString, QString>     m_fileToQrc; // this map contains the content of active resource set only.
                                            // Activating different resource set changes the contents.

private:
    void registerResourceSet(QtResourceSet *resourceSet);
    void unregisterResourceSet(QtResourceSet *resourceSet);

    const QByteArray *createResource(const QString &path, QStringList *contents, int *errorCount, QIODevice &errorDevice) const;
    void deleteResource(const QByteArray *data) const;
};

QtResourceModelPrivate::QtResourceModelPrivate() :
    q_ptr(0),
    m_currentResourceSet(0)
{
}

// --------------------- QtResourceSet
QtResourceSet::QtResourceSet() :
    d_ptr(new QtResourceSetPrivate)
{
    d_ptr->q_ptr = this;
}

QtResourceSet::QtResourceSet(QtResourceModel *model) :
     d_ptr(new QtResourceSetPrivate(model))
{
    d_ptr->q_ptr = this;
}

QtResourceSet::~QtResourceSet()
{
    delete d_ptr;
}

QStringList QtResourceSet::activeQrcPaths() const
{
    QtResourceSet *that = const_cast<QtResourceSet *>(this);
    return d_ptr->m_resourceModel->d_ptr->m_resourceSetToPaths.value(that);
}

void QtResourceSet::activateQrcPaths(const QStringList &paths, int *errorCount, QString *errorMessages)
{
    d_ptr->m_resourceModel->d_ptr->activate(this, paths, errorCount, errorMessages);
}

bool QtResourceSet::isModified(const QString &path) const
{
    return d_ptr->m_resourceModel->isModified(path);
}

void QtResourceSet::setModified(const QString &path)
{
    d_ptr->m_resourceModel->setModified(path);
}

// ------------------- QtResourceModelPrivate
const QByteArray *QtResourceModelPrivate::createResource(const QString &path, QStringList *contents, int *errorCount, QIODevice &errorDevice) const
{
    typedef RCCResourceLibrary::ResourceDataFileMap ResourceDataFileMap;
    const QByteArray *rc = 0;
    *errorCount = -1;
    contents->clear();
    do {
        // run RCC
        RCCBuilder builder;
        builder.writeBinary = true;
        builder.verbose = true;
        RCCResourceLibrary library;
        builder.initializeLibrary(library);
        library.setInputFiles(QStringList(path));

        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        if (!library.readFiles(/* ignore errors*/ true, errorDevice))
            break;
        // return code cannot be fully trusted, might still be empty
        const ResourceDataFileMap resMap = library.resourceDataFileMap();
        if (resMap.empty())
            break;

        if (!library.output(buffer, errorDevice))
            break;

        *errorCount = library.failedResources().size();
        *contents = resMap.keys();

        buffer.close();
        rc = new QByteArray(buffer.data());
    } while (false);

    if (debugResourceModel)
        qDebug() << "createResource" << path << "returns data=" << rc << " hasWarnings=" << *errorCount;
    return rc;
}

void QtResourceModelPrivate::deleteResource(const QByteArray *data) const
{
    if (data) {
        if (debugResourceModel)
            qDebug() << "deleteResource";
        delete data;
    }
}

void QtResourceModelPrivate::registerResourceSet(QtResourceSet *resourceSet)
{
    if (!resourceSet)
        return;

    // unregister old paths (all because the order of registration is important), later it can be optimized a bit
    QStringList toRegister = resourceSet->activeQrcPaths();
    QStringListIterator itRegister(toRegister);
    while (itRegister.hasNext()) {
        const QString path = itRegister.next();
        if (debugResourceModel)
            qDebug() << "registerResourceSet " << path;
        const PathDataMap::const_iterator itRcc = m_pathToData.constFind(path);
        if (itRcc != m_pathToData.constEnd()) { // otherwise data was not created yet
            if (!QResource::registerResource(reinterpret_cast<const uchar *>(itRcc.value()->constData()))) {
                qDebug() << "** WARNING: Failed to register " << path << " (QResource failure).";
            } else {
                QStringList contents = m_pathToContents.value(path);
                QStringListIterator itContents(contents);
                while (itContents.hasNext()) {
                    const QString filePath = itContents.next();
                    if (!m_fileToQrc.contains(filePath)) // the first loaded resource has higher priority in qt resource system
                        m_fileToQrc.insert(filePath, path);
                }
            }
        }
    }
}

void QtResourceModelPrivate::unregisterResourceSet(QtResourceSet *resourceSet)
{
    if (!resourceSet)
        return;

    // unregister old paths (all because the order of registration is importans), later it can be optimized a bit
    QStringList toUnregister = resourceSet->activeQrcPaths();
    QStringListIterator itUnregister(toUnregister);
    while (itUnregister.hasNext()) {
        const QString path = itUnregister.next();
        if (debugResourceModel)
            qDebug() << "unregisterResourceSet " << path;
        const PathDataMap::const_iterator itRcc = m_pathToData.constFind(path);
        if (itRcc != m_pathToData.constEnd()) { // otherwise data was not created yet
            if (!QResource::unregisterResource(reinterpret_cast<const uchar *>(itRcc.value()->constData())))
                qDebug() << "** WARNING: Failed to unregister " << path << " (QResource failure).";
        }
    }
    m_fileToQrc.clear();
}

void QtResourceModelPrivate::activate(QtResourceSet *resourceSet, const QStringList &newPaths, int *errorCountPtr, QString *errorMessages)
{
    if (debugResourceModel)
        qDebug() << "activate " << resourceSet;
    if (errorCountPtr)
        *errorCountPtr = 0;
    if (errorMessages)
        errorMessages->clear();

    QBuffer errorStream;
    errorStream.open(QIODevice::WriteOnly);

    int errorCount = 0;
    int generatedCount = 0;
    bool newResourceSetChanged = false;

    if (resourceSet && resourceSet->activeQrcPaths() != newPaths && !m_newlyCreated.contains(resourceSet))
        newResourceSetChanged = true;

    PathDataMap newPathToData = m_pathToData;

    QStringListIterator itPath(newPaths);
    while (itPath.hasNext()) {
        const QString path = itPath.next();
        if (resourceSet && !m_pathToResourceSet[path].contains(resourceSet))
            m_pathToResourceSet[path].append(resourceSet);
        const QMap<QString, bool>::iterator itMod = m_pathToModified.find(path);
        if (itMod == m_pathToModified.end() || itMod.value()) { // new path or path is already created, but needs to be recreated
            QStringList contents;
            int qrcErrorCount;
            generatedCount++;
            if (const QByteArray *data = createResource(path, &contents, &qrcErrorCount, errorStream)) {
                newPathToData.insert(path, data);
                if (qrcErrorCount) // Count single failed files as sort of 1/2 error
                    errorCount++;
            } else {
                newPathToData.remove(path);
                errorCount++;
            }
            m_pathToModified.insert(path, false);
            m_pathToContents.insert(path, contents);
            newResourceSetChanged = true;
            const QMap<QString, QList<QtResourceSet *> >::iterator itReload = m_pathToResourceSet.find(path);
            if (itReload != m_pathToResourceSet.end()) {
                QList<QtResourceSet *> resources = itReload.value();
                QListIterator<QtResourceSet *> itRes(resources);
                while (itRes.hasNext()) {
                    QtResourceSet *res = itRes.next();
                    if (res != resourceSet) {
                        m_resourceSetToReload[res] = true;
                    }
                }
            }
        } else { // path is already created, don't need to recreate
        }
    }

    QList<const QByteArray *> oldData = m_pathToData.values();
    QList<const QByteArray *> newData = newPathToData.values();

    QList<const QByteArray *> toDelete;
    QListIterator<const QByteArray *> itOld(oldData);
    if (itOld.hasNext()) {
        const QByteArray *array = itOld.next();
        if (!newData.contains(array))
            toDelete.append(array);
    }

    // Nothing can fail below here?
    if (generatedCount) {
        if (errorCountPtr)
            *errorCountPtr = errorCount;
        errorStream.close();
        const QString stderrOutput = QString::fromUtf8(errorStream.data());
        if (debugResourceModel)
            qDebug() << "Output: (" << errorCount << ")\n" << stderrOutput;
        if (errorMessages)
            *errorMessages = stderrOutput;
    }
    // register
    const QMap<QtResourceSet *, bool>::iterator itReload = m_resourceSetToReload.find(resourceSet);
    if (itReload != m_resourceSetToReload.end()) {
        if (itReload.value()) {
            newResourceSetChanged = true;
            m_resourceSetToReload.insert(resourceSet, false);
        }
    }

    QStringList oldActivePaths;
    if (m_currentResourceSet)
        oldActivePaths = m_currentResourceSet->activeQrcPaths();

    const bool needReregister = (oldActivePaths != newPaths) || newResourceSetChanged;

    QMap<QtResourceSet *, bool>::iterator itNew = m_newlyCreated.find(resourceSet);
    if (itNew != m_newlyCreated.end()) {
        m_newlyCreated.remove(resourceSet);
        if (needReregister)
            newResourceSetChanged = true;
    }

    if (!newResourceSetChanged && !needReregister && (m_currentResourceSet == resourceSet)) {
        foreach (const QByteArray *data, toDelete)
            deleteResource(data);

        return; // nothing changed
    }

    if (needReregister)
        unregisterResourceSet(m_currentResourceSet);

    foreach (const QByteArray *data, toDelete)
        deleteResource(data);

    m_pathToData = newPathToData;
    m_currentResourceSet = resourceSet;

    if (resourceSet)
        removeOldPaths(resourceSet, newPaths);

    if (needReregister)
        registerResourceSet(m_currentResourceSet);

    emit q_ptr->resourceSetActivated(m_currentResourceSet, newResourceSetChanged);

    // deactivates the paths from old current resource set
    // add new paths to the new current resource set
    // reloads all paths which are marked as modified from the current resource set;
    // activates the paths from current resource set
    // emits resourceSetActivated() (don't emit only in case when old resource set is the same as new one
    // AND no path was reloaded AND the list of paths is exactly the same)
}

void QtResourceModelPrivate::removeOldPaths(QtResourceSet *resourceSet, const QStringList &newPaths)
{
    QStringList oldPaths = m_resourceSetToPaths.value(resourceSet);
    if (oldPaths != newPaths) {
        // remove old
        QStringListIterator itOldPaths(oldPaths);
        while (itOldPaths.hasNext()) {
            QString oldPath = itOldPaths.next();
            if (!newPaths.contains(oldPath)) {
                const QMap<QString, QList<QtResourceSet *> >::iterator itRemove = m_pathToResourceSet.find(oldPath);
                if (itRemove != m_pathToResourceSet.end()) {
                    const int idx = itRemove.value().indexOf(resourceSet);
                    if (idx >= 0)
                        itRemove.value().removeAt(idx);
                    if (itRemove.value().count() == 0) {
                        PathDataMap::iterator it = m_pathToData.find(oldPath);
                        if (it != m_pathToData.end())
                            deleteResource(it.value());
                        m_pathToResourceSet.erase(itRemove);
                        m_pathToModified.remove(oldPath);
                        m_pathToContents.remove(oldPath);
                        m_pathToData.remove(oldPath);
                    }
                }
            }
        }
        m_resourceSetToPaths[resourceSet] = newPaths;
    }
}

// ----------------------- QtResourceModel
QtResourceModel::QtResourceModel(QObject *parent) :
    QObject(parent),
    d_ptr(new QtResourceModelPrivate)
{
    d_ptr->q_ptr = this;
}

QtResourceModel::~QtResourceModel()
{
    blockSignals(true);
    QList<QtResourceSet *> resourceList = resourceSets();
    QListIterator<QtResourceSet *> it(resourceList);
    while (it.hasNext())
        removeResourceSet(it.next());
    blockSignals(false);
}

QStringList QtResourceModel::loadedQrcFiles() const
{
    return d_ptr->m_pathToModified.keys();
}

bool QtResourceModel::isModified(const QString &path) const
{
    QMap<QString, bool>::const_iterator it = d_ptr->m_pathToModified.find(path);
    if (it != d_ptr->m_pathToModified.constEnd())
        return it.value();
    return true;
}

void QtResourceModel::setModified(const QString &path)
{
    QMap<QString, bool>::const_iterator itMod = d_ptr->m_pathToModified.find(path);
    if (itMod == d_ptr->m_pathToModified.constEnd())
        return;

    d_ptr->m_pathToModified[path] = true;
    QMap<QString, QList<QtResourceSet *> >::const_iterator it = d_ptr->m_pathToResourceSet.constFind(path);
    if (it == d_ptr->m_pathToResourceSet.constEnd())
        return;

    QList<QtResourceSet *> resourceList = it.value();
    QListIterator<QtResourceSet *> itReload(resourceList);
    while (itReload.hasNext())
        d_ptr->m_resourceSetToReload.insert(itReload.next(), true);
}

QList<QtResourceSet *> QtResourceModel::resourceSets() const
{
    return d_ptr->m_resourceSetToPaths.keys();
}

QtResourceSet *QtResourceModel::currentResourceSet() const
{
    return d_ptr->m_currentResourceSet;
}

void QtResourceModel::setCurrentResourceSet(QtResourceSet *resourceSet, int *errorCount, QString *errorMessages)
{
    d_ptr->activate(resourceSet, d_ptr->m_resourceSetToPaths.value(resourceSet), errorCount, errorMessages);
}

QtResourceSet *QtResourceModel::addResourceSet(const QStringList &paths)
{
    QtResourceSet *newResource = new QtResourceSet(this);
    d_ptr->m_resourceSetToPaths.insert(newResource, paths);
    d_ptr->m_resourceSetToReload.insert(newResource, false);
    d_ptr->m_newlyCreated.insert(newResource, true);
    QStringListIterator it(paths);
    while (it.hasNext()) {
        const QString path = it.next();
        d_ptr->m_pathToResourceSet[path].append(newResource);
    }
    return newResource;
}

// TODO
void QtResourceModel::removeResourceSet(QtResourceSet *resourceSet)
{
    if (!resourceSet)
        return;
    if (currentResourceSet() == resourceSet)
        setCurrentResourceSet(0);

    // remove rcc files for those paths which are not used in any other resource set
    d_ptr->removeOldPaths(resourceSet, QStringList());

    d_ptr->m_resourceSetToPaths.remove(resourceSet);
    d_ptr->m_resourceSetToReload.remove(resourceSet);
    d_ptr->m_newlyCreated.remove(resourceSet);
}

void QtResourceModel::reload(int *errorCount, QString *errorMessages)
{
    QMap<QString, bool>::iterator it = d_ptr->m_pathToModified.begin();
    QMap<QString, bool>::iterator itEnd = d_ptr->m_pathToModified.end(); // will it be valid when I iterate the map and change it???
    while (it != itEnd) {
        it = d_ptr->m_pathToModified.insert(it.key(), true);
        ++it;
    }

    QMap<QtResourceSet *, bool>::iterator itReload = d_ptr->m_resourceSetToReload.begin();
    QMap<QtResourceSet *, bool>::iterator itReloadEnd = d_ptr->m_resourceSetToReload.end();
    while (itReload != itReloadEnd) {
        itReload = d_ptr->m_resourceSetToReload.insert(itReload.key(), true); // empty resourceSets could be omitted here
        ++itReload;
    }

    d_ptr->activate(d_ptr->m_currentResourceSet, d_ptr->m_resourceSetToPaths.value(d_ptr->m_currentResourceSet), errorCount, errorMessages);
}

QMap<QString, QString> QtResourceModel::contents() const
{
    return d_ptr->m_fileToQrc;
}

QString QtResourceModel::qrcPath(const QString &file) const
{
    return d_ptr->m_fileToQrc.value(file);
}

QT_END_NAMESPACE
