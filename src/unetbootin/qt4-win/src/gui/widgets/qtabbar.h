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

#ifndef QTABBAR_H
#define QTABBAR_H

#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_TABBAR

class QIcon;
class QTabBarPrivate;
class QStyleOptionTab;

class Q_GUI_EXPORT QTabBar: public QWidget
{
    Q_OBJECT

    Q_ENUMS(Shape)
    Q_PROPERTY(Shape shape READ shape WRITE setShape)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool drawBase READ drawBase WRITE setDrawBase)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(Qt::TextElideMode elideMode READ elideMode WRITE setElideMode)
    Q_PROPERTY(bool usesScrollButtons READ usesScrollButtons WRITE setUsesScrollButtons)

public:
    explicit QTabBar(QWidget* parent=0);
    ~QTabBar();

    enum Shape { RoundedNorth, RoundedSouth, RoundedWest, RoundedEast,
                 TriangularNorth, TriangularSouth, TriangularWest, TriangularEast
#if defined(QT3_SUPPORT) && !defined(Q_MOC_RUN)
                , RoundedAbove = RoundedNorth, RoundedBelow = RoundedSouth,
                TriangularAbove = TriangularNorth, TriangularBelow = TriangularSouth
#endif
    };

    Shape shape() const;
    void setShape(Shape shape);

    int addTab(const QString &text);
    int addTab(const QIcon &icon, const QString &text);

    int insertTab(int index, const QString &text);
    int insertTab(int index, const QIcon&icon, const QString &text);

    void removeTab(int index);

    bool isTabEnabled(int index) const;
    void setTabEnabled(int index, bool);

    QString tabText(int index) const;
    void setTabText(int index, const QString &text);

    QColor tabTextColor(int index) const;
    void setTabTextColor(int index, const QColor &color);

    QIcon tabIcon(int index) const;
    void setTabIcon(int index, const QIcon &icon);

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode);

#ifndef QT_NO_TOOLTIP
    void setTabToolTip(int index, const QString &tip);
    QString tabToolTip(int index) const;
#endif

#ifndef QT_NO_WHATSTHIS
    void setTabWhatsThis(int index, const QString &text);
    QString tabWhatsThis(int index) const;
#endif

    void setTabData(int index, const QVariant &data);
    QVariant tabData(int index) const;

    QRect tabRect(int index) const;
    int tabAt(const QPoint &pos) const;


    int currentIndex() const;
    int count() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setDrawBase(bool drawTheBase);
    bool drawBase() const;

    QSize iconSize() const;
    void setIconSize(const QSize &size);

    bool usesScrollButtons() const;
    void setUsesScrollButtons(bool useButtons);

public Q_SLOTS:
    void setCurrentIndex(int index);

Q_SIGNALS:
    void currentChanged(int index);

protected:
    virtual QSize tabSizeHint(int index) const;
    virtual void tabInserted(int index);
    virtual void tabRemoved(int index);
    virtual void tabLayoutChange();

    bool event(QEvent *);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void mouseReleaseEvent (QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void changeEvent(QEvent *);
    void initStyleOption(QStyleOptionTab *option, int tabIndex) const;

#ifdef QT3_SUPPORT
public Q_SLOTS:
    QT_MOC_COMPAT void setCurrentTab(int index) { setCurrentIndex(index); }
Q_SIGNALS:
    QT_MOC_COMPAT void selected(int);
#endif

    friend class QAccessibleTabBar;
private:
    Q_DISABLE_COPY(QTabBar)
    Q_DECLARE_PRIVATE(QTabBar)
    Q_PRIVATE_SLOT(d_func(), void _q_scrollTabs())
};

#endif // QT_NO_TABBAR

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTABBAR_H
