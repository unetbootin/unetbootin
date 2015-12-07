/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef QTRESOURCEEDITOR_H
#define QTRESOURCEEDITOR_H

#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QtResourceModel;
class QDesignerDialogGuiInterface;

class QtResourceEditorDialog : public QDialog
{
    Q_OBJECT
public:
    QtResourceModel *model() const;
    void setResourceModel(QtResourceModel *model);

    QString selectedResource() const;

    static QString editResources(QtResourceModel *model, QDesignerDialogGuiInterface *dlgGui, QWidget *parent = 0);

    // Helper to display a message box with rcc logs in case of errors.
    static void displayResourceFailures(const QString &logOutput, QDesignerDialogGuiInterface *dlgGui, QWidget *parent = 0);

public slots:
    virtual void accept();

private:
    QtResourceEditorDialog(QDesignerDialogGuiInterface *dlgGui, QWidget *parent = 0);
    ~QtResourceEditorDialog();

    class QtResourceEditorDialogPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtResourceEditorDialog)
    Q_DISABLE_COPY(QtResourceEditorDialog)

    Q_PRIVATE_SLOT(d_func(), void slotQrcFileInserted(QtQrcFile *))
    Q_PRIVATE_SLOT(d_func(), void slotQrcFileMoved(QtQrcFile *))
    Q_PRIVATE_SLOT(d_func(), void slotQrcFileRemoved(QtQrcFile *))
    Q_PRIVATE_SLOT(d_func(), void slotResourcePrefixInserted(QtResourcePrefix *))
    Q_PRIVATE_SLOT(d_func(), void slotResourcePrefixMoved(QtResourcePrefix *))
    Q_PRIVATE_SLOT(d_func(), void slotResourcePrefixChanged(QtResourcePrefix *))
    Q_PRIVATE_SLOT(d_func(), void slotResourceLanguageChanged(QtResourcePrefix *))
    Q_PRIVATE_SLOT(d_func(), void slotResourcePrefixRemoved(QtResourcePrefix *))
    Q_PRIVATE_SLOT(d_func(), void slotResourceFileInserted(QtResourceFile *))
    Q_PRIVATE_SLOT(d_func(), void slotResourceFileMoved(QtResourceFile *))
    Q_PRIVATE_SLOT(d_func(), void slotResourceAliasChanged(QtResourceFile *))
    Q_PRIVATE_SLOT(d_func(), void slotResourceFileRemoved(QtResourceFile *))

    Q_PRIVATE_SLOT(d_func(), void slotCurrentQrcFileChanged(QListWidgetItem *))
    Q_PRIVATE_SLOT(d_func(), void slotCurrentTreeViewItemChanged(const QModelIndex &))
    Q_PRIVATE_SLOT(d_func(), void slotListWidgetContextMenuRequested(const QPoint &))
    Q_PRIVATE_SLOT(d_func(), void slotTreeViewContextMenuRequested(const QPoint &))
    Q_PRIVATE_SLOT(d_func(), void slotTreeViewItemChanged(QStandardItem *))

    Q_PRIVATE_SLOT(d_func(), void slotNewQrcFile())
    Q_PRIVATE_SLOT(d_func(), void slotImportQrcFile())
    Q_PRIVATE_SLOT(d_func(), void slotRemoveQrcFile())
    Q_PRIVATE_SLOT(d_func(), void slotMoveUpQrcFile())
    Q_PRIVATE_SLOT(d_func(), void slotMoveDownQrcFile())

    Q_PRIVATE_SLOT(d_func(), void slotNewPrefix())
    Q_PRIVATE_SLOT(d_func(), void slotAddFiles())
    Q_PRIVATE_SLOT(d_func(), void slotChangePrefix())
    Q_PRIVATE_SLOT(d_func(), void slotChangeLanguage())
    Q_PRIVATE_SLOT(d_func(), void slotChangeAlias())
    Q_PRIVATE_SLOT(d_func(), void slotClonePrefix())
    Q_PRIVATE_SLOT(d_func(), void slotRemove())
    Q_PRIVATE_SLOT(d_func(), void slotMoveUp())
    Q_PRIVATE_SLOT(d_func(), void slotMoveDown())
};

QT_END_NAMESPACE

#endif

