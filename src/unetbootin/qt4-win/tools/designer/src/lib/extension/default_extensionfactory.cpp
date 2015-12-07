/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include <QtDesigner/default_extensionfactory.h>
#include "qextensionmanager.h"
#include <qpointer.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QExtensionFactory

    \brief The QExtensionFactory class allows you to create a factory
    that is able to make instances of custom extensions in Qt
    Designer.

    \inmodule QtDesigner

    In \QD the extensions are not created until they are required. For
    that reason, when implementing a custom extension, you must also
    create a QExtensionFactory, i.e. a class that is able to make an
    instance of your extension, and register it using \QD's \l
    {QExtensionManager}{extension manager}.

    The QExtensionManager class provides extension management
    facilities for Qt Designer. When an extension is required, Qt
    Designer's \l {QExtensionManager}{extension manager} will run
    through all its registered factories calling
    QExtensionFactory::createExtension() for each until the first one
    that is able to create a requested extension for the selected
    object, is found. This factory will then make an instance of the
    extension.

    There are four available types of extensions in Qt Designer:
    QDesignerContainerExtension , QDesignerMemberSheetExtension,
    QDesignerPropertySheetExtension and QDesignerTaskMenuExtension. Qt
    Designer's behavior is the same whether the requested extension is
    associated with a multi page container, a member sheet, a property
    sheet or a task menu.

    You can either create a new QExtensionFactory and reimplement the
    QExtensionFactory::createExtension() function. For example:

    \snippet doc/src/snippets/code/tools.designer.src.lib.extension.default_extensionfactory.cpp 0

    Or you can use an existing factory, expanding the
    QExtensionFactory::createExtension() function to make the factory
    able to create your extension as well. For example:

    \snippet doc/src/snippets/code/tools.designer.src.lib.extension.default_extensionfactory.cpp 1

    For a complete example using the QExtensionFactory class, see the
    \l {designer/taskmenuextension}{Task Menu Extension example}. The
    example shows how to create a custom widget plugin for Qt
    Designer, and how to to use the QDesignerTaskMenuExtension class
    to add custom items to Qt Designer's task menu.

    \sa QExtensionManager, QAbstractExtensionFactory
*/

/*!
    Constructs an extension factory with the given \a parent.
*/
QExtensionFactory::QExtensionFactory(QExtensionManager *parent)
    : QObject(parent)
{
}

/*!
    Returns the extension specified by \a iid for the given \a object.

    \sa createExtension()
*/

QObject *QExtensionFactory::extension(QObject *object, const QString &iid) const
{
    if (!object)
        return 0;
    const IdObjectKey key = qMakePair(iid, object);

    ExtensionMap::iterator it = m_extensions.find(key);
    if (it == m_extensions.end()) {
        if (QObject *ext = createExtension(object, iid, const_cast<QExtensionFactory*>(this))) {
            connect(ext, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
            it = m_extensions.insert(key, ext);
        }
    }

    if (!m_extended.contains(object)) {
        connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
        m_extended.insert(object, true);
    }

    if (it == m_extensions.end())
        return 0;

    return it.value();
}

void QExtensionFactory::objectDestroyed(QObject *object)
{
    QMutableMapIterator< IdObjectKey, QObject*> it(m_extensions);
    while (it.hasNext()) {
        it.next();

        QObject *o = it.key().second;
        if (o == object || object == it.value()) {
            it.remove();
        }
    }

    m_extended.remove(object);
}

/*!
    Creates an extension specified by \a iid for the given \a object.
    The extension object is created as a child of the specified \a
    parent.

    \sa extension()
*/
QObject *QExtensionFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    Q_UNUSED(object);
    Q_UNUSED(iid);
    Q_UNUSED(parent);

    return 0;
}

/*!
    Returns the extension manager for the extension factory.
*/
QExtensionManager *QExtensionFactory::extensionManager() const
{
    return static_cast<QExtensionManager *>(parent());
}

QT_END_NAMESPACE
