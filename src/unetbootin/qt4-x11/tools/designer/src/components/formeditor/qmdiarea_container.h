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
