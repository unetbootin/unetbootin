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


#ifndef DESIGNERPROPERTYEDITOR_H
#define DESIGNERPROPERTYEDITOR_H

#include "shared_global_p.h"
#include <QtDesigner/QDesignerPropertyEditorInterface>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// Extends the QDesignerPropertyEditorInterface by property comment handling and
// a signal for resetproperty.

class QDESIGNER_SHARED_EXPORT QDesignerPropertyEditor: public QDesignerPropertyEditorInterface
{
    Q_OBJECT
public:
    QDesignerPropertyEditor(QWidget *parent = 0, Qt::WindowFlags flags = 0);

Q_SIGNALS:
    void propertyValueChanged(const QString &name, const QVariant &value, bool enableSubPropertyHandling);
    void propertyCommentChanged(const QString &name, const QString &value);
    void resetProperty(const QString &name);
    void addDynamicProperty(const QString &name, const QVariant &value);
    void removeDynamicProperty(const QString &name);
    void editorOpened();
    void editorClosed();

public Q_SLOTS:
    virtual void setPropertyComment(const QString &name, const QString &value) = 0;
    /* Quick update that assumes the actual count of properties has not changed
     * (as opposed to setObject()). N/A when for example executing a
     * layout command and margin properties appear. */
    virtual void updatePropertySheet() = 0;
    virtual void reloadResourceProperties() = 0;

private Q_SLOTS:
    void slotPropertyChanged(const QString &name, const QVariant &value);
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // DESIGNERPROPERTYEDITOR_H
