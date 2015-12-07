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

#include "composition.h"
#include <QBoxLayout>
#include <QRadioButton>
#include <QTimer>
#include <QDateTime>
#include <QSlider>
#include <QMouseEvent>
#include <qmath.h>

CompositionWidget::CompositionWidget(QWidget *parent)
    : QWidget(parent)
{
    CompositionRenderer *view = new CompositionRenderer(this);

    QGroupBox *mainGroup = new QGroupBox(parent);
    mainGroup->setAttribute(Qt::WA_ContentsPropagated);
    mainGroup->setTitle("Composition Modes");

    QGroupBox *modesGroup = new QGroupBox(mainGroup);
    modesGroup->setAttribute(Qt::WA_ContentsPropagated);
    modesGroup->setTitle("Mode");

    rbClear = new QRadioButton("Clear", modesGroup);
    connect(rbClear, SIGNAL(clicked()), view, SLOT(setClearMode()));
    rbSource = new QRadioButton("Source", modesGroup);
    connect(rbSource, SIGNAL(clicked()), view, SLOT(setSourceMode()));
    rbDest = new QRadioButton("Destination", modesGroup);
    connect(rbDest, SIGNAL(clicked()), view, SLOT(setDestMode()));
    rbSourceOver = new QRadioButton("Source Over", modesGroup);
    connect(rbSourceOver, SIGNAL(clicked()), view, SLOT(setSourceOverMode()));
    rbDestOver = new QRadioButton("Destination Over", modesGroup);
    connect(rbDestOver, SIGNAL(clicked()), view, SLOT(setDestOverMode()));
    rbSourceIn = new QRadioButton("Source In", modesGroup);
    connect(rbSourceIn, SIGNAL(clicked()), view, SLOT(setSourceInMode()));
    rbDestIn = new QRadioButton("Dest In", modesGroup);
    connect(rbDestIn, SIGNAL(clicked()), view, SLOT(setDestInMode()));
    rbSourceOut = new QRadioButton("Source Out", modesGroup);
    connect(rbSourceOut, SIGNAL(clicked()), view, SLOT(setSourceOutMode()));
    rbDestOut = new QRadioButton("Dest Out", modesGroup);
    connect(rbDestOut, SIGNAL(clicked()), view, SLOT(setDestOutMode()));
    rbSourceAtop = new QRadioButton("Source Atop", modesGroup);
    connect(rbSourceAtop, SIGNAL(clicked()), view, SLOT(setSourceAtopMode()));
    rbDestAtop = new QRadioButton("Dest Atop", modesGroup);
    connect(rbDestAtop, SIGNAL(clicked()), view, SLOT(setDestAtopMode()));
    rbXor = new QRadioButton("Xor", modesGroup);
    connect(rbXor, SIGNAL(clicked()), view, SLOT(setXorMode()));

    rbPlus = new QRadioButton("Plus", modesGroup);
    connect(rbPlus, SIGNAL(clicked()), view, SLOT(setPlusMode()));
    rbMultiply = new QRadioButton("Multiply", modesGroup);
    connect(rbMultiply, SIGNAL(clicked()), view, SLOT(setMultiplyMode()));
    rbScreen = new QRadioButton("Screen", modesGroup);
    connect(rbScreen, SIGNAL(clicked()), view, SLOT(setScreenMode()));
    rbOverlay = new QRadioButton("Overlay", modesGroup);
    connect(rbOverlay, SIGNAL(clicked()), view, SLOT(setOverlayMode()));
    rbDarken = new QRadioButton("Darken", modesGroup);
    connect(rbDarken, SIGNAL(clicked()), view, SLOT(setDarkenMode()));
    rbLighten = new QRadioButton("Lighten", modesGroup);
    connect(rbLighten, SIGNAL(clicked()), view, SLOT(setLightenMode()));
    rbColorDodge = new QRadioButton("Color Dodge", modesGroup);
    connect(rbColorDodge, SIGNAL(clicked()), view, SLOT(setColorDodgeMode()));
    rbColorBurn = new QRadioButton("Color Burn", modesGroup);
    connect(rbColorBurn, SIGNAL(clicked()), view, SLOT(setColorBurnMode()));
    rbHardLight = new QRadioButton("Hard Light", modesGroup);
    connect(rbHardLight, SIGNAL(clicked()), view, SLOT(setHardLightMode()));
    rbSoftLight = new QRadioButton("Soft Light", modesGroup);
    connect(rbSoftLight, SIGNAL(clicked()), view, SLOT(setSoftLightMode()));
    rbDifference = new QRadioButton("Difference", modesGroup);
    connect(rbDifference, SIGNAL(clicked()), view, SLOT(setDifferenceMode()));
    rbExclusion = new QRadioButton("Exclusion", modesGroup);
    connect(rbExclusion, SIGNAL(clicked()), view, SLOT(setExclusionMode()));

    QGroupBox *circleColorGroup = new QGroupBox(mainGroup);
    circleColorGroup->setAttribute(Qt::WA_ContentsPropagated);
    circleColorGroup->setTitle("Circle color");
    QSlider *circleColorSlider = new QSlider(Qt::Horizontal, circleColorGroup);
    circleColorSlider->setRange(0, 359);
    circleColorSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(circleColorSlider, SIGNAL(valueChanged(int)), view, SLOT(setCircleColor(int)));

    QGroupBox *circleAlphaGroup = new QGroupBox(mainGroup);
    circleAlphaGroup->setAttribute(Qt::WA_ContentsPropagated);
    circleAlphaGroup->setTitle("Circle alpha");
    QSlider *circleAlphaSlider = new QSlider(Qt::Horizontal, circleAlphaGroup);
    circleAlphaSlider->setRange(0, 255);
    circleAlphaSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(circleAlphaSlider, SIGNAL(valueChanged(int)), view, SLOT(setCircleAlpha(int)));

    QPushButton *showSourceButton = new QPushButton(mainGroup);
    showSourceButton->setText("Show Source");
#ifdef QT_OPENGL_SUPPORT
    QPushButton *enableOpenGLButton = new QPushButton(mainGroup);
    enableOpenGLButton->setText("Use OpenGL");
    enableOpenGLButton->setCheckable(true);
    enableOpenGLButton->setChecked(view->usesOpenGL());

    if (!QGLFormat::hasOpenGL() || !QGLPixelBuffer::hasOpenGLPbuffers())
        enableOpenGLButton->hide();
#endif
    QPushButton *whatsThisButton = new QPushButton(mainGroup);
    whatsThisButton->setText("What's This?");
    whatsThisButton->setCheckable(true);

    QPushButton *animateButton = new QPushButton(mainGroup);
    animateButton->setText("Animated");
    animateButton->setCheckable(true);
    animateButton->setChecked(true);

    QHBoxLayout *viewLayout = new QHBoxLayout(this);
    viewLayout->addWidget(view);
    viewLayout->addWidget(mainGroup);

    QVBoxLayout *mainGroupLayout = new QVBoxLayout(mainGroup);
    mainGroupLayout->addWidget(circleColorGroup);
    mainGroupLayout->addWidget(circleAlphaGroup);
    mainGroupLayout->addWidget(modesGroup);
    mainGroupLayout->addStretch();
    mainGroupLayout->addWidget(animateButton);
    mainGroupLayout->addWidget(whatsThisButton);
    mainGroupLayout->addWidget(showSourceButton);
#ifdef QT_OPENGL_SUPPORT
    mainGroupLayout->addWidget(enableOpenGLButton);
#endif

    QGridLayout *modesLayout = new QGridLayout(modesGroup);
    modesLayout->addWidget(rbClear,             0,      0);
    modesLayout->addWidget(rbSource,            1,      0);
    modesLayout->addWidget(rbDest,              2,      0);
    modesLayout->addWidget(rbSourceOver,        3,      0);
    modesLayout->addWidget(rbDestOver,          4,      0);
    modesLayout->addWidget(rbSourceIn,          5,      0);
    modesLayout->addWidget(rbDestIn,            6,      0);
    modesLayout->addWidget(rbSourceOut,         7,      0);
    modesLayout->addWidget(rbDestOut,           8,      0);
    modesLayout->addWidget(rbSourceAtop,        9,      0);
    modesLayout->addWidget(rbDestAtop,         10,      0);
    modesLayout->addWidget(rbXor,              11,      0);

    modesLayout->addWidget(rbPlus,              0,      1);
    modesLayout->addWidget(rbMultiply,          1,      1);
    modesLayout->addWidget(rbScreen,            2,      1);
    modesLayout->addWidget(rbOverlay,           3,      1);
    modesLayout->addWidget(rbDarken,            4,      1);
    modesLayout->addWidget(rbLighten,           5,      1);
    modesLayout->addWidget(rbColorDodge,        6,      1);
    modesLayout->addWidget(rbColorBurn,         7,      1);
    modesLayout->addWidget(rbHardLight,         8,      1);
    modesLayout->addWidget(rbSoftLight,         9,      1);
    modesLayout->addWidget(rbDifference,       10,      1);
    modesLayout->addWidget(rbExclusion,        11,      1);


    QVBoxLayout *circleColorLayout = new QVBoxLayout(circleColorGroup);
    circleColorLayout->addWidget(circleColorSlider);

    QVBoxLayout *circleAlphaLayout = new QVBoxLayout(circleAlphaGroup);
    circleAlphaLayout->addWidget(circleAlphaSlider);

    view->loadDescription(":/trolltech/arthurplugin/composition.html");
    view->loadSourceFile(":/trolltech/arthurplugin/composition.cpp");

    connect(whatsThisButton, SIGNAL(clicked(bool)), view, SLOT(setDescriptionEnabled(bool)));
    connect(view, SIGNAL(descriptionEnabledChanged(bool)), whatsThisButton, SLOT(setChecked(bool)));
    connect(showSourceButton, SIGNAL(clicked()), view, SLOT(showSource()));
#ifdef QT_OPENGL_SUPPORT
    connect(enableOpenGLButton, SIGNAL(clicked(bool)), view, SLOT(enableOpenGL(bool)));
#endif
    connect(animateButton, SIGNAL(toggled(bool)), view, SLOT(setAnimationEnabled(bool)));

    circleColorSlider->setValue(270);
    circleAlphaSlider->setValue(200);
    rbSourceOut->animateClick();

    setWindowTitle(tr("Composition Modes"));
}


void CompositionWidget::nextMode()
{
    /*
      if (!m_animation_enabled)
      return;
      if (rbClear->isChecked()) rbSource->animateClick();
      if (rbSource->isChecked()) rbDest->animateClick();
      if (rbDest->isChecked()) rbSourceOver->animateClick();
      if (rbSourceOver->isChecked()) rbDestOver->animateClick();
      if (rbDestOver->isChecked()) rbSourceIn->animateClick();
      if (rbSourceIn->isChecked()) rbDestIn->animateClick();
      if (rbDestIn->isChecked()) rbSourceOut->animateClick();
      if (rbSourceOut->isChecked()) rbDestOut->animateClick();
      if (rbDestOut->isChecked()) rbSourceAtop->animateClick();
      if (rbSourceAtop->isChecked()) rbDestAtop->animateClick();
      if (rbDestAtop->isChecked()) rbXor->animateClick();
      if (rbXor->isChecked()) rbClear->animateClick();
    */
}

CompositionRenderer::CompositionRenderer(QWidget *parent)
    : ArthurFrame(parent)
{
    m_animation_enabled = true;
#ifdef Q_WS_QWS
    m_image = QPixmap(":/trolltech/arthurplugin/flower.jpg");
    m_image.setAlphaChannel(QPixmap(":/trolltech/arthurplugin/flower_alpha.jpg"));
#else
    m_image = QImage(":/trolltech/arthurplugin/flower.jpg");
    m_image.setAlphaChannel(QImage(":/trolltech/arthurplugin/flower_alpha.jpg"));
#endif
    m_circle_alpha = 127;
    m_circle_hue = 255;
    m_current_object = NoObject;
    m_composition_mode = QPainter::CompositionMode_SourceOut;

    m_circle_pos = QPoint(200, 100);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#ifdef QT_OPENGL_SUPPORT
    m_pbuffer = 0;
    m_pbuffer_size = 1024;
#endif
}

QRectF rectangle_around(const QPointF &p, const QSizeF &size = QSize(250, 200))
{
    QRectF rect(p, size);
    rect.translate(-size.width()/2, -size.height()/2);
    return rect;
}

void CompositionRenderer::updateCirclePos()
{
    if (m_current_object != NoObject)
        return;
    QDateTime dt = QDateTime::currentDateTime();
    qreal t = (dt.toTime_t() * 1000 + dt.time().msec()) / 1000.0;

    qreal x = width() / qreal(2) + (qCos(t*8/11) + qSin(-t)) * width() / qreal(4);
    qreal y = height() / qreal(2) + (qSin(t*6/7) + qCos(t * qreal(1.5))) * height() / qreal(4);

    setCirclePos(QLineF(m_circle_pos, QPointF(x, y)).pointAt(0.02));
}

void CompositionRenderer::drawBase(QPainter &p)
{
    p.setPen(Qt::NoPen);

    QLinearGradient rect_gradient(0, 0, 0, height());
    rect_gradient.setColorAt(0, Qt::red);
    rect_gradient.setColorAt(.17, Qt::yellow);
    rect_gradient.setColorAt(.33, Qt::green);
    rect_gradient.setColorAt(.50, Qt::cyan);
    rect_gradient.setColorAt(.66, Qt::blue);
    rect_gradient.setColorAt(.81, Qt::magenta);
    rect_gradient.setColorAt(1, Qt::red);
    p.setBrush(rect_gradient);
    p.drawRect(width() / 2, 0, width() / 2, height());

    QLinearGradient alpha_gradient(0, 0, width(), 0);
    alpha_gradient.setColorAt(0, Qt::white);
    alpha_gradient.setColorAt(0.2, Qt::white);
    alpha_gradient.setColorAt(0.5, Qt::transparent);
    alpha_gradient.setColorAt(0.8, Qt::white);
    alpha_gradient.setColorAt(1, Qt::white);

    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(alpha_gradient);
    p.drawRect(0, 0, width(), height());

    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);

    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
#ifdef Q_WS_QWS
    p.drawPixmap(rect(), m_image);
#else
    p.drawImage(rect(), m_image);
#endif
}

void CompositionRenderer::drawSource(QPainter &p)
{
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.setCompositionMode(m_composition_mode);

    QRectF circle_rect = rectangle_around(m_circle_pos);
    QColor color = QColor::fromHsvF(m_circle_hue / 360.0, 1, 1, m_circle_alpha / 255.0);
    QLinearGradient circle_gradient(circle_rect.topLeft(), circle_rect.bottomRight());
    circle_gradient.setColorAt(0, color.light());
    circle_gradient.setColorAt(0.5, color);
    circle_gradient.setColorAt(1, color.dark());
    p.setBrush(circle_gradient);

    p.drawEllipse(circle_rect);
}

void CompositionRenderer::paint(QPainter *painter)
{
#ifdef QT_OPENGL_SUPPORT
    if (usesOpenGL()) {

        int new_pbuf_size = m_pbuffer_size;
        if (size().width() > m_pbuffer_size ||
            size().height() > m_pbuffer_size)
            new_pbuf_size *= 2;

        if (size().width() < m_pbuffer_size/2 &&
            size().height() < m_pbuffer_size/2)
            new_pbuf_size /= 2;

        if (!m_pbuffer || new_pbuf_size != m_pbuffer_size) {
            if (m_pbuffer) {
                m_pbuffer->deleteTexture(m_base_tex);
                m_pbuffer->deleteTexture(m_compositing_tex);
                delete m_pbuffer;
            }

            m_pbuffer = new QGLPixelBuffer(QSize(new_pbuf_size, new_pbuf_size), QGLFormat::defaultFormat(), glWidget());
            m_pbuffer->makeCurrent();
            m_base_tex = m_pbuffer->generateDynamicTexture();
            m_compositing_tex = m_pbuffer->generateDynamicTexture();
            m_pbuffer_size = new_pbuf_size;
        }

        if (size() != m_previous_size) {
            m_previous_size = size();
            QPainter p(m_pbuffer);
            p.setCompositionMode(QPainter::CompositionMode_Source);
            p.fillRect(QRect(0, 0, m_pbuffer->width(), m_pbuffer->height()), Qt::transparent);
            drawBase(p);
            p.end();
            m_pbuffer->updateDynamicTexture(m_base_tex);
        }

        qreal x_fraction = width()/float(m_pbuffer->width());
        qreal y_fraction = height()/float(m_pbuffer->height());

        {
            QPainter p(m_pbuffer);
            p.setCompositionMode(QPainter::CompositionMode_Source);
            p.fillRect(QRect(0, 0, m_pbuffer->width(), m_pbuffer->height()), Qt::transparent);

            p.save();
            glBindTexture(GL_TEXTURE_2D, m_base_tex);
            glEnable(GL_TEXTURE_2D);
            glColor4f(1.,1.,1.,1.);

            glBegin(GL_QUADS);
            {
                glTexCoord2f(0, 1.0);
                glVertex2f(0, 0);

                glTexCoord2f(x_fraction, 1.0);
                glVertex2f(width(), 0);

                glTexCoord2f(x_fraction, 1.0-y_fraction);
                glVertex2f(width(), height());

                glTexCoord2f(0, 1.0-y_fraction);
                glVertex2f(0, height());
            }
            glEnd();

            glDisable(GL_TEXTURE_2D);
            p.restore();

            drawSource(p);
            p.end();
            m_pbuffer->updateDynamicTexture(m_compositing_tex);
        }

        glWidget()->makeCurrent();
        glBindTexture(GL_TEXTURE_2D, m_compositing_tex);
        glEnable(GL_TEXTURE_2D);
        glColor4f(1.,1.,1.,1.);
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0, 1.0);
            glVertex2f(0, 0);

            glTexCoord2f(x_fraction, 1.0);
            glVertex2f(width(), 0);

            glTexCoord2f(x_fraction, 1.0-y_fraction);
            glVertex2f(width(), height());

            glTexCoord2f(0, 1.0-y_fraction);
            glVertex2f(0, height());
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else
#endif
    {
        // using a QImage
        if (m_buffer.size() != size()) {
#ifdef Q_WS_QWS
            m_base_buffer = QPixmap(size());
            m_base_buffer.fill(Qt::transparent);
#else
            m_buffer = QImage(size(), QImage::Format_ARGB32_Premultiplied);
            m_base_buffer = QImage(size(), QImage::Format_ARGB32_Premultiplied);

            m_base_buffer.fill(0);
#endif

            QPainter p(&m_base_buffer);

            drawBase(p);
        }

#ifdef Q_WS_QWS
        m_buffer = m_base_buffer;
#else
        memcpy(m_buffer.bits(), m_base_buffer.bits(), m_buffer.numBytes());
#endif

        {
            QPainter p(&m_buffer);
            drawSource(p);
        }

#ifdef Q_WS_QWS
        painter->drawPixmap(0, 0, m_buffer);
#else
        painter->drawImage(0, 0, m_buffer);
#endif
    }

    if (m_animation_enabled && m_current_object == NoObject) {
        updateCirclePos();
    }
}

void CompositionRenderer::mousePressEvent(QMouseEvent *e)
{
    setDescriptionEnabled(false);

    QRectF circle = rectangle_around(m_circle_pos);

    if (circle.contains(e->pos())) {
        m_current_object = Circle;
        m_offset = circle.center() - e->pos();
    } else {
        m_current_object = NoObject;
    }
}

void CompositionRenderer::mouseMoveEvent(QMouseEvent *e)
{
    if (m_current_object == Circle) setCirclePos(e->pos() + m_offset);
}

void CompositionRenderer::mouseReleaseEvent(QMouseEvent *)
{
    m_current_object = NoObject;

    if (m_animation_enabled)
        updateCirclePos();
}

void CompositionRenderer::setCirclePos(const QPointF &pos)
{
    const QRect oldRect = rectangle_around(m_circle_pos).toAlignedRect();
    m_circle_pos = pos;
    const QRect newRect = rectangle_around(m_circle_pos).toAlignedRect();
#ifdef QT_OPENGL_SUPPORT
    if (usesOpenGL())
        update();
    else
#endif
        update(oldRect | newRect);
}

