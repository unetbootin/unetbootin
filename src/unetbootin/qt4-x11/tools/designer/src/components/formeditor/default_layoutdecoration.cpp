/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "default_layoutdecoration.h"
#include "qlayout_widget_p.h"

#include <layoutinfo_p.h>

#include <QtDesigner/QDesignerMetaDataBaseItemInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// ---- QDesignerLayoutDecorationFactory ----
QDesignerLayoutDecorationFactory::QDesignerLayoutDecorationFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}

QObject *QDesignerLayoutDecorationFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (!object->isWidgetType() || iid != Q_TYPEID(QDesignerLayoutDecorationExtension))
        return 0;

    QWidget *widget = qobject_cast<QWidget*>(object);

    if (const QLayoutWidget *layoutWidget = qobject_cast<const QLayoutWidget*>(widget))
        return QLayoutSupport::createLayoutSupport(layoutWidget->formWindow(), widget, parent);

    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(widget))
        if (LayoutInfo::managedLayout(fw->core(), widget))
            return QLayoutSupport::createLayoutSupport(fw, widget, parent);

    return 0;
}
}

QT_END_NAMESPACE
