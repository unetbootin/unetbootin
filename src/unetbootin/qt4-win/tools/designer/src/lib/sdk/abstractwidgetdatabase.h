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

#ifndef ABSTRACTWIDGETDATABASE_H
#define ABSTRACTWIDGETDATABASE_H

#include <QtDesigner/sdk_global.h>

#include <QtCore/QObject>
#include <QtCore/QList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QIcon;
class QString;
class QDesignerFormEditorInterface;
class QDebug;

class QDesignerWidgetDataBaseItemInterface
{
public:
    virtual ~QDesignerWidgetDataBaseItemInterface() {}

    virtual QString name() const = 0;
    virtual void setName(const QString &name) = 0;

    virtual QString group() const = 0;
    virtual void setGroup(const QString &group) = 0;

    virtual QString toolTip() const = 0;
    virtual void setToolTip(const QString &toolTip) = 0;

    virtual QString whatsThis() const = 0;
    virtual void setWhatsThis(const QString &whatsThis) = 0;

    virtual QString includeFile() const = 0;
    virtual void setIncludeFile(const QString &includeFile) = 0;

    virtual QIcon icon() const = 0;
    virtual void setIcon(const QIcon &icon) = 0;

    virtual bool isCompat() const = 0;
    virtual void setCompat(bool compat) = 0;

    virtual bool isContainer() const = 0;
    virtual void setContainer(bool container) = 0;

    virtual bool isCustom() const = 0;
    virtual void setCustom(bool custom) = 0;

    virtual QString pluginPath() const = 0;
    virtual void setPluginPath(const QString &path) = 0;

    virtual bool isPromoted() const = 0;
    virtual void setPromoted(bool b) = 0;

    virtual QString extends() const = 0;
    virtual void setExtends(const QString &s) = 0;

    virtual void setDefaultPropertyValues(const QList<QVariant> &list) = 0;
    virtual QList<QVariant> defaultPropertyValues() const = 0;
};

class QDESIGNER_SDK_EXPORT QDesignerWidgetDataBaseInterface: public QObject
{
    Q_OBJECT
public:
    QDesignerWidgetDataBaseInterface(QObject *parent = 0);
    virtual ~QDesignerWidgetDataBaseInterface();

    virtual int count() const;
    virtual QDesignerWidgetDataBaseItemInterface *item(int index) const;

    virtual int indexOf(QDesignerWidgetDataBaseItemInterface *item) const;
    virtual void insert(int index, QDesignerWidgetDataBaseItemInterface *item);
    virtual void append(QDesignerWidgetDataBaseItemInterface *item);

    virtual int indexOfObject(QObject *object, bool resolveName = true) const;
    virtual int indexOfClassName(const QString &className, bool resolveName = true) const;

    virtual QDesignerFormEditorInterface *core() const;

    bool isContainer(QObject *object, bool resolveName = true) const;
    bool isCustom(QObject *object, bool resolveName = true) const;

Q_SIGNALS:
    void changed();

protected:
    QList<QDesignerWidgetDataBaseItemInterface *> m_items;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTWIDGETDATABASE_H
