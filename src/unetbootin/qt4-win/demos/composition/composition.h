/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

#ifndef COMPOSITION_H
#define COMPOSITION_H

#include "arthurwidgets.h"

#include <QPainter>
#include <QEvent>

QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QRadioButton)

#ifdef QT_OPENGL_SUPPORT
#include <QtOpenGL>
#endif

class CompositionWidget : public QWidget
{
    Q_OBJECT

public:
    CompositionWidget(QWidget *parent);

public slots:
void nextMode();

private:
    bool m_cycle_enabled;

    QRadioButton *rbClear;
    QRadioButton *rbSource;
    QRadioButton *rbDest;
    QRadioButton *rbSourceOver;
    QRadioButton *rbDestOver;
    QRadioButton *rbSourceIn;
    QRadioButton *rbDestIn;
    QRadioButton *rbSourceOut;
    QRadioButton *rbDestOut;
    QRadioButton *rbSourceAtop;
    QRadioButton *rbDestAtop;
    QRadioButton *rbXor;

    QRadioButton *rbPlus;
    QRadioButton *rbMultiply;
    QRadioButton *rbScreen;
    QRadioButton *rbOverlay;
    QRadioButton *rbDarken;
    QRadioButton *rbLighten;
    QRadioButton *rbColorDodge;
    QRadioButton *rbColorBurn;
    QRadioButton *rbHardLight;
    QRadioButton *rbSoftLight;
    QRadioButton *rbDifference;
    QRadioButton *rbExclusion;
};

class CompositionRenderer : public ArthurFrame
{
    Q_OBJECT

    enum ObjectType { NoObject, Circle, Rectangle, Image };

    Q_PROPERTY(int circleColor READ circleColor WRITE setCircleColor)
    Q_PROPERTY(int circleAlpha READ circleAlpha WRITE setCircleAlpha)
    Q_PROPERTY(bool animation READ animationEnabled WRITE setAnimationEnabled)

public:
    CompositionRenderer(QWidget *parent);

    void paint(QPainter *);

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void setCirclePos(const QPointF &pos);

    QSize sizeHint() const { return QSize(500, 400); }

    bool animationEnabled() const { return m_animation_enabled; }
    int circleColor() const { return m_circle_hue; }
    int circleAlpha() const { return m_circle_alpha; }

public slots:
    void setClearMode() { m_composition_mode = QPainter::CompositionMode_Clear; update(); }
    void setSourceMode() { m_composition_mode = QPainter::CompositionMode_Source; update(); }
    void setDestMode() { m_composition_mode = QPainter::CompositionMode_Destination; update(); }
    void setSourceOverMode() { m_composition_mode = QPainter::CompositionMode_SourceOver; update(); }
    void setDestOverMode() { m_composition_mode = QPainter::CompositionMode_DestinationOver; update(); }
    void setSourceInMode() { m_composition_mode = QPainter::CompositionMode_SourceIn; update(); }
    void setDestInMode() { m_composition_mode = QPainter::CompositionMode_DestinationIn; update(); }
    void setSourceOutMode() { m_composition_mode = QPainter::CompositionMode_SourceOut; update(); }
    void setDestOutMode() { m_composition_mode = QPainter::CompositionMode_DestinationOut; update(); }
    void setSourceAtopMode() { m_composition_mode = QPainter::CompositionMode_SourceAtop; update(); }
    void setDestAtopMode() { m_composition_mode = QPainter::CompositionMode_DestinationAtop; update(); }
    void setXorMode() { m_composition_mode = QPainter::CompositionMode_Xor; update(); }

    void setPlusMode() { m_composition_mode = QPainter::CompositionMode_Plus; update(); }
    void setMultiplyMode() { m_composition_mode = QPainter::CompositionMode_Multiply; update(); }
    void setScreenMode() { m_composition_mode = QPainter::CompositionMode_Screen; update(); }
    void setOverlayMode() { m_composition_mode = QPainter::CompositionMode_Overlay; update(); }
    void setDarkenMode() { m_composition_mode = QPainter::CompositionMode_Darken; update(); }
    void setLightenMode() { m_composition_mode = QPainter::CompositionMode_Lighten; update(); }
    void setColorDodgeMode() { m_composition_mode = QPainter::CompositionMode_ColorDodge; update(); }
    void setColorBurnMode() { m_composition_mode = QPainter::CompositionMode_ColorBurn; update(); }
    void setHardLightMode() { m_composition_mode = QPainter::CompositionMode_HardLight; update(); }
    void setSoftLightMode() { m_composition_mode = QPainter::CompositionMode_SoftLight; update(); }
    void setDifferenceMode() { m_composition_mode = QPainter::CompositionMode_Difference; update(); }
    void setExclusionMode() { m_composition_mode = QPainter::CompositionMode_Exclusion; update(); }

    void setCircleAlpha(int alpha) { m_circle_alpha = alpha; update(); }
    void setCircleColor(int hue) { m_circle_hue = hue; update(); }
    void setAnimationEnabled(bool enabled) { m_animation_enabled = enabled; update(); }

private:
    void updateCirclePos();
    void drawBase(QPainter &p);
    void drawSource(QPainter &p);

    QPainter::CompositionMode m_composition_mode;

#ifdef Q_WS_QWS
    QPixmap m_image;
    QPixmap m_buffer;
    QPixmap m_base_buffer;
#else
    QImage m_image;
    QImage m_buffer;
    QImage m_base_buffer;
#endif

    int m_circle_alpha;
    int m_circle_hue;

    QPointF m_circle_pos;
    QPointF m_offset;

    ObjectType m_current_object;
    bool m_animation_enabled;

#ifdef QT_OPENGL_SUPPORT
    QGLPixelBuffer *m_pbuffer;
    GLuint m_base_tex;
    GLuint m_compositing_tex;
    int m_pbuffer_size; // width==height==size of pbuffer
    QSize m_previous_size;
#endif
};

#endif // COMPOSITION_H
