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

#ifndef QDESIGNER_ACTIONS_H
#define QDESIGNER_ACTIONS_H

#include "assistantclient.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QPrinter>

QT_BEGIN_NAMESPACE

class QDesignerWorkbench;

class QDir;
class QTimer;
class QAction;
class QActionGroup;
class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QRect;
class QWidget;
class QPixmap;

namespace qdesigner_internal {
    class PreviewManager;
}

class QDesignerActions: public QObject
{
    Q_OBJECT
public:
    explicit QDesignerActions(QDesignerWorkbench *mainWindow);
    virtual ~QDesignerActions();

    QDesignerWorkbench *workbench() const;
    QDesignerFormEditorInterface *core() const;

    bool saveForm(QDesignerFormWindowInterface *fw);
    bool readInForm(const QString &fileName);
    bool writeOutForm(QDesignerFormWindowInterface *formWindow, const QString &fileName);

    QActionGroup *fileActions() const;
    QActionGroup *recentFilesActions() const;
    QActionGroup *editActions() const;
    QActionGroup *formActions() const;
    QActionGroup *windowActions() const;
    QActionGroup *toolActions() const;
    QActionGroup *helpActions() const;
    QActionGroup *uiMode() const;
    QAction *preferencesAction() const;
    QActionGroup *styleActions() const;
    // file actions
    QAction *openFormAction() const;
    QAction *closeFormAction() const;
    // window actions
    QAction *minimizeAction() const;
    // edit mode actions
    QAction *editWidgets() const;
    // form actions
    QAction *previewFormAction() const;
    QAction *viewCodeAction() const;

    void setBringAllToFrontVisible(bool visible);
    void setWindowListSeparatorVisible(bool visible);

    bool openForm(QWidget *parent);

public slots:
    void activeFormWindowChanged(QDesignerFormWindowInterface *formWindow);
    void createForm();
    void slotOpenForm();
    void helpRequested(const QString &manual, const QString &document);

signals:
    void useBigIcons(bool);

private slots:
    void saveForm();
    void saveFormAs();
    void saveAllForms();
    void saveFormAsTemplate();
    void previewForm(QAction *action = 0);
    void previewFormLater(QAction *action = 0);
    void viewCode();
    void notImplementedYet();
    void shutdown();
    void editWidgetsSlot();
    void openRecentForm();
    void clearRecentFiles();
    void closeForm();
    void showDesignerHelp();
    void showWhatsNew();
    void aboutPlugins();
    void aboutDesigner();
    void showWidgetSpecificHelp();
    void showFormSettings();
    void backupForms();
    void showNewFormDialog(const QString &fileName);
    void showPreferencesDialog();
    void savePreviewImage();
    void printPreviewImage();
    void updateCloseAction();

private:
    bool saveFormAs(QDesignerFormWindowInterface *fw);
    void fixActionContext();
    void updateRecentFileActions();
    void addRecentFile(const QString &fileName);
    void showHelp(const QString &help);
    void closePreview();
    QRect fixDialogRect(const QRect &rect) const;
    QString fixResourceFileBackupPath(QDesignerFormWindowInterface *fwi, const QDir& backupDir);
    void showStatusBarMessage(const QString &message) const;
    QActionGroup *createHelpActions();
    bool ensureBackupDirectories();
    QPixmap createPreviewPixmap(QDesignerFormWindowInterface *fw);

    enum { MaxRecentFiles = 10 };
    QDesignerWorkbench *m_workbench;
    QDesignerFormEditorInterface *m_core;
    AssistantClient m_assistantClient;
    QString m_openDirectory;
    QString m_saveDirectory;


    QString m_backupPath;
    QString m_backupTmpPath;

    QTimer* m_backupTimer;

    QActionGroup *m_fileActions;
    QActionGroup *m_recentFilesActions;
    QActionGroup *m_editActions;
    QActionGroup *m_formActions;
    QActionGroup *m_windowActions;
    QActionGroup *m_toolActions;
    QActionGroup *m_helpActions;
    QActionGroup *m_styleActions;

    QAction *m_editWidgetsAction;

    QAction *m_newFormAction;
    QAction *m_openFormAction;
    QAction *m_saveFormAction;
    QAction *m_saveFormAsAction;
    QAction *m_saveAllFormsAction;
    QAction *m_saveFormAsTemplateAction;
    QAction *m_closeFormAction;
    QAction *m_savePreviewImageAction;
    QAction *m_printPreviewAction;

    QAction *m_quitAction;

    QAction *m_previewFormAction;
    QAction *m_viewCodeAction;

    QAction *m_formSettings;

    QAction *m_minimizeAction;
    QAction *m_bringAllToFrontSeparator;
    QAction *m_bringAllToFrontAction;
    QAction *m_windowListSeparatorAction;

    QAction *m_preferencesAction;

#ifndef QT_NO_PRINTER
    QPrinter m_printer;
#endif
    qdesigner_internal::PreviewManager *m_previewManager;
};

QT_END_NAMESPACE

#endif // QDESIGNER_ACTIONS_H
