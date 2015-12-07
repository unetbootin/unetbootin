/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3FILEDIALOG_H
#define Q3FILEDIALOG_H

#include <QtCore/qdir.h>
#include <QtGui/qdialog.h>
#include <Qt3Support/q3urloperator.h>
#include <Qt3Support/q3valuelist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class QAbstractButton;
class QPushButton;
class QLabel;
class QWidget;
class QFileDialog;
class QTimer;
class Q3NetworkOperation;
class QLineEdit;
class Q3ListViewItem;
class Q3ListBoxItem;
class Q3FileDialogPrivate;
class Q3FileDialogQFileListView;
class QUrlInfo;

#ifndef QT_NO_FILEDIALOG

class Q_COMPAT_EXPORT Q3FileIconProvider : public QObject
{
    Q_OBJECT
public:
    Q3FileIconProvider(QObject * parent = 0, const char* name = 0);
    virtual const QPixmap * pixmap(const QFileInfo &);

private:
    Q_DISABLE_COPY(Q3FileIconProvider)
};

class Q_COMPAT_EXPORT Q3FilePreview
{
public:
    Q3FilePreview();
    virtual ~Q3FilePreview() {}
    virtual void previewUrl(const Q3Url &url) = 0;

};

class Q_COMPAT_EXPORT Q3FileDialog : public QDialog
{
    Q_OBJECT
    Q_ENUMS(Mode ViewMode PreviewMode)
    // ##### Why are this read-only properties ?
    Q_PROPERTY(QString selectedFile READ selectedFile)
    Q_PROPERTY(QString selectedFilter READ selectedFilter)
    Q_PROPERTY(QStringList selectedFiles READ selectedFiles)
    // #### Should not we be able to set the path ?
    Q_PROPERTY(QString dirPath READ dirPath)
    Q_PROPERTY(bool showHiddenFiles READ showHiddenFiles WRITE setShowHiddenFiles)
    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode)
    Q_PROPERTY(PreviewMode previewMode READ previewMode WRITE setPreviewMode)
    Q_PROPERTY(bool infoPreview READ isInfoPreviewEnabled WRITE setInfoPreviewEnabled)
    Q_PROPERTY(bool contentsPreview READ isContentsPreviewEnabled WRITE setContentsPreviewEnabled)

public:
    Q3FileDialog(const QString& dirName, const QString& filter = QString(),
                 QWidget* parent=0, const char* name=0, bool modal = false);
    Q3FileDialog(QWidget* parent=0, const char* name=0, bool modal = false);
    ~Q3FileDialog();

    // recommended static functions

    static QString getOpenFileName(const QString &initially = QString(),
                                    const QString &filter = QString(),
                                    QWidget *parent = 0, const char* name = 0,
                                    const QString &caption = QString(),
                                    QString *selectedFilter = 0,
                                    bool resolveSymlinks = true);
    static QString getSaveFileName(const QString &initially = QString(),
                                    const QString &filter = QString(),
                                    QWidget *parent = 0, const char* name = 0,
                                    const QString &caption = QString(),
                                    QString *selectedFilter = 0,
                                    bool resolveSymlinks = true);
    static QString getExistingDirectory(const QString &dir = QString(),
                                         QWidget *parent = 0,
                                         const char* name = 0,
                                         const QString &caption = QString(),
                                         bool dirOnly = true,
                                         bool resolveSymlinks = true);
    static QStringList getOpenFileNames(const QString &filter= QString(),
                                         const QString &dir = QString(),
                                         QWidget *parent = 0,
                                         const char* name = 0,
                                         const QString &caption = QString(),
                                         QString *selectedFilter = 0,
                                         bool resolveSymlinks = true);

    // other static functions

    static void setIconProvider(Q3FileIconProvider *);
    static Q3FileIconProvider * iconProvider();

    // non-static function for special needs

    QString selectedFile() const;
    QString selectedFilter() const;
    virtual void setSelectedFilter(const QString&);
    virtual void setSelectedFilter(int);

    void setSelection(const QString &);

    void selectAll(bool b);

    QStringList selectedFiles() const;

    QString dirPath() const;

    void setDir(const QDir &);
    const QDir *dir() const;

    void setShowHiddenFiles(bool s);
    bool showHiddenFiles() const;

    void rereadDir();
    void resortDir();

    enum Mode { AnyFile, ExistingFile, Directory, ExistingFiles, DirectoryOnly };
    void setMode(Mode);
    Mode mode() const;

    enum ViewMode { Detail, List };
    enum PreviewMode { NoPreview, Contents, Info };
    void setViewMode(ViewMode m);
    ViewMode viewMode() const;
    void setPreviewMode(PreviewMode m);
    PreviewMode previewMode() const;

    bool eventFilter(QObject *, QEvent *);

    bool isInfoPreviewEnabled() const;
    bool isContentsPreviewEnabled() const;
    void setInfoPreviewEnabled(bool);
    void setContentsPreviewEnabled(bool);

    void setInfoPreview(QWidget *w, Q3FilePreview *preview);
    void setContentsPreview(QWidget *w, Q3FilePreview *preview);

    Q3Url url() const;

    void addFilter(const QString &filter);

public Q_SLOTS:
    void done(int);
    void setDir(const QString&);
    void setUrl(const Q3UrlOperator &url);
    void setFilter(const QString&);
    void setFilters(const QString&);
    void setFilters(const char **);
    void setFilters(const QStringList&);

protected:
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);

    void addWidgets(QLabel *, QWidget *, QPushButton *);
    void addToolButton(QAbstractButton *b, bool separator = false);
    void addLeftWidget(QWidget *w);
    void addRightWidget(QWidget *w);

Q_SIGNALS:
    void fileHighlighted(const QString&);
    void fileSelected(const QString&);
    void filesSelected(const QStringList&);
    void dirEntered(const QString&);
    void filterSelected(const QString&);

private Q_SLOTS:
    void detailViewSelectionChanged();
    void listBoxSelectionChanged();
    void changeMode(int);
    void fileNameEditReturnPressed();
    void stopCopy();
    void removeProgressDia();

    void fileSelected(int);
    void fileHighlighted(int);
    void dirSelected(int);
    void pathSelected(int);

    void updateFileNameEdit(Q3ListViewItem *);
    void selectDirectoryOrFile(Q3ListViewItem *);
    void popupContextMenu(Q3ListViewItem *, const QPoint &, int);
    void popupContextMenu(Q3ListBoxItem *, const QPoint &);
    void updateFileNameEdit(Q3ListBoxItem *);
    void selectDirectoryOrFile(Q3ListBoxItem *);
    void fileNameEditDone();

    void okClicked();
    void filterClicked(); // not used
    void cancelClicked();

    void cdUpClicked();
    void newFolderClicked();

    void fixupNameEdit();

    void doMimeTypeLookup();

    void updateGeometries();
    void modeButtonsDestroyed();
    void urlStart(Q3NetworkOperation *op);
    void urlFinished(Q3NetworkOperation *op);
    void dataTransferProgress(int bytesDone, int bytesTotal, Q3NetworkOperation *);
    void insertEntry(const Q3ValueList<QUrlInfo> &fi, Q3NetworkOperation *op);
    void removeEntry(Q3NetworkOperation *);
    void createdDirectory(const QUrlInfo &info, Q3NetworkOperation *);
    void itemChanged(Q3NetworkOperation *);
    void goBack();

private:
    Q_DISABLE_COPY(Q3FileDialog)

    enum PopupAction {
        PA_Open = 0,
        PA_Delete,
        PA_Rename,
        PA_SortName,
        PA_SortSize,
        PA_SortType,
        PA_SortDate,
        PA_SortUnsorted,
        PA_Cancel,
        PA_Reload,
        PA_Hidden
    };

    void init();
    bool trySetSelection(bool isDir, const Q3UrlOperator &, bool);
    void deleteFile(const QString &filename);
    void popupContextMenu(const QString &filename, bool withSort,
                           PopupAction &action, const QPoint &p);
    void updatePreviews(const Q3Url &u);

    QString fileName;

    friend class Q3FileDialogQFileListView;
    friend class QFileListBox;

    Q3FileDialogPrivate *d;
    Q3FileDialogQFileListView  *files;

    QLineEdit  *nameEdit; // also filter
    QPushButton *okB;
    QPushButton *cancelB;

#if defined(Q_WS_WIN)
    static QString winGetOpenFileName(const QString &initialSelection,
                                       const QString &filter,
                                       QString* workingDirectory,
                                       QWidget *parent = 0,
                                       const char* name = 0,
                                       const QString& caption = QString(),
                                       QString* selectedFilter = 0);
    static QString winGetSaveFileName(const QString &initialSelection,
                                       const QString &filter,
                                       QString* workingDirectory,
                                       QWidget *parent = 0,
                                       const char* name = 0,
                                       const QString& caption = QString(),
                                       QString* selectedFilter = 0);
    static QStringList winGetOpenFileNames(const QString &filter,
                                            QString* workingDirectory,
                                            QWidget *parent = 0,
                                            const char* name = 0,
                                            const QString& caption = QString(),
                                            QString* selectedFilter = 0);
    static QString winGetExistingDirectory(const QString &initialDirectory,
                                            QWidget* parent = 0,
                                            const char* name = 0,
                                            const QString& caption = QString());
    static QString resolveLinkFile(const QString& linkfile);
    int old_qt_ntfs_permission_lookup;
#endif
#if defined(Q_WS_MAC)
    static QString macGetSaveFileName(const QString &, const QString &,
                                       QString *, QWidget *, const char*,
                                       const QString&, QString *);
    static QStringList macGetOpenFileNames(const QString &, QString*,
                                            QWidget *, const char *,
                                            const QString&, QString *,
                                            bool = true, bool = false);
#endif
};

#endif // QT_NO_FILEDIALOG

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3FILEDIALOG_H
