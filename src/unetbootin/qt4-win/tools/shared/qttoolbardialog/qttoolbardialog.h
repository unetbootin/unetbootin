/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QTTOOLBARDIALOG_H
#define QTTOOLBARDIALOG_H

#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QMainWindow;
class QAction;
class QToolBar;

class QtToolBarManagerPrivate;

class QtToolBarManager : public QObject
{
    Q_OBJECT
public:

    QtToolBarManager(QObject *parent = 0);
    ~QtToolBarManager();

    void setMainWindow(QMainWindow *mainWindow);
    QMainWindow *mainWindow() const;

    void addAction(QAction *action, const QString &category);
    void removeAction(QAction *action);

    void addToolBar(QToolBar *toolBar, const QString &category);
    void removeToolBar(QToolBar *toolBar);

    QList<QToolBar *> toolBars() const;

    QByteArray saveState(int version = 0) const;
    bool restoreState(const QByteArray &state, int version = 0);

private:

    friend class QtToolBarDialog;
    QtToolBarManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtToolBarManager)
    Q_DISABLE_COPY(QtToolBarManager)
};

class QtToolBarDialogPrivate;

class QtToolBarDialog : public QDialog
{
    Q_OBJECT
public:

    QtToolBarDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~QtToolBarDialog();

    void setToolBarManager(QtToolBarManager *toolBarManager);

protected:

    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:

    QtToolBarDialogPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtToolBarDialog)
    Q_DISABLE_COPY(QtToolBarDialog)

    Q_PRIVATE_SLOT(d_func(), void newClicked())
    Q_PRIVATE_SLOT(d_func(), void removeClicked())
    Q_PRIVATE_SLOT(d_func(), void defaultClicked())
    Q_PRIVATE_SLOT(d_func(), void okClicked())
    Q_PRIVATE_SLOT(d_func(), void applyClicked())
    Q_PRIVATE_SLOT(d_func(), void cancelClicked())
    Q_PRIVATE_SLOT(d_func(), void upClicked())
    Q_PRIVATE_SLOT(d_func(), void downClicked())
    Q_PRIVATE_SLOT(d_func(), void leftClicked())
    Q_PRIVATE_SLOT(d_func(), void rightClicked())
    Q_PRIVATE_SLOT(d_func(), void renameClicked())
    Q_PRIVATE_SLOT(d_func(), void toolBarRenamed(QListWidgetItem *))
    Q_PRIVATE_SLOT(d_func(), void currentActionChanged(QTreeWidgetItem *))
    Q_PRIVATE_SLOT(d_func(), void currentToolBarChanged(QListWidgetItem *))
    Q_PRIVATE_SLOT(d_func(), void currentToolBarActionChanged(QListWidgetItem *))
};

QT_END_NAMESPACE

#endif
