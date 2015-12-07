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

#ifndef QMDIAREA_CONTAINER_H
#define QMDIAREA_CONTAINER_H

#include <QtDesigner/QDesignerContainerExtension>


#include <qdesigner_propertysheet_p.h>
#include <extensionfactory_p.h>

#include <QtGui/QMdiArea>
#include <QtGui/QWorkspace>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// Container for QMdiArea
class QMdiAreaContainer: public QObject, public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)
public:
    explicit QMdiAreaContainer(QMdiArea *widget, QObject *parent = 0);

    virtual int count() const;
    virtual QWidget *widget(int index) const;
    virtual int currentIndex() const;
    virtual void setCurrentIndex(int index);
    virtual void addWidget(QWidget *widget);
    virtual void insertWidget(int index, QWidget *widget);
    virtual void remove(int index);

    // Semismart positioning of a new MDI child after cascading
    static void positionNewMdiChild(const QWidget *area, QWidget *mdiChild);

private:
    QMdiArea *m_mdiArea;
};

// PropertySheet for QMdiArea: Fakes window title and name.
// Also works for a QWorkspace as it relies on the container extension.

class QMdiAreaPropertySheet: public QDesignerPropertySheet
{
    Q_OBJECT
    Q_INTERFACES(QDesignerPropertySheetExtension)
public:
    explicit QMdiAreaPropertySheet(QWidget *mdiArea, QObject *parent = 0);

    virtual void setProperty(int index, const QVariant &value);
    virtual bool reset(int index);
    virtual bool isEnabled(int index) const;
    virtual bool isChanged(int index) const;
    virtual QVariant property(int index) const;

    // Check whether the property is to be saved. Returns false for the page
    // properties (as the property sheet has no concept of 'stored')
    static bool checkProperty(const QString &propertyName);

private:
    const QString m_windowTitleProperty;
    QWidget *currentWindow() const;
    QDesignerPropertySheetExtension *currentWindowSheet() const;

    enum MdiAreaProperty { MdiAreaSubWindowName, MdiAreaSubWindowTitle, MdiAreaNone };
    static MdiAreaProperty mdiAreaProperty(const QString &name);
};

// Factories

typedef ExtensionFactory<QDesignerContainerExtension,  QMdiArea,  QMdiAreaContainer> QMdiAreaContainerFactory;
typedef QDesignerPropertySheetFactory<QMdiArea, QMdiAreaPropertySheet> QMdiAreaPropertySheetFactory;
typedef QDesignerPropertySheetFactory<QWorkspace, QMdiAreaPropertySheet> QWorkspacePropertySheetFactory;
}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QMDIAREA_CONTAINER_H
