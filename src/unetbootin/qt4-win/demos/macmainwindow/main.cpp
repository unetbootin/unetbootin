/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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
#include <QtGui>

#ifdef Q_WS_MAC

#include <Carbon/Carbon.h>

// The SearchWidget class wraps a native HISearchField.
class SearchWidget : public QWidget
{
    Q_OBJECT
private:
    HIViewRef searchField;
    CFStringRef searchFieldText;

public:
    SearchWidget(QWidget *parent = 0)
    :QWidget(parent)
    {
        // Create a native search field and pass its window id to QWidget::create.
        searchFieldText = CFStringCreateWithCString(0, "search", 0);
        HISearchFieldCreate(NULL/*bounds*/, kHISearchFieldAttributesSearchIcon | kHISearchFieldAttributesCancel,
                            NULL/*menu ref*/, searchFieldText, &searchField);
        create(reinterpret_cast<WId>(searchField));

    
        // Use a Qt menu for the search field menu.
        QMenu *searchMenu = new QMenu(this);
        
        QAction * indexAction = searchMenu->addAction("Index Search");
        indexAction->setCheckable(true);
        indexAction->setChecked(true);

        QAction * fulltextAction = searchMenu->addAction("Full Text Search");
        fulltextAction->setCheckable(true);

        QActionGroup *searchActionGroup = new QActionGroup(this);
        searchActionGroup->addAction(indexAction);
        searchActionGroup->addAction(fulltextAction);
        searchActionGroup->setExclusive(true);
        
        MenuRef macSearchMenu = searchMenu->macMenu(0);
        HISearchFieldSetSearchMenu(searchField, macSearchMenu);
        setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    }

    ~SearchWidget()
    {
        CFRelease(searchField);
        CFRelease(searchFieldText);
    }

    // Get the size hint from the search field.
    QSize sizeHint() const
    {
        EventRef event;
        HIRect optimalBounds;
        CreateEvent(0, kEventClassControl,
            kEventControlGetOptimalBounds,
            GetCurrentEventTime(),
            kEventAttributeUserEvent, &event);

        SendEventToEventTargetWithOptions(event,
            HIObjectGetEventTarget(HIObjectRef(winId())),
            kEventTargetDontPropagate);

        GetEventParameter(event,
            kEventParamControlOptimalBounds, typeHIRect,
            0, sizeof(HIRect), 0, &optimalBounds);

        ReleaseEvent(event);
        return QSize(optimalBounds.size.width + 100, // make it a bit wider.
                     optimalBounds.size.height);
    }
};

class SearchWrapper : public QWidget
{
Q_OBJECT
public:
    SearchWrapper(QWidget *parent = 0)
    :QWidget(parent)
    {
        s = new SearchWidget(this);
        s->move(2,2);
        setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    }

    QSize sizeHint() const
    {
        return s->sizeHint() + QSize(6, 4);
    }
   
    SearchWidget *s;
};

class Spacer : public QWidget
{
Q_OBJECT
public:
    Spacer(QWidget *parent = 0)
    :QWidget(parent)
    {
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setSizePolicy(sizePolicy);
    }

    QSize sizeHint() const
    {
        return QSize(1, 1);
    }
};

class MacSplitterHandle : public QSplitterHandle
{
Q_OBJECT
public:
    MacSplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent) {   }

    // Paint the horizontal handle as a gradient, paint
    // the vertical handle as a line.
    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);

        QColor topColor(145, 145, 145);
        QColor bottomColor(142, 142, 142);
        QColor gradientStart(252, 252, 252);
        QColor gradientStop(223, 223, 223);

        if (orientation() == Qt::Vertical) {
            painter.setPen(topColor);
            painter.drawLine(0, 0, width(), 0);
            painter.setPen(bottomColor);
            painter.drawLine(0, height() - 1, width(), height() - 1);

            QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height() -3));
            linearGrad.setColorAt(0, gradientStart);
            linearGrad.setColorAt(1, gradientStop);
            painter.fillRect(QRect(QPoint(0,1), size() - QSize(0, 2)), QBrush(linearGrad));
        } else {
            painter.setPen(topColor);
            painter.drawLine(0, 0, 0, height());
        }
    }

    QSize sizeHint() const
    {
        QSize parent = QSplitterHandle::sizeHint();
        if (orientation() == Qt::Vertical) {
            return parent + QSize(0, 3);
        } else {
            return QSize(1, parent.height());
        }
    }
};

class MacSplitter : public QSplitter
{
public:
    QSplitterHandle *createHandle()
    {
        return new MacSplitterHandle(orientation(), this);
    }
};

class MacMainWindow : public QMainWindow
{
Q_OBJECT
public:
    MacMainWindow()
    {
        QSettings settings;
        restoreGeometry(settings.value("Geometry").toByteArray());

        setWindowTitle("Mac Main Window");

        splitter = new MacSplitter();

        // Set up the left-hand side blue side bar.
        sidebar = new QTreeView();
        sidebar->setFrameStyle(QFrame::NoFrame);
        sidebar->setAttribute(Qt::WA_MacShowFocusRect, false);
        sidebar->setAutoFillBackground(true);
        
        // Set the palette.
        QPalette palette = sidebar->palette();
        QColor macSidebarColor(231, 237, 246);
        QColor macSidebarHighlightColor(168, 183, 205);
        palette.setColor(QPalette::Base, macSidebarColor);
        palette.setColor(QPalette::Highlight, macSidebarHighlightColor);
        sidebar->setPalette(palette);

        sidebar->setModel(createItemModel());
        sidebar->header()->hide();
        sidebar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        sidebar->setTextElideMode(Qt::ElideMiddle);

        splitter->addWidget(sidebar);

        horizontalSplitter = new MacSplitter();
        horizontalSplitter->setOrientation(Qt::Vertical);
        splitter->addWidget(horizontalSplitter);
        
        splitter->setStretchFactor(0, 0);
        splitter->setStretchFactor(1, 1);

        // Set up the top document list view.
        documents = new QListView();
        documents->setFrameStyle(QFrame::NoFrame);
        documents->setAttribute(Qt::WA_MacShowFocusRect, false);
        documents->setModel(createDocumentModel());
        documents->setAlternatingRowColors(true);
        documents->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        horizontalSplitter->addWidget(documents);
        horizontalSplitter->setStretchFactor(0, 0);

        // Set up the text view.
        textedit = new QTextEdit();
        textedit->setFrameStyle(QFrame::NoFrame);
        textedit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        textedit->setText("<br><br><br><br><br><br><center><b>This demo shows how to create a \
                           Qt main window application that has the same appearance as other \
                           Mac OS X applications such as Mail or iTunes. This includes \
                           customizing the item views and QSplitter and wrapping native widgets \
                           such as the search field.</b></center>");

        horizontalSplitter->addWidget(textedit);
        horizontalSplitter->setStretchFactor(1, 1);

        setCentralWidget(splitter);

        toolBar = addToolBar(tr("Search"));
        toolBar->addWidget(new Spacer());
        toolBar->addWidget(new SearchWrapper());
        
        setUnifiedTitleAndToolBarOnMac(true);
    }

    ~MacMainWindow()
    {
        QSettings settings;
        settings.setValue("Geometry", saveGeometry());
    }

    QAbstractItemModel *createItemModel()
    {
        QStandardItemModel *model = new QStandardItemModel();
        QStandardItem *parentItem = model->invisibleRootItem();

        QStandardItem *documentationItem = new QStandardItem("Documentation"); 
        parentItem->appendRow(documentationItem);

        QStandardItem *assistantItem = new QStandardItem("Qt MainWindow Manual"); 
        documentationItem->appendRow(assistantItem);

        QStandardItem *designerItem = new QStandardItem("Qt Designer Manual"); 
        documentationItem->appendRow(designerItem);

        QStandardItem *qtItem = new QStandardItem("Qt Reference Documentation"); 
        qtItem->appendRow(new QStandardItem("Classes"));
        qtItem->appendRow(new QStandardItem("Overviews"));
        qtItem->appendRow(new QStandardItem("Tutorial & Examples"));
        documentationItem->appendRow(qtItem);

        QStandardItem *bookmarksItem = new QStandardItem("Bookmarks"); 
        parentItem->appendRow(bookmarksItem);
        bookmarksItem->appendRow(new QStandardItem("QWidget"));
        bookmarksItem->appendRow(new QStandardItem("QObject"));
        bookmarksItem->appendRow(new QStandardItem("QWizard"));

        return model;
    }
    
    void resizeEvent(QResizeEvent *e)
    {
        toolBar->updateGeometry();
    }
    
    QAbstractItemModel *createDocumentModel()
    {
        QStandardItemModel *model = new QStandardItemModel();
        QStandardItem *parentItem = model->invisibleRootItem(); 
        parentItem->appendRow(new QStandardItem("QWidget Class Reference"));
        parentItem->appendRow(new QStandardItem("QObject Class Reference"));
        parentItem->appendRow(new QStandardItem("QListView Class Reference"));

        return model;
    }

public:
    QSplitter *splitter;
    QSplitter *horizontalSplitter;
    QTreeView *sidebar;
    QListView *documents;
    QTextEdit *textedit;
    QVBoxLayout *layout;
    SearchWidget *searchWidget;
    QToolBar * toolBar;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MacMainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}

#include "main.moc"

#else
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    qDebug() << "This is a Mac-only demo";
}

#endif
