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

#include "colorswatch.h"

#include <QAction>
#include <QtEvents>
#include <QFrame>
#include <QMainWindow>
#include <QMenu>
#include <QPainter>
#include <QImage>
#include <QColor>
#include <QDialog>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPainterPath>
#include <QPushButton>
#include <QHBoxLayout>
#include <QtDebug>

#undef DEBUG_SIZEHINTS

QColor bgColorForName(const QString &name)
{
    if (name == "Black")
        return QColor("#D8D8D8");
    else if (name == "White")
        return QColor("#F1F1F1");
    else if (name == "Red")
        return QColor("#F1D8D8");
    else if (name == "Green")
        return QColor("#D8E4D8");
    else if (name == "Blue")
        return QColor("#D8D8F1");
    else if (name == "Yellow")
        return QColor("#F1F0D8");
    return QColor(name).light(110);
}

QColor fgColorForName(const QString &name)
{
    if (name == "Black")
        return QColor("#6C6C6C");
    else if (name == "White")
        return QColor("#F8F8F8");
    else if (name == "Red")
        return QColor("#F86C6C");
    else if (name == "Green")
        return QColor("#6CB26C");
    else if (name == "Blue")
        return QColor("#6C6CF8");
    else if (name == "Yellow")
        return QColor("#F8F76C");
    return QColor(name);
}

class ColorDock : public QFrame
{
    Q_OBJECT
public:
    ColorDock(const QString &c, QWidget *parent);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    void setCustomSizeHint(const QSize &size);

public slots:
    void changeSizeHints();

protected:
    void paintEvent(QPaintEvent *);
    QString color;
    QSize szHint, minSzHint;
};

ColorDock::ColorDock(const QString &c, QWidget *parent)
    : QFrame(parent) , color(c)
{
    QFont font = this->font();
    font.setPointSize(8);
    setFont(font);
    szHint = QSize(-1, -1);
    minSzHint = QSize(125, 75);
}

QSize ColorDock::sizeHint() const
{
    return szHint;
}

QSize ColorDock::minimumSizeHint() const
{
    return minSzHint;
}

void ColorDock::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), bgColorForName(color));

    p.save();

    extern void render_qt_text(QPainter *, int, int, const QColor &);
    render_qt_text(&p, width(), height(), fgColorForName(color));

    p.restore();

#ifdef DEBUG_SIZEHINTS
    p.setRenderHint(QPainter::Antialiasing, false);

    QSize sz = size();
    QSize szHint = sizeHint();
    QSize minSzHint = minimumSizeHint();
    QSize maxSz = maximumSize();
    QString text = QString::fromLatin1("sz: %1x%2\nszHint: %3x%4\nminSzHint: %5x%6\n"
                                        "maxSz: %8x%9")
                    .arg(sz.width()).arg(sz.height())
                    .arg(szHint.width()).arg(szHint.height())
                    .arg(minSzHint.width()).arg(minSzHint.height())
                    .arg(maxSz.width()).arg(maxSz.height());

    QRect r = fontMetrics().boundingRect(rect(), Qt::AlignLeft|Qt::AlignTop, text);
    r.adjust(-2, -2, 1, 1);
    p.translate(4, 4);
    QColor bg = Qt::yellow;
    bg.setAlpha(120);
    p.setBrush(bg);
    p.setPen(Qt::black);
    p.drawRect(r);
    p.drawText(rect(), Qt::AlignLeft|Qt::AlignTop, text);
#endif // DEBUG_SIZEHINTS
}

static QSpinBox *createSpinBox(int value, QWidget *parent, int max = 1000)
{
    QSpinBox *result = new QSpinBox(parent);
    result->setMinimum(-1);
    result->setMaximum(max);
    result->setValue(value);
    return result;
}

void ColorDock::changeSizeHints()
{
    QDialog dialog(this);
    dialog.setWindowTitle(color);

    QVBoxLayout *topLayout = new QVBoxLayout(&dialog);

    QGridLayout *inputLayout = new QGridLayout();
    topLayout->addLayout(inputLayout);

    inputLayout->addWidget(new QLabel(tr("Size Hint:"), &dialog), 0, 0);
    inputLayout->addWidget(new QLabel(tr("Min Size Hint:"), &dialog), 1, 0);
    inputLayout->addWidget(new QLabel(tr("Max Size:"), &dialog), 2, 0);
    inputLayout->addWidget(new QLabel(tr("Dockwgt Max Size:"), &dialog), 3, 0);

    QSpinBox *szHintW = createSpinBox(szHint.width(), &dialog);
    inputLayout->addWidget(szHintW, 0, 1);
    QSpinBox *szHintH = createSpinBox(szHint.height(), &dialog);
    inputLayout->addWidget(szHintH, 0, 2);

    QSpinBox *minSzHintW = createSpinBox(minSzHint.width(), &dialog);
    inputLayout->addWidget(minSzHintW, 1, 1);
    QSpinBox *minSzHintH = createSpinBox(minSzHint.height(), &dialog);
    inputLayout->addWidget(minSzHintH, 1, 2);

    QSize maxSz = maximumSize();
    QSpinBox *maxSzW = createSpinBox(maxSz.width(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(maxSzW, 2, 1);
    QSpinBox *maxSzH = createSpinBox(maxSz.height(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(maxSzH, 2, 2);

    QSize dwMaxSz = parentWidget()->maximumSize();
    QSpinBox *dwMaxSzW = createSpinBox(dwMaxSz.width(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(dwMaxSzW, 3, 1);
    QSpinBox *dwMaxSzH = createSpinBox(dwMaxSz.height(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(dwMaxSzH, 3, 2);

    inputLayout->setColumnStretch(1, 1);
    inputLayout->setColumnStretch(2, 1);

    topLayout->addStretch();

    QHBoxLayout *buttonBox = new QHBoxLayout();
    topLayout->addLayout(buttonBox);

    QPushButton *okButton = new QPushButton(tr("Ok"), &dialog);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), &dialog);
    connect(okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    buttonBox->addStretch();
    buttonBox->addWidget(cancelButton);
    buttonBox->addWidget(okButton);


    if (!dialog.exec())
        return;

    szHint = QSize(szHintW->value(), szHintH->value());
    minSzHint = QSize(minSzHintW->value(), minSzHintH->value());
    maxSz = QSize(maxSzW->value(), maxSzH->value());
    setMaximumSize(maxSz);
    dwMaxSz = QSize(dwMaxSzW->value(), dwMaxSzH->value());
    parentWidget()->setMaximumSize(dwMaxSz);
    updateGeometry();
    update();
}

void ColorDock::setCustomSizeHint(const QSize &size)
{
    szHint = size;
    updateGeometry();
}

ColorSwatch::ColorSwatch(const QString &colorName, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    setObjectName(colorName + QLatin1String(" Dock Widget"));
    setWindowTitle(objectName() + QLatin1String(" [*]"));

    QFrame *swatch = new ColorDock(colorName, this);
    swatch->setFrameStyle(QFrame::Box | QFrame::Sunken);

    setWidget(swatch);

    changeSizeHintsAction = new QAction(tr("Change Size Hints"), this);
    connect(changeSizeHintsAction, SIGNAL(triggered()), swatch, SLOT(changeSizeHints()));

    closableAction = new QAction(tr("Closable"), this);
    closableAction->setCheckable(true);
    connect(closableAction, SIGNAL(triggered(bool)), SLOT(changeClosable(bool)));

    movableAction = new QAction(tr("Movable"), this);
    movableAction->setCheckable(true);
    connect(movableAction, SIGNAL(triggered(bool)), SLOT(changeMovable(bool)));

    floatableAction = new QAction(tr("Floatable"), this);
    floatableAction->setCheckable(true);
    connect(floatableAction, SIGNAL(triggered(bool)), SLOT(changeFloatable(bool)));

    verticalTitleBarAction = new QAction(tr("Vertical title bar"), this);
    verticalTitleBarAction->setCheckable(true);
    connect(verticalTitleBarAction, SIGNAL(triggered(bool)),
            SLOT(changeVerticalTitleBar(bool)));

    floatingAction = new QAction(tr("Floating"), this);
    floatingAction->setCheckable(true);
    connect(floatingAction, SIGNAL(triggered(bool)), SLOT(changeFloating(bool)));

    allowedAreasActions = new QActionGroup(this);
    allowedAreasActions->setExclusive(false);

    allowLeftAction = new QAction(tr("Allow on Left"), this);
    allowLeftAction->setCheckable(true);
    connect(allowLeftAction, SIGNAL(triggered(bool)), SLOT(allowLeft(bool)));

    allowRightAction = new QAction(tr("Allow on Right"), this);
    allowRightAction->setCheckable(true);
    connect(allowRightAction, SIGNAL(triggered(bool)), SLOT(allowRight(bool)));

    allowTopAction = new QAction(tr("Allow on Top"), this);
    allowTopAction->setCheckable(true);
    connect(allowTopAction, SIGNAL(triggered(bool)), SLOT(allowTop(bool)));

    allowBottomAction = new QAction(tr("Allow on Bottom"), this);
    allowBottomAction->setCheckable(true);
    connect(allowBottomAction, SIGNAL(triggered(bool)), SLOT(allowBottom(bool)));

    allowedAreasActions->addAction(allowLeftAction);
    allowedAreasActions->addAction(allowRightAction);
    allowedAreasActions->addAction(allowTopAction);
    allowedAreasActions->addAction(allowBottomAction);

    areaActions = new QActionGroup(this);
    areaActions->setExclusive(true);

    leftAction = new QAction(tr("Place on Left") , this);
    leftAction->setCheckable(true);
    connect(leftAction, SIGNAL(triggered(bool)), SLOT(placeLeft(bool)));

    rightAction = new QAction(tr("Place on Right") , this);
    rightAction->setCheckable(true);
    connect(rightAction, SIGNAL(triggered(bool)), SLOT(placeRight(bool)));

    topAction = new QAction(tr("Place on Top") , this);
    topAction->setCheckable(true);
    connect(topAction, SIGNAL(triggered(bool)), SLOT(placeTop(bool)));

    bottomAction = new QAction(tr("Place on Bottom") , this);
    bottomAction->setCheckable(true);
    connect(bottomAction, SIGNAL(triggered(bool)), SLOT(placeBottom(bool)));

    areaActions->addAction(leftAction);
    areaActions->addAction(rightAction);
    areaActions->addAction(topAction);
    areaActions->addAction(bottomAction);

    connect(movableAction, SIGNAL(triggered(bool)), areaActions, SLOT(setEnabled(bool)));

    connect(movableAction, SIGNAL(triggered(bool)), allowedAreasActions, SLOT(setEnabled(bool)));

    connect(floatableAction, SIGNAL(triggered(bool)), floatingAction, SLOT(setEnabled(bool)));

    connect(floatingAction, SIGNAL(triggered(bool)), floatableAction, SLOT(setDisabled(bool)));
    connect(movableAction, SIGNAL(triggered(bool)), floatableAction, SLOT(setEnabled(bool)));

    tabMenu = new QMenu(this);
    tabMenu->setTitle(tr("Tab into"));
    connect(tabMenu, SIGNAL(triggered(QAction*)), this, SLOT(tabInto(QAction*)));

    splitHMenu = new QMenu(this);
    splitHMenu->setTitle(tr("Split horizontally into"));
    connect(splitHMenu, SIGNAL(triggered(QAction*)), this, SLOT(splitInto(QAction*)));

    splitVMenu = new QMenu(this);
    splitVMenu->setTitle(tr("Split vertically into"));
    connect(splitVMenu, SIGNAL(triggered(QAction*)), this, SLOT(splitInto(QAction*)));

    windowModifiedAction = new QAction(tr("Modified"), this);
    windowModifiedAction->setCheckable(true);
    windowModifiedAction->setChecked(false);
    connect(windowModifiedAction, SIGNAL(toggled(bool)), this, SLOT(setWindowModified(bool)));

    menu = new QMenu(colorName, this);
    menu->addAction(toggleViewAction());
    QAction *action = menu->addAction(tr("Raise"));
    connect(action, SIGNAL(triggered()), this, SLOT(raise()));
    menu->addAction(changeSizeHintsAction);
    menu->addSeparator();
    menu->addAction(closableAction);
    menu->addAction(movableAction);
    menu->addAction(floatableAction);
    menu->addAction(floatingAction);
    menu->addAction(verticalTitleBarAction);
    menu->addSeparator();
    menu->addActions(allowedAreasActions->actions());
    menu->addSeparator();
    menu->addActions(areaActions->actions());
    menu->addSeparator();
    menu->addMenu(splitHMenu);
    menu->addMenu(splitVMenu);
    menu->addMenu(tabMenu);
    menu->addSeparator();
    menu->addAction(windowModifiedAction);

    connect(menu, SIGNAL(aboutToShow()), this, SLOT(updateContextMenu()));

    if(colorName == "Black") {
        leftAction->setShortcut(Qt::CTRL|Qt::Key_W);
        rightAction->setShortcut(Qt::CTRL|Qt::Key_E);
        toggleViewAction()->setShortcut(Qt::CTRL|Qt::Key_R);
    }
}

void ColorSwatch::updateContextMenu()
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
    const Qt::DockWidgetAreas areas = allowedAreas();

    closableAction->setChecked(features() & QDockWidget::DockWidgetClosable);
    if (windowType() == Qt::Drawer) {
        floatableAction->setEnabled(false);
        floatingAction->setEnabled(false);
        movableAction->setEnabled(false);
        verticalTitleBarAction->setChecked(false);
    } else {
        floatableAction->setChecked(features() & QDockWidget::DockWidgetFloatable);
        floatingAction->setChecked(isWindow());
        // done after floating, to get 'floatable' correctly initialized
        movableAction->setChecked(features() & QDockWidget::DockWidgetMovable);
        verticalTitleBarAction
            ->setChecked(features() & QDockWidget::DockWidgetVerticalTitleBar);
    }

    allowLeftAction->setChecked(isAreaAllowed(Qt::LeftDockWidgetArea));
    allowRightAction->setChecked(isAreaAllowed(Qt::RightDockWidgetArea));
    allowTopAction->setChecked(isAreaAllowed(Qt::TopDockWidgetArea));
    allowBottomAction->setChecked(isAreaAllowed(Qt::BottomDockWidgetArea));

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftDockWidgetArea);
        allowRightAction->setEnabled(area != Qt::RightDockWidgetArea);
        allowTopAction->setEnabled(area != Qt::TopDockWidgetArea);
        allowBottomAction->setEnabled(area != Qt::BottomDockWidgetArea);
    }

    leftAction->blockSignals(true);
    rightAction->blockSignals(true);
    topAction->blockSignals(true);
    bottomAction->blockSignals(true);

    leftAction->setChecked(area == Qt::LeftDockWidgetArea);
    rightAction->setChecked(area == Qt::RightDockWidgetArea);
    topAction->setChecked(area == Qt::TopDockWidgetArea);
    bottomAction->setChecked(area == Qt::BottomDockWidgetArea);

    leftAction->blockSignals(false);
    rightAction->blockSignals(false);
    topAction->blockSignals(false);
    bottomAction->blockSignals(false);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftDockWidgetArea);
        rightAction->setEnabled(areas & Qt::RightDockWidgetArea);
        topAction->setEnabled(areas & Qt::TopDockWidgetArea);
        bottomAction->setEnabled(areas & Qt::BottomDockWidgetArea);
    }

    tabMenu->clear();
    splitHMenu->clear();
    splitVMenu->clear();
    QList<ColorSwatch*> dock_list = qFindChildren<ColorSwatch*>(mainWindow);
    foreach (ColorSwatch *dock, dock_list) {
//        if (!dock->isVisible() || dock->isFloating())
//            continue;
        tabMenu->addAction(dock->objectName());
        splitHMenu->addAction(dock->objectName());
        splitVMenu->addAction(dock->objectName());
    }
}

void ColorSwatch::splitInto(QAction *action)
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    QList<ColorSwatch*> dock_list = qFindChildren<ColorSwatch*>(mainWindow);
    ColorSwatch *target = 0;
    foreach (ColorSwatch *dock, dock_list) {
        if (action->text() == dock->objectName()) {
            target = dock;
            break;
        }
    }
    if (target == 0)
        return;

    Qt::Orientation o = action->parent() == splitHMenu
                        ? Qt::Horizontal : Qt::Vertical;
    mainWindow->splitDockWidget(target, this, o);
}

void ColorSwatch::tabInto(QAction *action)
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    QList<ColorSwatch*> dock_list = qFindChildren<ColorSwatch*>(mainWindow);
    ColorSwatch *target = 0;
    foreach (ColorSwatch *dock, dock_list) {
        if (action->text() == dock->objectName()) {
            target = dock;
            break;
        }
    }
    if (target == 0)
        return;

    mainWindow->tabifyDockWidget(target, this);
}

void ColorSwatch::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    menu->exec(event->globalPos());
}

void ColorSwatch::allow(Qt::DockWidgetArea area, bool a)
{
    Qt::DockWidgetAreas areas = allowedAreas();
    areas = a ? areas | area : areas & ~area;
    setAllowedAreas(areas);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftDockWidgetArea);
        rightAction->setEnabled(areas & Qt::RightDockWidgetArea);
        topAction->setEnabled(areas & Qt::TopDockWidgetArea);
        bottomAction->setEnabled(areas & Qt::BottomDockWidgetArea);
    }
}

void ColorSwatch::place(Qt::DockWidgetArea area, bool p)
{
    if (!p) return;

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    mainWindow->addDockWidget(area, this);

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftDockWidgetArea);
        allowRightAction->setEnabled(area != Qt::RightDockWidgetArea);
        allowTopAction->setEnabled(area != Qt::TopDockWidgetArea);
        allowBottomAction->setEnabled(area != Qt::BottomDockWidgetArea);
    }
}

void ColorSwatch::setCustomSizeHint(const QSize &size)
{
    if (ColorDock *dock = qobject_cast<ColorDock*>(widget()))
        dock->setCustomSizeHint(size);
}

void ColorSwatch::changeClosable(bool on)
{ setFeatures(on ? features() | DockWidgetClosable : features() & ~DockWidgetClosable); }

void ColorSwatch::changeMovable(bool on)
{ setFeatures(on ? features() | DockWidgetMovable : features() & ~DockWidgetMovable); }

void ColorSwatch::changeFloatable(bool on)
{ setFeatures(on ? features() | DockWidgetFloatable : features() & ~DockWidgetFloatable); }

void ColorSwatch::changeFloating(bool floating)
{ setFloating(floating); }

void ColorSwatch::allowLeft(bool a)
{ allow(Qt::LeftDockWidgetArea, a); }

void ColorSwatch::allowRight(bool a)
{ allow(Qt::RightDockWidgetArea, a); }

void ColorSwatch::allowTop(bool a)
{ allow(Qt::TopDockWidgetArea, a); }

void ColorSwatch::allowBottom(bool a)
{ allow(Qt::BottomDockWidgetArea, a); }

void ColorSwatch::placeLeft(bool p)
{ place(Qt::LeftDockWidgetArea, p); }

void ColorSwatch::placeRight(bool p)
{ place(Qt::RightDockWidgetArea, p); }

void ColorSwatch::placeTop(bool p)
{ place(Qt::TopDockWidgetArea, p); }

void ColorSwatch::placeBottom(bool p)
{ place(Qt::BottomDockWidgetArea, p); }

void ColorSwatch::changeVerticalTitleBar(bool on)
{
    setFeatures(on ? features() | DockWidgetVerticalTitleBar
                    : features() & ~DockWidgetVerticalTitleBar);
}

#include "colorswatch.moc"
