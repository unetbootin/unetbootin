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

#ifndef PROPERTYSHEET_H
#define PROPERTYSHEET_H

#include <QtDesigner/extension.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QVariant;

class QDesignerPropertySheetExtension
{
public:
    virtual ~QDesignerPropertySheetExtension() {}

    virtual int count() const = 0;

    virtual int indexOf(const QString &name) const = 0;

    virtual QString propertyName(int index) const = 0;
    virtual QString propertyGroup(int index) const = 0;
    virtual void setPropertyGroup(int index, const QString &group) = 0;

    virtual bool hasReset(int index) const = 0;
    virtual bool reset(int index) = 0;

    virtual bool isVisible(int index) const = 0;
    virtual void setVisible(int index, bool b) = 0;

    virtual bool isAttribute(int index) const = 0;
    virtual void setAttribute(int index, bool b) = 0;

    virtual QVariant property(int index) const = 0;
    virtual void setProperty(int index, const QVariant &value) = 0;

    virtual bool isChanged(int index) const = 0;
    virtual void setChanged(int index, bool changed) = 0;

};

Q_DECLARE_EXTENSION_INTERFACE(QDesignerPropertySheetExtension,
    "com.trolltech.Qt.Designer.PropertySheet")

QT_END_NAMESPACE

QT_END_HEADER

#endif // PROPERTYSHEET_H
