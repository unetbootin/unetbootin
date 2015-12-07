/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtSVG module of the Qt Toolkit.
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

#ifndef QSVGTINYDOCUMENT_P_H
#define QSVGTINYDOCUMENT_P_H

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

#include "qsvgstructure_p.h"

#ifndef QT_NO_SVG

#include "QtCore/qrect.h"
#include "QtCore/qlist.h"
#include "QtCore/qhash.h"
#include "QtCore/qdatetime.h"
#include "QtCore/qxmlstream.h"
#include "qsvgstyle_p.h"
#include "qsvgfont_p.h"

QT_BEGIN_NAMESPACE

class QPainter;
class QByteArray;
class QSvgFont;

class Q_SVG_EXPORT QSvgTinyDocument : public QSvgStructureNode
{
public:
    static QSvgTinyDocument * load(const QString &file);
    static QSvgTinyDocument * load(const QByteArray &contents);
    static QSvgTinyDocument * load(QXmlStreamReader *contents);
public:
    QSvgTinyDocument();
    ~QSvgTinyDocument();
    Type type() const;

    QSize size() const;
    void setWidth(int len, bool percent);
    void setHeight(int len, bool percent);
    int width() const;
    int height() const;
    bool widthPercent() const;
    bool heightPercent() const;

    bool preserveAspectRatio() const;

    QRectF viewBox() const;
    void setViewBox(const QRectF &rect);

    virtual void draw(QPainter *p, QSvgExtraStates &);//from the QSvgNode

    void draw(QPainter *p);
    void draw(QPainter *p, const QRectF &bounds);
    void draw(QPainter *p, const QString &id,
              const QRectF &bounds=QRectF());

    QMatrix matrixForElement(const QString &id) const;
    QRectF boundsOnElement(const QString &id) const;
    bool   elementExists(const QString &id) const;

    void addSvgFont(QSvgFont *);
    QSvgFont *svgFont(const QString &family) const;

    void restartAnimation();
    int currentElapsed() const;
    bool animated() const;
    void setAnimated(bool a);
    int animationDuration() const;
    int currentFrame() const;
    void setCurrentFrame(int);
    void setFramesPerSecond(int num);
private:
    void mapSourceToTarget(QPainter *p, const QRectF &targetRect, const QRectF &sourceRect = QRectF());
private:
    QSize  m_size;
    bool   m_widthPercent;
    bool   m_heightPercent;

    mutable QRectF m_viewBox;

    QHash<QString, QSvgRefCounter<QSvgFont> > m_fonts;

    QTime m_time;
    bool  m_animated;
    int   m_animationDuration;
    int   m_fps;

    QSvgExtraStates m_states;
};

inline QSize QSvgTinyDocument::size() const
{
    if (m_size.isEmpty()) {
        return viewBox().size().toSize();
    } else {
        return m_size;
    }
}

inline int QSvgTinyDocument::width() const
{
    return size().width();
}

inline int QSvgTinyDocument::height() const
{
    return size().height();
}

inline bool QSvgTinyDocument::widthPercent() const
{
    return m_widthPercent;
}

inline bool QSvgTinyDocument::heightPercent() const
{
    return m_heightPercent;
}

inline QRectF QSvgTinyDocument::viewBox() const
{
    if (m_viewBox.isNull()) {
        m_viewBox = transformedBounds(QTransform());
    }

    return m_viewBox;
}

inline bool QSvgTinyDocument::preserveAspectRatio() const
{
    return false;
}

inline int QSvgTinyDocument::currentElapsed() const
{
    return m_time.elapsed();
}

inline int QSvgTinyDocument::animationDuration() const
{
    return m_animationDuration;
}

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGTINYDOCUMENT_P_H
