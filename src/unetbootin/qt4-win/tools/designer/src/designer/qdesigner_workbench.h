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

#ifndef QDESIGNER_WORKBENCH_H
#define QDESIGNER_WORKBENCH_H

#include "designer_enums.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QList>
#include <QtCore/QRect>

QT_BEGIN_NAMESPACE

class QDesignerActions;
class QDesignerToolWindow;
class QDesignerFormWindow;
struct Preferences;

class QAction;
class QActionGroup;
class QDockWidget;
class QMenu;
class QMenuBar;
class QMainWindow;
class QToolBar;
class QMdiArea;
class QMdiSubWindow;
class QCloseEvent;
class QFont;
class QtToolBarManager;

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QDesignerFormWindowManagerInterface;

namespace qdesigner_internal {
class QDesignerIntegration;
}

class QDesignerWorkbench: public QObject
{
    Q_OBJECT

public:
    QDesignerWorkbench();
    virtual ~QDesignerWorkbench();

    UIMode mode() const;

    QDesignerFormEditorInterface *core() const;

    QDesignerToolWindow *findToolWindow(QWidget *widget) const;
    QDesignerFormWindow *findFormWindow(QWidget *widget) const;

    QDesignerFormWindow *openForm(const QString &fileName, QString *errorMessage);
    QDesignerFormWindow *openTemplate(const QString &templateFileName,
                                      const QString &editorFileName,
                                      QString *errorMessage);

    int toolWindowCount() const;
    QDesignerToolWindow *toolWindow(int index) const;

    int formWindowCount() const;
    QDesignerFormWindow *formWindow(int index) const;

    QDesignerActions *actionManager() const;

    QActionGroup *modeActionGroup() const;

    QRect availableGeometry() const;
    int marginHint() const;

    void saveSettings() const;
    void applyPreferences(const Preferences&);

    bool readInForm(const QString &fileName) const;
    bool writeOutForm(QDesignerFormWindowInterface *formWindow, const QString &fileName) const;
    bool saveForm(QDesignerFormWindowInterface *fw);
    bool handleClose();
    void closeAllToolWindows();
    bool readInBackup();
    void updateBackup(QDesignerFormWindowInterface* fwi);

signals:
    void modeChanged(UIMode mode);
    void initialized();

public slots:
    void addToolWindow(QDesignerToolWindow *toolWindow);
    void addFormWindow(QDesignerFormWindow *formWindow);
    void removeToolWindow(QDesignerToolWindow *toolWindow);
    void removeFormWindow(QDesignerFormWindow *formWindow);
    void setUIMode(UIMode mode);
    void bringAllToFront();
    void toggleFormMinimizationState();
    void configureToolBars();

// ### private slots:
    void switchToNeutralMode();
    void switchToDockedMode();
    void switchToTopLevelMode();

    void initializeCorePlugins();

private slots:
    void initialize();
    void activateMdiAreaChildWindow(QMdiSubWindow*);
    void updateWindowMenu(QDesignerFormWindowInterface *fw);
    void formWindowActionTriggered(QAction *a);
    void showToolBars();
    void adjustMDIFormPositions();
    void minimizationStateChanged(QDesignerFormWindowInterface *formWindow, bool minimized);

private:
    QWidget *magicalParent() const;
    Qt::WindowFlags magicalWindowFlags(const QWidget *widgetForFlags) const;
    QDockWidget *magicalDockWidget(QWidget *widget) const;

    QDesignerFormWindowManagerInterface *formWindowManager() const;

    bool eventFilter(QObject *object, QEvent *event);

private:
    QDesignerFormWindow *loadForm(const QString &fileName, bool *uic3Converted, QString *errorMessage);
    void resizeForm(QDesignerFormWindow *fw,  const QWidget *mainContainer) const;
    void saveGeometries();
    bool isFormWindowMinimized(const QDesignerFormWindow *fw);
    void setFormWindowMinimized(QDesignerFormWindow *fw, bool minimized);
    void setDesignerUIFont(const QFont &);

    void createToolBarManager(QMainWindow *mw);
    void removeToolBarManager();
    void updateToolBarMenu();

    QDesignerFormEditorInterface *m_core;
    qdesigner_internal::QDesignerIntegration *m_integration;

    QDesignerActions *m_actionManager;
    QActionGroup *m_toolActions;
    QActionGroup *m_windowActions;

    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_formMenu;
    QMenu *m_toolMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;

    QMenuBar *m_globalMenuBar;
    QToolBar *m_toolToolBar;
    QToolBar *m_formToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_fileToolBar;

    QtToolBarManager *m_toolBarManager;

    UIMode m_mode;

    QList<QDesignerToolWindow*> m_toolWindows;
    QList<QDesignerFormWindow*> m_formWindows;

    QMdiArea *m_mdiArea;
    QMenu *m_toolbarMenu;
    QAction *m_configureToolBars;

    // Helper class to remember the position of a window while switching user interface modes.
    class Position {
    public:
        Position(const QDockWidget *dockWidget);
        Position(const QMdiSubWindow *mdiSubWindow, const QPoint &mdiAreaOffset);
        Position(const QWidget *topLevelWindow, const QPoint &desktopTopLeft);

        void applyTo(QMdiSubWindow *mdiSubWindow, const QPoint &mdiAreaOffset) const;
        void applyTo(QWidget *topLevelWindow, const QPoint &desktopTopLeft) const;
        void applyTo(QDockWidget *dockWidget) const;

        QPoint position() const { return m_position; }
    private:
        bool m_minimized;
        // Position referring to top-left corner (desktop in top-level mode or main window in MDI mode)
        QPoint m_position;
    };
    typedef  QHash<QWidget*, Position> PositionMap;
    PositionMap m_Positions;

    QSet<QDesignerToolWindow*> m_toolWindowExtras;
    QSet<QDesignerFormWindow*> m_formWindowExtras;

    enum State { StateInitializing, StateUp, StateClosing };
    State m_state;
};

QT_END_NAMESPACE

#endif // QDESIGNER_WORKBENCH_H
