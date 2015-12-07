/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFILEDIALOG_P_H
#define QFILEDIALOG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_FILEDIALOG

#include "qfiledialog.h"
#include "private/qdialog_p.h"
#include "qplatformdefs.h"

#include "qfilesystemmodel_p.h"
#include <qlistview.h>
#include <qtreeview.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qurl.h>
#include <qstackedwidget.h>
#include <qdialogbuttonbox.h>
#include <qabstractproxymodel.h>
#include <qcompleter.h>
#include <qtimeline.h>
#include <qdebug.h>
#include "qsidebar_p.h"

#if defined (Q_OS_UNIX)
#include <unistd.h>
#endif

QT_BEGIN_NAMESPACE

class QFileDialogListView;
class QFileDialogTreeView;
class QFileDialogLineEdit;
class QGridLayout;
class QCompleter;
class QHBoxLayout;
class Ui_QFileDialog;

struct QFileDialogArgs
{
    QFileDialogArgs() : parent(0), mode(QFileDialog::AnyFile) {}

    QWidget *parent;
    QString caption;
    QString directory;
    QString selection;
    QString filter;
    QFileDialog::FileMode mode;
    QFileDialog::Options options;
};

#define UrlRole (Qt::UserRole + 1)

class Q_AUTOTEST_EXPORT QFileDialogPrivate : public QDialogPrivate
{
    Q_DECLARE_PUBLIC(QFileDialog)

public:
    QFileDialogPrivate() :
#ifndef QT_NO_PROXYMODEL
    proxyModel(0),
#endif
    model(0),
    fileMode(QFileDialog::AnyFile),
    acceptMode(QFileDialog::AcceptOpen),
    confirmOverwrite(true),
    currentHistoryLocation(-1),
    renameAction(0),
    deleteAction(0),
    showHiddenAction(0),
    useDefaultCaption(true),
    defaultFileTypes(true),
    showNameFilterDetails(true),
    qFileDialogUi(0)
    {};

    void createToolButtons();
    void createMenuActions();
    void createWidgets();

    void init(const QString &directory = QString(), const QString &nameFilter = QString(),
              const QString &caption = QString());
    bool itemViewKeyboardEvent(QKeyEvent *event);
    QString getEnvironmentVariable(const QString &string);
    static QString workingDirectory(const QString &path);
    static QString initialSelection(const QString &path);
    QStringList typedFiles() const;
    static bool removeDirectory(const QString &path);

    inline QModelIndex mapToSource(const QModelIndex &index) const;
    inline QModelIndex mapFromSource(const QModelIndex &index) const;
    inline QModelIndex rootIndex() const;
    inline void setRootIndex(const QModelIndex &index) const;
    inline QModelIndex select(const QModelIndex &index) const;
    inline QString rootPath() const;

    QLineEdit *lineEdit() const;

    int maxNameLength(const QString &path) {
#if defined(Q_OS_UNIX)
        return ::pathconf(QFile::encodeName(path).data(), _PC_NAME_MAX);
#elif defined(Q_OS_WIN)
#ifndef Q_OS_WINCE
        DWORD maxLength;
        QString drive = path.left(3);
        if (QT_WA_INLINE(::GetVolumeInformationW(reinterpret_cast<const WCHAR *>(drive.utf16()), NULL, 0, NULL, &maxLength, NULL, NULL, 0),
            ::GetVolumeInformationA(drive.toLocal8Bit().constData(), NULL, 0, NULL, &maxLength, NULL, NULL, 0)) == FALSE)
            return -1;
        return maxLength;
#else
        Q_UNUSED(path);
        return MAX_PATH;
#endif //Q_OS_WINCE
#else
        Q_UNUSED(path);
#endif
        return -1;
    }

    QString basename(const QString &path) const
    {
        int separator = QDir::toNativeSeparators(path).lastIndexOf(QDir::separator());
        if (separator != -1)
            return path.mid(separator + 1);
        return path;
    }

    QDir::Filters filterForMode(QDir::Filters filters) const
    {
        if (fileMode == QFileDialog::DirectoryOnly) {
            filters |= QDir::Drives | QDir::AllDirs | QDir::Dirs;
            filters &= ~QDir::Files;
        } else {
            filters |= QDir::Drives | QDir::AllDirs | QDir::Files | QDir::Dirs;
        }
        return filters;
    }

    QAbstractItemView *currentView() const;

    static inline QString toInternal(const QString &path)
    {
#if defined(Q_FS_FAT) || defined(Q_OS_OS2EMX)
        QString n(path);
        for (int i = 0; i < (int)n.length(); ++i)
            if (n[i] == QLatin1Char('\\')) n[i] = QLatin1Char('/');
#if defined(Q_OS_WINCE)
        if ((n.size() > 1) && (n.startsWith(QLatin1String("//"))))
            n = n.mid(1);
#endif
        return n;
#else // the compile should optimize away this
        return path;
#endif
    }

    void retranslateWindowTitle();
    void retranslateStrings();

    void _q_goHome();
    void _q_pathChanged(const QString &);
    void _q_navigateBackward();
    void _q_navigateForward();
    void _q_navigateToParent();
    void _q_createDirectory();
    void _q_showListView();
    void _q_showDetailsView();
    void _q_showContextMenu(const QPoint &position);
    void _q_renameCurrent();
    void _q_deleteCurrent();
    void _q_showHidden();
    void _q_showHeader(QAction *);
    void _q_updateOkButton();
    void _q_currentChanged(const QModelIndex &index);
    void _q_enterDirectory(const QModelIndex &index);
    void _q_goToDirectory(const QString &);
    void _q_useNameFilter(int index);
    void _q_selectionChanged();
    void _q_goToUrl(const QUrl &url);
    void _q_autoCompleteFileName(const QString &);
    void _q_rowsInserted(const QModelIndex & parent);
    void _q_fileRenamed(const QString &path, const QString oldName, const QString newName);

    // layout
#ifndef QT_NO_PROXYMODEL
    QAbstractProxyModel *proxyModel;
#endif

    // data
    QStringList watching;
    QFileSystemModel *model;
    QCompleter *completer;

    QFileDialog::FileMode fileMode;
    QFileDialog::AcceptMode acceptMode;
    bool confirmOverwrite;
    QString defaultSuffix;
    QString setWindowTitle;

    QStringList currentHistory;
    int currentHistoryLocation;

    QAction *renameAction;
    QAction *deleteAction;
    QAction *showHiddenAction;
    QAction *newFolderAction;

    bool useDefaultCaption;
    bool defaultFileTypes;
    QStringList nameFilters;
    bool showNameFilterDetails;

    Ui_QFileDialog *qFileDialogUi;

    QString acceptLabel;
};

class QFileDialogLineEdit : public QLineEdit
{
public:
    QFileDialogLineEdit(QWidget *parent = 0) : QLineEdit(parent), hideOnEsc(false), d_ptr(0){}
    void init(QFileDialogPrivate *d_pointer) {d_ptr = d_pointer; }
    void keyPressEvent(QKeyEvent *e);
    bool hideOnEsc;
private:
    QFileDialogPrivate *d_ptr;
};

class QFileDialogComboBox : public QComboBox
{
public:
    QFileDialogComboBox(QWidget *parent = 0) : QComboBox(parent), urlModel(0) {}
    void init(QFileDialogPrivate *d_pointer);
    void showPopup();
    void setHistory(const QStringList &paths);
    QStringList history() const { return m_history; }
    void paintEvent(QPaintEvent *);

private:
    QUrlModel *urlModel;
    QFileDialogPrivate *d_ptr;
    QStringList m_history;
};

class QFileDialogListView : public QListView
{
public:
    QFileDialogListView(QWidget *parent = 0);
    void init(QFileDialogPrivate *d_pointer);
    QSize sizeHint() const;
protected:
    void keyPressEvent(QKeyEvent *e);
private:
    QFileDialogPrivate *d_ptr;
};

class QFileDialogTreeView : public QTreeView
{
public:
    QFileDialogTreeView(QWidget *parent);
    void init(QFileDialogPrivate *d_pointer);
    QSize sizeHint() const;

protected:
    void keyPressEvent(QKeyEvent *e);
private:
    QFileDialogPrivate *d_ptr;
};

inline QModelIndex QFileDialogPrivate::mapToSource(const QModelIndex &index) const {
#ifdef QT_NO_PROXYMODEL
    return index;
#else
    return proxyModel ? proxyModel->mapToSource(index) : index;
#endif
}
inline QModelIndex QFileDialogPrivate::mapFromSource(const QModelIndex &index) const {
#ifdef QT_NO_PROXYMODEL
    return index;
#else
    return proxyModel ? proxyModel->mapFromSource(index) : index;
#endif
}

inline QString QFileDialogPrivate::rootPath() const {
    return model->rootPath();
}

#ifndef QT_NO_COMPLETER
/*!
    QCompleter that can deal with QFileSystemModel
  */
class QFSCompletor :  public QCompleter {
public:
    QFSCompletor(QAbstractItemModel *model, QObject *parent = 0) : QCompleter(model, parent){}
    QString pathFromIndex(const QModelIndex &index) const;
    QStringList splitPath(const QString& path) const;
};
#endif // QT_NO_COMPLETER

QT_END_NAMESPACE

#endif // QT_NO_FILEDIALOG

#endif // QFILEDIALOG_P_H
