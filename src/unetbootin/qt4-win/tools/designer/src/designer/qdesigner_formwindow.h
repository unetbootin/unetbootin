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

#ifndef QDESIGNER_FORMWINDOW_H
#define QDESIGNER_FORMWINDOW_H

#include <QtCore/QPointer>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QDesignerWorkbench;
class QDesignerFormWindowInterface;

class QDesignerFormWindow: public QWidget
{
    Q_OBJECT
public:
    QDesignerFormWindow(QDesignerFormWindowInterface *formWindow, QDesignerWorkbench *workbench,
                        QWidget *parent = 0, Qt::WindowFlags flags = 0);

    void firstShow();

    virtual ~QDesignerFormWindow();

    QAction *action() const;
    QDesignerWorkbench *workbench() const;
    QDesignerFormWindowInterface *editor() const;

    QRect geometryHint() const;

public slots:
    void updateChanged();

private slots:
    void updateWindowTitle(const QString &fileName);
    void geometryChanged();
    void widgetActivated(QWidget *widget);

signals:
    void minimizationStateChanged(QDesignerFormWindowInterface *formWindow, bool minimized);
    void triggerAction();

protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *ev);
    virtual void resizeEvent(QResizeEvent* rev);

private:
    int getNumberOfUntitledWindows() const;
    QPointer<QDesignerFormWindowInterface> m_editor;
    QPointer<QDesignerWorkbench> m_workbench;
    QAction *m_action;
    bool m_initialized;
    bool m_windowTitleInitialized;
};

QT_END_NAMESPACE

#endif // QDESIGNER_FORMWINDOW_H
