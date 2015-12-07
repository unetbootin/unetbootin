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

#include "qplatformdefs.h"

#include "q3filedialog.h"

#ifndef QT_NO_FILEDIALOG

#include "private/qapplication_p.h"
#include "q3buttongroup.h"
#include "q3header.h"
#include "q3listview.h"
#include "qapplication.h"
#include "qbitmap.h"
#include "qcheckbox.h"
#include "q3cleanuphandler.h"
#include "qcombobox.h"
#include "q3combobox.h"
#include "q3cstring.h"
#include "qcursor.h"
#include "qdesktopwidget.h"
#include "q3dragobject.h"
#include "qevent.h"
#include "qfile.h"
#include "qlabel.h"
#include "qlayout.h"
#include "qlibrary.h"
#include "qlineedit.h"
#include "q3listbox.h"
#include "qmap.h"
#include "qmessagebox.h"
#include "qmime.h"
#include "qpainter.h"
#include "qpointer.h"
#include "q3popupmenu.h"
#include "q3progressbar.h"
#include "q3ptrvector.h"
#include "qpushbutton.h"
#include "qregexp.h"
#include "qsplitter.h"
#include "q3strlist.h"
#include "qstyle.h"
#include "qtimer.h"
#include "qtoolbutton.h"
#include "qtooltip.h"
#include "q3widgetstack.h"
#include "q3urloperator.h"
#include "q3vbox.h"
#include "qurlinfo.h"

#ifdef Q_WS_WIN
#ifndef QT_NO_THREAD
#  include "qwindowsstyle.h"
#  include "private/qmutexpool_p.h"
#endif
#endif // Q_WS_WIN

#ifndef Q_OS_WINCE
#include <time.h>
#else
#include <shellapi.h>
#endif // Q_OS_WINCE
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#ifdef Q_WS_MAC
#include "qmacstyle_mac.h"
#include "private/qt_mac_p.h"
#include "private/qunicodetables_p.h"
#undef check
#endif

#if defined(Q_OS_OPENBSD)
#include <sys/param.h>
#endif

QT_BEGIN_NAMESPACE

/* XPM */
static const char * const start_xpm[]={
    "16 15 8 1",
    "a c #cec6bd",
    "# c #000000",
    "e c #ffff00",
    "b c #999999",
    "f c #cccccc",
    "d c #dcdcdc",
    "c c #ffffff",
    ". c None",
    ".....######aaaaa",
    "...bb#cccc##aaaa",
    "..bcc#cccc#d#aaa",
    ".bcef#cccc#dd#aa",
    ".bcfe#cccc#####a",
    ".bcef#ccccccccc#",
    "bbbbbbbbbbbbccc#",
    "bccccccccccbbcc#",
    "bcefefefefee#bc#",
    ".bcefefefefef#c#",
    ".bcfefefefefe#c#",
    "..bcfefefefeeb##",
    "..bbbbbbbbbbbbb#",
    "...#############",
    "................"};

/* XPM */
static const char * const end_xpm[]={
    "16 15 9 1",
    "d c #a0a0a0",
    "c c #c3c3c3",
    "# c #cec6bd",
    ". c #000000",
    "f c #ffff00",
    "e c #999999",
    "g c #cccccc",
    "b c #ffffff",
    "a c None",
    "......####aaaaaa",
    ".bbbb..###aaaaaa",
    ".bbbb.c.##aaaaaa",
    ".bbbb....ddeeeea",
    ".bbbbbbb.bbbbbe.",
    ".bbbbbbb.bcfgfe.",
    "eeeeeeeeeeeeefe.",
    "ebbbbbbbbbbeege.",
    "ebfgfgfgfgff.ee.",
    "aebfgfgfgfgfg.e.",
    "aebgfgfgfgfgf.e.",
    "aaebgfgfgfgffe..",
    "aaeeeeeeeeeeeee.",
    "aaa.............",
    "aaaaaaaaaaaaaaaa"};

/* XPM */
static const char* const open_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "c c #cccccc",
    "a c #ffffff",
    "................",
    "................",
    "...*****........",
    "..*aaaaa*.......",
    ".*abcbcba******.",
    ".*acbcbcaaaaaa*d",
    ".*abcbcbcbcbcb*d",
    "*************b*d",
    "*aaaaaaaaaa**c*d",
    "*abcbcbcbcbbd**d",
    ".*abcbcbcbcbcd*d",
    ".*acbcbcbcbcbd*d",
    "..*acbcbcbcbb*dd",
    "..*************d",
    "...ddddddddddddd",
    "................"};

/* XPM */
static const char * const link_dir_xpm[]={
    "16 16 10 1",
    "h c #808080",
    "g c #a0a0a0",
    "d c #000000",
    "b c #ffff00",
    "f c #303030",
    "# c #999999",
    "a c #cccccc",
    "e c #585858",
    "c c #ffffff",
    ". c None",
    "................",
    "................",
    "..#####.........",
    ".#ababa#........",
    "#abababa######..",
    "#cccccccccccc#d.",
    "#cbababababab#d.",
    "#cabababababa#d.",
    "#cbababdddddddd.",
    "#cababadccccccd.",
    "#cbababdcececcd.",
    "#cababadcefdfcd.",
    "#cbababdccgdhcd.",
    "#######dccchccd.",
    ".dddddddddddddd.",
    "................"};

/* XPM */
static const char * const link_file_xpm[]={
    "16 16 10 1",
    "h c #808080",
    "g c #a0a0a0",
    "d c #c3c3c3",
    ". c #7f7f7f",
    "c c #000000",
    "b c #bfbfbf",
    "f c #303030",
    "e c #585858",
    "a c #ffffff",
    "# c None",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaadc###",
    ".aaaaaaaaaadc###",
    ".aaaacccccccc###",
    ".aaaacaaaaaac###",
    ".aaaacaeaeaac###",
    ".aaaacaefcfac###",
    ".aaaacaagchac###",
    ".ddddcaaahaac###",
    "ccccccccccccc###"};

/* XPM */
static const char* const file_xpm[]={
    "16 16 5 1",
    ". c #7f7f7f",
    "# c None",
    "c c #000000",
    "b c #bfbfbf",
    "a c #ffffff",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".bbbbbbbbbbbc###",
    "ccccccccccccc###"};

/* XPM */
static const char * const closed_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "a c #cccccc",
    "c c #ffffff",
    "................",
    "................",
    "..*****.........",
    ".*ababa*........",
    "*abababa******..",
    "*cccccccccccc*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "**************d.",
    ".dddddddddddddd.",
    "................"};


/* XPM */
static const char* const cdtoparent_xpm[]={
    "15 13 3 1",
    ". c None",
    "* c #000000",
    "a c #ffff99",
    "..*****........",
    ".*aaaaa*.......",
    "***************",
    "*aaaaaaaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaa***aaaaaaa*",
    "*aa*****aaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa******aaa*",
    "*aaaaaaaaaaaaa*",
    "*aaaaaaaaaaaaa*",
    "***************"};


/* XPM */
static const char* const newfolder_xpm[] = {
    "15 14 4 1",
    "        c None",
    ".        c #000000",
    "+        c #FFFF00",
    "@        c #FFFFFF",
    "          .    ",
    "               ",
    "          .    ",
    "       .     . ",
    "  ....  . . .  ",
    " .+@+@.  . .   ",
    "..........  . .",
    ".@+@+@+@+@..   ",
    ".+@+@+@+@+. .  ",
    ".@+@+@+@+@.  . ",
    ".+@+@+@+@+.    ",
    ".@+@+@+@+@.    ",
    ".+@+@+@+@+.    ",
    "...........    "};

/* XPM */
static const char* const detailedview_xpm[]={
    "14 11 3 1",
    ". c None",
    "* c #000000",
    "a c #000099",
    ".****.***.***.",
    "..............",
    "aaaaaaaaaaaaaa",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***."};

/* XPM */
static const char* const previewinfoview_xpm[]={
    "13 13 4 1",
    ". c #00007f",
    "a c black",
    "# c #cec6bd",
    "b c #000000",
    "..#####aaaaaa",
    ".#.#bb#a#####",
    "...####a#bbb#",
    "#######a#####",
    "#######a#bb##",
    "..#####a#####",
    ".#.#bb#a#bbb#",
    "...####a#####",
    "#######a#bb##",
    "#######a#####",
    "..#####a#bbb#",
    ".#.#bb#a#####",
    "...####aaaaaa"};

/* XPM */
static const char* const previewcontentsview_xpm[]={
    "14 13 5 1",
    ". c #00007f",
    "a c black",
    "c c #7f007f",
    "# c #cec6bd",
    "b c #000000",
    "..#####aaaaaaa",
    ".#.#bb#a#####a",
    "...####a#ccc#a",
    "#######a#ccc#a",
    "#######a#####a",
    "..#####a#bbb#a",
    ".#.#bb#a#####a",
    "...####a#bbb#a",
    "#######a#####a",
    "#######a#bbb#a",
    "..#####a#####a",
    ".#.#bb#a#####a",
    "...####aaaaaaa"};

/* XPM */
static const char* const mclistview_xpm[]={
    "15 11 4 1",
    "* c None",
    "b c #000000",
    ". c #000099",
    "a c #ffffff",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****",
    "***************",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****",
    "***************",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****"};

/* XPM */
static const char * const back_xpm [] = {
    "13 11 3 1",
    "a c #00ffff",
    "# c #000000",
    ". c None",
    ".....#.......",
    "....##.......",
    "...#a#.......",
    "..#aa########",
    ".#aaaaaaaaaa#",
    "#aaaaaaaaaaa#",
    ".#aaaaaaaaaa#",
    "..#aa########",
    "...#a#.......",
    "....##.......",
    ".....#......."};

static QPixmap * openFolderIcon = 0;
static QPixmap * closedFolderIcon = 0;
static QPixmap * detailViewIcon = 0;
static QPixmap * multiColumnListViewIcon = 0;
static QPixmap * cdToParentIcon = 0;
static QPixmap * newFolderIcon = 0;
static QPixmap * fifteenTransparentPixels = 0;
static QPixmap * symLinkDirIcon = 0;
static QPixmap * symLinkFileIcon = 0;
static QPixmap * fileIcon = 0;
static QPixmap * startCopyIcon = 0;
static QPixmap * endCopyIcon = 0;
static QPixmap * previewContentsViewIcon = 0;
static QPixmap * previewInfoViewIcon = 0;
static QPixmap *goBackIcon = 0;
static Q3FileIconProvider * fileIconProvider = 0;
static int lastWidth = 0;
static int lastHeight = 0;
static QString * workingDirectory = 0;

static bool bShowHiddenFiles = false;
static int sortFilesBy = (int)QDir::Name;
static bool sortAscending = true;
static bool detailViewMode = false;

static Q3CleanupHandler<QPixmap> qfd_cleanup_pixmap;
static Q3CleanupHandler<QString> qfd_cleanup_string;

static QString toRootIfNotExists( const QString &path )
{
    if ( !path.isEmpty() )
        return path;

    QFileInfoList drives = QDir::drives();
    Q_ASSERT( !drives.isEmpty() );
    return drives.first().filePath();
}

static bool isDirectoryMode(int m)
{
    return m == Q3FileDialog::Directory || m == Q3FileDialog::DirectoryOnly;
}

static void updateLastSize(Q3FileDialog *that)
{
    int extWidth = 0;
    int extHeight = 0;
    if (that->extension() && that->extension()->isVisible()) {
        if (that->orientation() == Qt::Vertical)
            extHeight = that->extension()->height();
        else
            extWidth = that->extension()->width();
    }
    lastWidth = that->width() - extWidth;
    lastHeight = that->height() - extHeight;
}

// Don't remove the lines below!
//
// resolving the W methods manually is needed, because Windows 95 doesn't include
// these methods in Shell32.lib (not even stubs!), so you'd get an unresolved symbol
// when Qt calls getEsistingDirectory(), etc.
#if defined(Q_WS_WIN)

typedef UINT (WINAPI *PtrExtractIconEx)(LPCTSTR,int,HICON*,HICON*,UINT);
static PtrExtractIconEx ptrExtractIconEx = 0;

static void resolveLibs()
{
#ifndef Q_OS_WINCE
    static bool triedResolve = false;

    if (!triedResolve) {
#ifndef QT_NO_THREAD
        // protect initialization
        QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
        // check triedResolve again, since another thread may have already
        // done the initialization
        if (triedResolve) {
            // another thread did initialize the security function pointers,
            // so we shouldn't do it again.
            return;
        }
#endif
        triedResolve = true;
        if (qt_winUnicode()) {
            QLibrary lib(QLatin1String("shell32"));
            ptrExtractIconEx = (PtrExtractIconEx) lib.resolve("ExtractIconExW");
        }
    }
#endif
}
#ifdef Q_OS_WINCE
#define PtrExtractIconEx ExtractIconEx
#endif

class QWindowsIconProvider : public Q3FileIconProvider
{
public:
    QWindowsIconProvider(QObject *parent=0, const char *name=0);
    ~QWindowsIconProvider();

    const QPixmap * pixmap(const QFileInfo &fi);

private:
    QPixmap defaultFolder;
    QPixmap defaultFile;
    QPixmap defaultExe;
    QPixmap pix;
    int pixw, pixh;
    QMap< QString, QPixmap > cache;

};
#endif

static void makeVariables() {
    if (!openFolderIcon) {
        workingDirectory = new QString(toRootIfNotExists( QDir::currentDirPath() ));
        qfd_cleanup_string.add(&workingDirectory);

        openFolderIcon = new QPixmap((const char **)open_xpm);
        qfd_cleanup_pixmap.add(&openFolderIcon);
        symLinkDirIcon = new QPixmap((const char **)link_dir_xpm);
        qfd_cleanup_pixmap.add(&symLinkDirIcon);
        symLinkFileIcon = new QPixmap((const char **)link_file_xpm);
        qfd_cleanup_pixmap.add(&symLinkFileIcon);
        fileIcon = new QPixmap((const char **)file_xpm);
        qfd_cleanup_pixmap.add(&fileIcon);
        closedFolderIcon = new QPixmap((const char **)closed_xpm);
        qfd_cleanup_pixmap.add(&closedFolderIcon);
        detailViewIcon = new QPixmap((const char **)detailedview_xpm);
        qfd_cleanup_pixmap.add(&detailViewIcon);
        multiColumnListViewIcon = new QPixmap((const char **)mclistview_xpm);
        qfd_cleanup_pixmap.add(&multiColumnListViewIcon);
        cdToParentIcon = new QPixmap((const char **)cdtoparent_xpm);
        qfd_cleanup_pixmap.add(&cdToParentIcon);
        newFolderIcon = new QPixmap((const char **)newfolder_xpm);
        qfd_cleanup_pixmap.add(&newFolderIcon);
        previewInfoViewIcon
            = new QPixmap((const char **)previewinfoview_xpm);
        qfd_cleanup_pixmap.add(&previewInfoViewIcon);
        previewContentsViewIcon
            = new QPixmap((const char **)previewcontentsview_xpm);
        qfd_cleanup_pixmap.add(&previewContentsViewIcon);
        startCopyIcon = new QPixmap((const char **)start_xpm);
        qfd_cleanup_pixmap.add(&startCopyIcon);
        endCopyIcon = new QPixmap((const char **)end_xpm);
        qfd_cleanup_pixmap.add(&endCopyIcon);
        goBackIcon = new QPixmap((const char **)back_xpm);
        qfd_cleanup_pixmap.add(&goBackIcon);
        fifteenTransparentPixels = new QPixmap(closedFolderIcon->width(), 1);
        qfd_cleanup_pixmap.add(&fifteenTransparentPixels);
        QBitmap m(fifteenTransparentPixels->width(), 1);
        m.fill(Qt::color0);
        fifteenTransparentPixels->setMask(m);
        bShowHiddenFiles = false;
        sortFilesBy = (int)QDir::Name;
        detailViewMode = false;
#if defined(Q_WS_WIN)
        if (!fileIconProvider)
            fileIconProvider = new QWindowsIconProvider(qApp);
#endif
    }
}

/******************************************************************
 *
 * Definitions of view classes
 *
 ******************************************************************/

class QRenameEdit : public QLineEdit
{
    Q_OBJECT

public:
    QRenameEdit(QWidget *parent);

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusOutEvent(QFocusEvent *e);

signals:
    void cancelRename();
    void doRename();

private slots:
    void slotReturnPressed();

private:
    bool doRenameAlreadyEmitted;
};

QRenameEdit::QRenameEdit(QWidget *parent)
    : QLineEdit(parent, "qt_rename_edit"), doRenameAlreadyEmitted(false)
{
    connect(this, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
}

class QFileListBox : public Q3ListBox
{
    friend class Q3FileDialog;

    Q_OBJECT

private:
    QFileListBox(QWidget *parent, Q3FileDialog *d);

    void clear();
    void show();
    void startRename(bool check = true);
    void viewportMousePressEvent(QMouseEvent *e);
    void viewportMouseReleaseEvent(QMouseEvent *e);
    void viewportMouseDoubleClickEvent(QMouseEvent *e);
    void viewportMouseMoveEvent(QMouseEvent *e);
#ifndef QT_NO_DRAGANDDROP
    void viewportDragEnterEvent(QDragEnterEvent *e);
    void viewportDragMoveEvent(QDragMoveEvent *e);
    void viewportDragLeaveEvent(QDragLeaveEvent *e);
    void viewportDropEvent(QDropEvent *e);
    bool acceptDrop(const QPoint &pnt, QWidget *source);
    void setCurrentDropItem(const QPoint &pnt);
#endif
    void keyPressEvent(QKeyEvent *e);

private slots:
    void rename();
    void cancelRename();
    void doubleClickTimeout();
    void changeDirDuringDrag();
    void dragObjDestroyed();
    void contentsMoved(int, int);

private:
    QRenameEdit *lined;
    Q3FileDialog *filedialog;
    bool renaming;
    QTimer* renameTimer;
    Q3ListBoxItem *renameItem, *dragItem;
    QPoint pressPos, oldDragPos;
    bool mousePressed;
    int urls;
    QString startDragDir;
    Q3ListBoxItem *currDropItem;
    QTimer *changeDirTimer;
    bool firstMousePressEvent;
    Q3UrlOperator startDragUrl;

};


class Q3FileDialogQFileListView : public Q3ListView
{
    Q_OBJECT

public:
    Q3FileDialogQFileListView(QWidget *parent, Q3FileDialog *d);

    void clear();
    void startRename(bool check = true);
    void setSorting(int column, bool increasing = true);

    QRenameEdit *lined;
    bool renaming;
    Q3ListViewItem *renameItem;

private:
    void viewportMousePressEvent(QMouseEvent *e);
    void viewportMouseDoubleClickEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void viewportMouseReleaseEvent(QMouseEvent *e);
    void viewportMouseMoveEvent(QMouseEvent *e);
#ifndef QT_NO_DRAGANDDROP
    void viewportDragEnterEvent(QDragEnterEvent *e);
    void viewportDragMoveEvent(QDragMoveEvent *e);
    void viewportDragLeaveEvent(QDragLeaveEvent *e);
    void viewportDropEvent(QDropEvent *e);
    bool acceptDrop(const QPoint &pnt, QWidget *source);
    void setCurrentDropItem(const QPoint &pnt);
#endif

private slots:
    void rename();
    void cancelRename();
    void changeSortColumn2(int column);
    void doubleClickTimeout();
    void changeDirDuringDrag();
    void dragObjDestroyed();
    void contentsMoved(int, int);

private:
    Q3FileDialog *filedialog;
    QTimer* renameTimer;
    QPoint pressPos, oldDragPos;
    bool mousePressed;
    int urls;
    QString startDragDir;
    Q3ListViewItem *currDropItem, *dragItem;
    QTimer *changeDirTimer;
    bool firstMousePressEvent;
    bool ascending;
    int sortcolumn;
    Q3UrlOperator startDragUrl;

};

/****************************************************************************
 *
 * Classes for copy progress dialog
 *
 ****************************************************************************/

class QFDProgressAnimation : public QWidget
{
    Q_OBJECT

public:
    QFDProgressAnimation(QWidget *parent);
    void start();

private slots:
    void next();

protected:
    void paintEvent(QPaintEvent *e);

private:
    int step;
    QTimer *timer;

};

QFDProgressAnimation::QFDProgressAnimation(QWidget *parent)
    : QWidget(parent, "qt_progressanimation")
{
    setFixedSize(300, 50);
    step = -1;
    next();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
             this, SLOT(next()));
}

void QFDProgressAnimation::start()
{
    timer->start(150, false);
}

void QFDProgressAnimation::next()
{
    ++step;
    if (step > 10)
        step = 0;
    repaint();
}

void QFDProgressAnimation::paintEvent(QPaintEvent *)
{
    erase();

    QPainter p;
    p.begin(this);
    if (step == 0) {
        p.drawPixmap(5, (height() - startCopyIcon->height()) / 2,
                      *startCopyIcon);
        p.drawPixmap(width() - 5 - openFolderIcon->width(),
                      (height() - openFolderIcon->height()) / 2 , *openFolderIcon);
    } else if (step == 10) {
        p.drawPixmap(5, (height() - openFolderIcon->height()) / 2,
                      *openFolderIcon);
        p.drawPixmap(width() - 5 - endCopyIcon->width(),
                      (height() - endCopyIcon->height()) / 2 , *endCopyIcon);
    } else {
        p.drawPixmap(5, (height() - openFolderIcon->height()) / 2,
                      *openFolderIcon);
        p.drawPixmap(width() - 5 - openFolderIcon->width(),
                      (height() - openFolderIcon->height()) / 2 , *openFolderIcon);
        int x = 10 + openFolderIcon->width();
        int w = width() - 2 * x;
        int s = w / 9;
        p.drawPixmap(x + s * step, (height() - fileIcon->height()) / 2 - fileIcon->height(),
                      *fileIcon);
    }
}


class QFDProgressDialog : public QDialog
{
    Q_OBJECT

public:
    QFDProgressDialog(QWidget *parent, const QString &fn, int steps);

    void setReadProgress(int p);
    void setWriteProgress(int p);
    void setWriteLabel(const QString &s);

signals:
    void cancelled();

private:
    Q3ProgressBar *readBar;
    Q3ProgressBar *writeBar;
    QLabel *writeLabel;
    QFDProgressAnimation *animation;

};

QFDProgressDialog::QFDProgressDialog(QWidget *parent, const QString &fn, int steps)
    : QDialog(parent, "", true)
{
    setWindowTitle(Q3FileDialog::tr("Copy or Move a File"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setMargin(5);

    animation = new QFDProgressAnimation(this);
    layout->addWidget(animation);

    layout->addWidget(new QLabel(Q3FileDialog::tr("Read: %1").arg(fn),
                       this, "qt_read_lbl"));
    readBar = new Q3ProgressBar(steps, this, "qt_readbar");
    readBar->reset();
    readBar->setProgress(0);
    layout->addWidget(readBar);
    writeLabel = new QLabel(Q3FileDialog::tr("Write: %1").arg(QString()),
                             this, "qt_write_lbl");
    layout->addWidget(writeLabel);
    writeBar = new Q3ProgressBar(steps, this, "qt_writebar");
    writeBar->reset();
    writeBar->setProgress(0);
    layout->addWidget(writeBar);

    QPushButton *b = new QPushButton(Q3FileDialog::tr("Cancel"), this,
                                      "qt_cancel_btn");
    b->setFixedSize(b->sizeHint());
    layout->addWidget(b);
    connect(b, SIGNAL(clicked()),
             this, SIGNAL(cancelled()));

    animation->start();
}

void QFDProgressDialog::setReadProgress(int p)
{
    readBar->setProgress(p);
}

void QFDProgressDialog::setWriteProgress(int p)
{
    writeBar->setProgress(p);
}

void QFDProgressDialog::setWriteLabel(const QString &s)
{
    writeLabel->setText(Q3FileDialog::tr("Write: %1").arg(s));
}

/************************************************************************
 *
 * Private Q3FileDialog members
 *
 ************************************************************************/

class Q3FileDialogPrivate {
public:
    ~Q3FileDialogPrivate();

    QStringList history;

    bool geometryDirty;
    Q3ComboBox * paths;
    QComboBox * types;
    QLabel * pathL;
    QLabel * fileL;
    QLabel * typeL;

    QVBoxLayout * topLevelLayout;
    QHBoxLayout *buttonLayout, *leftLayout, *rightLayout;
    Q3PtrList<QHBoxLayout> extraWidgetsLayouts;
    Q3PtrList<QLabel> extraLabels;
    Q3PtrList<QWidget> extraWidgets;
    Q3PtrList<QWidget> extraButtons;
    Q3PtrList<QAbstractButton> toolButtons;

    Q3WidgetStack * stack;

    QToolButton * cdToParent, *newFolder, * detailView, * mcView,
        *previewInfo, *previewContents, *goBack;
    Q3ButtonGroup * modeButtons;

    QString currentFileName;
    Q3ListViewItem *last;

    Q3ListBoxItem *lastEFSelected;

    struct File: public Q3ListViewItem {
        File(Q3FileDialogPrivate * dlgp,
              const QUrlInfo * fi, Q3ListViewItem * parent)
            : Q3ListViewItem(parent, dlgp->last), info(*fi), d(dlgp), i(0), hasMimePixmap(false)
        { setup(); dlgp->last = this; }
        File(Q3FileDialogPrivate * dlgp,
              const QUrlInfo * fi, Q3ListView * parent)
            : Q3ListViewItem(parent, dlgp->last), info(*fi), d(dlgp), i(0), hasMimePixmap(false)
        { setup(); dlgp->last = this; }
        File(Q3FileDialogPrivate * dlgp,
              const QUrlInfo * fi, Q3ListView * parent, Q3ListViewItem * after)
            : Q3ListViewItem(parent, after), info(*fi), d(dlgp), i(0), hasMimePixmap(false)
        { setup(); if (!nextSibling()) dlgp->last = this; }
        ~File();

        QString text(int column) const;
        const QPixmap * pixmap(int) const;

        QUrlInfo info;
        Q3FileDialogPrivate * d;
        Q3ListBoxItem *i;
        bool hasMimePixmap;
    };

    class MCItem: public Q3ListBoxItem {
    public:
        MCItem(Q3ListBox *, Q3ListViewItem * item);
        MCItem(Q3ListBox *, Q3ListViewItem * item, Q3ListBoxItem *after);
        QString text() const;
        const QPixmap *pixmap() const;
        int height(const Q3ListBox *) const;
        int width(const Q3ListBox *) const;
        void paint(QPainter *);
        Q3ListViewItem * i;
    };

    class UrlInfoList : public Q3PtrList<QUrlInfo> {
    public:
        UrlInfoList() { setAutoDelete(true); }
        int compareItems(Q3PtrCollection::Item n1, Q3PtrCollection::Item n2) {
            if (!n1 || !n2)
                return 0;

            QUrlInfo *i1 = (QUrlInfo *)n1;
            QUrlInfo *i2 = (QUrlInfo *)n2;

            if (i1->isDir() && !i2->isDir())
                return -1;
            if (!i1->isDir() && i2->isDir())
                return 1;

            if (i1->name() == QLatin1String(".."))
                return -1;
            if (i2->name() == QLatin1String(".."))
                return 1;

            if (sortFilesBy == QDir::Name) {
#if defined(Q_OS_WIN32)
		QString name1 = i1->name().lower();
		QString name2 = i2->name().lower();
		return name1.localeAwareCompare( name2 );
#else
		QString name1 = i1->name();
		QString name2 = i2->name();
		return name1.localeAwareCompare( name2 );
#endif
            }
            if (QUrlInfo::equal(*i1, *i2, sortFilesBy))
                return 0;
            else if (QUrlInfo::greaterThan(*i1, *i2, sortFilesBy))
                return 1;
            else if (QUrlInfo::lessThan(*i1, *i2, sortFilesBy))
                return -1;
            // can't happen...
            return 0;
        }
        QUrlInfo *operator[](int i) {
            return at(i);
        }
    };

    UrlInfoList sortedList;
    Q3PtrList<File> pendingItems;

    QFileListBox * moreFiles;

    Q3FileDialog::Mode mode;

    QString rw;
    QString ro;
    QString wo;
    QString inaccessible;

    QString symLinkToFile;
    QString file;
    QString symLinkToDir;
    QString dir;
    QString symLinkToSpecial;
    QString special;
    Q3WidgetStack *preview;
    bool infoPreview, contentsPreview;
    QSplitter *splitter;
    Q3UrlOperator url, oldUrl;
    QWidget *infoPreviewWidget, *contentsPreviewWidget;
    Q3FilePreview *infoPreviewer, *contentsPreviewer;
    bool hadDotDot;

    bool ignoreNextKeyPress;
    // ignores the next refresh operation in case the user forced a selection
    bool ignoreNextRefresh;
    QFDProgressDialog *progressDia;
    bool checkForFilter;
    bool ignoreStop;

    QTimer *mimeTypeTimer;
    const Q3NetworkOperation *currListChildren;

    // this is similar to QUrl::encode but does encode "*" and
    // doesn't encode whitespaces
    static QString encodeFileName(const QString& fName) {

        QString newStr;
        Q3CString cName = fName.utf8();
        const Q3CString sChars(
#ifdef Q_WS_WIN
            "#%"
#else
            "<>#@\"&%$:,;?={}|^~[]\'`\\*"
#endif
           );

        int len = cName.length();
        if (!len)
            return QString();
        for (int i = 0; i < len ;++i) {
            uchar inCh = (uchar)cName[i];
            if (inCh >= 128 || sChars.contains(inCh))
            {
                newStr += QLatin1Char('%');
                ushort c = inCh / 16;
                c += c > 9 ? 'A' - 10 : '0';
                newStr += QLatin1Char((char)c);
                c = inCh % 16;
                c += c > 9 ? 'A' - 10 : '0';
                newStr += QLatin1Char((char)c);
            } else {
                newStr += QLatin1Char((char)inCh);
            }
        }
        return newStr;
    }

    static bool fileExists(const Q3UrlOperator &url, const QString& name)
    {
        Q3Url u(url, Q3FileDialogPrivate::encodeFileName(name));
        if (u.isLocalFile()) {
            QFileInfo f(u.path());
            return f.exists();
        } else {
            Q3NetworkProtocol *p = Q3NetworkProtocol::getNetworkProtocol(url.protocol());
            if (p && (p->supportedOperations()&Q3NetworkProtocol::OpListChildren)) {
                QUrlInfo ui(url.info(name.isEmpty() ? QString::fromLatin1(".") : name));
                return ui.isValid();
            }
        }
        return true;
    }

#ifndef Q_NO_CURSOR
    bool cursorOverride; // Remember if the cursor was overridden or not.
#endif
};

Q3FileDialogPrivate::~Q3FileDialogPrivate()
{
    delete modeButtons;
}



/************************************************************************
 *
 * Internal class QRenameEdit
 *
 ************************************************************************/

void QRenameEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        emit cancelRename();
    else
        QLineEdit::keyPressEvent(e);
    e->accept();
}

void QRenameEdit::focusOutEvent(QFocusEvent *)
{
    if (!doRenameAlreadyEmitted) {
        doRenameAlreadyEmitted = true;
        emit doRename();
    }
}

void QRenameEdit::slotReturnPressed()
{
    doRenameAlreadyEmitted = true;
    emit doRename();
}

/************************************************************************
 *
 * Internal class QFileListBox
 *
 ************************************************************************/

QFileListBox::QFileListBox(QWidget *parent, Q3FileDialog *dlg)
    : Q3ListBox(parent, "filelistbox"), filedialog(dlg),
      renaming(false), renameItem(0), mousePressed(false),
      firstMousePressEvent(true)
{
    changeDirTimer = new QTimer(this);
    Q3VBox *box = new Q3VBox(viewport(), "qt_vbox");
    box->setFrameStyle(QFrame::Box | QFrame::Plain);
    lined = new QRenameEdit(box);
    lined->setFixedHeight(lined->sizeHint().height());
    box->hide();
    box->setBackgroundRole(QPalette::Base);
    renameTimer = new QTimer(this);
    connect(lined, SIGNAL(doRename()),
             this, SLOT (rename()));
    connect(lined, SIGNAL(cancelRename()),
             this, SLOT(cancelRename()));
    connect(renameTimer, SIGNAL(timeout()),
             this, SLOT(doubleClickTimeout()));
    connect(changeDirTimer, SIGNAL(timeout()),
             this, SLOT(changeDirDuringDrag()));
    connect(this, SIGNAL(contentsMoving(int,int)),
             this, SLOT(contentsMoved(int,int)));
    viewport()->setAcceptDrops(true);
    dragItem = 0;
}

void QFileListBox::show()
{
    setBackgroundRole(QPalette::Base);
    viewport()->setBackgroundRole(QPalette::Base);
    Q3ListBox::show();
}

void QFileListBox::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter ||
           e->key() == Qt::Key_Return) &&
         renaming)
        return;

    QString keyPressed = ((QKeyEvent *)e)->text().toLower();
    QChar keyChar = keyPressed[0];
    if (keyChar.isLetterOrNumber()) {
        Q3ListBoxItem * i = 0;
        if (currentItem() != -1)
        i = item(currentItem());
        else
        i = firstItem();
        if (i->next())
        i = i->next();
        else
        i = firstItem();
        while (i != item(currentItem())) {
            QString it = text(index(i));
            if (it[0].toLower() == keyChar) {
            clearSelection();
            setCurrentItem(i);
            } else {
            if (i->next())
            i = i->next();
            else {
                if (!item(currentItem())) {
                    clearSelection();
                    break;
                }
                i = firstItem();
                }
            }
        }
    }
    cancelRename();
    Q3ListBox::keyPressEvent(e);
}

void QFileListBox::viewportMousePressEvent(QMouseEvent *e)
{
    pressPos = e->pos();
    mousePressed = false;

    bool didRename = renaming;

    cancelRename();
    if (!hasFocus() && !viewport()->hasFocus())
        setFocus();

    if (e->button() != Qt::LeftButton) {
        Q3ListBox::viewportMousePressEvent(e);
        firstMousePressEvent = false;
        return;
    }

    int i = currentItem();
    bool wasSelected = false;
    if (i != -1)
        wasSelected = item(i)->isSelected();
    Q3ListBox::mousePressEvent(e);

    Q3FileDialogPrivate::MCItem *i1 = (Q3FileDialogPrivate::MCItem*)item(currentItem());
    if (i1)
        mousePressed =  (!((Q3FileDialogPrivate::File*)i1->i)->info.isDir())
                        || (filedialog->mode() == Q3FileDialog::Directory) || (filedialog->mode() == Q3FileDialog::DirectoryOnly);

    if (itemAt(e->pos()) != item(i)) {
        firstMousePressEvent = false;
        return;
    }

     if (!firstMousePressEvent && !didRename && i == currentItem() && currentItem() != -1 &&
          wasSelected && QUrlInfo(filedialog->d->url.info(QString(QLatin1Char('.')))).isWritable() && item(currentItem())->text() != QLatin1String("..")) {
        renameTimer->start(QApplication::doubleClickInterval(), true);
        renameItem = item(i);
    }

    firstMousePressEvent = false;
}

void QFileListBox::viewportMouseReleaseEvent(QMouseEvent *e)
{
    dragItem = 0;
    Q3ListBox::viewportMouseReleaseEvent(e);
    mousePressed = false;
}

void QFileListBox::viewportMouseDoubleClickEvent(QMouseEvent *e)
{
    renameTimer->stop();
    Q3ListBox::viewportMouseDoubleClickEvent(e);
}

void QFileListBox::viewportMouseMoveEvent(QMouseEvent *e)
{
    if (!dragItem)
        dragItem = itemAt(e->pos());
    renameTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    if ( (pressPos - e->pos()).manhattanLength() > QApplication::startDragDistance() && mousePressed) {
        Q3ListBoxItem *item = dragItem;
        dragItem = 0;
        if (item) {
            if (!itemRect(item).contains(e->pos()))
                return;
            Q3UriDrag* drag = new Q3UriDrag(viewport());
            QStringList files;
            if (filedialog->mode() == Q3FileDialog::ExistingFiles)
                files = filedialog->selectedFiles();
            else
                files = QStringList(filedialog->selectedFile());
            drag->setFileNames(files);

            if (lined->parentWidget()->isVisible())
                cancelRename();

            connect(drag, SIGNAL(destroyed()),
                     this, SLOT(dragObjDestroyed()));
            drag->drag();

            mousePressed = false;
        }
    } else
#endif
    {
        Q3ListBox::viewportMouseMoveEvent(e);
    }

}

void QFileListBox::dragObjDestroyed()
{
#ifndef QT_NO_DRAGANDDROP
    //#######
    //filedialog->rereadDir();
#endif
}

#ifndef QT_NO_DRAGANDDROP
void QFileListBox::viewportDragEnterEvent(QDragEnterEvent *e)
{
    startDragUrl = filedialog->d->url;
    startDragDir = filedialog->dirPath();
    currDropItem = 0;

    if (!Q3UriDrag::canDecode(e)) {
        e->ignore();
        return;
    }

    QStringList l;
    Q3UriDrag::decodeLocalFiles(e, l);
    urls = (int)l.count();

    if (acceptDrop(e->pos(), e->source())) {
        e->accept();
        setCurrentDropItem(e->pos());
    } else {
        e->ignore();
        setCurrentDropItem(QPoint(-1, -1));
    }

    oldDragPos = e->pos();
}

void QFileListBox::viewportDragMoveEvent(QDragMoveEvent *e)
{
    if (acceptDrop(e->pos(), e->source())) {
        switch (e->action()) {
        case QDropEvent::Copy:
            e->acceptAction();
            break;
        case QDropEvent::Move:
            e->acceptAction();
            break;
        case QDropEvent::Link:
            break;
        default:
            break;
        }
        if (oldDragPos != e->pos())
            setCurrentDropItem(e->pos());
    } else {
        changeDirTimer->stop();
        e->ignore();
        setCurrentDropItem(QPoint(-1, -1));
    }

    oldDragPos = e->pos();
}

void QFileListBox::viewportDragLeaveEvent(QDragLeaveEvent *)
{
    changeDirTimer->stop();
    setCurrentDropItem(QPoint(-1, -1));
//########
//     if (startDragDir != filedialog->d->url)
//        filedialog->setUrl(startDragUrl);
}

void QFileListBox::viewportDropEvent(QDropEvent *e)
{
    changeDirTimer->stop();

    if (!Q3UriDrag::canDecode(e)) {
        e->ignore();
        return;
    }

    Q3StrList l;
    Q3UriDrag::decode(e, l);

    bool move = e->action() == QDropEvent::Move;
//     bool supportAction = move || e->action() == QDropEvent::Copy;

    Q3UrlOperator dest;
    if (currDropItem)
        dest = Q3UrlOperator(filedialog->d->url, Q3FileDialogPrivate::encodeFileName(currDropItem->text()));
    else
        dest = filedialog->d->url;
    QStringList lst;
    for (uint i = 0; i < l.count(); ++i) {
        lst << QLatin1String(l.at(i));
    }

    filedialog->d->url.copy(lst, dest, move);

    // ##### what is supportAction for?
    e->acceptAction();
    currDropItem = 0;
}

bool QFileListBox::acceptDrop(const QPoint &pnt, QWidget *source)
{
    Q3ListBoxItem *item = itemAt(pnt);
    if (!item || (item && !itemRect(item).contains(pnt))) {
        if (source == viewport() && startDragDir == filedialog->dirPath())
            return false;
        return true;
    }

    QUrlInfo fi(filedialog->d->url.info(item->text().isEmpty() ? QString::fromLatin1(".") : item->text()));

    if (fi.isDir() && itemRect(item).contains(pnt))
        return true;
    return false;
}

void QFileListBox::setCurrentDropItem(const QPoint &pnt)
{
    changeDirTimer->stop();

    Q3ListBoxItem *item = 0;
    if (pnt != QPoint(-1, -1))
        item = itemAt(pnt);
    if (item && !QUrlInfo(filedialog->d->url.info(item->text().isEmpty() ? QString::fromLatin1(".") : item->text())).isDir())
        item = 0;
    if (item && !itemRect(item).contains(pnt))
        item = 0;

    currDropItem = item;
    if (currDropItem)
        setCurrentItem(currDropItem);
    changeDirTimer->start(750);
}
#endif // QT_NO_DRAGANDDROP

void QFileListBox::changeDirDuringDrag()
{
#ifndef QT_NO_DRAGANDDROP
    if (!currDropItem)
        return;
    changeDirTimer->stop();
    Q3Url u(filedialog->d->url, Q3FileDialogPrivate::encodeFileName(currDropItem->text()));
    filedialog->setDir(u);
    currDropItem = 0;
#endif
}

void QFileListBox::doubleClickTimeout()
{
    startRename();
    renameTimer->stop();
}

void QFileListBox::startRename(bool check)
{
    if (check && (!renameItem || renameItem != item(currentItem())))
        return;

    int i = currentItem();
    setSelected(i, true);
    QRect r = itemRect(item(i));
    int bdr = item(i)->pixmap() ?
              item(i)->pixmap()->width() : 16;
    int x = r.x() + bdr;
    int y = r.y();
    int w = item(i)->width(this) - bdr;
    int h = qMax(lined->height() + 2, r.height());
    y = y + r.height() / 2 - h / 2;

    lined->parentWidget()->setGeometry(x, y, w + 6, h);
    lined->setFocus();
    lined->setText(item(i)->text());
    lined->selectAll();
    lined->setFrame(false);
    lined->parentWidget()->show();
    viewport()->setFocusProxy(lined);
    renaming = true;
}

void QFileListBox::clear()
{
    cancelRename();
    Q3ListBox::clear();
}

void QFileListBox::rename()
{
    if (!lined->text().isEmpty()) {
        QString file = currentText();

        if (lined->text() != file)
            filedialog->d->url.rename(file, lined->text());
    }
    cancelRename();
}

void QFileListBox::cancelRename()
{
    renameItem = 0;
    lined->parentWidget()->hide();
    viewport()->setFocusProxy(this);
    renaming = false;
    updateItem(currentItem());
    if (lined->hasFocus())
        viewport()->setFocus();
}

void QFileListBox::contentsMoved(int, int)
{
    changeDirTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    setCurrentDropItem(QPoint(-1, -1));
#endif
}

/************************************************************************
 *
 * Internal class QFileListView
 *
 ************************************************************************/

Q3FileDialogQFileListView::Q3FileDialogQFileListView(QWidget *parent, Q3FileDialog *dlg)
    : Q3ListView(parent, "qt_filedlg_listview"), renaming(false), renameItem(0),
    filedialog(dlg), mousePressed(false),
    firstMousePressEvent(true)
{
    changeDirTimer = new QTimer(this);
    Q3VBox *box = new Q3VBox(viewport(), "qt_vbox");
    box->setFrameStyle(QFrame::Box | QFrame::Plain);
    lined = new QRenameEdit(box);
    lined->setFixedHeight(lined->sizeHint().height());
    box->hide();
    box->setBackgroundRole(QPalette::Base);
    renameTimer = new QTimer(this);
    connect(lined, SIGNAL(doRename()),
             this, SLOT (rename()));
    connect(lined, SIGNAL(cancelRename()),
             this, SLOT(cancelRename()));
    header()->setMovingEnabled(false);
    connect(renameTimer, SIGNAL(timeout()),
             this, SLOT(doubleClickTimeout()));
    connect(changeDirTimer, SIGNAL(timeout()),
             this, SLOT(changeDirDuringDrag()));
    disconnect(header(), SIGNAL(sectionClicked(int)),
                this, SLOT(changeSortColumn(int)));
    connect(header(), SIGNAL(sectionClicked(int)),
             this, SLOT(changeSortColumn2(int)));
    connect(this, SIGNAL(contentsMoving(int,int)),
             this, SLOT(contentsMoved(int,int)));

    viewport()->setAcceptDrops(true);
    sortcolumn = 0;
    ascending = true;
    dragItem = 0;
}

void Q3FileDialogQFileListView::setSorting(int column, bool increasing)
{
    if (column == -1) {
        Q3ListView::setSorting(column, increasing);
        return;
    }

    sortAscending = ascending = increasing;
    sortcolumn = column;
    switch (column) {
    case 0:
        sortFilesBy = QDir::Name;
        break;
    case 1:
        sortFilesBy = QDir::Size;
        break;
    case 3:
        sortFilesBy = QDir::Time;
        break;
    default:
        sortFilesBy = QDir::Name; // #### ???
        break;
    }

    filedialog->resortDir();
}

void Q3FileDialogQFileListView::changeSortColumn2(int column)
{
    int lcol = header()->mapToLogical(column);
    setSorting(lcol, sortcolumn == lcol ? !ascending : true);
}

void Q3FileDialogQFileListView::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter ||
           e->key() == Qt::Key_Return) &&
         renaming)
        return;

    QString keyPressed = e->text().toLower();
    QChar keyChar = keyPressed[0];
    if (keyChar.isLetterOrNumber()) {
        Q3ListViewItem * i = 0;
        if (currentItem())
        i = currentItem();
        else
        i = firstChild();
        if (i->nextSibling())
        i = i->nextSibling();
        else
        i = firstChild();
        while (i != currentItem()) {
            QString it = i->text(0);
            if (it[0].toLower() == keyChar) {
            clearSelection();
            ensureItemVisible(i);
            setCurrentItem(i);
            } else {
            if (i->nextSibling())
            i = i->nextSibling();
            else
            i = firstChild();
            }
        }
        return;
    }

    cancelRename();
    Q3ListView::keyPressEvent(e);
}

void Q3FileDialogQFileListView::viewportMousePressEvent(QMouseEvent *e)
{
    pressPos = e->pos();
    mousePressed = false;

    bool didRename = renaming;
    cancelRename();
    if (!hasFocus() && !viewport()->hasFocus())
        setFocus();

    if (e->button() != Qt::LeftButton) {
        Q3ListView::viewportMousePressEvent(e);
        firstMousePressEvent = false;
        return;
    }

    Q3ListViewItem *i = currentItem();
    Q3ListView::viewportMousePressEvent(e);

    Q3FileDialogPrivate::File *i1 = (Q3FileDialogPrivate::File*)currentItem();
    if (i1)
        mousePressed = !i1->info.isDir() || (filedialog->mode() == Q3FileDialog::Directory) || (filedialog->mode() == Q3FileDialog::DirectoryOnly);


    if (itemAt(e->pos()) != i ||
         e->x() + contentsX() > columnWidth(0)) {
        firstMousePressEvent = false;
        return;
    }

    if (!firstMousePressEvent && !didRename && i == currentItem() && currentItem() &&
         QUrlInfo(filedialog->d->url.info(QString(QLatin1Char('.')))).isWritable() && currentItem()->text(0) != QLatin1String("..")) {
        renameTimer->start(QApplication::doubleClickInterval(), true);
        renameItem = currentItem();
    }

    firstMousePressEvent = false;
}

void Q3FileDialogQFileListView::viewportMouseDoubleClickEvent(QMouseEvent *e)
{
    renameTimer->stop();
    Q3ListView::viewportMouseDoubleClickEvent(e);
}

void Q3FileDialogQFileListView::viewportMouseReleaseEvent(QMouseEvent *e)
{
    Q3ListView::viewportMouseReleaseEvent(e);
    mousePressed = false;
    dragItem = 0;
}

void Q3FileDialogQFileListView::viewportMouseMoveEvent(QMouseEvent *e)
{
    renameTimer->stop();
    if (!dragItem)
        dragItem = itemAt(e->pos());
#ifndef QT_NO_DRAGANDDROP
    if ( (pressPos - e->pos()).manhattanLength() > QApplication::startDragDistance() && mousePressed) {
        Q3ListViewItem *item = dragItem;
        dragItem = 0;
        if (item) {
            Q3UriDrag* drag = new Q3UriDrag(viewport());
            QStringList files;
            if (filedialog->mode() == Q3FileDialog::ExistingFiles)
                files = filedialog->selectedFiles();
            else
                files = QStringList(filedialog->selectedFile());
            drag->setFileNames(files);

            if (lined->isVisible())
                cancelRename();

            connect(drag, SIGNAL(destroyed()),
                     this, SLOT(dragObjDestroyed()));
            drag->drag();

            mousePressed = false;
        }
    }
#endif
}

void Q3FileDialogQFileListView::dragObjDestroyed()
{
#ifndef QT_NO_DRAGANDDROP
    //######
    //filedialog->rereadDir();
#endif
}

#ifndef QT_NO_DRAGANDDROP
void Q3FileDialogQFileListView::viewportDragEnterEvent(QDragEnterEvent *e)
{
    startDragUrl = filedialog->d->url;
    startDragDir = filedialog->dirPath();
    currDropItem = 0;

    if (!Q3UriDrag::canDecode(e)) {
        e->ignore();
        return;
    }

    QStringList l;
    Q3UriDrag::decodeLocalFiles(e, l);
    urls = (int)l.count();

    if (acceptDrop(e->pos(), e->source())) {
        e->accept();
        setCurrentDropItem(e->pos());
    } else {
        e->ignore();
        setCurrentDropItem(QPoint(-1, -1));
    }

    oldDragPos = e->pos();
}

void Q3FileDialogQFileListView::viewportDragMoveEvent(QDragMoveEvent *e)
{
    if (acceptDrop(e->pos(), e->source())) {
        if (oldDragPos != e->pos())
            setCurrentDropItem(e->pos());
        switch (e->action()) {
        case QDropEvent::Copy:
            e->acceptAction();
            break;
        case QDropEvent::Move:
            e->acceptAction();
            break;
        case QDropEvent::Link:
            break;
        default:
            break;
        }
    } else {
        changeDirTimer->stop();
        e->ignore();
        setCurrentDropItem(QPoint(-1, -1));
    }

    oldDragPos = e->pos();
}

void Q3FileDialogQFileListView::viewportDragLeaveEvent(QDragLeaveEvent *)
{
    changeDirTimer->stop();
    setCurrentDropItem(QPoint(-1, -1));
//########
//     if (startDragDir != filedialog->d->url)
//        filedialog->setUrl(startDragUrl);
}

void Q3FileDialogQFileListView::viewportDropEvent(QDropEvent *e)
{
    changeDirTimer->stop();

    if (!Q3UriDrag::canDecode(e)) {
        e->ignore();
        return;
    }

    QStringList l;
    Q3UriDrag::decodeToUnicodeUris(e, l);

    bool move = e->action() == QDropEvent::Move;
//     bool supportAction = move || e->action() == QDropEvent::Copy;

    Q3UrlOperator dest;
    if (currDropItem)
        dest = Q3UrlOperator(filedialog->d->url, Q3FileDialogPrivate::encodeFileName(currDropItem->text(0)));
    else
        dest = filedialog->d->url;
    filedialog->d->url.copy(l, dest, move);

    // ##### what is supportAction for?
    e->acceptAction();
    currDropItem = 0;
}

bool Q3FileDialogQFileListView::acceptDrop(const QPoint &pnt, QWidget *source)
{
    Q3ListViewItem *item = itemAt(pnt);
    if (!item || (item && !itemRect(item).contains(pnt))) {
        if (source == viewport() && startDragDir == filedialog->dirPath())
            return false;
        return true;
    }

    QUrlInfo fi(filedialog->d->url.info(item->text(0).isEmpty() ? QString::fromLatin1(".") : item->text(0)));

    if (fi.isDir() && itemRect(item).contains(pnt))
        return true;
    return false;
}

void Q3FileDialogQFileListView::setCurrentDropItem(const QPoint &pnt)
{
    changeDirTimer->stop();

    Q3ListViewItem *item = itemAt(pnt);
    if (pnt == QPoint(-1, -1))
        item = 0;
    if (item && !QUrlInfo(filedialog->d->url.info(item->text(0).isEmpty() ? QString::fromLatin1(".") : item->text(0))).isDir())
        item = 0;

    if (item && !itemRect(item).contains(pnt))
        item = 0;

    currDropItem = item;

    if (currDropItem)
        setCurrentItem(currDropItem);

    changeDirTimer->start(750);
}
#endif // QT_NO_DRAGANDDROP

void Q3FileDialogQFileListView::changeDirDuringDrag()
{
#ifndef QT_NO_DRAGANDDROP
    if (!currDropItem)
        return;
    changeDirTimer->stop();
    Q3Url u(filedialog->d->url, Q3FileDialogPrivate::encodeFileName(currDropItem->text(0)));
    filedialog->setDir(u);
    currDropItem = 0;
#endif // QT_NO_DRAGANDDROP
}


void Q3FileDialogQFileListView::doubleClickTimeout()
{
    startRename();
    renameTimer->stop();
}

void Q3FileDialogQFileListView::startRename(bool check)
{
    if (check && (!renameItem || renameItem != currentItem()))
        return;

    Q3ListViewItem *i = currentItem();
    setSelected(i, true);

    QRect r = itemRect(i);
    int bdr = i->pixmap(0) ?
              i->pixmap(0)->width() : 16;
    int x = r.x() + bdr;
    int y = r.y();
    int w = columnWidth(0) - bdr;
    int h = qMax(lined->height() + 2, r.height());
    y = y + r.height() / 2 - h / 2;

    lined->parentWidget()->setGeometry(x, y, w + 6, h);
    lined->setFocus();
    lined->setText(i->text(0));
    lined->selectAll();
    lined->setFrame(false);
    lined->parentWidget()->show();
    viewport()->setFocusProxy(lined);
    renaming = true;
}

void Q3FileDialogQFileListView::clear()
{
    cancelRename();
    Q3ListView::clear();
}

void Q3FileDialogQFileListView::rename()
{
    if (!lined->text().isEmpty()) {
        QString file = currentItem()->text(0);

        if (lined->text() != file)
            filedialog->d->url.rename(file, lined->text());
    }
    cancelRename();
}

void Q3FileDialogQFileListView::cancelRename()
{
    renameItem = 0;
    lined->parentWidget()->hide();
    viewport()->setFocusProxy(this);
    renaming = false;
    if (currentItem())
        currentItem()->repaint();
    if (lined->hasFocus())
        viewport()->setFocus();
}

void Q3FileDialogQFileListView::contentsMoved(int, int)
{
    changeDirTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    setCurrentDropItem(QPoint(-1, -1));
#endif
}


Q3FileDialogPrivate::File::~File()
{
    if (d->pendingItems.findRef(this))
        d->pendingItems.removeRef(this);
}

QString Q3FileDialogPrivate::File::text(int column) const
{
    makeVariables();

    switch(column) {
    case 0:
        return info.name();
    case 1:
        if (info.isFile()) {
            QIODevice::Offset size = info.size();
            return QString::number(size);
        } else {
            return QString::fromLatin1("");
        }
    case 2:
        if (info.isFile() && info.isSymLink()) {
            return d->symLinkToFile;
        } else if (info.isFile()) {
            return d->file;
        } else if (info.isDir() && info.isSymLink()) {
            return d->symLinkToDir;
        } else if (info.isDir()) {
            return d->dir;
        } else if (info.isSymLink()) {
            return d->symLinkToSpecial;
        } else {
            return d->special;
        }
    case 3: {
        return info.lastModified().toString(Qt::LocalDate);
    }
    case 4:
        if (info.isReadable())
            return info.isWritable() ? d->rw : d->ro;
        else
            return info.isWritable() ? d->wo : d->inaccessible;
    }

    return QString::fromLatin1("<--->");
}

const QPixmap * Q3FileDialogPrivate::File::pixmap(int column) const
{
    if (column) {
        return 0;
    } else if (Q3ListViewItem::pixmap(column)) {
        return Q3ListViewItem::pixmap(column);
    } else if (info.isSymLink()) {
        if (info.isFile())
            return symLinkFileIcon;
        else
            return symLinkDirIcon;
    } else if (info.isDir()) {
        return closedFolderIcon;
    } else if (info.isFile()) {
        return fileIcon;
    } else {
        return fifteenTransparentPixels;
    }
}

Q3FileDialogPrivate::MCItem::MCItem(Q3ListBox * lb, Q3ListViewItem * item)
    : Q3ListBoxItem()
{
    i = item;
    if (lb)
        lb->insertItem(this);
}

Q3FileDialogPrivate::MCItem::MCItem(Q3ListBox * lb, Q3ListViewItem * item, Q3ListBoxItem *after)
    : Q3ListBoxItem()
{
    i = item;
    if (lb)
        lb->insertItem(this, after);
}

QString Q3FileDialogPrivate::MCItem::text() const
{
    return i->text(0);
}


const QPixmap *Q3FileDialogPrivate::MCItem::pixmap() const
{
    return i->pixmap(0);
}


int Q3FileDialogPrivate::MCItem::height(const Q3ListBox * lb) const
{
    int hf = lb->fontMetrics().height();
    int hp = pixmap() ? pixmap()->height() : 0;
    return qMax(hf, hp) + 2;
}


int Q3FileDialogPrivate::MCItem::width(const Q3ListBox * lb) const
{
    QFontMetrics fm = lb->fontMetrics();
    int w = 2;
    if (pixmap())
        w += pixmap()->width() + 4;
    else
        w += 18;
    w += fm.width(text());
    w += -fm.minLeftBearing();
    w += -fm.minRightBearing();
    w += 6;
    return w;
}


void Q3FileDialogPrivate::MCItem::paint(QPainter * ptr)
{
    QFontMetrics fm = ptr->fontMetrics();

    int h;

    if (pixmap())
        h = qMax(fm.height(), pixmap()->height()) + 2;
    else
        h = fm.height() + 2;

    const QPixmap * pm = pixmap();
    if (pm)
        ptr->drawPixmap(2, 1, *pm);

    ptr->drawText(pm ? pm->width() + 4 : 22, h - fm.descent() - 2,
                   text());
}

static QStringList makeFiltersList(const QString &filter)
{
    if (filter.isEmpty())
        return QStringList();

    int i = filter.indexOf(QLatin1String(";;"), 0);
    QString sep(QLatin1String(";;"));
    if (i == -1) {
        if (filter.contains(QLatin1Char('\n'))) {
            sep = QLatin1Char('\n');
            i = filter.indexOf(sep);
        }
    }

    return QStringList::split(sep, filter);
}

/*!
  \class Q3FileDialog qfiledialog.h
  \brief The Q3FileDialog class provides dialogs that allow users to select files or directories.

  \compat

  The Q3FileDialog class enables a user to traverse their file system in
  order to select one or many files or a directory.

  The easiest way to create a Q3FileDialog is to use the static
  functions. On Windows, these static functions will call the native
  Windows file dialog and on Mac OS X, these static function will call
  the native Mac OS X file dialog.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 0

  In the above example, a modal Q3FileDialog is created using a static
  function. The startup directory is set to "/home". The file filter
  is set to "Images (*.png *.xpm *.jpg)". The parent of the file dialog
  is set to \e this and it is given the identification name - "open file
  dialog". The caption at the top of file dialog is set to "Choose a
  file". If you want to use multiple filters, separate each one with
  \e two semicolons, e.g.
  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 1

  You can create your own Q3FileDialog without using the static
  functions. By calling setMode(), you can set what can be returned by
  the Q3FileDialog.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 2

  In the above example, the mode of the file dialog is set to \l
  AnyFile, meaning that the user can select any file, or even specify a
  file that doesn't exist. This mode is useful for creating a "File Save
  As" file dialog. Use \l ExistingFile if the user must select an
  existing file or \l Directory if only a directory may be selected.
  (See the \l Q3FileDialog::Mode enum for the complete list of modes.)

  You can retrieve the dialog's mode with mode(). Use setFilter() to set
  the dialog's file filter, e.g.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 3

  In the above example, the filter is set to "Images (*.png *.xpm
  *.jpg)", this means that only files with the extension \c png, \c xpm
  or \c jpg will be shown in the Q3FileDialog. You can apply
  several filters by using setFilters() and add additional filters with
  addFilter(). Use setSelectedFilter() to select one of the filters
  you've given as the file dialog's default filter. Whenever the user
  changes the filter the filterSelected() signal is emitted.

  The file dialog has two view modes, Q3FileDialog::List which simply
  lists file and directory names and Q3FileDialog::Detail which
  displays additional information alongside each name, e.g. file size,
  modification date, etc. Set the mode with setViewMode().

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 4

  The last important function you will need to use when creating your
  own file dialog is selectedFile().

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 5

  In the above example, a modal file dialog is created and shown. If
  the user clicked OK, then the file they selected is put in \c
  fileName.

  If you are using the \l ExistingFiles mode then you will need to use
  selectedFiles() which will return the selected files in a QStringList.

  The dialog's working directory can be set with setDir(). The display
  of hidden files is controlled with setShowHiddenFiles(). The dialog
  can be forced to re-read the directory with rereadDir() and re-sort
  the directory with resortDir(). All the files in the current directory
  can be selected with selectAll().

  \section1 Creating and using preview widgets

  There are two kinds of preview widgets that can be used with
  Q3FileDialogs: \e content preview widgets and \e information preview
  widgets. They are created and used in the same way except that the
  function names differ, e.g. setContentsPreview() and setInfoPreview().

  A preview widget is a widget that is placed inside a Q3FileDialog so
  that the user can see either the contents of the file, or information
  about the file.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 6

  In the above snippet, we create a preview widget which inherits from
  QLabel and Q3FilePreview. File preview widgets \e must inherit from
  Q3FilePreview.

  Inside the class we reimplement Q3FilePreview::previewUrl(), this is
  where we determine what happens when a file is selected. In the
  above example we only show a preview of the file if it is a valid
  pixmap. Here's how to make a file dialog use a preview widget:

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 7

  The first line creates an instance of our preview widget. We then
  create our file dialog and call setContentsPreviewEnabled(true),
  this tell the file dialog to preview the contents of the currently
  selected file. We then call setContentsPreview() -- note that we pass
  the same preview widget twice. Finally, before showing the file
  dialog, we call setPreviewMode() setting the mode to \e Contents which
  will show the contents preview of the file that the user has selected.

  If you create another preview widget that is used for displaying
  information about a file, create it in the same way as the contents
  preview widget and call setInfoPreviewEnabled(), and
  setInfoPreview(). Then the user will be able to switch between the
  two preview modes.

  For more information about creating a Q3FilePreview widget see
  \l{Q3FilePreview}.
*/


/*! \enum Q3FileDialog::Mode

  This enum is used to indicate what the user may select in the file
  dialog, i.e. what the dialog will return if the user clicks OK.

  \value AnyFile  The name of a file, whether it exists or not.
  \value ExistingFile  The name of a single existing file.
  \value Directory  The name of a directory. Both files and directories
  are displayed.
  \value DirectoryOnly  The name of a directory. The file dialog will only display directories.
  \value ExistingFiles  The names of zero or more existing files.

  See setMode().
*/

/*!
  \enum Q3FileDialog::ViewMode

  This enum describes the view mode of the file dialog, i.e. what
  information about each file will be displayed.

  \value List  Display file and directory names with icons.
  \value Detail  Display file and directory names with icons plus
  additional information, such as file size and modification date.

  See setViewMode().
*/

/*!
  \enum Q3FileDialog::PreviewMode

  This enum describes the preview mode of the file dialog.

  \value NoPreview  No preview is shown at all.
  \value Contents  Show a preview of the contents of the current file
  using the contents preview widget.
  \value Info  Show information about the current file using the
  info preview widget.

  See setPreviewMode(), setContentsPreview() and setInfoPreview().
*/

/*!
  \fn void Q3FileDialog::detailViewSelectionChanged()
  \internal
*/

/*!
  \fn void Q3FileDialog::listBoxSelectionChanged()
  \internal
*/

extern const char qt3_file_dialog_filter_reg_exp[] = "([a-zA-Z0-9]*)\\(([a-zA-Z0-9_.*? +;#\\[\\]]*)\\)$";

/*!
  Constructs a file dialog called \a name, with the parent, \a parent.
  If \a modal is true then the file dialog is modal; otherwise it is
  modeless.
*/

Q3FileDialog::Q3FileDialog(QWidget *parent, const char *name, bool modal)
    : QDialog(parent, name, modal,
               (modal ?
                (Qt::WStyle_Customize | Qt::WStyle_DialogBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu) : Qt::WindowFlags(0)))
{
    init();
    d->mode = ExistingFile;
    d->types->insertItem(tr("All Files (*)"));
    d->cursorOverride = false;
    emit dirEntered(d->url.dirPath());
    rereadDir();
}


/*!
  Constructs a file dialog called \a name with the parent, \a parent.
  If \a modal is true then the file dialog is modal; otherwise it is
  modeless.

  If \a dirName is specified then it will be used as the dialog's
  working directory, i.e. it will be the directory that is shown when
  the dialog appears. If \a filter is specified it will be used as the
  dialog's file filter.

*/

Q3FileDialog::Q3FileDialog(const QString& dirName, const QString & filter,
                          QWidget *parent, const char *name, bool modal)
    : QDialog(parent, name, modal,
              (modal ? (Qt::WStyle_Customize | Qt::WStyle_DialogBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu)
               : Qt::WindowFlags(0)))
{
    init();
    d->mode = ExistingFile;
    rereadDir();
    Q3UrlOperator u(dirName);
    if (!dirName.isEmpty() && (!u.isLocalFile() || QDir(dirName).exists()))
        setSelection(dirName);
    else if (workingDirectory && !workingDirectory->isEmpty())
        setDir(*workingDirectory);

    if (!filter.isEmpty()) {
        setFilters(filter);
        if (!dirName.isEmpty()) {
            int dotpos = dirName.indexOf(QLatin1Char('.'), 0, Qt::CaseInsensitive);
            if (dotpos != -1) {
                for (int b=0 ; b<d->types->count() ; b++) {
                    if (d->types->text(b).contains(dirName.right(dirName.length() - dotpos))) {
                        d->types->setCurrentItem(b);
                        setFilter(d->types->text(b));
                        return;
                    }
                }
            }
        }
    } else {
        d->types->insertItem(tr("All Files (*)"));
    }
}


/*!
  \internal
  Initializes the file dialog.
*/

void Q3FileDialog::init()
{
    setSizeGripEnabled(true);
    d = new Q3FileDialogPrivate();
    d->mode = AnyFile;
    d->last = 0;
    d->lastEFSelected = 0;
    d->moreFiles = 0;
    d->infoPreview = false;
    d->contentsPreview = false;
    d->hadDotDot = false;
    d->ignoreNextKeyPress = false;
    d->progressDia = 0;
    d->checkForFilter = false;
    d->ignoreNextRefresh = false;
    d->ignoreStop = false;
    d->mimeTypeTimer = new QTimer(this);
    d->cursorOverride = false;
    connect(d->mimeTypeTimer, SIGNAL(timeout()),
             this, SLOT(doMimeTypeLookup()));

    d->url = Q3UrlOperator(toRootIfNotExists( QDir::currentDirPath() ));
    d->oldUrl = d->url;
    d->currListChildren = 0;

    connect(&d->url, SIGNAL(start(Q3NetworkOperation*)),
             this, SLOT(urlStart(Q3NetworkOperation*)));
    connect(&d->url, SIGNAL(finished(Q3NetworkOperation*)),
             this, SLOT(urlFinished(Q3NetworkOperation*)));
    connect(&d->url, SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)),
             this, SLOT(insertEntry(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)));
    connect(&d->url, SIGNAL(removed(Q3NetworkOperation*)),
             this, SLOT(removeEntry(Q3NetworkOperation*)));
    connect(&d->url, SIGNAL(createdDirectory(QUrlInfo,Q3NetworkOperation*)),
             this, SLOT(createdDirectory(QUrlInfo,Q3NetworkOperation*)));
    connect(&d->url, SIGNAL(itemChanged(Q3NetworkOperation*)),
             this, SLOT(itemChanged(Q3NetworkOperation*)));
    connect(&d->url, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)),
             this, SLOT(dataTransferProgress(int,int,Q3NetworkOperation*)));

    nameEdit = new QLineEdit(this, "name/filter editor");
    nameEdit->setMaxLength(255); //_POSIX_MAX_PATH
    connect(nameEdit, SIGNAL(textChanged(QString)),
             this, SLOT(fileNameEditDone()));
    nameEdit->installEventFilter(this);

    d->splitter = new QSplitter(this, "qt_splitter");

    d->stack = new Q3WidgetStack(d->splitter, "files and more files");

    d->splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    files = new Q3FileDialogQFileListView(d->stack, this);
    QFontMetrics fm = fontMetrics();
    files->addColumn(tr("Name"));
    files->addColumn(tr("Size"));
    files->setColumnAlignment(1, Qt::AlignRight);
    files->addColumn(tr("Type"));
    files->addColumn(tr("Date"));
    files->addColumn(tr("Attributes"));
    files->header()->setStretchEnabled(true, 0);

    files->setMinimumSize(50, 25 + 2*fm.lineSpacing());

    connect(files, SIGNAL(selectionChanged()),
            this, SLOT(detailViewSelectionChanged()));
    connect(files, SIGNAL(currentChanged(Q3ListViewItem*)),
            this, SLOT(updateFileNameEdit(Q3ListViewItem*)));
    connect(files, SIGNAL(doubleClicked(Q3ListViewItem*)),
            this, SLOT(selectDirectoryOrFile(Q3ListViewItem*)));
    connect(files, SIGNAL(returnPressed(Q3ListViewItem*)),
            this, SLOT(selectDirectoryOrFile(Q3ListViewItem*)));
    connect(files, SIGNAL(contextMenuRequested(Q3ListViewItem*,QPoint,int)),
            this, SLOT(popupContextMenu(Q3ListViewItem*,QPoint,int)));

    files->installEventFilter(this);
    files->viewport()->installEventFilter(this);

    d->moreFiles = new QFileListBox(d->stack, this);
    d->moreFiles->setRowMode(Q3ListBox::FitToHeight);
    d->moreFiles->setVariableWidth(true);

    connect(d->moreFiles, SIGNAL(selected(Q3ListBoxItem*)),
             this, SLOT(selectDirectoryOrFile(Q3ListBoxItem*)));
    connect(d->moreFiles, SIGNAL(selectionChanged()),
             this, SLOT(listBoxSelectionChanged()));
    connect(d->moreFiles, SIGNAL(highlighted(Q3ListBoxItem*)),
      this, SLOT(updateFileNameEdit(Q3ListBoxItem*)));
    connect(d->moreFiles, SIGNAL(contextMenuRequested(Q3ListBoxItem*,QPoint)),
             this, SLOT(popupContextMenu(Q3ListBoxItem*,QPoint)));

    d->moreFiles->installEventFilter(this);
    d->moreFiles->viewport()->installEventFilter(this);

    okB = new QPushButton(tr("&OK"), this, "OK"); //### Or "Save (see other "OK")
    okB->setDefault(true);
    okB->setEnabled(false);
    connect(okB, SIGNAL(clicked()), this, SLOT(okClicked()));
    cancelB = new QPushButton(tr("Cancel") , this, "Cancel");
    connect(cancelB, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    d->paths = new Q3ComboBox(true, this, "directory history/editor");
    d->paths->setDuplicatesEnabled(false);
    d->paths->setInsertionPolicy(Q3ComboBox::NoInsertion);
    makeVariables();

    QFileInfoList rootDrives = QDir::drives();
    for (int i = 0; i < rootDrives.size(); ++i) {
        QFileInfo fi = rootDrives.at(i);
        d->paths->insertItem(*openFolderIcon, fi.absFilePath());
    }

    if (QDir::homeDirPath().size()) {
        if (!d->paths->listBox()->findItem(QDir::homeDirPath()))
            d->paths->insertItem(*openFolderIcon, QDir::homeDirPath());
    }

    connect(d->paths, SIGNAL(activated(QString)),
             this, SLOT(setDir(QString)));

    d->paths->installEventFilter(this);
    QObjectList ol = d->paths->queryList("QLineEdit");
    if (ol.size())
        ol.at(0)->installEventFilter(this);

    d->geometryDirty = true;
    d->types = new QComboBox(true, this, "file types");
    d->types->setDuplicatesEnabled(false);
    d->types->setEditable(false);
    connect(d->types, SIGNAL(activated(QString)),
             this, SLOT(setFilter(QString)));
    connect(d->types, SIGNAL(activated(QString)),
             this, SIGNAL(filterSelected(QString)));

    d->pathL = new QLabel(d->paths, tr("Look &in:"), this, "qt_looin_lbl");
    d->fileL = new QLabel(nameEdit, tr("File &name:"), this, "qt_filename_lbl");
    d->typeL = new QLabel(d->types, tr("File &type:"), this, "qt_filetype_lbl");

    d->goBack = new QToolButton(this, "go back");
    d->goBack->setEnabled(false);
    d->goBack->setFocusPolicy(Qt::TabFocus);
    connect(d->goBack, SIGNAL(clicked()), this, SLOT(goBack()));
#ifndef QT_NO_TOOLTIP
    QToolTip::add(d->goBack, tr("Back"));
#endif
    d->goBack->setIconSet(*goBackIcon);

    d->cdToParent = new QToolButton(this, "cd to parent");
    d->cdToParent->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_TOOLTIP
    QToolTip::add(d->cdToParent, tr("One directory up"));
#endif
    d->cdToParent->setIconSet(*cdToParentIcon);
    connect(d->cdToParent, SIGNAL(clicked()),
             this, SLOT(cdUpClicked()));

    d->newFolder = new QToolButton(this, "new folder");
    d->newFolder->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_TOOLTIP
    QToolTip::add(d->newFolder, tr("Create New Folder"));
#endif
    d->newFolder->setIconSet(*newFolderIcon);
    connect(d->newFolder, SIGNAL(clicked()),
             this, SLOT(newFolderClicked()));

    d->modeButtons = new Q3ButtonGroup(0, "invisible group");
    connect(d->modeButtons, SIGNAL(destroyed()),
             this, SLOT(modeButtonsDestroyed()));
    d->modeButtons->setExclusive(true);
    connect(d->modeButtons, SIGNAL(clicked(int)),
             d->stack, SLOT(raiseWidget(int)));
    connect(d->modeButtons, SIGNAL(clicked(int)),
             this, SLOT(changeMode(int)));

    d->mcView = new QToolButton(this, "mclistbox view");
    d->mcView->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_TOOLTIP
    QToolTip::add(d->mcView, tr("List View"));
#endif
    d->mcView->setIconSet(*multiColumnListViewIcon);
    d->mcView->setToggleButton(true);
    d->stack->addWidget(d->moreFiles, d->modeButtons->insert(d->mcView));
    d->detailView = new QToolButton(this, "list view");
    d->detailView->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_TOOLTIP
    QToolTip::add(d->detailView, tr("Detail View"));
#endif
    d->detailView->setIconSet(*detailViewIcon);
    d->detailView->setToggleButton(true);
    d->stack->addWidget(files, d->modeButtons->insert(d->detailView));

    d->previewInfo = new QToolButton(this, "preview info view");
    d->previewInfo->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_TOOLTIP
    QToolTip::add(d->previewInfo, tr("Preview File Info"));
#endif
    d->previewInfo->setIconSet(*previewInfoViewIcon);
    d->previewInfo->setToggleButton(true);
    d->modeButtons->insert(d->previewInfo);

    d->previewContents = new QToolButton(this, "preview info view");
#if defined(Q_WS_WIN) && !defined(Q_OS_WINCE)
    if ((qWinVersion() & Qt::WV_NT_based) > Qt::WV_NT)
#else
    if (!qstrcmp(style()->className(), "QWindowsStyle"))
#endif
    {
        d->goBack->setAutoRaise(true);
        d->cdToParent->setAutoRaise(true);
        d->newFolder->setAutoRaise(true);
        d->mcView->setAutoRaise(true);
        d->detailView->setAutoRaise(true);
        d->previewInfo->setAutoRaise(true);
        d->previewContents->setAutoRaise(true);
    }
    d->previewContents->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_TOOLTIP
    QToolTip::add(d->previewContents, tr("Preview File Contents"));
#endif
    d->previewContents->setIconSet(*previewContentsViewIcon);
    d->previewContents->setToggleButton(true);
    d->modeButtons->insert(d->previewContents);

    connect(d->detailView, SIGNAL(clicked()),
             d->moreFiles, SLOT(cancelRename()));
    connect(d->detailView, SIGNAL(clicked()),
             files, SLOT(cancelRename()));
    connect(d->mcView, SIGNAL(clicked()),
             d->moreFiles, SLOT(cancelRename()));
    connect(d->mcView, SIGNAL(clicked()),
             files, SLOT(cancelRename()));

    d->stack->raiseWidget(d->moreFiles);
    d->mcView->setOn(true);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setMargin(6);
    d->leftLayout = new QHBoxLayout(lay, 5);
    d->topLevelLayout = new QVBoxLayout((QWidget*)0, 5);
    lay->addLayout(d->topLevelLayout, 1);

    QHBoxLayout * h;

    d->preview = new Q3WidgetStack(d->splitter, "qt_preview");

    d->infoPreviewWidget = new QWidget(d->preview, "qt_preview_info");
    d->contentsPreviewWidget = new QWidget(d->preview, "qt_preview_contents");
    d->infoPreviewer = d->contentsPreviewer = 0;

    h = new QHBoxLayout(0);
    d->buttonLayout = h;
    d->topLevelLayout->addLayout(h);
    h->addWidget(d->pathL);
    h->addSpacing(8);
    h->addWidget(d->paths);
    h->addSpacing(8);
    if (d->goBack)
        h->addWidget(d->goBack);
    h->addWidget(d->cdToParent);
    h->addSpacing(2);
    h->addWidget(d->newFolder);
    h->addSpacing(4);
    h->addWidget(d->mcView);
    h->addWidget(d->detailView);
    h->addWidget(d->previewInfo);
    h->addWidget(d->previewContents);

    d->topLevelLayout->addWidget(d->splitter);

    h = new QHBoxLayout();
    d->topLevelLayout->addLayout(h);
    h->addWidget(d->fileL);
    h->addWidget(nameEdit);
    h->addSpacing(15);
    h->addWidget(okB);

    h = new QHBoxLayout();
    d->topLevelLayout->addLayout(h);
    h->addWidget(d->typeL);
    h->addWidget(d->types);
    h->addSpacing(15);
    h->addWidget(cancelB);

    d->rightLayout = new QHBoxLayout(lay, 5);
    d->topLevelLayout->setStretchFactor(d->mcView, 1);
    d->topLevelLayout->setStretchFactor(files, 1);

    updateGeometries();

    if (d->goBack) {
        setTabOrder(d->paths, d->goBack);
        setTabOrder(d->goBack, d->cdToParent);
    } else {
        setTabOrder(d->paths, d->cdToParent);
    }
    setTabOrder(d->cdToParent, d->newFolder);
    setTabOrder(d->newFolder, d->mcView);
    setTabOrder(d->mcView, d->detailView);
    setTabOrder(d->detailView, d->moreFiles);
    setTabOrder(d->moreFiles, files);
    setTabOrder(files, nameEdit);
    setTabOrder(nameEdit, d->types);
    setTabOrder(d->types, okB);
    setTabOrder(okB, cancelB);

    d->rw = tr("Read-write");
    d->ro = tr("Read-only");
    d->wo = tr("Write-only");
    d->inaccessible = tr("Inaccessible");

    d->symLinkToFile = tr("Symlink to File");
    d->symLinkToDir = tr("Symlink to Directory");
    d->symLinkToSpecial = tr("Symlink to Special");
    d->file = tr("File");
    d->dir = tr("Dir");
    d->special = tr("Special");

    if (lastWidth == 0) {
        QRect screen = QApplication::desktop()->screenGeometry(pos());
        if (screen.width() < 1024 || screen.height() < 768) {
            resize(qMin(screen.width(), 420), qMin(screen.height(), 236));
        } else {
            QSize s = files->sizeHint();
            s = QSize(s.width() + 300, s.height() + 82);

            if (s.width() * 3 > screen.width() * 2)
                s.setWidth(screen.width() * 2 / 3);

            if (s.height() * 3 > screen.height() * 2)
                s.setHeight(screen.height() * 2 / 3);
            else if (s.height() * 3 < screen.height())
                s.setHeight(screen.height() / 3);

            resize(s);
        }
        updateLastSize(this);
    } else {
        resize(lastWidth, lastHeight);
    }

    if (detailViewMode) {
        d->stack->raiseWidget(files);
        d->mcView->setOn(false);
        d->detailView->setOn(true);
    }

    d->preview->hide();
    nameEdit->setFocus();

    connect(nameEdit, SIGNAL(returnPressed()),
             this, SLOT(fileNameEditReturnPressed()));
}

/*!
  \internal
*/

void Q3FileDialog::fileNameEditReturnPressed()
{
    d->oldUrl = d->url;
    if (!isDirectoryMode(d->mode)) {
        okClicked();
    } else {
        d->currentFileName.clear();
        if (nameEdit->text().isEmpty()) {
            emit fileSelected(selectedFile());
            accept();
        } else {
            QUrlInfo f;
            Q3FileDialogPrivate::File * c
                = (Q3FileDialogPrivate::File *)files->currentItem();
            if (c && files->isSelected(c))
                f = c->info;
            else
                f = QUrlInfo(d->url.info(nameEdit->text().isEmpty() ? QString::fromLatin1(".") : nameEdit->text()));
            if (f.isDir()) {
                setUrl(Q3UrlOperator(d->url,
                                      Q3FileDialogPrivate::encodeFileName(nameEdit->text() + QLatin1Char('/'))));
                d->checkForFilter = true;
                trySetSelection(true, d->url, true);
                d->checkForFilter = false;
            }
        }
        nameEdit->setText(QString());
    }
}

/*!
  \internal
  Update the info and content preview widgets to display \a u.
*/

void Q3FileDialog::updatePreviews(const Q3Url &u)
{
    if (d->infoPreviewer)
        d->infoPreviewer->previewUrl(u);
    if (d->contentsPreviewer)
        d->contentsPreviewer->previewUrl(u);
}

/*!
  \internal
  Changes the preview mode to the mode specified at \a id.
*/

void Q3FileDialog::changeMode(int id)
{
    if (!d->infoPreview && !d->contentsPreview)
        return;

    QAbstractButton*btn = d->modeButtons->find(id);
    if (!btn)
        return;

    if (btn == d->previewContents && !d->contentsPreview)
        return;
    if (btn == d->previewInfo && !d->infoPreview)
        return;

    if (btn != d->previewContents && btn != d->previewInfo) {
        d->preview->hide();
    } else {
        if (files->currentItem())
            updatePreviews(Q3Url(d->url, files->currentItem()->text(0)));
        if (btn == d->previewInfo)
            d->preview->raiseWidget(d->infoPreviewWidget);
        else
            d->preview->raiseWidget(d->contentsPreviewWidget);
        d->preview->show();
    }
}

/*!
  Destroys the file dialog.
*/

Q3FileDialog::~Q3FileDialog()
{
    // since clear might call setContentsPos which would emit
    // a signal and thus cause a recompute of sizes...
    files->blockSignals(true);
    d->moreFiles->blockSignals(true);
    files->clear();
    d->moreFiles->clear();
    d->moreFiles->blockSignals(false);
    files->blockSignals(false);

#ifndef QT_NO_CURSOR
    if (d->cursorOverride)
        QApplication::restoreOverrideCursor();
#endif

    delete d;
    d = 0;
}


/*!
  \property Q3FileDialog::selectedFile

  \brief the name of the selected file

  If a file was selected selectedFile contains the file's name including
  its absolute path; otherwise selectedFile is empty.

  \sa QString::isEmpty(), selectedFiles, selectedFilter
*/

QString Q3FileDialog::selectedFile() const
{
    QString s = d->currentFileName;
    // remove the protocol because we do not want to encode it...
    QString prot = Q3Url(s).protocol();
    if (!prot.isEmpty()) {
        prot += QLatin1Char(':');
        s.remove(0, prot.length());
    }
    Q3Url u(prot + Q3FileDialogPrivate::encodeFileName(s));
    if (u.isLocalFile()) {
        QString s = u.toString();
        if (s.left(5) == QLatin1String("file:"))
            s.remove((uint)0, 5);
        return s;
    }
    return d->currentFileName;
}

/*!
  \property Q3FileDialog::selectedFilter

  \brief the filter which the user has selected in the file dialog

  \sa filterSelected(), selectedFiles, selectedFile
*/

QString Q3FileDialog::selectedFilter() const
{
    return d->types->currentText();
}

/*! \overload

  Sets the current filter selected in the file dialog to the
  \a{n}-th filter in the filter list.

  \sa filterSelected(), selectedFilter(), selectedFiles(), selectedFile()
*/

void Q3FileDialog::setSelectedFilter(int n)
{
    d->types->setCurrentItem(n);
    QString f = d->types->currentText();
    QRegExp r(QString::fromLatin1(qt3_file_dialog_filter_reg_exp));
    int index = r.indexIn(f);
    if (index >= 0)
        f = r.cap(2);
    d->url.setNameFilter(f);
    rereadDir();
}

/*!
  Sets the current filter selected in the file dialog to the first
  one that contains the text \a mask.
*/

void Q3FileDialog::setSelectedFilter(const QString& mask)
{
    int n;

    for (n = 0; n < d->types->count(); n++) {
        if (d->types->text(n).contains(mask, Qt::CaseInsensitive)) {
            d->types->setCurrentItem(n);
            QString f = mask;
            QRegExp r(QString::fromLatin1(qt3_file_dialog_filter_reg_exp));
            int index = r.indexIn(f);
            if (index >= 0)
                f = r.cap(2);
            d->url.setNameFilter(f);
            rereadDir();
            return;
        }
    }
}

/*!
  \property Q3FileDialog::selectedFiles

  \brief the list of selected files

  If one or more files are selected, selectedFiles contains their
  names including their absolute paths. If no files are selected or
  the mode isn't ExistingFiles selectedFiles is an empty list.

  It is more convenient to use selectedFile() if the mode is
  \l ExistingFile, \c Directory or \c DirectoryOnly.

  Note that if you want to iterate over the list, you should
  iterate over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 8

  \sa selectedFile, selectedFilter, QList::isEmpty()
*/

QStringList Q3FileDialog::selectedFiles() const
{
    QStringList lst;

    if (mode() == ExistingFiles) {
        QStringList selectedLst;
        QString selectedFiles = nameEdit->text();
        if (selectedFiles.lastIndexOf(QLatin1Char('\"')) == -1) {
            //probably because Enter was pressed on the nameEdit, so we have one file
            //not in "" but raw
            selectedLst.append(selectedFiles);
        } else {
            selectedFiles.truncate(selectedFiles.lastIndexOf(QLatin1Char('\"')));
            selectedLst = selectedLst.split(QLatin1String("\" "), selectedFiles);
        }
        for (QStringList::Iterator it = selectedLst.begin(); it != selectedLst.end(); ++it) {
            Q3Url u;
            if ((*it)[0] == QLatin1Char('\"')) {
                u = Q3Url(d->url, Q3FileDialogPrivate::encodeFileName((*it).mid(1)));
            } else {
                u = Q3Url(d->url, Q3FileDialogPrivate::encodeFileName((*it)));
            }
            if (u.isLocalFile()) {
                QString s = u.toString();
                if (s.left(5) == QLatin1String("file:"))
                    s.remove((uint)0, 5);
                lst << s;
            } else {
                lst << u.toString();
            }
        }
    }

    return lst;
}

/*!
  Sets the default selection to \a filename. If \a filename is
  absolute, setDir() is also called to set the file dialog's working
  directory to the filename's directory.

  \omit
  Only for external use. Not useful inside Q3FileDialog.
  \endomit
*/

void Q3FileDialog::setSelection(const QString & filename)
{
    d->oldUrl = d->url;
    QString nf = d->url.nameFilter();
    if (Q3Url::isRelativeUrl(filename))
        d->url = Q3UrlOperator(d->url, Q3FileDialogPrivate::encodeFileName(filename));
    else
        d->url = Q3UrlOperator(filename);
    d->url.setNameFilter(nf);
    d->checkForFilter = true;
    bool isDirOk;
    bool isDir = d->url.isDir(&isDirOk);
    if (!isDirOk)
        isDir = d->url.path().right(1) == QString(QLatin1Char('/'));
    if (!isDir) {
        Q3UrlOperator u(d->url);
        d->url.setPath(d->url.dirPath());
        trySetSelection(false, u, true);
        d->ignoreNextRefresh = true;
        nameEdit->selectAll();
        rereadDir();
        emit dirEntered(d->url.dirPath());
    } else {
        if (!d->url.path().isEmpty() &&
             d->url.path().right(1) != QString(QLatin1Char('/'))) {
            QString p = d->url.path();
            p += QLatin1Char('/');
            d->url.setPath(p);
        }
        trySetSelection(true, d->url, false);
        rereadDir();
        emit dirEntered(d->url.dirPath());
        nameEdit->setText(QString::fromLatin1(""));
    }
    d->checkForFilter = false;
}

/*!
  \property Q3FileDialog::dirPath

  \brief the file dialog's working directory

  \sa dir(), setDir()
*/

QString Q3FileDialog::dirPath() const
{
    return d->url.dirPath();
}


/*!

  Sets the filter used in the file dialog to \a newFilter.

  If \a newFilter contains a pair of parentheses containing one or more
  of "anything*something" separated by spaces or by
  semicolons then only the text contained in the parentheses is used as
  the filter. This means that these calls are all equivalent:

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 9

  \sa setFilters()
*/

void Q3FileDialog::setFilter(const QString & newFilter)
{
    if (newFilter.isEmpty())
        return;
    QString f = newFilter;
    QRegExp r(QString::fromLatin1(qt3_file_dialog_filter_reg_exp));
    int index = r.indexIn(f);
    if (index >= 0)
        f = r.cap(2);
    d->url.setNameFilter(f);
    if (d->types->count() == 1)  {
        d->types->clear();
        d->types->insertItem(newFilter);
    } else {
        for (int i = 0; i < d->types->count(); ++i) {
            if (d->types->text(i).left(newFilter.length()) == newFilter ||
                 d->types->text(i).left(f.length()) == f) {
                d->types->setCurrentItem(i);
                break;
            }
        }
    }
    rereadDir();
}


/*! \overload
  Sets the file dialog's working directory to \a pathstr.

  \sa dir()
*/

void Q3FileDialog::setDir(const QString & pathstr)
{
    QString dr = pathstr;
    if (dr.isEmpty())
        return;

#if defined(Q_OS_UNIX)
    if (dr.length() && dr[0] == QLatin1Char('~')) {
        int i = 0;
        while(i < (int)dr.length() && dr[i] != QLatin1Char('/'))
            i++;
        Q3CString user;
        if (i == 1) {
#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)

#  ifndef _POSIX_LOGIN_NAME_MAX
#    define _POSIX_LOGIN_NAME_MAX 9
#  endif

            char name[_POSIX_LOGIN_NAME_MAX];
            if (::getlogin_r(name, _POSIX_LOGIN_NAME_MAX) == 0)
                user = name;
            else
#else
            user = ::getlogin();
            if (user.isEmpty())
#endif
                user = qgetenv("LOGNAME");
        } else
            user = dr.mid(1, i-1).local8Bit();
        dr = dr.mid(i, dr.length());
        struct passwd *pw;
#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(Q_OS_FREEBSD) && !defined(Q_OS_OPENBSD)
        struct passwd mt_pw;
        char buffer[2048];
        if (::getpwnam_r(user, &mt_pw, buffer, 2048, &pw) == 0 && pw == &mt_pw)
#else
        pw = ::getpwnam(user);
        if (pw)
#endif
            dr.prepend(QString::fromLocal8Bit(pw->pw_dir));
    }
#endif

    setUrl(dr);
}

/*!
  Returns the current directory shown in the file dialog.

  The ownership of the QDir pointer is transferred to the caller, so
  it must be deleted by the caller when no longer required.

  \sa setDir()
*/

const QDir *Q3FileDialog::dir() const
{
    if (d->url.isLocalFile())
        return  new QDir(d->url.path());
    else
        return 0;
}

/*!
  Sets the file dialog's working directory to \a dir.
  \sa dir()
*/

void Q3FileDialog::setDir(const QDir &dir)
{
    d->oldUrl = d->url;
    QString nf(d->url.nameFilter());
    d->url = dir.canonicalPath();
    d->url.setNameFilter(nf);
    QUrlInfo i(d->url.info(nameEdit->text().isEmpty()? QString::fromLatin1(".") : nameEdit->text()));
    d->checkForFilter = true;
    trySetSelection(i.isDir(), Q3UrlOperator(d->url, Q3FileDialogPrivate::encodeFileName(nameEdit->text())), false);
    d->checkForFilter = false;
    rereadDir();
    emit dirEntered(d->url.path());
}

/*!
  Sets the file dialog's working directory to the directory specified at \a url.

  \sa url()
*/

void Q3FileDialog::setUrl(const Q3UrlOperator &url)
{
    d->oldUrl = d->url;
    QString nf = d->url.nameFilter();

    QString operatorPath = url.toString(false, false);
    if (Q3Url::isRelativeUrl(operatorPath)) {
        d->url = Q3Url(d->url, operatorPath);
    } else {
        d->url = url;
    }
    d->url.setNameFilter(nf);

    d->checkForFilter = true;
    if (!d->url.isDir()) {
        Q3UrlOperator u = d->url;
        d->url.setPath(d->url.dirPath());
        trySetSelection(false, u, false);
        rereadDir();
        emit dirEntered(d->url.dirPath());
        QString fn = u.fileName();
        nameEdit->setText(fn);
    } else {
        trySetSelection(true, d->url, false);
        rereadDir();
        emit dirEntered(d->url.dirPath());
    }
    d->checkForFilter = false;
}

/*!
  \property Q3FileDialog::showHiddenFiles

  \brief whether hidden files are shown in the file dialog

  The default is false, i.e. don't show hidden files.
*/

void Q3FileDialog::setShowHiddenFiles(bool s)
{
    if (s == bShowHiddenFiles)
        return;

    bShowHiddenFiles = s;
    rereadDir();
}

bool Q3FileDialog::showHiddenFiles() const
{
    return bShowHiddenFiles;
}

/*!
  Rereads the current directory shown in the file dialog.

  The only time you will need to call this function is if the contents of
  the directory change and you wish to refresh the file dialog to reflect
  the change.

  \sa resortDir()
*/

void Q3FileDialog::rereadDir()
{
#ifndef QT_NO_CURSOR
    if (!d->cursorOverride) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        d->cursorOverride = true;
    }
#endif
    d->pendingItems.clear();
    if (d->mimeTypeTimer->isActive())
        d->mimeTypeTimer->stop();
    d->currListChildren = d->url.listChildren();
#ifndef QT_NO_CURSOR
    if (d->cursorOverride) {
        QApplication::restoreOverrideCursor();
        d->cursorOverride = false;
    }
#endif
}


/*!
  \fn void Q3FileDialog::fileHighlighted(const QString& file)

  This signal is emitted when the user highlights the given \a file,
  i.e. makes it the current file.

  \sa fileSelected(), filesSelected()
*/

/*!
  \fn void Q3FileDialog::fileSelected(const QString& file)

  This signal is emitted when the user selects the given \a file.

  \sa filesSelected(), fileHighlighted(), selectedFile()
*/

/*!
  \fn void Q3FileDialog::filesSelected(const QStringList& files)

  This signal is emitted when the user selects the given \a files in \e
  ExistingFiles mode.

  \sa fileSelected(), fileHighlighted(), selectedFiles()
*/

/*!
  \fn void Q3FileDialog::dirEntered(const QString& directory)

  This signal is emitted when the user enters the given \a directory.

  \sa dir()
*/

/*!
  \fn void Q3FileDialog::filterSelected(const QString& filter)

  This signal is emitted when the user selects the given \a filter.

  \sa selectedFilter()
*/

extern bool qt_resolve_symlinks; // defined in q3url.cpp
extern Q_GUI_EXPORT bool qt_use_native_dialogs; //qtgui

/*!
  This is a convenience static function that returns an existing file
  selected by the user. If the user pressed Cancel, it returns a null
  string.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 10

  The function creates a modal file dialog called \a name, with
  parent, \a parent. If a parent is not 0, the dialog will be shown
  centered over the parent.

  The file dialog's working directory will be set to \a startWith. If \a
  startWith includes a file name, the file will be selected. The filter
  is set to \a filter so that only those files which match the filter
  are shown. The filter selected is set to \a selectedFilter. The parameters
  \a startWith, \a selectedFilter and \a filter may be an empty string.

  The dialog's caption is set to \a caption. If \a caption is not
  specified then a default caption will be used.

  Under Windows and Mac OS X, this static function will use the native
  file dialog and not a Q3FileDialog, unless the style of the application
  is set to something other than the native style (Note that on Windows the
  dialog will spin a blocking modal event loop that will not dispatch any
  QTimers and if parent is not 0 then it will position the dialog just under
  the parent's title bar).

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  \sa getOpenFileNames(), getSaveFileName(), getExistingDirectory()
*/

QString Q3FileDialog::getOpenFileName(const QString & startWith,
                                      const QString& filter,
                                      QWidget *parent, const char* name,
                                      const QString& caption,
                                      QString *selectedFilter,
                                      bool resolveSymlinks)
{
    bool save_qt_resolve_symlinks = qt_resolve_symlinks;
    qt_resolve_symlinks = resolveSymlinks;

    QStringList filters;
    if (!filter.isEmpty())
        filters = makeFiltersList(filter);

    makeVariables();
    QString initialSelection;
    //### Problem with the logic here: If a startWith is given and a file
    // with that name exists in D->URL, the box will be opened at D->URL instead of
    // the last directory used ('workingDirectory').
    //
    // hm... isn't that problem exactly the documented behaviour? the
    // documented behaviour sounds meaningful.
    if (!startWith.isEmpty()) {
        Q3UrlOperator u(Q3FileDialogPrivate::encodeFileName(startWith));
        if (u.isLocalFile() && QFileInfo(u.path()).isDir()) {
            *workingDirectory = startWith;
        } else {
            if (u.isLocalFile()) {
                QFileInfo fi(u.dirPath());
                if (fi.exists()) {
                    *workingDirectory = u.dirPath();
                    initialSelection = u.fileName();
                }
            } else {
                *workingDirectory = u.toString();
                initialSelection.clear();
            }
        }
    }

    if (workingDirectory->isNull())
        *workingDirectory = toRootIfNotExists( QDir::currentDirPath() );

#if defined(Q_WS_WIN)
    if (qt_use_native_dialogs && qobject_cast<QWindowsStyle *>(qApp->style()))
        return winGetOpenFileName(initialSelection, filter, workingDirectory,
                                   parent, name, caption, selectedFilter);
#elif defined(Q_WS_MAC)
    if(qt_use_native_dialogs && qobject_cast<QMacStyle *>(qApp->style())) {
        QStringList files = macGetOpenFileNames(filter, startWith.isEmpty() ? 0 : workingDirectory,
                                                parent, name, caption, selectedFilter, false);
        return files.isEmpty() ? QString() : files.first().normalized(QString::NormalizationForm_C);
    }
#endif

    Q3FileDialog *dlg = new Q3FileDialog(*workingDirectory, QString(), parent, name ? name : "qt_filedlg_gofn", true);

    if (!caption.isNull())
        dlg->setWindowTitle(caption);
    else
        dlg->setWindowTitle(Q3FileDialog::tr("Open"));

    dlg->setFilters(filters);
    if (selectedFilter)
        dlg->setFilter(*selectedFilter);
    dlg->setMode(Q3FileDialog::ExistingFile);
    QString result;
    if (!initialSelection.isEmpty())
        dlg->setSelection(initialSelection);
    if (dlg->exec() == QDialog::Accepted) {
        result = dlg->selectedFile();
        *workingDirectory = dlg->d->url;
        if (selectedFilter)
            *selectedFilter = dlg->selectedFilter();
    }
    delete dlg;

    qt_resolve_symlinks = save_qt_resolve_symlinks;

    return result;
}

/*!
  This is a convenience static function that will return a file name
  selected by the user. The file does not have to exist.

  It creates a modal file dialog called \a name, with parent, \a parent.
  If a parent is not 0, the dialog will be shown centered over the
  parent.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 11

  The file dialog's working directory will be set to \a startWith. If \a
  startWith includes a file name, the file will be selected. The filter
  is set to \a filter so that only those files which match the filter
  are shown. The filter selected is set to \a selectedFilter. The parameters
  \a startWith, \a selectedFilter and \a filter may be an empty string.

  The dialog's caption is set to \a caption. If \a caption is not
  specified then a default caption will be used.

  Under Windows and Mac OS X, this static function will use the native
  file dialog and not a Q3FileDialog, unless the style of the application
  is set to something other than the native style. (Note that on Windows the
  dialog will spin a blocking modal event loop that will not dispatch any
  QTimers and if parent is not 0 then it will position the dialog just under
  the parent's title bar.  And on the Mac the filter argument is ignored).

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  \sa getOpenFileName(), getOpenFileNames(), getExistingDirectory()
*/

QString Q3FileDialog::getSaveFileName(const QString & startWith,
                                      const QString& filter,
                                      QWidget *parent, const char* name,
                                      const QString& caption,
                                      QString *selectedFilter,
                                      bool resolveSymlinks)
{
    bool save_qt_resolve_symlinks = qt_resolve_symlinks;
    qt_resolve_symlinks = resolveSymlinks;

    QStringList filters;
    if (!filter.isEmpty())
        filters = makeFiltersList(filter);

    makeVariables();
    QString initialSelection;
    if (!startWith.isEmpty()) {
        Q3UrlOperator u(Q3FileDialogPrivate::encodeFileName(startWith));
        if (u.isLocalFile() && QFileInfo(u.path()).isDir()) {
            *workingDirectory = startWith;
        } else {
            if (u.isLocalFile()) {
                QFileInfo fi(u.dirPath());
                if (fi.exists()) {
                    *workingDirectory = u.dirPath();
                    initialSelection = u.fileName();
                }
            } else {
                *workingDirectory = u.toString();
                initialSelection.clear();
            }
        }
    }

    if (workingDirectory->isNull())
        *workingDirectory = toRootIfNotExists( QDir::currentDirPath() );

#if defined(Q_WS_WIN)
    if (qt_use_native_dialogs && qobject_cast<QWindowsStyle *>(qApp->style()))
        return winGetSaveFileName(initialSelection, filter, workingDirectory,
                                   parent, name, caption, selectedFilter);
#elif defined(Q_WS_MAC)
    if(qt_use_native_dialogs && qobject_cast<QMacStyle *>(qApp->style()))
        return macGetSaveFileName(initialSelection.isNull() ? startWith : initialSelection,
            filter, startWith.isEmpty() ? 0 : workingDirectory, parent, name,
            caption, selectedFilter).normalized(QString::NormalizationForm_C);
#endif

    Q3FileDialog *dlg = new Q3FileDialog(*workingDirectory, QString(), parent, name ? name : "qt_filedlg_gsfn", true);

    if (!caption.isNull())
        dlg->setWindowTitle(caption);
    else
        dlg->setWindowTitle(Q3FileDialog::tr("Save As"));

    QString result;
    dlg->setFilters(filters);
    if (selectedFilter)
        dlg->setFilter(*selectedFilter);
    dlg->setMode(Q3FileDialog::AnyFile);
    if (!initialSelection.isEmpty())
        dlg->setSelection(initialSelection);
    if (dlg->exec() == QDialog::Accepted) {
        result = dlg->selectedFile();
        *workingDirectory = dlg->d->url;
        if (selectedFilter)
            *selectedFilter = dlg->selectedFilter();
    }
    delete dlg;

    qt_resolve_symlinks = save_qt_resolve_symlinks;

    return result;
}

/*!
  \internal
  Activated when the "OK" button is clicked.
*/

void Q3FileDialog::okClicked()
{
    QString fn(nameEdit->text());

#if defined(Q_WS_WIN)
    QFileInfo fi(d->url.path() + fn);
    if (fi.isSymLink()) {
        nameEdit->setText(fi.symLinkTarget());
    }
#endif

    if (fn.contains(QLatin1Char('*'))) {
        addFilter(fn);
        nameEdit->blockSignals(true);
        nameEdit->setText(QString::fromLatin1(""));
        nameEdit->blockSignals(false);
        return;
    }

    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    updateLastSize(this);

    if (isDirectoryMode(d->mode)) {
        QUrlInfo f(d->url.info(nameEdit->text().isEmpty() ? QString::fromLatin1(".") : nameEdit->text()));
        if (f.isDir()) {
            d->currentFileName = d->url;
            if (d->currentFileName.right(1) != QString(QLatin1Char('/')))
                d->currentFileName += QLatin1Char('/');
            if (f.name() != QString(QLatin1Char('.')))
                d->currentFileName += f.name();
            accept();
            return;
        }
        // Since it's not a directory and we clicked ok, we
        // don't really want to do anything else
        return;
    }

    // if we're in multi-selection mode and something is selected,
    // accept it and be done.
    if (mode() == ExistingFiles) {
        if (! nameEdit->text().isEmpty()) {
            QStringList sf = selectedFiles();
            bool isdir = false;
            if (sf.count() == 1) {
                Q3UrlOperator u(d->url, sf[0]);
                bool ok;
                isdir = u.isDir(&ok) && ok;
            }
            if (!isdir) {
                emit filesSelected(sf);
                accept();
                return;
            }
        }
    }

    if (mode() == AnyFile) {
        Q3UrlOperator u(d->url, Q3FileDialogPrivate::encodeFileName(nameEdit->text()));
        if (!u.isDir()) {
            d->currentFileName = u;
            emit fileSelected(selectedFile());
            accept();
            return;
        }
    }

    if (mode() == ExistingFile) {
        if (!Q3FileDialogPrivate::fileExists(d->url, nameEdit->text()))
            return;
    }

    // If selection is valid, return it, else try
    // using selection as a directory to change to.
    if (!d->currentFileName.isNull() && !d->currentFileName.contains(QLatin1Char('*'))) {
        emit fileSelected(selectedFile());
        accept();
    } else {
        QUrlInfo f;
        Q3FileDialogPrivate::File * c
            = (Q3FileDialogPrivate::File *)files->currentItem();
        Q3FileDialogPrivate::MCItem * m
            = (Q3FileDialogPrivate::MCItem *)d->moreFiles->item(d->moreFiles->currentItem());
        if ((c && files->isVisible() && files->hasFocus())
            || (m && d->moreFiles->isVisible())) {
            if (c && files->isVisible())
                f = c->info;
            else
                f = ((Q3FileDialogPrivate::File*)m->i)->info;
        } else {
            f = QUrlInfo(d->url.info(nameEdit->text().isEmpty() ? QString::fromLatin1(".") : nameEdit->text()));
        }
        if (f.isDir()) {
#if defined(Q_WS_WIN)
            if (f.isSymLink())
                setUrl(Q3UrlOperator(d->url, Q3FileDialogPrivate::encodeFileName(fn + QLatin1Char('/'))));
            else
#else
                setUrl(Q3UrlOperator(d->url, Q3FileDialogPrivate::encodeFileName(f.name() + QLatin1Char('/'))));
#endif
            d->checkForFilter = true;
            trySetSelection(true, d->url, true);
            d->checkForFilter = false;
        } else {
            if (!nameEdit->text().contains(QLatin1Char('/')) &&
                 !nameEdit->text().contains(QLatin1String("\\"))
#if defined(Q_OS_WIN32)
                 && nameEdit->text()[1] != QLatin1Char(':')
#endif
                )
                addFilter(nameEdit->text());
            else if (nameEdit->text()[0] == QLatin1Char('/') ||
                      nameEdit->text()[0] == QLatin1Char('\\')
#if defined(Q_OS_WIN32)
                      || nameEdit->text()[1] == QLatin1Char(':')
#endif
                     )
                setDir(nameEdit->text());
            else if (nameEdit->text().left(3) == QLatin1String("../") || nameEdit->text().left(3) == QLatin1String("..\\"))
                setDir(Q3Url(d->url.toString(), Q3FileDialogPrivate::encodeFileName(nameEdit->text())).toString());
        }
        nameEdit->setText(QLatin1String(""));
    }
}

/*!
  \internal
  Activated when the "Filter" button is clicked.
*/

void Q3FileDialog::filterClicked()
{
    // unused
}

/*!
  \internal
  Activated when the "Cancel" button is clicked.
*/

void Q3FileDialog::cancelClicked()
{
    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    updateLastSize(this);
    reject();
}


/*!\reimp
*/

void Q3FileDialog::resizeEvent(QResizeEvent * e)
{
    QDialog::resizeEvent(e);
    updateGeometries();
}

/*
  \internal
  The only correct way to try to set currentFileName
*/
bool Q3FileDialog::trySetSelection(bool isDir, const Q3UrlOperator &u, bool updatelined)
{
    if (!isDir && !u.path().isEmpty() && u.path().right(1) == QString(QLatin1Char('/')))
        isDir = true;
    if (u.fileName().contains(QLatin1Char('*')) && d->checkForFilter) {
        QString fn(u.fileName());
        if (fn.contains(QLatin1Char('*'))) {
            addFilter(fn);
            d->currentFileName.clear();
            d->url.setFileName(QString());
            nameEdit->setText(QString::fromLatin1(""));
            return false;
        }
    }

    if (isDir && d->preview && d->preview->isVisible())
        updatePreviews(u);

    QString old = d->currentFileName;

    if (isDirectoryMode(mode())) {
        if (isDir)
            d->currentFileName = u;
        else
            d->currentFileName.clear();
    } else if (!isDir && mode() == ExistingFiles) {
        d->currentFileName = u;
    } else if (!isDir || (mode() == AnyFile && !isDir)) {
        d->currentFileName = u;
    } else {
        d->currentFileName.clear();
    }
    if (updatelined && !d->currentFileName.isEmpty()) {
        // If the selection is valid, or if its a directory, allow OK.
        if (!d->currentFileName.isNull() || isDir) {
            if (u.fileName() != QLatin1String("..")) {
                QString fn = u.fileName();
                nameEdit->setText(fn);
            } else {
                nameEdit->setText(QLatin1String(""));
            }
        } else
            nameEdit->setText(QString::fromLatin1(""));
    }

    if (!d->currentFileName.isNull() || isDir) {
        okB->setEnabled(true);
    } else if (!isDirectoryMode(d->mode)) {
        okB->setEnabled(false);
    }

    if (d->currentFileName.length() && old != d->currentFileName)
        emit fileHighlighted(selectedFile());

    return !d->currentFileName.isNull();
}


/*!  Make sure the minimum and maximum sizes of everything are sane.
*/

void Q3FileDialog::updateGeometries()
{
    if (!d || !d->geometryDirty)
        return;

    d->geometryDirty = false;

    QSize r, t;

    // we really should use QSize::expandedTo()
#define RM r.setWidth(qMax(r.width(),t.width())); \
r.setHeight(qMax(r.height(),t.height()))

    // labels first
    r = d->pathL->sizeHint();
    t = d->fileL->sizeHint();
    RM;
    t = d->typeL->sizeHint();
    RM;
    d->pathL->setFixedSize(d->pathL->sizeHint());
    d->fileL->setFixedSize(r);
    d->typeL->setFixedSize(r);

    // single-line input areas
    r = d->paths->sizeHint();
    t = nameEdit->sizeHint();
    RM;
    t = d->types->sizeHint();
    RM;
    r.setWidth(t.width() * 2 / 3);
    t.setWidth(QWIDGETSIZE_MAX);
    t.setHeight(r.height());
    d->paths->setMinimumSize(r);
    d->paths->setMaximumSize(t);
    nameEdit->setMinimumSize(r);
    nameEdit->setMaximumSize(t);
    d->types->setMinimumSize(r);
    d->types->setMaximumSize(t);

    // buttons on top row
    r = QSize(0, d->paths->minimumSize().height());
    t = QSize(21, 20);
    RM;
    if (r.height()+1 > r.width())
        r.setWidth(r.height()+1);
    if (d->goBack)
        d->goBack->setFixedSize(r);
    d->cdToParent->setFixedSize(r);
    d->newFolder->setFixedSize(r);
    d->mcView->setFixedSize(r);
    d->detailView->setFixedSize(r);

    QAbstractButton *b = 0;
    if (!d->toolButtons.isEmpty()) {
        for (b = d->toolButtons.first(); b; b = d->toolButtons.next())
            b->setFixedSize(b->sizeHint().width(), r.height());
    }

    if (d->infoPreview) {
        d->previewInfo->show();
        d->previewInfo->setFixedSize(r);
    } else {
        d->previewInfo->hide();
        d->previewInfo->setFixedSize(QSize(0, 0));
    }

    if (d->contentsPreview) {
        d->previewContents->show();
        d->previewContents->setFixedSize(r);
    } else {
        d->previewContents->hide();
        d->previewContents->setFixedSize(QSize(0, 0));
    }

    // open/save, cancel
    r = QSize(75, 20);
    t = okB->sizeHint();
    RM;
    t = cancelB->sizeHint();
    RM;

    okB->setFixedSize(r);
    cancelB->setFixedSize(r);

    d->topLevelLayout->activate();

#undef RM
}


/*! Updates the file name edit box to \a newItem in the file dialog
 when the cursor moves in the listview.
*/

void Q3FileDialog::updateFileNameEdit(Q3ListViewItem * newItem)
{
    if (!newItem)
        return;

    if (mode() == ExistingFiles) {
        detailViewSelectionChanged();
        Q3Url u(d->url, Q3FileDialogPrivate::encodeFileName(((Q3FileDialogPrivate::File*)files->currentItem())->info.name()));
        QFileInfo fi(u.toString(false, false));
        if (!fi.isDir())
            emit fileHighlighted(u.toString(false, false));
    } else if (files->isSelected(newItem)) {
        Q3FileDialogPrivate::File * i = (Q3FileDialogPrivate::File *)newItem;
        if (i && i->i && !i->i->isSelected()) {
            d->moreFiles->blockSignals(true);
            d->moreFiles->setSelected(i->i, true);
            d->moreFiles->blockSignals(false);
        }
        // Encode the filename in case it had any special characters in it
        QString encFile = Q3FileDialogPrivate::encodeFileName(newItem->text(0));
        trySetSelection(i->info.isDir(), Q3UrlOperator(d->url, encFile), true);
    }
}

void Q3FileDialog::detailViewSelectionChanged()
{
    if (d->mode != ExistingFiles)
        return;

    nameEdit->clear();
    QString str;
    Q3ListViewItem * i = files->firstChild();
    d->moreFiles->blockSignals(true);
    while(i) {
        if (d->moreFiles && isVisible()) {
            Q3FileDialogPrivate::File *f = (Q3FileDialogPrivate::File *)i;
            if (f->i && f->i->isSelected() != i->isSelected())
                d->moreFiles->setSelected(f->i, i->isSelected());
        }
        if (i->isSelected() && !((Q3FileDialogPrivate::File *)i)->info.isDir())
            str += QString(QLatin1String("\"%1\" ")).arg(i->text(0));
        i = i->nextSibling();
    }
    d->moreFiles->blockSignals(false);
    nameEdit->setText(str);
    nameEdit->setCursorPosition(str.length());
    okB->setEnabled(true);
    if (d->preview && d->preview->isVisible() && files->currentItem()) {
        Q3Url u = Q3Url(d->url, Q3FileDialogPrivate::encodeFileName(((Q3FileDialogPrivate::File*)files->currentItem())->info.name()));
        updatePreviews(u);
    }
}

void Q3FileDialog::listBoxSelectionChanged()
{
    if (d->mode != ExistingFiles)
        return;

    if (d->ignoreNextRefresh) {
        d->ignoreNextRefresh = false;
        return;
    }

    nameEdit->clear();
    QString str;
    Q3ListBoxItem * i = d->moreFiles->item(0);
    Q3ListBoxItem * j = 0;
    int index = 0;
    files->blockSignals(true);
    while(i) {
        Q3FileDialogPrivate::MCItem *mcitem = (Q3FileDialogPrivate::MCItem *)i;
        if (files && isVisible()) {
            if (mcitem->i->isSelected() != mcitem->isSelected()) {
                files->setSelected(mcitem->i, mcitem->isSelected());

                // What happens here is that we want to emit signal highlighted for
                // newly added items.  But Q3ListBox apparently emits selectionChanged even
                // when a user clicks on the same item twice.  So, basically emulate the behaivor
                // we have in the "Details" view which only emits highlighted the first time we
                // click on the item.  Perhaps at some point we should have a call to
                // updateFileNameEdit(Q3ListViewItem) which also emits fileHighlighted() for
                // ExistingFiles.  For better or for worse, this clones the behaivor of the
                // "Details" view quite well.
                if (mcitem->isSelected() && i != d->lastEFSelected) {
                    Q3Url u(d->url, Q3FileDialogPrivate::encodeFileName(((Q3FileDialogPrivate::File*)(mcitem)->i)->info.name()));
                    d->lastEFSelected = i;
                    emit fileHighlighted(u.toString(false, false));
                }
            }
        }
        if (d->moreFiles->isSelected(i)
             && !((Q3FileDialogPrivate::File*)(mcitem)->i)->info.isDir()) {
            str += QString(QLatin1String("\"%1\" ")).arg(i->text());
            if (j == 0)
                j = i;
        }
        i = d->moreFiles->item(++index);
    }

    files->blockSignals(false);
    nameEdit->setText(str);
    nameEdit->setCursorPosition(str.length());
    okB->setEnabled(true);
    if (d->preview && d->preview->isVisible() && j) {
        Q3Url u = Q3Url(d->url,
                        Q3FileDialogPrivate::encodeFileName(((Q3FileDialogPrivate::File*)((Q3FileDialogPrivate::MCItem*)j)->i)->info.name()));
        updatePreviews(u);
    }
}

/*! \overload */

void Q3FileDialog::updateFileNameEdit(Q3ListBoxItem * newItem)
{
    if (!newItem)
        return;
    Q3FileDialogPrivate::MCItem * i = (Q3FileDialogPrivate::MCItem *)newItem;
    if (i->i) {
        i->i->listView()->setSelected(i->i, i->isSelected());
        updateFileNameEdit(i->i);
    }
}


/*!  Updates the dialog when the file name edit changes. */

void Q3FileDialog::fileNameEditDone()
{
    QUrlInfo f(d->url.info(nameEdit->text().isEmpty() ? QString::fromLatin1(".") : nameEdit->text()));
    if (mode() != Q3FileDialog::ExistingFiles) {
        Q3UrlOperator u(d->url, Q3FileDialogPrivate::encodeFileName(nameEdit->text()));
        trySetSelection(f.isDir(), u, false);
        if (d->preview && d->preview->isVisible())
            updatePreviews(u);
    }
}



/*! This private slot reacts to double-clicks in the list view. The item that
was double-clicked is specified in \a newItem */

void Q3FileDialog::selectDirectoryOrFile(Q3ListViewItem * newItem)
{

    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    updateLastSize(this);

    if (!newItem)
        return;

    if (d->url.isLocalFile()) {
        QFileInfo fi(d->url.path() + newItem->text(0));
#if defined(Q_WS_WIN)
        if (fi.isSymLink()) {
            nameEdit->setText(fi.symLinkTarget());
            okClicked();
            return;
        }
#endif
    }

    Q3FileDialogPrivate::File * i = (Q3FileDialogPrivate::File *)newItem;

    QString oldName = nameEdit->text();
    if (i->info.isDir()) {
        setUrl(Q3UrlOperator(d->url, Q3FileDialogPrivate::encodeFileName(i->info.name()) + QLatin1Char('/')));
        if (isDirectoryMode(mode())) {
            QUrlInfo f (d->url.info(QString::fromLatin1(".")));
            trySetSelection(f.isDir(), d->url, true);
        }
    } else if (newItem->isSelectable() &&
                trySetSelection(i->info.isDir(), Q3UrlOperator(d->url, Q3FileDialogPrivate::encodeFileName(i->info.name())), true)) {
        if (!isDirectoryMode(mode())) {
            if (mode() == ExistingFile) {
                if (Q3FileDialogPrivate::fileExists(d->url, nameEdit->text())) {
                    emit fileSelected(selectedFile());
                    accept();
                }
            } else {
                emit fileSelected(selectedFile());
                accept();
            }
        }
    } else if (isDirectoryMode(d->mode)) {
        d->currentFileName = d->url;
        accept();
    }
    if (!oldName.isEmpty() && !isDirectoryMode(mode()))
        nameEdit->setText(oldName);
}


void Q3FileDialog::selectDirectoryOrFile(Q3ListBoxItem * newItem)
{
    if (!newItem)
        return;

    Q3FileDialogPrivate::MCItem * i = (Q3FileDialogPrivate::MCItem *)newItem;
    if (i->i) {
        i->i->listView()->setSelected(i->i, i->isSelected());
        selectDirectoryOrFile(i->i);
    }
}


void Q3FileDialog::popupContextMenu(Q3ListViewItem *item, const QPoint &p,
                                    int)
{
    if (item) {
        files->setCurrentItem(item);
        files->setSelected(item, true);
    }

    PopupAction action;
    popupContextMenu(item ? item->text(0) : QString(), true, action, p);

    if (action == PA_Open)
        selectDirectoryOrFile(item);
    else if (action == PA_Rename)
        files->startRename(false);
    else if (action == PA_Delete)
        deleteFile(item ? item->text(0) : QString());
    else if (action == PA_Reload)
        rereadDir();
    else if (action == PA_Hidden) {
        bShowHiddenFiles = !bShowHiddenFiles;
        rereadDir();
    } else if (action == PA_SortName) {
        sortFilesBy = (int)QDir::Name;
        sortAscending = true;
        resortDir();
    } else if (action == PA_SortSize) {
        sortFilesBy = (int)QDir::Size;
        sortAscending = true;
        resortDir();
    } else if (action == PA_SortDate) {
        sortFilesBy = (int)QDir::Time;
        sortAscending = true;
        resortDir();
    } else if (action == PA_SortUnsorted) {
        sortFilesBy = (int)QDir::Unsorted;
        sortAscending = true;
        resortDir();
    }

}

void Q3FileDialog::popupContextMenu(Q3ListBoxItem *item, const QPoint & p)
{
    PopupAction action;
    popupContextMenu(item ? item->text() : QString(), false, action, p);

    if (action == PA_Open)
        selectDirectoryOrFile(item);
    else if (action == PA_Rename)
        d->moreFiles->startRename(false);
    else if (action == PA_Delete)
        deleteFile(item->text());
    else if (action == PA_Reload)
        rereadDir();
    else if (action == PA_Hidden) {
        bShowHiddenFiles = !bShowHiddenFiles;
        rereadDir();
    } else if (action == PA_SortName) {
        sortFilesBy = (int)QDir::Name;
        sortAscending = true;
        resortDir();
    } else if (action == PA_SortSize) {
        sortFilesBy = (int)QDir::Size;
        sortAscending = true;
        resortDir();
    } else if (action == PA_SortDate) {
        sortFilesBy = (int)QDir::Time;
        sortAscending = true;
        resortDir();
    } else if (action == PA_SortUnsorted) {
        sortFilesBy = (int)QDir::Unsorted;
        sortAscending = true;
        resortDir();
    }
}

void Q3FileDialog::popupContextMenu(const QString &filename, bool,
                                    PopupAction &action, const QPoint &p)
{
    action = PA_Cancel;

    bool glob = filename.isEmpty();

    Q3PopupMenu m(0, "file dialog context menu");
    m.setCheckable(true);

    if (!glob) {
        QString okt;
        if (QUrlInfo(d->url.info(filename.isEmpty() ? QString::fromLatin1(".") : fileName)).isDir()) {
            okt = tr("&Open");
        } else {
            if (mode() == AnyFile)
                okt = tr("&Save");
            else
                okt = tr("&Open");
        }
        int ok = m.insertItem(okt);

        m.insertSeparator();
        int rename = m.insertItem(tr("&Rename"));
        int del = m.insertItem(tr("&Delete"));

        if (filename.isEmpty() || !QUrlInfo(d->url.info(filename)).isWritable() ||
             filename == QLatin1String("..")) {
            if (filename.isEmpty() || !QUrlInfo(d->url.info(filename)).isReadable())
                m.setItemEnabled(ok, false);
            m.setItemEnabled(rename, false);
            m.setItemEnabled(del, false);
        }

        m.move(p);
        int res = m.exec(QCursor::pos(), -1);

        if (res == ok)
            action = PA_Open;
        else if (res == rename)
            action = PA_Rename;
        else if (res == del)
            action = PA_Delete;
    } else {
        int reload = m.insertItem(tr("R&eload"));

        Q3PopupMenu m2(0, "sort menu");

        int sname = m2.insertItem(tr("Sort by &Name"));
        //int stype = m2.insertItem(tr("Sort by &Type"));
        int ssize = m2.insertItem(tr("Sort by &Size"));
        int sdate = m2.insertItem(tr("Sort by &Date"));
        m2.insertSeparator();
        int sunsorted = m2.insertItem(tr("&Unsorted"));

        //m2.setItemEnabled(stype, false);

        if (sortFilesBy == (int)QDir::Name)
            m2.setItemChecked(sname, true);
        else if (sortFilesBy == (int)QDir::Size)
            m2.setItemChecked(ssize, true);
//        else if (sortFilesBy == 0x16)
//            m2.setItemChecked(stype, true);
        else if (sortFilesBy == (int)QDir::Time)
            m2.setItemChecked(sdate, true);
        else if (sortFilesBy == (int)QDir::Unsorted)
            m2.setItemChecked(sunsorted, true);

        m.insertItem(tr("Sort"), &m2);

        m.insertSeparator();

        int hidden = m.insertItem(tr("Show &hidden files"));
        m.setItemChecked(hidden, bShowHiddenFiles);

        m.move(p);
        int res = m.exec(QCursor::pos(), -1);

        if (res == reload)
            action = PA_Reload;
        else if (res == hidden)
            action = PA_Hidden;
        else if (res == sname)
            action = PA_SortName;
//        else if (res == stype)
//            action = PA_SortType;
        else if (res == sdate)
            action = PA_SortDate;
        else if (res == ssize)
            action = PA_SortSize;
        else if (res == sunsorted)
            action = PA_SortUnsorted;
    }

}

void Q3FileDialog::deleteFile(const QString &filename)
{
    if (filename.isEmpty())
        return;

    QString encoded = Q3FileDialogPrivate::encodeFileName(filename);
    QUrlInfo fi(d->url.info(encoded.isEmpty() ? QString::fromLatin1(".") : encoded));
    QString t = tr("the file");
    if (fi.isDir())
        t = tr("the directory");
    if (fi.isSymLink())
        t = tr("the symlink");

    if (QMessageBox::warning(this,
                               tr("Delete %1").arg(t),
                               tr("<qt>Are you sure you wish to delete %1 \"%2\"?</qt>")
                               .arg(t).arg(filename),
                               tr("&Yes"), tr("&No"), QString(), 1) == 0)
        d->url.remove(Q3FileDialogPrivate::encodeFileName(filename));

}

void Q3FileDialog::fileSelected(int )
{
    // unused
}

void Q3FileDialog::fileHighlighted(int)
{
    // unused
}

void Q3FileDialog::dirSelected(int)
{
    // unused
}

void Q3FileDialog::pathSelected(int)
{
    // unused
}


void Q3FileDialog::cdUpClicked()
{
    QString oldName = nameEdit->text();
    setUrl(Q3UrlOperator(d->url, QLatin1String("..")));
    if (!oldName.isEmpty())
        nameEdit->setText(oldName);
}

void Q3FileDialog::newFolderClicked()
{
    QString foldername(tr("New Folder 1"));
    int i = 0;
    QStringList lst;
    Q3ListViewItemIterator it(files);
    for (; it.current(); ++it)
        if (it.current()->text(0).contains(tr("New Folder")))
            lst.append(it.current()->text(0));

    if (!lst.count() == 0)
        while (lst.contains(foldername))
            foldername = tr("New Folder %1").arg(++i);

    d->url.mkdir(foldername);
}

void Q3FileDialog::createdDirectory(const QUrlInfo &info, Q3NetworkOperation *)
{
    resortDir();
    if (d->moreFiles->isVisible()) {
        for (uint i = 0; i < d->moreFiles->count(); ++i) {
            if (d->moreFiles->text(i) == info.name()) {
                d->moreFiles->setCurrentItem(i);
                d->moreFiles->startRename(false);
                break;
            }
        }
    } else {
        Q3ListViewItem *item = files->firstChild();
        while (item) {
            if (item->text(0) == info.name()) {
                files->setSelected(item, true);
                files->setCurrentItem(item);
                files->startRename(false);
                break;
            }
            item = item->nextSibling();
        }
    }
}


/*!
  This is a convenience static function that will return an existing directory
  selected by the user.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 12

  This function creates a modal file dialog called \a name, with
  parent, \a parent. If parent is not 0, the dialog will be shown
  centered over the parent.

  The dialog's working directory is set to \a dir, and the caption is
  set to \a caption. Either of these may be an empty string in which case
  the current directory and a default caption will be used respectively.

  If \a dirOnly is true, then only directories will be shown in
  the file dialog; otherwise both directories and files will be shown.

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  Note that on Windows the dialog will spin a blocking modal event loop
  that will not dispatch any QTimers and if parent is not 0 then it will
  position the dialog just under the parent's title bar.

  \sa getOpenFileName(), getOpenFileNames(), getSaveFileName()
*/

QString Q3FileDialog::getExistingDirectory(const QString & dir,
                                           QWidget *parent,
                                           const char* name,
                                           const QString& caption,
                                           bool dirOnly,
                                           bool resolveSymlinks)
{
    bool save_qt_resolve_symlinks = qt_resolve_symlinks;
    qt_resolve_symlinks = resolveSymlinks;

    makeVariables();
    QString wd;
    if (workingDirectory)
        wd = *workingDirectory;

#if defined(Q_WS_WIN)
    QString initialDir;
    if (!dir.isEmpty()) {
        Q3UrlOperator u(dir);
        if (QFileInfo(u.path()).isDir())
            initialDir = dir;
    } else
        initialDir.clear();
    if (qt_use_native_dialogs && qobject_cast<QWindowsStyle *>(qApp->style()) && dirOnly)
        return winGetExistingDirectory(initialDir, parent, name, caption);
#endif
#if defined(Q_WS_MAC)
    if(qt_use_native_dialogs && qobject_cast<QMacStyle *>(qApp->style()))
        return macGetOpenFileNames(QLatin1String(""), 0, parent, name, caption,
        0, false, true).first().normalized(QString::NormalizationForm_C);
#endif

    Q3FileDialog *dlg = new Q3FileDialog(parent, name ? name : "qt_filedlg_ged", true);

    if (!caption.isNull())
        dlg->setWindowTitle(caption);
    else
        dlg->setWindowTitle(Q3FileDialog::tr("Find Directory"));

    dlg->setMode(dirOnly ? DirectoryOnly : Directory);

    dlg->d->types->clear();
    dlg->d->types->insertItem(Q3FileDialog::tr("Directories"));
    dlg->d->types->setEnabled(false);

    QString dir_(dir);
    dir_ = dir_.simplified();
    if (dir_.isEmpty() && !wd.isEmpty())
        dir_ = wd;
    Q3UrlOperator u(dir_);
    if (u.isLocalFile()) {
        if (!dir_.isEmpty()) {
            QFileInfo f(u.path());
        if (f.exists())
        if (f.isDir()) {
                dlg->setDir(dir_);
                wd = dir_;
            }
        } else if (!wd.isEmpty()) {
            Q3Url tempUrl(wd);
            QFileInfo f(tempUrl.path());
            if (f.isDir()) {
                dlg->setDir(wd);
            }
        } else {
            QString theDir = dir_;
            if (theDir.isEmpty()) {
                theDir = toRootIfNotExists( QDir::currentDirPath() );
            } if (!theDir.isEmpty()) {
                Q3Url tempUrl(theDir);
                QFileInfo f(tempUrl.path());
                if (f.isDir()) {
                    wd = theDir;
                    dlg->setDir(theDir);
                }
            }
        }
    } else {
        dlg->setUrl(dir_);
    }

    QString result;
    dlg->setSelection(dlg->d->url.toString());

    if (dlg->exec() == QDialog::Accepted) {
        result = dlg->selectedFile();
        wd = result;
    }
    delete dlg;

    if (!result.isEmpty() && result.right(1) != QString(QLatin1Char('/')))
        result += QLatin1Char('/');

    qt_resolve_symlinks = save_qt_resolve_symlinks;

    return result;
}


/*!
  \property Q3FileDialog::mode
  \brief the file dialog's mode

  The default mode is \l ExistingFile.
*/

void Q3FileDialog::setMode(Mode newMode)
{
    if (d->mode != newMode) {
        d->mode = newMode;
        QString sel = d->currentFileName;
        int maxnamelen = 255; // _POSIX_MAX_PATH
        if (isDirectoryMode(newMode)) {
            files->setSelectionMode(Q3ListView::Single);
            d->moreFiles->setSelectionMode(Q3ListBox::Single);
            if (sel.isNull())
                sel = QString::fromLatin1(".");
            d->types->setEnabled(false);
        } else if (newMode == ExistingFiles) {
            maxnamelen = INT_MAX;
            files->setSelectionMode(Q3ListView::Extended);
            d->moreFiles->setSelectionMode(Q3ListBox::Extended);
            d->types->setEnabled(true);
        } else {
            files->setSelectionMode(Q3ListView::Single);
            d->moreFiles->setSelectionMode(Q3ListBox::Single);
            d->types->setEnabled(true);
        }
        nameEdit->setMaxLength(maxnamelen);
        rereadDir();
        QUrlInfo f(d->url.info(QString(QLatin1Char('.'))));
        trySetSelection(f.isDir(), d->url, false);
    }

    QString okt;
    bool changeFilters = false;
    if (mode() == AnyFile) {
        okt = tr("&Save");
        d->fileL->setText(tr("File &name:"));
        if (d->types->count() == 1) {
            d->types->setCurrentItem(0);
            if (d->types->currentText() == QLatin1String("Directories")) {
                changeFilters = true;
            }
        }
    }
    else if (mode() == Directory || mode() == DirectoryOnly) {
        okt = tr("&OK");
        d->fileL->setText(tr("Directory:"));
        d->types->clear();
        d->types->insertItem(tr("Directories"));
    }
    else {
        okt = tr("&Open");
        d->fileL->setText(tr("File &name:"));
        if (d->types->count() == 1) {
            d->types->setCurrentItem(0);
            if (d->types->currentText() == QLatin1String("Directories")) {
                changeFilters = true;
            }
        }
    }

    if (changeFilters) {
        d->types->clear();
        d->types->insertItem(tr("All Files (*)"));
    }

    okB->setText(okt);
}

Q3FileDialog::Mode Q3FileDialog::mode() const
{
    return d->mode;
}

/*! \reimp
*/

void Q3FileDialog::done(int i)
{
    if (i == QDialog::Accepted && (d->mode == ExistingFile || d->mode == ExistingFiles)) {
        QStringList selection = selectedFiles();
        for (int f = 0; f < selection.count(); f++) {
            QString file = selection[f];
            if (file.isNull())
                continue;
            if (d->url.isLocalFile() && !QFile::exists(file)) {
                QMessageBox::information(this, tr("Error"),
                                          tr("%1\nFile not found.\nCheck path and filename.").arg(file));
                return;
            }
        }
    }
    QDialog::done(i);
}

/*!
  \property Q3FileDialog::viewMode

  \brief the file dialog's view mode

  If you set the view mode to be \e Detail (the default), then you
  will see the file's details, such as the size of the file and the
  date the file was last modified in addition to the file's name.

  If you set the view mode to be \e List, then you will just
  see a list of the files and folders.

  See \l Q3FileDialog::ViewMode
*/


Q3FileDialog::ViewMode Q3FileDialog::viewMode() const
{
    if (detailViewMode)
        return Detail;
    else
        return List;
}

void Q3FileDialog::setViewMode(ViewMode m)
{
    if (m == Detail) {
        detailViewMode = true;
        d->stack->raiseWidget(files);
        d->detailView->setOn(true);
        d->mcView->setOn(false);
    } else if (m == List) {
        detailViewMode = false;
        d->stack->raiseWidget(d->moreFiles);
        d->detailView->setOn(false);
        d->mcView->setOn(true);
    }
}


/*!
  \property Q3FileDialog::previewMode

  \brief the preview mode for the file dialog

  If you set the mode to be a mode other than \e NoPreview, you must
  use setInfoPreview() or setContentsPreview() to set the dialog's
  preview widget to your preview widget and enable the preview
  widget(s) with setInfoPreviewEnabled() or
  setContentsPreviewEnabled().

  \sa infoPreview, contentsPreview, viewMode
*/

void Q3FileDialog::setPreviewMode(PreviewMode m)
{
    if (m == NoPreview) {
        d->previewInfo->setOn(false);
        d->previewContents->setOn(false);
    } else if (m == Info && d->infoPreview) {
        d->previewInfo->setOn(true);
        d->previewContents->setOn(false);
        changeMode(d->modeButtons->id(d->previewInfo));
    } else if (m == Contents && d->contentsPreview) {
        d->previewInfo->setOn(false);
        d->previewContents->setOn(true);
        changeMode(d->modeButtons->id(d->previewContents));
    }
}
Q3FileDialog::PreviewMode Q3FileDialog::previewMode() const
{
    if (d->infoPreview && d->infoPreviewWidget->isVisible())
        return Info;
    else if (d->contentsPreview && d->contentsPreviewWidget->isVisible())
        return Contents;

    return NoPreview;
}


/*!
  Adds the specified widgets to the bottom of the file dialog. The
  label \a l is placed underneath the "file name" and the "file types"
  labels. The widget \a w is placed underneath the file types combobox.
  The button \a b is placed underneath the Cancel push button.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 13

  If you don't want to have one of the widgets added, pass 0 in that
  widget's position.

  Every time you call this function, a new row of widgets will be added
  to the bottom of the file dialog.

  \sa addToolButton(), addLeftWidget(), addRightWidget()
*/

void Q3FileDialog::addWidgets(QLabel * l, QWidget * w, QPushButton * b)
{
    if (!l && !w && !b)
        return;

    d->geometryDirty = true;

    QHBoxLayout *lay = new QHBoxLayout();
    d->extraWidgetsLayouts.append(lay);
    d->topLevelLayout->addLayout(lay);

    if (!l)
        l = new QLabel(this, "qt_intern_lbl");
    d->extraLabels.append(l);
    lay->addWidget(l);

    if (!w)
        w = new QWidget(this, "qt_intern_widget");
    d->extraWidgets.append(w);
    lay->addWidget(w);
    lay->addSpacing(15);

    if (b) {
        d->extraButtons.append(b);
        lay->addWidget(b);
    } else {
        QWidget *wid = new QWidget(this, "qt_extrabuttons_widget");
        d->extraButtons.append(wid);
        lay->addWidget(wid);
    }

    updateGeometries();
}

/*!
  Adds the tool button \a b to the row of tool buttons at the top of the
  file dialog. The button is appended to the right of
  this row. If \a separator is true, a small space is inserted between the
  last button of the row and the new button \a b.

  \sa addWidgets(), addLeftWidget(), addRightWidget()
*/

void Q3FileDialog::addToolButton(QAbstractButton *b, bool separator)
{
    if (!b || !d->buttonLayout)
        return;

    d->geometryDirty = true;

    d->toolButtons.append(b);
    if (separator)
        d->buttonLayout->addSpacing(8);
    d->buttonLayout->addWidget(b);

    updateGeometries();
}

/*!
  Adds the widget \a w to the left-hand side of the file dialog.

  \sa addRightWidget(), addWidgets(), addToolButton()
*/

void Q3FileDialog::addLeftWidget(QWidget *w)
{
    if (!w)
        return;
    d->geometryDirty = true;

    d->leftLayout->addWidget(w);
    d->leftLayout->addSpacing(5);

    updateGeometries();
}

/*!
  Adds the widget \a w to the right-hand side of the file dialog.

  \sa addLeftWidget(), addWidgets(), addToolButton()
*/

void Q3FileDialog::addRightWidget(QWidget *w)
{
    if (!w)
        return;
    d->geometryDirty = true;

    d->rightLayout->addSpacing(5);
    d->rightLayout->addWidget(w);

    updateGeometries();
}

/*! \reimp */

void Q3FileDialog::keyPressEvent(QKeyEvent * ke)
{
    if (!d->ignoreNextKeyPress &&
         ke && (ke->key() == Qt::Key_Enter ||
                 ke->key() == Qt::Key_Return)) {
        ke->ignore();
        if (d->paths->hasFocus()) {
            ke->accept();
            if (d->url == Q3Url(d->paths->currentText()))
                nameEdit->setFocus();
        } else if (d->types->hasFocus()) {
            ke->accept();
            // ### is there a suitable condition for this?  only valid
            // wildcards?
            nameEdit->setFocus();
        } else if (nameEdit->hasFocus()) {
            if (d->currentFileName.isNull()) {
                // maybe change directory
                QUrlInfo i(d->url.info(nameEdit->text().isEmpty() ? QString::fromLatin1(".") :nameEdit->text()));
                if (i.isDir()) {
                    nameEdit->setText(QString::fromLatin1(""));
                    setDir(Q3UrlOperator(d->url, Q3FileDialogPrivate::encodeFileName(i.name())));
                }
                ke->accept();
            } else if (mode() == ExistingFiles) {
                QUrlInfo i(d->url.info(nameEdit->text().isEmpty() ? QString::fromLatin1(".") : nameEdit->text()));
                if (i.isFile()) {
                    Q3ListViewItem * i = files->firstChild();
                    while (i && nameEdit->text() != i->text(0))
                        i = i->nextSibling();
                    if (i)
                        files->setSelected(i, true);
                    else
                        ke->accept(); // strangely, means to ignore that event
                }
            }
        } else if (files->hasFocus() || d->moreFiles->hasFocus()) {
            ke->accept();
        }
    } else if (ke->key() == Qt::Key_Escape) {
        ke->ignore();
    }

    d->ignoreNextKeyPress = false;

    if (!ke->isAccepted()) {
        QDialog::keyPressEvent(ke);
    }
}


/*! \class Q3FileIconProvider qfiledialog.h

  \brief The Q3FileIconProvider class provides icons for Q3FileDialog to
  use.

  \compat

  By default Q3FileIconProvider is not used, but any application or
  library can subclass it, reimplement pixmap() to return a suitable
  icon, and make all Q3FileDialog objects use it by calling the static
  function Q3FileDialog::setIconProvider().

  It is advisable to make all the icons that Q3FileIconProvider returns be
  the same size or at least the same width. This makes the list view
  look much better.

  \sa Q3FileDialog
*/


/*! Constructs an empty file icon provider called \a name, with the
  parent \a parent.
*/

Q3FileIconProvider::Q3FileIconProvider(QObject * parent, const char* name)
    : QObject(parent, name)
{
    // nothing necessary
}


/*!
  Returns a pointer to a pixmap that should be used to
  signify the file with the information \a info.

  If pixmap() returns 0, Q3FileDialog draws the default pixmap.

  The default implementation returns particular icons for files, directories,
  link-files and link-directories. It returns a blank "icon" for other types.

  If you return a pixmap here, it should measure 16x16 pixels.
*/

const QPixmap * Q3FileIconProvider::pixmap(const QFileInfo & info)
{
    if (info.isSymLink()) {
        if (info.isFile())
            return symLinkFileIcon;
        else
            return symLinkDirIcon;
    } else if (info.isDir()) {
        return closedFolderIcon;
    } else if (info.isFile()) {
        return fileIcon;
    } else {
        return fifteenTransparentPixels;
    }
}

/*!
  Sets the Q3FileIconProvider used by the file dialog to \a provider.

  The default is that there is no Q3FileIconProvider and Q3FileDialog
  just draws a folder icon next to each directory and nothing next
  to files.

  \sa Q3FileIconProvider, iconProvider()
*/

void Q3FileDialog::setIconProvider(Q3FileIconProvider * provider)
{
    fileIconProvider = provider;
}


/*!
  Returns a pointer to the icon provider currently set on the file dialog.
  By default there is no icon provider, and this function returns 0.

  \sa setIconProvider(), Q3FileIconProvider
*/

Q3FileIconProvider * Q3FileDialog::iconProvider()
{
    return fileIconProvider;
}


#if defined(Q_WS_WIN)

// ### FIXME: this code is duplicated in qdns.cpp
static QString getWindowsRegString(HKEY key, const QString &subKey)
{
    QString s;
    QT_WA({
        char buf[1024];
        DWORD bsz = sizeof(buf);
        int r = RegQueryValueEx(key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)buf, &bsz);
        if (r == ERROR_SUCCESS) {
            s = QString::fromUcs2((unsigned short *)buf);
        } else if (r == ERROR_MORE_DATA) {
            char *ptr = new char[bsz+1];
            r = RegQueryValueEx(key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)ptr, &bsz);
            if (r == ERROR_SUCCESS)
                s = QLatin1String(ptr);
            delete [] ptr;
        }
    } , {
        char buf[512];
        DWORD bsz = sizeof(buf);
        int r = RegQueryValueExA(key, subKey.local8Bit(), 0, 0, (LPBYTE)buf, &bsz);
        if (r == ERROR_SUCCESS) {
            s = QLatin1String(buf);
        } else if (r == ERROR_MORE_DATA) {
            char *ptr = new char[bsz+1];
            r = RegQueryValueExA(key, subKey.local8Bit(), 0, 0, (LPBYTE)ptr, &bsz);
            if (r == ERROR_SUCCESS)
                s = QLatin1String(ptr);
            delete [] ptr;
        }
    });
    return s;
}

QPixmap fromHICON(HICON hIcon)
{
    ICONINFO icoInfo;
    if (GetIconInfo(hIcon, &icoInfo) && icoInfo.hbmColor) {
        return QPixmap::fromWinHBITMAP(icoInfo.hbmColor);
    }
    return QPixmap();
}

QWindowsIconProvider::QWindowsIconProvider(QObject *parent, const char *name)
    : Q3FileIconProvider(parent, name)
{
    pixw = GetSystemMetrics(SM_CXSMICON);
    pixh = GetSystemMetrics(SM_CYSMICON);

    HKEY k;
    HICON si;
    int r;
    QString s;
    UINT res = 0;

    // ---------- get default folder pixmap
    const wchar_t iconFolder[] = L"folder\\DefaultIcon"; // workaround for Borland
    QT_WA({
        r = RegOpenKeyEx(HKEY_CLASSES_ROOT,
                           iconFolder,
                           0, KEY_READ, &k);
    } , {
        r = RegOpenKeyExA(HKEY_CLASSES_ROOT,
                           "folder\\DefaultIcon",
                           0, KEY_READ, &k);
    });
    resolveLibs();
    if (r == ERROR_SUCCESS) {
        s = getWindowsRegString(k, QString());
        RegCloseKey(k);

        QStringList lst = QStringList::split(QLatin1String(","), s);

        if (lst.count() >= 2) { // don't just assume that lst has two entries
#ifndef Q_OS_WINCE
            QT_WA({
                res = ptrExtractIconEx((TCHAR*)lst[0].simplifyWhiteSpace().ucs2(),
                                       lst[1].simplifyWhiteSpace().toInt(),
                                       0, &si, 1);
            } , {
                res = ExtractIconExA(lst[0].simplifyWhiteSpace().local8Bit(),
                                     lst[1].simplifyWhiteSpace().toInt(),
                                     0, &si, 1);
            });
#else
            res = (UINT)ExtractIconEx((TCHAR*)lst[0].simplifyWhiteSpace().ucs2(),
                                        lst[1].simplifyWhiteSpace().toInt(),
                                        0, &si, 1);
#endif
        }

        if (res) {
            defaultFolder = fromHICON(si);
            defaultFolder.setMask(defaultFolder.createHeuristicMask());
            *closedFolderIcon = defaultFolder;
            DestroyIcon(si);
        } else {
            defaultFolder = *closedFolderIcon;
        }
    } else {
        RegCloseKey(k);
    }

    //------------------------------- get default file pixmap
#ifndef Q_OS_WINCE
    QT_WA({
        res = ptrExtractIconEx(L"shell32.dll",
                                 0, 0, &si, 1);
    } , {
        res = ExtractIconExA("shell32.dll",
                                 0, 0, &si, 1);
    });
#else
        res = (UINT)ExtractIconEx(L"shell32.dll",
                                    0, 0, &si, 1);
#endif

    if (res) {
        defaultFile  = fromHICON(si);
        defaultFile.setMask(defaultFile.createHeuristicMask());
        *fileIcon = defaultFile;
        DestroyIcon(si);
    } else {
        defaultFile = *fileIcon;
    }

    //------------------------------- get default exe pixmap
#ifndef Q_OS_WINCE
    QT_WA({
        res = ptrExtractIconEx(L"shell32.dll",
                              2, 0, &si, 1);
    } , {
        res = ExtractIconExA("shell32.dll",
                          2, 0, &si, 1);
    });
#else
        res = (UINT)ExtractIconEx(L"ceshell.dll",
                                    10, 0, &si, 1);
#endif

    if (res) {
        defaultExe = fromHICON(si);
        defaultExe.setMask(defaultExe.createHeuristicMask());
        DestroyIcon(si);
    } else {
        defaultExe = *fileIcon;
    }
}

QWindowsIconProvider::~QWindowsIconProvider()
{
    if (this == fileIconProvider)
        fileIconProvider = 0;
}

const QPixmap * QWindowsIconProvider::pixmap(const QFileInfo &fi)
{
    if (fi.isSymLink()) {
        QString real = fi.symLinkTarget();
        if (!real.isEmpty())
            return pixmap(QFileInfo(real));
    }

    QString ext = fi.extension(false).upper();
    QString key = ext;
    ext.prepend(QLatin1String("."));
    QMap< QString, QPixmap >::Iterator it;

    if (fi.isDir()) {
        return &defaultFolder;
    } else if (ext.toLower() != QLatin1String(".exe")) {
        it = cache.find(key);
        if (it != cache.end())
            return &(*it);

        HKEY k, k2;
        int r;
        QT_WA({
            r = RegOpenKeyEx(HKEY_CLASSES_ROOT, (TCHAR*)ext.ucs2(),
                              0, KEY_READ, &k);
        } , {
            r = RegOpenKeyExA(HKEY_CLASSES_ROOT, ext.local8Bit(),
                               0, KEY_READ, &k);
        });
        QString s;
        if (r == ERROR_SUCCESS) {
            s = getWindowsRegString(k, QString());
        } else {
            cache[key] = defaultFile;
            RegCloseKey(k);
            return &defaultFile;
        }
        RegCloseKey(k);

        QT_WA({
            r = RegOpenKeyEx(HKEY_CLASSES_ROOT, (TCHAR*)QString(s + QLatin1String("\\DefaultIcon")).ucs2(),
                               0, KEY_READ, &k2);
        } , {
            r = RegOpenKeyExA(HKEY_CLASSES_ROOT, QString(s + QLatin1String("\\DefaultIcon")).local8Bit() ,
                               0, KEY_READ, &k2);
        });
        if (r == ERROR_SUCCESS) {
            s = getWindowsRegString(k2, QString());
        } else {
            cache[key] = defaultFile;
            RegCloseKey(k2);
            return &defaultFile;
        }
        RegCloseKey(k2);

        if (s.isEmpty())
            return &defaultFile;

        QStringList lst = QStringList::split(QLatin1String(","), s);

        HICON si;
        UINT res = 0;
        if (lst.count() >= 2) { // don't just assume that lst has two entries
            QString filepath = lst[0].stripWhiteSpace();
            if (!filepath.isEmpty()) {
                if (filepath.find(QLatin1String("%1")) != -1) {
                    filepath = filepath.arg(fi.filePath());
                    if (ext.toLower() == QLatin1String(".dll")) {
                        pix = defaultFile;
                        return &pix;
                    }
                }
                if (filepath[0] == QLatin1Char('"') && filepath[(int)filepath.length()-1] == QLatin1Char('"'))
                    filepath = filepath.mid(1, filepath.length()-2);

                resolveLibs();
#ifndef Q_OS_WINCE
                QT_WA({
                    res = ptrExtractIconEx((TCHAR*)filepath.ucs2(), lst[1].stripWhiteSpace().toInt(),
                        0, &si, 1);
                } , {
                    res = ExtractIconExA(filepath.local8Bit(), lst[1].stripWhiteSpace().toInt(),
                        0, &si, 1);
                });
#else
                res = (UINT)ExtractIconEx((TCHAR*)filepath.ucs2(), lst[1].stripWhiteSpace().toInt(),
                    0, &si, 1);
#endif
            }
        }
        if (res) {
            pix = fromHICON(si);
            pix.setMask(pix.createHeuristicMask());
            DestroyIcon(si);
        } else {
            pix = defaultFile;
        }

        cache[key] = pix;
        return &pix;
    } else {
        HICON si;
        UINT res = 0;
        if (!fi.absFilePath().isEmpty()) {
#ifndef Q_OS_WINCE
            QT_WA({
                res = ptrExtractIconEx((TCHAR*)fi.absFilePath().ucs2(), -1,
                                      0, 0, 1);
            } , {
                res = ExtractIconExA(fi.absFilePath().local8Bit(), -1,
                                      0, 0, 1);
            });

            if (res) {
                QT_WA({
                    res = ptrExtractIconEx((TCHAR*)fi.absFilePath().ucs2(), res - 1,
                                          0, &si, 1);
                } , {
                    res = ExtractIconExA(fi.absFilePath().local8Bit(), res - 1,
                                          0, &si, 1);
                });
            }
#else
                res = (UINT)ExtractIconEx((TCHAR*)fi.absFilePath().ucs2(), -1,
                                            0, 0, 1);
                if (res)
                    res = (UINT)ExtractIconEx((TCHAR*)fi.absFilePath().ucs2(), res - 1,
                                                0, &si, 1);
#endif

        }

        if (res) {
            pix = fromHICON(si);
            pix.setMask(pix.createHeuristicMask());
            DestroyIcon(si);
        } else {
            pix = defaultExe;
        }

        return &pix;
    }

    // can't happen!
    return 0;
}
#endif



/*!
  \reimp
*/
bool Q3FileDialog::eventFilter(QObject * o, QEvent * e)
{
    if (e->type() == QEvent::KeyPress && ((QKeyEvent*)e)->key() == Qt::Key_F5) {
        rereadDir();
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (e->type() == QEvent::KeyPress && ((QKeyEvent*)e)->key() == Qt::Key_F2 &&
                (o == files || o == files->viewport())) {
        if (files->isVisible() && files->currentItem()) {
            if (QUrlInfo(d->url.info(QString(QLatin1Char('.')))).isWritable() && files->currentItem()->text(0) != QLatin1String("..")) {
                files->renameItem = files->currentItem();
                files->startRename(true);
            }
        }
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (e->type() == QEvent::KeyPress && ((QKeyEvent*)e)->key() == Qt::Key_F2 &&
                (o == d->moreFiles || o == d->moreFiles->viewport())) {
        if (d->moreFiles->isVisible() && d->moreFiles->currentItem() != -1) {
            if (QUrlInfo(d->url.info(QString(QLatin1Char('.')))).isWritable() &&
                 d->moreFiles->item(d->moreFiles->currentItem())->text() != QLatin1String("..")) {
                d->moreFiles->renameItem = d->moreFiles->item(d->moreFiles->currentItem());
                d->moreFiles->startRename(true);
            }
        }
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (e->type() == QEvent::KeyPress && d->moreFiles->renaming) {
        d->moreFiles->lined->setFocus();
        QApplication::sendEvent(d->moreFiles->lined, e);
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (e->type() == QEvent::KeyPress && files->renaming) {
        files->lined->setFocus();
        QApplication::sendEvent(files->lined, e);
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (e->type() == QEvent::KeyPress &&
                ((QKeyEvent *)e)->key() == Qt::Key_Backspace &&
                (o == files ||
                  o == d->moreFiles ||
                  o == files->viewport() ||
                  o == d->moreFiles->viewport())) {
        cdUpClicked();
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (e->type() == QEvent::KeyPress &&
                ((QKeyEvent *)e)->key() == Qt::Key_Delete &&
                (o == files ||
                  o == files->viewport())) {
        if (files->currentItem())
            deleteFile(files->currentItem()->text(0));
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (e->type() == QEvent::KeyPress &&
                ((QKeyEvent *)e)->key() == Qt::Key_Delete &&
                (o == d->moreFiles ||
                  o == d->moreFiles->viewport())) {
        int c = d->moreFiles->currentItem();
        if (c >= 0)
            deleteFile(d->moreFiles->item(c)->text());
        ((QKeyEvent *)e)->accept();
        return true;
    } else if (o == files && e->type() == QEvent::FocusOut && files->currentItem()) {
    } else if (o == files && e->type() == QEvent::KeyPress) {
        QTimer::singleShot(0, this, SLOT(fixupNameEdit()));
    } else if (o == nameEdit && e->type() == QEvent::KeyPress && d->mode != AnyFile) {
        if ((nameEdit->cursorPosition() == (int)nameEdit->text().length() || nameEdit->hasSelectedText()) &&
             isprint(((QKeyEvent *)e)->ascii())) {
#if defined(Q_WS_WIN)
            QString nt(nameEdit->text().toLower());
#else
            QString nt(nameEdit->text());
#endif
            nt.truncate(nameEdit->cursorPosition());
            nt += QLatin1Char((char)(((QKeyEvent *)e)->ascii()));
            Q3ListViewItem * i = files->firstChild();
#if defined(Q_WS_WIN)
            while(i && i->text(0).left(nt.length()).toLower() != nt)
#else
            while(i && i->text(0).left(nt.length()) != nt)
#endif
                i = i->nextSibling();
            if (i) {
                nt = i->text(0);
                int cp = nameEdit->cursorPosition()+1;
                nameEdit->validateAndSet(nt, cp, cp, nt.length());
                return true;
            }
        }
    } else if (o == nameEdit && e->type() == QEvent::FocusIn) {
        fileNameEditDone();
    } else if (d->moreFiles->renaming && o != d->moreFiles->lined && e->type() == QEvent::FocusIn) {
        d->moreFiles->lined->setFocus();
        return true;
    } else if (files->renaming && o != files->lined && e->type() == QEvent::FocusIn) {
        files->lined->setFocus();
        return true;
    } else if ((o == d->moreFiles || o == d->moreFiles->viewport()) &&
                e->type() == QEvent::FocusIn) {
        if ((o == d->moreFiles->viewport() && !d->moreFiles->viewport()->hasFocus())
            || (o == d->moreFiles && !d->moreFiles->hasFocus()))
            ((QWidget*)o)->setFocus();
        return false;
    }

    return QDialog::eventFilter(o, e);
}

/*!
  Sets the filters used in the file dialog to \a filters. Each group
  of filters must be separated by \c{;;} (\e two semicolons).

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 14

*/

void Q3FileDialog::setFilters(const QString &filters)
{
    QStringList lst = makeFiltersList(filters);
    setFilters(lst);
}

/*!
  \overload

  \a types must be a null-terminated list of strings.

*/

void Q3FileDialog::setFilters(const char ** types)
{
    if (!types || !*types)
        return;

    d->types->clear();
    while(types && *types) {
        d->types->insertItem(QString::fromLatin1(*types));
        types++;
    }
    d->types->setCurrentItem(0);
    setFilter(d->types->text(0));
}


/*!
    \overload

    \a types is a list of filter strings.
*/

void Q3FileDialog::setFilters(const QStringList & types)
{
    if (types.count() < 1)
        return;

    d->types->clear();
    for (QStringList::ConstIterator it = types.begin(); it != types.end(); ++it)
        d->types->insertItem(*it);
    d->types->setCurrentItem(0);
    setFilter(d->types->text(0));
}

/*!
  Adds the filter \a filter to the list of filters and makes it the
  current filter.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 15

  In the above example, a file dialog is created, and the file filter "Images
  (*.png *.jpg *.xpm)" is added and is set as the current filter. The original
  filter, "All Files (*)", is still available.

  \sa setFilter(), setFilters()
*/

void Q3FileDialog::addFilter(const QString &filter)
{
    if (filter.isEmpty())
        return;
    QString f = filter;
    QRegExp r(QString::fromLatin1(qt3_file_dialog_filter_reg_exp));
    int index = r.indexIn(f);
    if (index >= 0)
        f = r.cap(2);
    for (int i = 0; i < d->types->count(); ++i) {
        QString f2(d->types->text(i));
        int index = r.indexIn(f2);
        if (index >= 0)
            f2 = r.cap(1);
        if (f2 == f) {
            d->types->setCurrentItem(i);
            setFilter(f2);
            return;
        }
    }

    d->types->insertItem(filter);
    d->types->setCurrentItem(d->types->count() - 1);
    setFilter(d->types->text(d->types->count() - 1));
}

/*!
  Since modeButtons is a top-level widget, it may be destroyed by the
  kernel at application exit. Notice if this happens to
  avoid double deletion.
*/

void Q3FileDialog::modeButtonsDestroyed()
{
    if (d)
        d->modeButtons = 0;
}


/*!
  This is a convenience static function that will return one or more
  existing files selected by the user.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 16

  This function creates a modal file dialog called \a name, with
  parent \a parent. If \a parent is not 0, the dialog will be shown
  centered over the parent.

  The file dialog's working directory will be set to \a dir. If \a
  dir includes a file name, the file will be selected. The filter
  is set to \a filter so that only those files which match the filter
  are shown. The filter selected is set to \a selectedFilter. The parameters
  \a dir, \a selectedFilter and \a filter may be empty strings.

  The dialog's caption is set to \a caption. If \a caption is not
  specified then a default caption will be used.

  Under Windows and Mac OS X, this static function will use the native
  file dialog and not a Q3FileDialog, unless the style of the application
  is set to something other than the native style. (Note that on Windows the
  dialog will spin a blocking modal event loop that will not dispatch any
  QTimers and if parent is not 0 then it will position the dialog just under
  the parent's title bar).

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  Note that if you want to iterate over the list of files, you should
  iterate over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 17

  \sa getOpenFileName(), getSaveFileName(), getExistingDirectory()
*/

QStringList Q3FileDialog::getOpenFileNames(const QString & filter,
                                           const QString& dir,
                                           QWidget *parent,
                                           const char* name,
                                           const QString& caption,
                                           QString *selectedFilter,
                                           bool resolveSymlinks)
{
    bool save_qt_resolve_symlinks = qt_resolve_symlinks;
    qt_resolve_symlinks = resolveSymlinks;

    QStringList filters;
    if (!filter.isEmpty())
        filters = makeFiltersList(filter);

    makeVariables();

    if (workingDirectory->isNull())
        *workingDirectory = toRootIfNotExists( QDir::currentDirPath() );

    if (!dir.isEmpty()) {
        // #### works only correct for local files
        Q3UrlOperator u(Q3FileDialogPrivate::encodeFileName(dir));
        if (u.isLocalFile() && QFileInfo(u.path()).isDir()) {
            *workingDirectory = dir;
        } else {
            *workingDirectory = u.toString();
        }
    }

#if defined(Q_WS_WIN)
    if (qt_use_native_dialogs && qobject_cast<QWindowsStyle *>(qApp->style()))
        return winGetOpenFileNames(filter, workingDirectory, parent, name, caption, selectedFilter);
#elif defined(Q_WS_MAC)
    if (qt_use_native_dialogs && qobject_cast<QMacStyle *>(qApp->style())) {
        QStringList sl = macGetOpenFileNames(filter, dir.isEmpty() ? 0 : workingDirectory,
                                             parent, name, caption, selectedFilter);
        for (int i = 0; i < sl.count(); ++i)
            sl.replace(i, sl.at(i).normalized(QString::NormalizationForm_C));
        return sl;
    }
#endif

    Q3FileDialog *dlg = new Q3FileDialog(*workingDirectory, QString(), parent, name ? name : "qt_filedlg_gofns", true);

    if (!caption.isNull())
        dlg->setWindowTitle(caption);
    else
        dlg->setWindowTitle(Q3FileDialog::tr("Open"));

    dlg->setFilters(filters);
    if (selectedFilter)
        dlg->setFilter(*selectedFilter);
    dlg->setMode(Q3FileDialog::ExistingFiles);
    QString result;
    QStringList lst;
    if (dlg->exec() == QDialog::Accepted) {
        lst = dlg->selectedFiles();
        *workingDirectory = dlg->d->url;
        if (selectedFilter)
            *selectedFilter = dlg->selectedFilter();
    }
    delete dlg;

    qt_resolve_symlinks = save_qt_resolve_symlinks;

    return lst;
}

/*!  Updates the line edit to match the speed-key usage in Q3ListView. */

void Q3FileDialog::fixupNameEdit()
{
    if (files->currentItem()) {
        if (((Q3FileDialogPrivate::File*)files->currentItem())->info.isFile())
            nameEdit->setText(files->currentItem()->text(0));
    }
}

/*!
  Returns the URL of the current working directory in the file dialog.

  \sa setUrl()
*/

Q3Url Q3FileDialog::url() const
{
    return d->url;
}

static bool isRoot(const Q3Url &u)
{
#if defined(Q_OS_UNIX)
    if (u.path() == QString(QLatin1Char('/')))
        return true;
#elif defined(Q_OS_WIN32)
    QString p = u.path();
    if (p.length() == 3 &&
         p.right(2) == QLatin1String(":/"))
        return true;
    if (p[0] == QLatin1Char('/') && p[1] == QLatin1Char('/')) {
        int slashes = p.count(QLatin1Char('/'));
        if (slashes <= 3)
            return true;
        if (slashes == 4 && p[(int)p.length() - 1] == QLatin1Char('/'))
            return true;
    }
#else
#if defined(Q_CC_GNU)
#warning "case not covered.."
#endif
#endif

    if (!u.isLocalFile() && u.path() == QString(QLatin1Char('/')))
        return true;

    return false;
}

#if defined(Q_WS_WIN)
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

void Q3FileDialog::urlStart(Q3NetworkOperation *op)
{
    if (!op)
        return;

#if defined(Q_WS_WIN)
    old_qt_ntfs_permission_lookup = qt_ntfs_permission_lookup;
    qt_ntfs_permission_lookup = 0;
#endif
    if (op->operation() == Q3NetworkProtocol::OpListChildren) {
#ifndef QT_NO_CURSOR
        if (!d->cursorOverride) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            d->cursorOverride = true;
        }
#endif
        if (isRoot(d->url))
            d->cdToParent->setEnabled(false);
        else
            d->cdToParent->setEnabled(true);
        d->mimeTypeTimer->stop();
        d->sortedList.clear();
        d->pendingItems.clear();
        d->moreFiles->clearSelection();
        files->clearSelection();
        d->moreFiles->clear();
        files->clear();
        files->setSorting(-1);

        QString s = d->url.toString(false, false);
        bool found = false;
        for (int i = 0; i < d->paths->count(); ++i) {
#if defined(Q_WS_WIN)
            if (d->paths->text(i).toLower() == s.toLower()) {
#else
            if (d->paths->text(i) == s) {
#endif
                found = true;
                d->paths->setCurrentItem(i);
                break;
            }
        }
        if (!found) {
            d->paths->insertItem(*openFolderIcon, s, -1);
            d->paths->setCurrentItem(d->paths->count() - 1);
        }
        d->last = 0;
        d->hadDotDot = false;

        if (d->goBack && (d->history.isEmpty() || d->history.last() != d->url.toString())) {
            d->history.append(d->url.toString());
            if (d->history.count() > 1)
                d->goBack->setEnabled(true);
        }
    }
}

void Q3FileDialog::urlFinished(Q3NetworkOperation *op)
{
    if (!op)
        return;

#ifndef QT_NO_CURSOR
    if (op->operation() == Q3NetworkProtocol::OpListChildren &&
         d->cursorOverride) {
        QApplication::restoreOverrideCursor();
        d->cursorOverride = false;
    }
#endif

    if (op->state() == Q3NetworkProtocol::StFailed) {
        if (d->paths->hasFocus())
            d->ignoreNextKeyPress = true;

        if (d->progressDia) {
            d->ignoreStop = true;
            d->progressDia->close();
            delete d->progressDia;
            d->progressDia = 0;
        }

        int ecode = op->errorCode();
        QMessageBox::critical(this, tr("Error"), op->protocolDetail());

        if (ecode == Q3NetworkProtocol::ErrListChildren || ecode == Q3NetworkProtocol::ErrParse ||
             ecode == Q3NetworkProtocol::ErrUnknownProtocol || ecode == Q3NetworkProtocol::ErrLoginIncorrect ||
             ecode == Q3NetworkProtocol::ErrValid || ecode == Q3NetworkProtocol::ErrHostNotFound ||
             ecode == Q3NetworkProtocol::ErrFileNotExisting) {
            d->url = d->oldUrl;
            rereadDir();
        } else {
            // another error happened, no need to go back to last dir
        }
    } else if (op->operation() == Q3NetworkProtocol::OpListChildren &&
                op == d->currListChildren) {
        if (!d->hadDotDot && !isRoot(d->url)) {
            bool ok = true;
#if defined(Q_WS_WIN)
            if (d->url.path().left(2) == QLatin1String("//"))
                ok = false;
#endif
            if (ok) {
                QUrlInfo ui(d->url.info(QLatin1String("..")));
                ui.setName(QLatin1String(".."));
                ui.setDir(true);
                ui.setFile(false);
                ui.setSymLink(false);
                ui.setSize(0);
                Q3ValueList<QUrlInfo> lst;
                lst << ui;
                insertEntry(lst, 0);
            }
        }
        resortDir();
    } else if (op->operation() == Q3NetworkProtocol::OpGet) {
    } else if (op->operation() == Q3NetworkProtocol::OpPut) {
        rereadDir();
        if (d->progressDia) {
            d->ignoreStop = true;
            d->progressDia->close();
        }
        delete d->progressDia;
        d->progressDia = 0;
    }

#if defined(Q_WS_WIN)
    qt_ntfs_permission_lookup = old_qt_ntfs_permission_lookup;
#endif
}

void Q3FileDialog::dataTransferProgress(int bytesDone, int bytesTotal, Q3NetworkOperation *op)
{
    if (!op)
        return;

    QString label;
    Q3Url u(op->arg(0));
    if (u.isLocalFile()) {
        label = u.path();
    } else {
        label = QLatin1String("%1 (on %2)");
        label = label.arg(u.path()).arg(u.host());
    }

    if (!d->progressDia) {
        if (bytesDone < bytesTotal) {
            d->ignoreStop = false;
            d->progressDia = new QFDProgressDialog(this, label, bytesTotal);
            connect(d->progressDia, SIGNAL(cancelled()),
                     this, SLOT(stopCopy()));
            d->progressDia->show();
        } else
            return;
    }

    if (d->progressDia) {
        if (op->operation() == Q3NetworkProtocol::OpGet) {
            if (d->progressDia) {
                d->progressDia->setReadProgress(bytesDone);
            }
        } else if (op->operation() == Q3NetworkProtocol::OpPut) {
            if (d->progressDia) {
                d->progressDia->setWriteLabel(label);
                d->progressDia->setWriteProgress(bytesDone);
            }
        } else {
            return;
        }
    }
}

void Q3FileDialog::insertEntry(const Q3ValueList<QUrlInfo> &lst, Q3NetworkOperation *op)
{
    if (op && op->operation() == Q3NetworkProtocol::OpListChildren &&
         op != d->currListChildren)
        return;
    Q3ValueList<QUrlInfo>::ConstIterator it = lst.begin();
    for (; it != lst.end(); ++it) {
        const QUrlInfo &inf = *it;
        if (d->mode == DirectoryOnly && !inf.isDir())
            continue;
        if (inf.name() == QLatin1String("..")) {
            d->hadDotDot = true;
            if (isRoot(d->url))
                continue;
#if defined(Q_WS_WIN)
            if (d->url.path().left(2) == QLatin1String("//"))
                continue;
#endif
        } else if (inf.name() == QString(QLatin1Char('.')))
            continue;

#if defined(Q_WS_WIN)
        // Workaround a Windows bug, '..' is apparantly hidden in directories
        // that are one level away from root
        if (!bShowHiddenFiles && inf.name() != QLatin1String("..")) {
            if (d->url.isLocalFile()) {
                QString file = d->url.path();
                if (!file.endsWith(QLatin1String("/")))
                    file.append(QLatin1String("/"));
                file += inf.name();
                QT_WA({
                    if (GetFileAttributesW((TCHAR*)file.ucs2()) & FILE_ATTRIBUTE_HIDDEN)
                        continue;
                } , {
                    if (GetFileAttributesA(file.local8Bit()) & FILE_ATTRIBUTE_HIDDEN)
                        continue;
                });
            } else {
                if (inf.name() != QLatin1String("..") && inf.name()[0] == QLatin1Char('.'))
                    continue;
            }
        }
#else
        if (!bShowHiddenFiles && inf.name() != QLatin1String("..")) {
            if (inf.name()[0] == QLatin1Char('.'))
                continue;
        }
#endif
        if (!d->url.isLocalFile()) {
            Q3FileDialogPrivate::File * i = 0;
            Q3FileDialogPrivate::MCItem *i2 = 0;
            i = new Q3FileDialogPrivate::File(d, &inf, files);
            i2 = new Q3FileDialogPrivate::MCItem(d->moreFiles, i);

            if ((d->mode == ExistingFiles && inf.isDir())
                || (isDirectoryMode(d->mode) && inf.isFile())) {
                i->setSelectable(false);
                i2->setSelectable(false);
            }

            i->i = i2;
        }

        d->sortedList.append(new QUrlInfo(inf));
    }
}

void Q3FileDialog::removeEntry(Q3NetworkOperation *op)
{
    if (!op)
        return;

    QUrlInfo *i = 0;
    Q3ListViewItemIterator it(files);
    bool ok1 = false, ok2 = false;
    for (i = d->sortedList.first(); it.current(); ++it, i = d->sortedList.next()) {
        QString encName = Q3FileDialogPrivate::encodeFileName(
            ((Q3FileDialogPrivate::File*)it.current())->info.name());
        if (encName == op->arg(0)) {
            d->pendingItems.removeRef((Q3FileDialogPrivate::File*)it.current());
            delete ((Q3FileDialogPrivate::File*)it.current())->i;
            delete it.current();
            ok1 = true;
        }
        if (i && i->name() == op->arg(0)) {
            d->sortedList.removeRef(i);
            i = d->sortedList.prev();
            ok2 = true;
        }
        if (ok1 && ok2)
            break;
    }
}

void Q3FileDialog::itemChanged(Q3NetworkOperation *op)
{
    if (!op)
        return;

    QUrlInfo *i = 0;
    Q3ListViewItemIterator it1(files);
    bool ok1 = false, ok2 = false;
    // first check whether the new file replaces an existing file.
    for (i = d->sortedList.first(); it1.current(); ++it1, i = d->sortedList.next()) {
        if (((Q3FileDialogPrivate::File*)it1.current())->info.name() == op->arg(1)) {
            delete ((Q3FileDialogPrivate::File*)it1.current())->i;
            delete it1.current();
            ok1 = true;
        }
        if (i && i->name() == op->arg(1)) {
            d->sortedList.removeRef(i);
            i = d->sortedList.prev();
            ok2 = true;
        }
        if (ok1 && ok2)
            break;
    }

    i = 0;
    Q3ListViewItemIterator it(files);
    ok1 = false;
    ok2 = false;
    for (i = d->sortedList.first(); it.current(); ++it, i = d->sortedList.next()) {
        if (((Q3FileDialogPrivate::File*)it.current())->info.name() == op->arg(0)) {
            ((Q3FileDialogPrivate::File*)it.current())->info.setName(op->arg(1));
            ok1 = true;
        }
        if (i && i->name() == op->arg(0)) {
            i->setName(op->arg(1));
            ok2 = true;
        }
        if (ok1 && ok2)
            break;
    }

    resortDir();
}

/*!
  \property Q3FileDialog::infoPreview

  \brief whether the file dialog can provide preview information about
  the currently selected file

  The default is false.
*/
bool Q3FileDialog::isInfoPreviewEnabled() const
{
    return d->infoPreview;
}

void Q3FileDialog::setInfoPreviewEnabled(bool info)
{
    if (info == d->infoPreview)
        return;
    d->geometryDirty = true;
    d->infoPreview = info;
    updateGeometries();
}


/*!
  \property Q3FileDialog::contentsPreview

  \brief whether the file dialog can provide a contents preview of the
  currently selected file

  The default is false.

  \sa setContentsPreview() setInfoPreviewEnabled()
*/
// ### improve the above documentation: how is the preview done, how can I add
// support for customized preview, etc.

bool Q3FileDialog::isContentsPreviewEnabled() const
{
    return d->contentsPreview;
}

void Q3FileDialog::setContentsPreviewEnabled(bool contents)
{
    if (contents == d->contentsPreview)
        return;
    d->geometryDirty = true;
    d->contentsPreview = contents;
    updateGeometries();
}


/*!
  Sets the widget to be used for displaying information about the file
  to the widget \a w and a preview of that information to the
  Q3FilePreview \a preview.

  Normally you would create a preview widget that derives from both QWidget and
  Q3FilePreview, so you should pass the same widget twice.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 18

  \sa setContentsPreview(), setInfoPreviewEnabled(), setPreviewMode()

*/

void Q3FileDialog::setInfoPreview(QWidget *w, Q3FilePreview *preview)
{
    if (!w || !preview)
        return;

    if (d->infoPreviewWidget) {
        d->preview->removeWidget(d->infoPreviewWidget);
	if ((void*)d->infoPreviewer == (void*)d->infoPreviewWidget)
	    d->infoPreviewer = 0;
        delete d->infoPreviewWidget;
    }
    if (d->infoPreviewer)
	delete d->infoPreviewer;
    d->infoPreviewWidget = w;
    d->infoPreviewer = preview;
    w->reparent(d->preview, 0, QPoint(0, 0));
}

/*!
  Sets the widget to be used for displaying the contents of the file
  to the widget \a w and a preview of those contents to the
  Q3FilePreview \a preview.

  Normally you would create a preview widget that derives from both QWidget and
  Q3FilePreview, so you should pass the same widget twice.

  \snippet doc/src/snippets/code/src_qt3support_dialogs_q3filedialog.cpp 19

  \sa setContentsPreviewEnabled(), setInfoPreview(), setPreviewMode()
*/

void Q3FileDialog::setContentsPreview(QWidget *w, Q3FilePreview *preview)
{
    if (!w || !preview)
        return;

    if (d->contentsPreviewWidget) {
        d->preview->removeWidget(d->contentsPreviewWidget);
	if ((void*)d->contentsPreviewWidget == (void*)d->contentsPreviewer)
	    d->contentsPreviewer = 0;
        delete d->contentsPreviewWidget;
    }
    if (d->contentsPreviewer)
	delete d->contentsPreviewer;
    d->contentsPreviewWidget = w;
    d->contentsPreviewer = preview;
    w->reparent(d->preview, 0, QPoint(0, 0));
}

/*!
  Re-sorts the displayed directory.

  \sa rereadDir()
*/

void Q3FileDialog::resortDir()
{
    d->mimeTypeTimer->stop();
    d->pendingItems.clear();

    Q3FileDialogPrivate::File *item = 0;
    Q3FileDialogPrivate::MCItem *item2 = 0;

    d->sortedList.sort();

    if (files->childCount() > 0 || d->moreFiles->count() > 0) {
        d->moreFiles->clear();
        files->clear();
        d->last = 0;
        files->setSorting(-1);
    }

    QUrlInfo *i = sortAscending ? d->sortedList.first() : d->sortedList.last();
    for (; i; i = sortAscending ? d->sortedList.next() : d->sortedList.prev()) {
        item = new Q3FileDialogPrivate::File(d, i, files);
        item2 = new Q3FileDialogPrivate::MCItem(d->moreFiles, item, item2);
        item->i = item2;
        d->pendingItems.append(item);
        if ((d->mode == ExistingFiles && item->info.isDir()) ||
            (isDirectoryMode(d->mode) && item->info.isFile())) {
            item->setSelectable(false);
            item2->setSelectable(false);
        }
    }

    // ##### As the Q3FileIconProvider only support QFileInfo and no
    // QUrlInfo it can be only used for local files at the moment. In
    // 3.0 we have to change the API of Q3FileIconProvider to work on
    // QUrlInfo so that also remote filesystems can be show mime-type
    // specific icons.
    if (d->url.isLocalFile())
        d->mimeTypeTimer->start(0);
}

/*!
  Stops the current copy operation.
*/

void Q3FileDialog::stopCopy()
{
    if (d->ignoreStop)
        return;

    d->url.blockSignals(true);
    d->url.stop();
    if (d->progressDia) {
        d->ignoreStop = true;
        QTimer::singleShot(100, this, SLOT(removeProgressDia()));
    }
    d->url.blockSignals(false);
}

/*!
  \internal
*/

void Q3FileDialog::removeProgressDia()
{
    if (d->progressDia)
        delete d->progressDia;
    d->progressDia = 0;
}

/*!
  \internal
*/

void Q3FileDialog::doMimeTypeLookup()
{
    if (!iconProvider()) {
        d->pendingItems.clear();
        d->mimeTypeTimer->stop();
        return;
    }

    d->mimeTypeTimer->stop();
    if (d->pendingItems.count() == 0) {
        return;
    }

    QRect r;
    Q3FileDialogPrivate::File *item = d->pendingItems.first();
    if (item) {
        QFileInfo fi;
        if (d->url.isLocalFile()) {
            fi.setFile(Q3Url(d->url.path(), Q3FileDialogPrivate::encodeFileName(item->info.name())).path(false));
        } else
            fi.setFile(item->info.name()); // #####
        const QPixmap *p = iconProvider()->pixmap(fi);
        if (p && p != item->pixmap(0) &&
             (!item->pixmap(0) || p->serialNumber() != item->pixmap(0)->serialNumber()) &&
             p != fifteenTransparentPixels) {
            item->hasMimePixmap = true;

            // evil hack to avoid much too much repaints!
            QPointer<Q3FileDialog> that(this); // this may be deleted by an event handler
            qApp->processEvents();
            if (that.isNull())
                return;
            files->setUpdatesEnabled(false);
            files->viewport()->setUpdatesEnabled(false);
            if (item != d->pendingItems.first())
                return;
            item->setPixmap(0, *p);
            qApp->processEvents();
            if (that.isNull())
                return;
            files->setUpdatesEnabled(true);
            files->viewport()->setUpdatesEnabled(true);

            if (files->isVisible()) {
                QRect ir(files->itemRect(item));
                if (ir != QRect(0, 0, -1, -1)) {
                    r = r.united(ir);
                }
            } else {
                QRect ir(d->moreFiles->itemRect(item->i));
                if (ir != QRect(0, 0, -1, -1)) {
                    r = r.united(ir);
                }
            }
        }
        if (d->pendingItems.count())
            d->pendingItems.removeFirst();
    }

    if (d->moreFiles->isVisible()) {
        d->moreFiles->viewport()->repaint(r);
    } else {
        files->viewport()->repaint(r);
    }

    if (d->pendingItems.count())
        d->mimeTypeTimer->start(0);
    else if (d->moreFiles->isVisible())
        d->moreFiles->triggerUpdate(true);
}

/*!
  If \a b is true then all the files in the current directory are selected;
  otherwise, they are deselected.
*/

void Q3FileDialog::selectAll(bool b)
{
    if (d->mode != ExistingFiles)
        return;
    d->moreFiles->selectAll(b);
    files->selectAll(b);
}

void Q3FileDialog::goBack()
{
    if (!d->goBack || !d->goBack->isEnabled() || d->history.isEmpty())
        return;
    d->history.removeLast();
    if (d->history.size() < 2)
        d->goBack->setEnabled(false);
    setUrl(d->history.last());
}

// a class with wonderfully inflexible flexibility. why doesn't it
// just subclass QWidget in the first place? 'you have to derive your
// preview widget from QWidget and from this class' indeed.

/*!
  \class Q3FilePreview qfiledialog.h
  \brief The Q3FilePreview class provides file previewing in Q3FileDialog.

  \compat

  This class is an abstract base class which is used to implement
  widgets that can display a preview of a file in a Q3FileDialog.

  You must derive the preview widget from both QWidget and from this
  class. Then you must reimplement this class's previewUrl() function,
  which is called by the file dialog if the preview of a file
  (specified as a URL) should be shown.

  See also Q3FileDialog::setPreviewMode(), Q3FileDialog::setContentsPreview(),
  Q3FileDialog::setInfoPreview(), Q3FileDialog::setInfoPreviewEnabled(),
  Q3FileDialog::setContentsPreviewEnabled().
*/

/*!
  Constructs the Q3FilePreview.
*/

Q3FilePreview::Q3FilePreview()
{
}

/*!
    \fn Q3FilePreview::~Q3FilePreview()

    Destroys the file preview object.
*/

/*!
  \fn void Q3FilePreview::previewUrl(const Q3Url &url)

  This function is called by Q3FileDialog if a preview
  for the \a url should be shown. Reimplement this
  function to provide file previewing.
*/


QT_END_NAMESPACE

#include "moc_q3filedialog.cpp"
#include "q3filedialog.moc"

#endif
