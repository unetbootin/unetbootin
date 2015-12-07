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

#include <QtDesigner/abstractformeditorplugin.h>

QT_BEGIN_NAMESPACE

/*!
    \internal
    \class QDesignerFormEditorPluginInterface
    \brief The QDesignerFormEditorPluginInterface class provides an interface that is used to
    manage plugins for Qt Designer's form editor component.
    \inmodule QtDesigner

    \sa QDesignerFormEditorInterface
*/

/*!
    \fn virtual QDesignerFormEditorPluginInterface::~QDesignerFormEditorPluginInterface()

    Destroys the plugin interface.
*/

/*!
    \fn virtual bool QDesignerFormEditorPluginInterface::isInitialized() const = 0

    Returns true if the plugin interface is initialized; otherwise returns false.
*/

/*!
    \fn virtual void QDesignerFormEditorPluginInterface::initialize(QDesignerFormEditorInterface *core) = 0

    Initializes the plugin interface for the specified \a core interface.
*/

/*!
    \fn virtual QAction *QDesignerFormEditorPluginInterface::action() const = 0

    Returns the action associated with this interface.
*/

/*!
    \fn virtual QDesignerFormEditorInterface *QDesignerFormEditorPluginInterface::core() const = 0

    Returns the core form editor interface associated with this component.
*/

QT_END_NAMESPACE
