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

#include "pathstroke.h"
#include "arthurstyle.h"
#include "arthurwidgets.h"

#include <stdio.h>

extern void draw_round_rect(QPainter *p, const QRect &bounds, int radius);


PathStrokeControls::PathStrokeControls(QWidget* parent, PathStrokeRenderer* renderer, bool smallScreen)
      : QWidget(parent)
{
    m_renderer = renderer;

    if (smallScreen)
        layoutForSmallScreens();
    else
        layoutForDesktop();
}

void PathStrokeControls::createCommonControls(QWidget* parent)
{
    m_capGroup = new QGroupBox(parent);
    m_capGroup->setAttribute(Qt::WA_ContentsPropagated);
    m_capGroup->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QRadioButton *flatCap = new QRadioButton(m_capGroup);
    QRadioButton *squareCap = new QRadioButton(m_capGroup);
    QRadioButton *roundCap = new QRadioButton(m_capGroup);
    m_capGroup->setTitle("Cap Style");
    flatCap->setText("Flat");
    squareCap->setText("Square");
    roundCap->setText("Round");
    flatCap->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    squareCap->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    roundCap->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_joinGroup = new QGroupBox(parent);
    m_joinGroup->setAttribute(Qt::WA_ContentsPropagated);
    m_joinGroup->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QRadioButton *bevelJoin = new QRadioButton(m_joinGroup);
    QRadioButton *miterJoin = new QRadioButton(m_joinGroup);
    QRadioButton *roundJoin = new QRadioButton(m_joinGroup);
    m_joinGroup->setTitle("Join Style");
    bevelJoin->setText("Bevel");
    miterJoin->setText("Miter");
    roundJoin->setText("Round");

    m_styleGroup = new QGroupBox(parent);
    m_styleGroup->setAttribute(Qt::WA_ContentsPropagated);
    m_styleGroup->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QRadioButton *solidLine = new QRadioButton(m_styleGroup);
    QRadioButton *dashLine = new QRadioButton(m_styleGroup);
    QRadioButton *dotLine = new QRadioButton(m_styleGroup);
    QRadioButton *dashDotLine = new QRadioButton(m_styleGroup);
    QRadioButton *dashDotDotLine = new QRadioButton(m_styleGroup);
    QRadioButton *customDashLine = new QRadioButton(m_styleGroup);
    m_styleGroup->setTitle("Pen Style");

    QPixmap line_solid(":res/images/line_solid.png");
    solidLine->setIcon(line_solid);
    solidLine->setIconSize(line_solid.size());
    QPixmap line_dashed(":res/images/line_dashed.png");
    dashLine->setIcon(line_dashed);
    dashLine->setIconSize(line_dashed.size());
    QPixmap line_dotted(":res/images/line_dotted.png");
    dotLine->setIcon(line_dotted);
    dotLine->setIconSize(line_dotted.size());
    QPixmap line_dash_dot(":res/images/line_dash_dot.png");
    dashDotLine->setIcon(line_dash_dot);
    dashDotLine->setIconSize(line_dash_dot.size());
    QPixmap line_dash_dot_dot(":res/images/line_dash_dot_dot.png");
    dashDotDotLine->setIcon(line_dash_dot_dot);
    dashDotDotLine->setIconSize(line_dash_dot_dot.size());
    customDashLine->setText("Custom");

    int fixedHeight = bevelJoin->sizeHint().height();
    solidLine->setFixedHeight(fixedHeight);
    dashLine->setFixedHeight(fixedHeight);
    dotLine->setFixedHeight(fixedHeight);
    dashDotLine->setFixedHeight(fixedHeight);
    dashDotDotLine->setFixedHeight(fixedHeight);

    m_pathModeGroup = new QGroupBox(parent);
    m_pathModeGroup->setAttribute(Qt::WA_ContentsPropagated);
    m_pathModeGroup->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QRadioButton *curveMode = new QRadioButton(m_pathModeGroup);
    QRadioButton *lineMode = new QRadioButton(m_pathModeGroup);
    m_pathModeGroup->setTitle("Line Style");
    curveMode->setText("Curves");
    lineMode->setText("Lines");


    // Layouts
    QVBoxLayout *capGroupLayout = new QVBoxLayout(m_capGroup);
    capGroupLayout->addWidget(flatCap);
    capGroupLayout->addWidget(squareCap);
    capGroupLayout->addWidget(roundCap);

    QVBoxLayout *joinGroupLayout = new QVBoxLayout(m_joinGroup);
    joinGroupLayout->addWidget(bevelJoin);
    joinGroupLayout->addWidget(miterJoin);
    joinGroupLayout->addWidget(roundJoin);

    QVBoxLayout *styleGroupLayout = new QVBoxLayout(m_styleGroup);
    styleGroupLayout->addWidget(solidLine);
    styleGroupLayout->addWidget(dashLine);
    styleGroupLayout->addWidget(dotLine);
    styleGroupLayout->addWidget(dashDotLine);
    styleGroupLayout->addWidget(dashDotDotLine);
    styleGroupLayout->addWidget(customDashLine);

    QVBoxLayout *pathModeGroupLayout = new QVBoxLayout(m_pathModeGroup);
    pathModeGroupLayout->addWidget(curveMode);
    pathModeGroupLayout->addWidget(lineMode);


    // Connections
    connect(flatCap, SIGNAL(clicked()), m_renderer, SLOT(setFlatCap()));
    connect(squareCap, SIGNAL(clicked()), m_renderer, SLOT(setSquareCap()));
    connect(roundCap, SIGNAL(clicked()), m_renderer, SLOT(setRoundCap()));

    connect(bevelJoin, SIGNAL(clicked()), m_renderer, SLOT(setBevelJoin()));
    connect(miterJoin, SIGNAL(clicked()), m_renderer, SLOT(setMiterJoin()));
    connect(roundJoin, SIGNAL(clicked()), m_renderer, SLOT(setRoundJoin()));

    connect(curveMode, SIGNAL(clicked()), m_renderer, SLOT(setCurveMode()));
    connect(lineMode, SIGNAL(clicked()), m_renderer, SLOT(setLineMode()));

    connect(solidLine, SIGNAL(clicked()), m_renderer, SLOT(setSolidLine()));
    connect(dashLine, SIGNAL(clicked()), m_renderer, SLOT(setDashLine()));
    connect(dotLine, SIGNAL(clicked()), m_renderer, SLOT(setDotLine()));
    connect(dashDotLine, SIGNAL(clicked()), m_renderer, SLOT(setDashDotLine()));
    connect(dashDotDotLine, SIGNAL(clicked()), m_renderer, SLOT(setDashDotDotLine()));
    connect(customDashLine, SIGNAL(clicked()), m_renderer, SLOT(setCustomDashLine()));

    // Set the defaults:
    flatCap->setChecked(true);
    bevelJoin->setChecked(true);
    curveMode->setChecked(true);
    solidLine->setChecked(true);
}


void PathStrokeControls::layoutForDesktop()
{
    QGroupBox *mainGroup = new QGroupBox(this);
    mainGroup->setFixedWidth(180);
    mainGroup->setTitle("Path Stroking");

    createCommonControls(mainGroup);

    QGroupBox* penWidthGroup = new QGroupBox(mainGroup);
    penWidthGroup->setAttribute(Qt::WA_ContentsPropagated);
    QSlider *penWidth = new QSlider(Qt::Horizontal, penWidthGroup);
    penWidth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    penWidthGroup->setTitle("Pen Width");
    penWidth->setRange(0, 500);

    QPushButton *animated = new QPushButton(mainGroup);
    animated->setText("Animate");
    animated->setCheckable(true);

    QPushButton *showSourceButton = new QPushButton(mainGroup);
    showSourceButton->setText("Show Source");
#ifdef QT_OPENGL_SUPPORT
    QPushButton *enableOpenGLButton = new QPushButton(mainGroup);
    enableOpenGLButton->setText("Use OpenGL");
    enableOpenGLButton->setCheckable(true);
    enableOpenGLButton->setChecked(m_renderer->usesOpenGL());
    if (!QGLFormat::hasOpenGL())
        enableOpenGLButton->hide();
#endif
    QPushButton *whatsThisButton = new QPushButton(mainGroup);
    whatsThisButton->setText("What's This?");
    whatsThisButton->setCheckable(true);


    // Layouts:
    QVBoxLayout *penWidthLayout = new QVBoxLayout(penWidthGroup);
    penWidthLayout->addWidget(penWidth);

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(mainGroup);

    QVBoxLayout *mainGroupLayout = new QVBoxLayout(mainGroup);
    mainGroupLayout->setMargin(3);
    mainGroupLayout->addWidget(m_capGroup);
    mainGroupLayout->addWidget(m_joinGroup);
    mainGroupLayout->addWidget(m_styleGroup);
    mainGroupLayout->addWidget(penWidthGroup);
    mainGroupLayout->addWidget(m_pathModeGroup);
    mainGroupLayout->addWidget(animated);
    mainGroupLayout->addStretch(1);
    mainGroupLayout->addWidget(showSourceButton);
#ifdef QT_OPENGL_SUPPORT
    mainGroupLayout->addWidget(enableOpenGLButton);
#endif
    mainGroupLayout->addWidget(whatsThisButton);


    // Set up connections
    connect(animated, SIGNAL(toggled(bool)),
            m_renderer, SLOT(setAnimation(bool)));

    connect(penWidth, SIGNAL(valueChanged(int)),
            m_renderer, SLOT(setPenWidth(int)));

    connect(showSourceButton, SIGNAL(clicked()), m_renderer, SLOT(showSource()));
#ifdef QT_OPENGL_SUPPORT
    connect(enableOpenGLButton, SIGNAL(clicked(bool)), m_renderer, SLOT(enableOpenGL(bool)));
#endif
    connect(whatsThisButton, SIGNAL(clicked(bool)), m_renderer, SLOT(setDescriptionEnabled(bool)));
    connect(m_renderer, SIGNAL(descriptionEnabledChanged(bool)),
            whatsThisButton, SLOT(setChecked(bool)));


    // Set the defaults
    animated->setChecked(true);
    penWidth->setValue(50);

}

void PathStrokeControls::layoutForSmallScreens()
{
    createCommonControls(this);

    m_capGroup->layout()->setMargin(0);
    m_joinGroup->layout()->setMargin(0);
    m_styleGroup->layout()->setMargin(0);
    m_pathModeGroup->layout()->setMargin(0);

    QPushButton* okBtn = new QPushButton("OK", this);
    okBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okBtn->setMinimumSize(100,okBtn->minimumSize().height());

    QPushButton* quitBtn = new QPushButton("Quit", this);
    quitBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    quitBtn->setMinimumSize(100, okBtn->minimumSize().height());

    QLabel *penWidthLabel = new QLabel(" Width:");
    QSlider *penWidth = new QSlider(Qt::Horizontal, this);
    penWidth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    penWidth->setRange(0, 500);

#ifdef QT_OPENGL_SUPPORT
    QPushButton *enableOpenGLButton = new QPushButton(this);
    enableOpenGLButton->setText("Use OpenGL");
    enableOpenGLButton->setCheckable(true);
    enableOpenGLButton->setChecked(m_renderer->usesOpenGL());
    if (!QGLFormat::hasOpenGL())
        enableOpenGLButton->hide();
#endif

    // Layouts:
    QHBoxLayout *penWidthLayout = new QHBoxLayout(0);
    penWidthLayout->addWidget(penWidthLabel, 0, Qt::AlignRight);
    penWidthLayout->addWidget(penWidth);

    QVBoxLayout *leftLayout = new QVBoxLayout(0);
    leftLayout->addWidget(m_capGroup);
    leftLayout->addWidget(m_joinGroup);
#ifdef QT_OPENGL_SUPPORT
    leftLayout->addWidget(enableOpenGLButton);
#endif
    leftLayout->addLayout(penWidthLayout);

    QVBoxLayout *rightLayout = new QVBoxLayout(0);
    rightLayout->addWidget(m_styleGroup);
    rightLayout->addWidget(m_pathModeGroup);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);

    // Add spacers around the form items so we don't look stupid at higher resolutions
    mainLayout->addItem(new QSpacerItem(0,0), 0, 0, 1, 4);
    mainLayout->addItem(new QSpacerItem(0,0), 1, 0, 2, 1);
    mainLayout->addItem(new QSpacerItem(0,0), 1, 3, 2, 1);
    mainLayout->addItem(new QSpacerItem(0,0), 3, 0, 1, 4);

    mainLayout->addLayout(leftLayout, 1, 1);
    mainLayout->addLayout(rightLayout, 1, 2);
    mainLayout->addWidget(quitBtn, 2, 1, Qt::AlignHCenter | Qt::AlignTop);
    mainLayout->addWidget(okBtn, 2, 2, Qt::AlignHCenter | Qt::AlignTop);

#ifdef QT_OPENGL_SUPPORT
    connect(enableOpenGLButton, SIGNAL(clicked(bool)), m_renderer, SLOT(enableOpenGL(bool)));
#endif

    connect(penWidth, SIGNAL(valueChanged(int)), m_renderer, SLOT(setPenWidth(int)));
    connect(quitBtn, SIGNAL(clicked()), this, SLOT(emitQuitSignal()));
    connect(okBtn, SIGNAL(clicked()), this, SLOT(emitOkSignal()));

    m_renderer->setAnimation(true);
    penWidth->setValue(50);
}

void PathStrokeControls::emitQuitSignal()
{   emit quitPressed();  }

void PathStrokeControls::emitOkSignal()
{   emit okPressed();   }


PathStrokeWidget::PathStrokeWidget(bool smallScreen)
{
    setWindowTitle(tr("Path Stroking"));

    // Widget construction and property setting
    m_renderer = new PathStrokeRenderer(this, smallScreen);

    m_controls = new PathStrokeControls(0, m_renderer, smallScreen);

    // Layouting
    QHBoxLayout *viewLayout = new QHBoxLayout(this);
    viewLayout->addWidget(m_renderer);

    if (!smallScreen)
        viewLayout->addWidget(m_controls);

    m_renderer->loadSourceFile(":res/pathstroke.cpp");
    m_renderer->loadDescription(":res/pathstroke.html");

    connect(m_renderer, SIGNAL(clicked()), this, SLOT(showControls()));
    connect(m_controls, SIGNAL(okPressed()), this, SLOT(hideControls()));
    connect(m_controls, SIGNAL(quitPressed()), QApplication::instance(), SLOT(quit()));
}


void PathStrokeWidget::showControls()
{
    m_controls->showFullScreen();
}


void PathStrokeWidget::hideControls()
{
    m_controls->hide();
}


void PathStrokeWidget::setStyle( QStyle * style )
{
    QWidget::setStyle(style);
    if (m_controls != 0)
    {
        m_controls->setStyle(style);
        
        QList<QWidget *> widgets = qFindChildren<QWidget *>(m_controls);
        foreach (QWidget *w, widgets)
            w->setStyle(style);
    }
}


PathStrokeRenderer::PathStrokeRenderer(QWidget *parent, bool smallScreen)
    : ArthurFrame(parent)
{
    m_smallScreen = smallScreen;
    m_pointSize = 10;
    m_activePoint = -1;
    m_capStyle = Qt::FlatCap;
    m_joinStyle = Qt::BevelJoin;
    m_pathMode = CurveMode;
    m_penWidth = 1;
    m_penStyle = Qt::SolidLine;
    m_wasAnimated = true;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void PathStrokeRenderer::paint(QPainter *painter)
{
    if (m_points.isEmpty())
        initializePoints();

    painter->setRenderHint(QPainter::Antialiasing);

    QPalette pal = palette();
    painter->setPen(Qt::NoPen);

    // Construct the path
    QPainterPath path;
    path.moveTo(m_points.at(0));

    if (m_pathMode == LineMode) {
        for (int i=1; i<m_points.size(); ++i) {
            path.lineTo(m_points.at(i));
        }
    } else {
        int i=1;
        while (i + 2 < m_points.size()) {
            path.cubicTo(m_points.at(i), m_points.at(i+1), m_points.at(i+2));
            i += 3;
        }
        while (i < m_points.size()) {
            path.lineTo(m_points.at(i));
            ++i;
        }
    }

    // Draw the path
    {
        QColor lg = Qt::red;

        // The "custom" pen
        if (m_penStyle == Qt::NoPen) {
            QPainterPathStroker stroker;
            stroker.setWidth(m_penWidth);
            stroker.setJoinStyle(m_joinStyle);
            stroker.setCapStyle(m_capStyle);

            QVector<qreal> dashes;
            qreal space = 4;
            dashes << 1 << space
                   << 3 << space
                   << 9 << space
                   << 27 << space
                   << 9 << space
                   << 3 << space;
            stroker.setDashPattern(dashes);
            QPainterPath stroke = stroker.createStroke(path);
            painter->fillPath(stroke, lg);

        } else {
            QPen pen(lg, m_penWidth, m_penStyle, m_capStyle, m_joinStyle);
            painter->strokePath(path, pen);
        }
    }

    if (1) {
        // Draw the control points
        painter->setPen(QColor(50, 100, 120, 200));
        painter->setBrush(QColor(200, 200, 210, 120));
        for (int i=0; i<m_points.size(); ++i) {
            QPointF pos = m_points.at(i);
            painter->drawEllipse(QRectF(pos.x() - m_pointSize,
                                       pos.y() - m_pointSize,
                                       m_pointSize*2, m_pointSize*2));
        }
        painter->setPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawPolyline(m_points);
    }

}

void PathStrokeRenderer::initializePoints()
{
    const int count = 7;
    m_points.clear();
    m_vectors.clear();

    QMatrix m;
    qreal rot = 360 / count;
    QPointF center(width() / 2, height() / 2);
    QMatrix vm;
    vm.shear(2, -1);
    vm.scale(3, 3);

    for (int i=0; i<count; ++i) {
        m_vectors << QPointF(.1f, .25f) * (m * vm);
        m_points << QPointF(0, 100) * m + center;
        m.rotate(rot);
    }
}

void PathStrokeRenderer::updatePoints()
{
    qreal pad = 10;
    qreal left = pad;
    qreal right = width() - pad;
    qreal top = pad;
    qreal bottom = height() - pad;

    Q_ASSERT(m_points.size() == m_vectors.size());
    for (int i=0; i<m_points.size(); ++i) {

        if (i == m_activePoint)
            continue;

        QPointF pos = m_points.at(i);
        QPointF vec = m_vectors.at(i);
        pos += vec;
        if (pos.x() < left || pos.x() > right) {
            vec.setX(-vec.x());
            pos.setX(pos.x() < left ? left : right);
        } if (pos.y() < top || pos.y() > bottom) {
            vec.setY(-vec.y());
            pos.setY(pos.y() < top ? top : bottom);
        }
        m_points[i] = pos;
        m_vectors[i] = vec;
    }
    update();
}

void PathStrokeRenderer::mousePressEvent(QMouseEvent *e)
{
    setDescriptionEnabled(false);
    m_activePoint = -1;
    qreal distance = -1;
    for (int i=0; i<m_points.size(); ++i) {
        qreal d = QLineF(e->pos(), m_points.at(i)).length();
        if ((distance < 0 && d < 8 * m_pointSize) || d < distance) {
            distance = d;
            m_activePoint = i;
        }
    }

    if (m_activePoint != -1) {
        m_wasAnimated = m_timer.isActive();
        setAnimation(false);
        mouseMoveEvent(e);
    }
    
    // If we're not running in small screen mode, always assume we're dragging
    m_mouseDrag = !m_smallScreen;
    m_mousePress = e->pos();
}

void PathStrokeRenderer::mouseMoveEvent(QMouseEvent *e)
{
    // If we've moved more then 25 pixels, assume user is dragging
    if (!m_mouseDrag && QPoint(m_mousePress - e->pos()).manhattanLength() > 25)
        m_mouseDrag = true;

    if (m_mouseDrag && m_activePoint >= 0 && m_activePoint < m_points.size()) {
        m_points[m_activePoint] = e->pos();
        update();
    }
}

void PathStrokeRenderer::mouseReleaseEvent(QMouseEvent *)
{
    m_activePoint = -1;
    setAnimation(m_wasAnimated);

    if (!m_mouseDrag && m_smallScreen)
        emit clicked();
}

void PathStrokeRenderer::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_timer.timerId()) {
        updatePoints();
        QApplication::syncX();
    } // else if (e->timerId() == m_fpsTimer.timerId()) {
//         emit frameRate(m_frameCount);
//         m_frameCount = 0;
//     }
}

void PathStrokeRenderer::setAnimation(bool animation)
{
    m_timer.stop();
//     m_fpsTimer.stop();

    if (animation) {
        m_timer.start(25, this);
//         m_fpsTimer.start(1000, this);
//         m_frameCount = 0;
    }
}
