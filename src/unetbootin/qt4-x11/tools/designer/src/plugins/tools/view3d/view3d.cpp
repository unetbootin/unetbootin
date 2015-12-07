/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

#include "abstractformeditor.h"
#include "abstractmetadatabase.h"
#include "abstractformwindow.h"
#include "view3d.h"

#define SELECTION_BUFSIZE 512

/*******************************************************************************
** QView3DWidget
*/

class QView3DWidget : public QGLWidget
{
    Q_OBJECT
public:
    QView3DWidget(QWidget *parent);
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void clear();

    void addTexture(QWidget *w, const QPixmap &pm);

    void beginAddingWidgets(QWidget *form);
    void addWidget(int depth, QWidget *w);
    void endAddingWidgets();

    QWidget *widgetAt(const QPoint &pos);

signals:
    void updateForm();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void keyReleaseEvent(QKeyEvent *);

    void contextMenuEvent(QContextMenuEvent *);

private:
    QWidget *m_form;
    QPoint m_old_pos;
    bool m_layer_coloring;
    bool m_use_mipmaps;
    GLuint m_form_list_id;

    typedef QMap<QWidget*, GLuint> TextureMap;
    TextureMap m_texture_map;

    typedef QMap<GLuint, QWidget*> WidgetNameMap;
    GLuint m_next_widget_name;
    WidgetNameMap m_widget_name_map;
};

QView3DWidget::QView3DWidget(QWidget *parent)
    : QGLWidget(parent)
    , m_layer_coloring(true)
    , m_form_list_id(0)
    , m_next_widget_name(0)
{
    setFocusPolicy(Qt::StrongFocus);
}

static int nearestSize(int v)
{
    int n = 0, last = 0;
    for (int s = 0; s < 32; ++s) {
        if (((v>>s) & 1) == 1) {
            ++n;
            last = s;
        }
    }
    if (n > 1)
        return 1 << (last+1);
    return 1 << last;
}

// static int pm_cnt = 0;

void QView3DWidget::addTexture(QWidget *w, const QPixmap &pm)
{
    int tx_w = nearestSize(pm.width());
    int tx_h = nearestSize(pm.height());

    QPixmap tmp(tx_w, tx_h);
    tmp.fill(QColor(0,0,0));
    QPainter p(&tmp);
    p.drawPixmap(0, tx_h - pm.height(), pm);
    p.end();
    QImage tex = tmp.toImage();

//    QString file_name = QString("pixmapDump%1.png").arg(pm_cnt++);
//    qDebug() << "grabWidget():" << file_name << tex.save(file_name, "PNG");

    tex = QGLWidget::convertToGLFormat(tex);

    GLuint tx_id;
    glGenTextures(1, &tx_id);
    glBindTexture(GL_TEXTURE_2D, tx_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (m_use_mipmaps) {
        //glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.f);
    } else {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width(), tex.height(), 0, GL_RGBA,
                GL_UNSIGNED_BYTE, tex.bits());
    m_texture_map[w] = tx_id;
}

void QView3DWidget::addWidget(int depth, QWidget *widget)
{
    TextureMap::const_iterator it = m_texture_map.find(widget);
    Q_ASSERT(it != m_texture_map.end());

    makeCurrent();

    int w = m_form->size().width();
    int h = m_form->size().height();
    int max = qMax(w, h);
    QRect r = widget->rect();
    QPoint pos = widget->mapToGlobal(QPoint(0, 0));
    r.moveTopLeft(m_form->mapFromGlobal(pos));

    float s = r.width()/float(nearestSize(r.width()));
    float t = r.height()/float(nearestSize(r.height()));

    if (m_layer_coloring)
        glColor4f(1.0 - depth/10.0, 1.0 - depth/10.0, 1.0, 1.0 - depth/20.0);
    else
        glColor4f(1.0, 1.0, 1.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, *it);
    glBegin(GL_QUADS);
    glLoadName(m_next_widget_name);
    glTexCoord2f(0.0, 0.0); glVertex3f(r.left() - w/2, r.bottom() - h/2, depth*max/8);
    glTexCoord2f(s, 0.0);   glVertex3f(r.right() - w/2, r.bottom()- h/2, depth*max/8);
    glTexCoord2f(s, t);     glVertex3f(r.right() - w/2, r.top() - h/2, depth*max/8);
    glTexCoord2f(0.0, t);   glVertex3f(r.left() - w/2, r.top() - h/2, depth*max/8);
    glEnd();

    m_widget_name_map[m_next_widget_name++] = widget;
}

void QView3DWidget::clear()
{
    makeCurrent();
    glDeleteLists(m_form_list_id, 1);
    foreach (GLuint id, m_texture_map)
        glDeleteTextures(1, &id);
    m_texture_map.clear();
    m_widget_name_map.clear();
    m_next_widget_name = 0;
}

void QView3DWidget::beginAddingWidgets(QWidget *form)
{
    makeCurrent();
    m_form = form;
    m_form_list_id = glGenLists(1);
    glNewList(m_form_list_id, GL_COMPILE);
    glInitNames();
    glPushName(-1);
    m_next_widget_name = 0;
}

void QView3DWidget::endAddingWidgets()
{
    makeCurrent();
    glEndList();
}

void QView3DWidget::initializeGL()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    qglClearColor(palette().color(QPalette::Window).dark());
    glColor3f (1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glShadeModel(GL_FLAT);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    QString extensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
    m_use_mipmaps = false;//  extensions.contains("GL_SGIS_generate_mipmap");
}

void QView3DWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/2, width/2, height/2, -height/2, -999999, 999999);
}

void QView3DWidget::paintGL()
{
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCallList(m_form_list_id);

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(-width()/2, -height()/2, 0.0);

    QFontMetrics fm(font());
    glColor4f(0.4, 0.4, 0.4, 0.7);
    glRecti(0, height() - fm.lineSpacing()*2.5, width(), height());

    glColor3f(1.0, 1.0, 1.0);
    renderText(10, height() - fm.lineSpacing()*1.5,
               "Press and hold left/right mouse button = tilt the view.");
    renderText(10, height() - fm.lineSpacing()*0.5,
               "Mouse wheel = zoom. 't' = toggle layer coloring. 'r' = reset transform.");
    glPopMatrix();
    glPopAttrib();
}

QWidget *QView3DWidget::widgetAt(const QPoint &pos)
{
    makeCurrent();
    GLuint selectBuf[SELECTION_BUFSIZE];
    glSelectBuffer(SELECTION_BUFSIZE, selectBuf);
    glRenderMode (GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glCallList(m_form_list_id);
    return 0;
}

void QView3DWidget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_T) {
        m_layer_coloring = !m_layer_coloring;
        emit updateForm();
    } else if (e->key() == Qt::Key_R) {
        makeCurrent();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    updateGL();
}

void QView3DWidget::mousePressEvent(QMouseEvent *e)
{
    m_old_pos = e->pos();
}

void QView3DWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_old_pos = e->pos();
}

void QView3DWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & (Qt::LeftButton | Qt::RightButton)) {
        GLfloat rx = (GLfloat) (e->x() - m_old_pos.x()) / width();
        GLfloat ry = (GLfloat) (e->y() - m_old_pos.y()) / height();

        makeCurrent();
        glMatrixMode(GL_MODELVIEW);
        if (e->buttons() & Qt::LeftButton) {
            // Left button down - rotate around X and Y axes
            glRotatef(-180*ry, 1, 0, 0);
            glRotatef(180*rx, 0, 1, 0);
        } else if (e->buttons() & Qt::RightButton) {
            // Right button down - rotate around X and Z axes
            glRotatef(-180*ry, 1, 0, 0);
            glRotatef(-180*rx, 0, 0, 1);
        }
        updateGL();
        m_old_pos = e->pos();
    } else {

    }
}

void QView3DWidget::wheelEvent(QWheelEvent *e)
{
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    if (e->delta() < 0)
        glScalef(0.9, 0.9, 0.9);
    else
        glScalef(1.1, 1.1, 1.1);
    updateGL();
}

void QView3DWidget::contextMenuEvent(QContextMenuEvent *e)
{
    e->accept();
}

/*******************************************************************************
** Misc tools
*/

class WalkWidgetTreeFunction
{
public:
    virtual void operator () (int depth, QWidget *widget) const = 0;
};

static bool skipWidget(QDesignerFormEditorInterface *core, QWidget *widget)
{
    QDesignerMetaDataBaseItemInterface *item = core->metaDataBase()->item(widget);
    if (item == 0)
        return true;
    QString name = widget->metaObject()->className();
    if (name == "QLayoutWidget")
        return true;

    return false;
}

static void walkWidgetTree(QDesignerFormEditorInterface *core, int depth, QWidget *widget, const WalkWidgetTreeFunction &func)
{
    if (widget == 0)
        return;
    if (!widget->isVisible())
        return;

    if (!skipWidget(core, widget))
        func(depth++, widget);

    QObjectList child_obj_list = widget->children();
    foreach (QObject *child_obj, child_obj_list) {
        QWidget *child = qobject_cast<QWidget*>(child_obj);
        if (child != 0)
            walkWidgetTree(core, depth, child, func);
    }
}

static void grabWidget_helper(QWidget *widget, QPixmap &res, QPixmap &buf,
                              const QRect &r, const QPoint &offset, QDesignerFormEditorInterface *core)
{
    buf.fill(widget, r.topLeft());
    QPainter::setRedirected(widget, &buf, r.topLeft());
    QPaintEvent e(r & widget->rect());
    QApplication::sendEvent(widget, &e);
    QPainter::restoreRedirected(widget);
    {
        QPainter pt(&res);
        pt.drawPixmap(offset.x(), offset.y(), buf, 0, 0, r.width(), r.height());
    }

    const QObjectList children = widget->children();
    for (int i = 0; i < children.size(); ++i) {
        QWidget *child = qobject_cast<QWidget*>(children.at(i));
        if (child == 0 || child->isWindow())
            continue;
        if (child->isHidden() || !child->geometry().intersects(r))
            continue;
        if (core->metaDataBase()->item(child) != 0)
            continue;
        QRect cr = r & child->geometry();
        cr.translate(-child->pos());
        grabWidget_helper(child, res, buf, cr, offset + child->pos(), core);
    }
}

static QPixmap grabWidget(QWidget * widget, QDesignerFormEditorInterface *core)
{
    if (!widget)
        return QPixmap();

    QRect r = widget->rect();
    QSize s = widget->size();

    QPixmap res(s), buf(s);

    grabWidget_helper(widget, res, buf, r, QPoint(), core);

    return res;
}

/*******************************************************************************
** QView3D
*/

class AddTexture : public WalkWidgetTreeFunction
{
public:
    inline AddTexture(QDesignerFormEditorInterface *core, QView3DWidget *w)
        : m_core(core), m_3d_widget(w) {}
    inline virtual void operator ()(int, QWidget *w) const
        { m_3d_widget->addTexture(w, ::grabWidget(w, m_core)); }
    QDesignerFormEditorInterface *m_core;
    QView3DWidget *m_3d_widget;
};

class AddWidget : public WalkWidgetTreeFunction
{
public:
    inline AddWidget(QView3DWidget *w) : m_3d_widget(w) {}
    inline virtual void operator ()(int depth, QWidget *w) const
        { m_3d_widget->addWidget(depth, w); }
    QView3DWidget *m_3d_widget;
};

QView3D::QView3D(QDesignerFormWindowInterface *form_window, QWidget *parent)
    : QWidget(parent)
{
    m_form_window = form_window;
    m_3d_widget = new QView3DWidget(this);
    connect(m_3d_widget, SIGNAL(updateForm()), this, SLOT(updateForm()));

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_3d_widget, 0, 0, 1, 1);

    updateForm();
}

void QView3D::updateForm()
{
    QWidget *w = m_form_window->mainContainer();
    if (w == 0)
        return;

    m_3d_widget->clear();

    walkWidgetTree(m_form_window->core(), 0, w, AddTexture(m_form_window->core(), m_3d_widget));
    m_3d_widget->beginAddingWidgets(w);
    walkWidgetTree(m_form_window->core(), 0, w, AddWidget(m_3d_widget));
    m_3d_widget->endAddingWidgets();
}

#include "view3d.moc"
