/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSVG module of the Qt Toolkit.
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

    virtual void draw(QPainter *p);//from the QSvgNode

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
        m_viewBox = transformedBounds(QMatrix());
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
