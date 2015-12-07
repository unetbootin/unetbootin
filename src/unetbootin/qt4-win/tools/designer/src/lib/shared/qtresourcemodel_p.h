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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QTRESOURCEMODEL_H
#define QTRESOURCEMODEL_H

#include "shared_global_p.h"
#include <QtCore/QMap>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QtResourceModel;

class QDESIGNER_SHARED_EXPORT QtResourceSet // one instance per one form
{
public:
    QStringList activeQrcPaths() const;

    // activateQrcPaths(): if this QtResourceSet is active it emits resourceSetActivated();
    // otherwise only in case if active QtResource set contains one of
    // paths which was marked as modified by this resource set, the signal
    // is emitted (with reload = true);
    // If new path appears on the list it is automatically added to
    // loaded list of QtResourceModel. In addition it is marked as modified in case
    // QtResourceModel didn't contain the path.
    // If some path is removed from that list (and is not used in any other
    // resource set) it is automatically unloaded. The removed file can also be
    // marked as modified (later when another resource set which contains
    // removed path is activated will be reloaded)
    void activateQrcPaths(const QStringList &paths, int *errorCount = 0, QString *errorMessages = 0);

    bool isModified(const QString &path) const; // for all paths in resource model (redundant here, maybe it should be removed from here)
    void setModified(const QString &path);      // for all paths in resource model (redundant here, maybe it should be removed from here)
private:
    QtResourceSet();
    QtResourceSet(QtResourceModel *model);
    ~QtResourceSet();
    friend class QtResourceModel;

    class QtResourceSetPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtResourceSet)
    Q_DISABLE_COPY(QtResourceSet)
};

class QDESIGNER_SHARED_EXPORT QtResourceModel : public QObject // one instance per whole designer
{
    Q_OBJECT
public:
    QtResourceModel(QObject *parent = 0);
    ~QtResourceModel();

    QStringList loadedQrcFiles() const;
    bool isModified(const QString &path) const; // only for paths which are on loadedQrcFiles() list
    void setModified(const QString &path);      // only for paths which are on loadedQrcPaths() list

    QList<QtResourceSet *> resourceSets() const;

    QtResourceSet *currentResourceSet() const;
    void setCurrentResourceSet(QtResourceSet *resourceSet, int *errorCount = 0, QString *errorMessages = 0);

    QtResourceSet *addResourceSet(const QStringList &paths);
    void removeResourceSet(QtResourceSet *resourceSet);

    void reload(int *errorCount = 0, QString *errorMessages = 0);

    // Contents of the current resource set (content file to qrc path)
    QMap<QString, QString> contents() const;
    // Find the qrc file belonging to the contained file (from current resource set)
    QString qrcPath(const QString &file) const;

signals:
    void resourceSetActivated(QtResourceSet *resourceSet, bool resourceSetChanged); // resourceSetChanged since last time it was activated!

private:
    friend class QtResourceSet;

    class QtResourceModelPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtResourceModel)
    Q_DISABLE_COPY(QtResourceModel)
};

QT_END_NAMESPACE

#endif
