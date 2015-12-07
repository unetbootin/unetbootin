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

#ifndef ABSTRACTPROPERTYEDITOR_H
#define ABSTRACTPROPERTYEDITOR_H

#include <QtDesigner/sdk_global.h>

#include <QtGui/QWidget>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QString;
class QVariant;

class QDESIGNER_SDK_EXPORT QDesignerPropertyEditorInterface: public QWidget
{
    Q_OBJECT
public:
    QDesignerPropertyEditorInterface(QWidget *parent, Qt::WindowFlags flags = 0);
    virtual ~QDesignerPropertyEditorInterface();

    virtual QDesignerFormEditorInterface *core() const;

    virtual bool isReadOnly() const = 0;
    virtual QObject *object() const = 0;

    virtual QString currentPropertyName() const = 0;

Q_SIGNALS:
    void propertyChanged(const QString &name, const QVariant &value);

public Q_SLOTS:
    virtual void setObject(QObject *object) = 0;
    virtual void setPropertyValue(const QString &name, const QVariant &value, bool changed = true) = 0;
    virtual void setReadOnly(bool readOnly) = 0;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTPROPERTYEDITOR_H
