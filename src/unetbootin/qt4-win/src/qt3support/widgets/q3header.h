/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3HEADER_H
#define Q3HEADER_H

#include <QtGui/qicon.h>
#include <QtGui/qwidget.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_HEADER

class QShowEvent;
class Q3HeaderData;
class Q3Table;
class Q3ListView;

class Q_COMPAT_EXPORT Q3Header : public QWidget
{
    friend class Q3Table;
    friend class Q3TableHeader;
    friend class Q3ListView;

    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(bool tracking READ tracking WRITE setTracking)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(bool moving READ isMovingEnabled WRITE setMovingEnabled)
    Q_PROPERTY(bool stretching READ isStretchEnabled WRITE setStretchEnabled)

public:
    Q3Header(QWidget* parent=0, const char* name=0);
    Q3Header(int, QWidget* parent=0, const char* name=0);
    ~Q3Header();

    int                addLabel(const QString &, int size = -1);
    int                addLabel(const QIcon&, const QString &, int size = -1);
    void         removeLabel(int section);
    virtual void setLabel(int, const QString &, int size = -1);
    virtual void setLabel(int, const QIcon&, const QString &, int size = -1);
    QString         label(int section) const;
    QIcon*         iconSet(int section) const;

    virtual void setOrientation(Qt::Orientation);
    Qt::Orientation orientation() const;
    virtual void setTracking(bool enable);
    bool        tracking() const;

    virtual void setClickEnabled(bool, int section = -1);
    virtual void setResizeEnabled(bool, int section = -1);
    virtual void setMovingEnabled(bool);
    virtual void setStretchEnabled(bool b, int section);
    void         setStretchEnabled(bool b) { setStretchEnabled(b, -1); }
    bool         isClickEnabled(int section = -1) const;
    bool         isResizeEnabled(int section = -1) const;
    bool         isMovingEnabled() const;
    bool         isStretchEnabled() const;
    bool         isStretchEnabled(int section) const;

    void         resizeSection(int section, int s);
    int                sectionSize(int section) const;
    int                sectionPos(int section) const;
    int                sectionAt(int pos) const;
    int                count() const;
    int         headerWidth() const;
    QRect        sectionRect(int section) const;

    virtual void setCellSize(int , int); // obsolete, do not use
    int                cellSize(int i) const { return sectionSize(mapToSection(i)); } // obsolete, do not use
    int                cellPos(int) const; // obsolete, do not use
    int                cellAt(int pos) const { return mapToIndex(sectionAt(pos + offset())); } // obsolete, do not use

    int         offset() const;

    QSize        sizeHint() const;

    int                mapToSection(int index) const;
    int                mapToIndex(int section) const;
    int                mapToLogical(int) const; // obsolete, do not use
    int                mapToActual(int) const; // obsolete, do not use

    void         moveSection(int section, int toIndex);
    virtual void moveCell(int, int); // obsolete, do not use

    void         setSortIndicator(int section, bool ascending = true); // obsolete, do not use
    inline void setSortIndicator(int section, Qt::SortOrder order)
        { setSortIndicator(section, (order == Qt::AscendingOrder)); }
    int                sortIndicatorSection() const;
    Qt::SortOrder        sortIndicatorOrder() const;

    void        adjustHeaderSize() { adjustHeaderSize(-1); }

public Q_SLOTS:
    void         setUpdatesEnabled(bool enable);
    virtual void setOffset(int pos);

Q_SIGNALS:
    void        clicked(int section);
    void        pressed(int section);
    void        released(int section);
    void        sizeChange(int section, int oldSize, int newSize);
    void        indexChange(int section, int fromIndex, int toIndex);
    void        sectionClicked(int); // obsolete, do not use
    void        moved(int, int); // obsolete, do not use
    void        sectionHandleDoubleClicked(int section);

protected:
    void        paintEvent(QPaintEvent *);
    void        showEvent(QShowEvent *e);
    void         resizeEvent(QResizeEvent *e);
    QRect        sRect(int index);

    virtual void paintSection(QPainter *p, int index, const QRect& fr);
    virtual void paintSectionLabel(QPainter* p, int index, const QRect& fr);

    void        changeEvent(QEvent *);
    void        mousePressEvent(QMouseEvent *);
    void        mouseReleaseEvent(QMouseEvent *);
    void        mouseMoveEvent(QMouseEvent *);
    void        mouseDoubleClickEvent(QMouseEvent *);

    void        keyPressEvent(QKeyEvent *);
    void        keyReleaseEvent(QKeyEvent *);

private:
    void        handleColumnMove(int fromIdx, int toIdx);
    void         adjustHeaderSize(int diff);
    void        init(int);

    void        paintRect(int p, int s);
    void        markLine(int idx);
    void        unMarkLine(int idx);
    int                pPos(int i) const;
    int                pSize(int i) const;
    int         findLine(int);
    int                handleAt(int p);
    bool         reverse() const;
    void         calculatePositions(bool onlyVisible = false, int start = 0);
    void        handleColumnResize(int, int, bool, bool = true);
    QSize        sectionSizeHint(int section, const QFontMetrics& fm) const;
    void        setSectionSizeAndHeight(int section, int size);

    void         resizeArrays(int size);
    void         setIsATableHeader(bool b);
    int                offs;
    int                handleIdx;
    int                oldHIdxSize;
    int                moveToIdx;
    enum State { Idle, Sliding, Pressed, Moving, Blocked };
    State        state;
    int        clickPos;
    bool        trackingIsOn;
    int oldHandleIdx;
    int        cachedPos; // not used
    Qt::Orientation orient;

    Q3HeaderData *d;

private:
    Q_DISABLE_COPY(Q3Header)
};


inline Qt::Orientation Q3Header::orientation() const
{
    return orient;
}

inline void Q3Header::setTracking(bool enable) { trackingIsOn = enable; }
inline bool Q3Header::tracking() const { return trackingIsOn; }

extern Q_COMPAT_EXPORT bool qt_qheader_label_return_null_strings; // needed for professional edition

#endif // QT_NO_HEADER

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3HEADER_H
