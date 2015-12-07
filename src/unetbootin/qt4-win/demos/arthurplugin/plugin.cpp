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

#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerCustomWidgetInterface>

#include <QtCore/qplugin.h>
#include <QtGui/QIcon>
#include <QtGui/QLineEdit>

#include "xform.h"
#include "pathdeform.h"
#include "gradients.h"
#include "pathstroke.h"
#include "hoverpoints.h"
#include "composition.h"

QT_FORWARD_DECLARE_CLASS(QDesignerFormEditorInterface)

class PathDeformRendererEx : public PathDeformRenderer
{
    Q_OBJECT
public:
    PathDeformRendererEx(QWidget *parent) : PathDeformRenderer(parent) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class DemoPlugin : public QDesignerCustomWidgetInterface
{
    Q_INTERFACES(QDesignerCustomWidgetInterface)
        public:
    DemoPlugin() : m_initialized(false) { }
    bool isContainer() const { return false; }
    bool isInitialized() const { return m_initialized; }
    QIcon icon() const { return QIcon(); }
    QString codeTemplate() const { return QString(); }
    QString whatsThis() const { return QString(); }
    QString toolTip() const { return QString(); }
    QString group() const { return "Arthur Widgets [Demo]"; }
    void initialize(QDesignerFormEditorInterface *)
    {
        if (m_initialized)
            return;
        m_initialized = true;
    }
private:
    bool m_initialized;
};

class DeformPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    DeformPlugin(QObject *parent = 0) : QObject(parent) { }

    QString includeFile() const { return "deform.h"; }
    QString name() const { return "PathDeformRendererEx"; }

    QWidget *createWidget(QWidget *parent)
    {
        PathDeformRenderer *deform = new PathDeformRendererEx(parent);
        deform->setRadius(70);
        deform->setAnimated(false);
        deform->setFontSize(20);
        deform->setText("Arthur Widgets Demo");

        return deform;
    }

};

class XFormRendererEx : public XFormView
{
    Q_OBJECT
public:
    XFormRendererEx(QWidget *parent) : XFormView(parent) { textEditor = new QLineEdit; }
    QSize sizeHint() const { return QSize(300, 200); }

public slots:
void setText(const QString &text) { textEditor->setText(text); }
};

class XFormPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    XFormPlugin(QObject *parent = 0) : QObject(parent) { }
    QString includeFile() const { return "xform.h"; }
    QString name() const { return "XFormRendererEx"; }

    QWidget *createWidget(QWidget *parent)
    {
        XFormRendererEx *xform = new XFormRendererEx(parent);
        xform->setText("Qt - Hello World!!");
        return xform;
    }
};


class GradientEditorPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    GradientEditorPlugin(QObject *parent = 0) : QObject(parent) { }
    QString includeFile() const { return "gradients.h"; }
    QString name() const { return "GradientEditor"; }

    QWidget *createWidget(QWidget *parent)
    {
        GradientEditor *editor = new GradientEditor(parent);
        return editor;
    }
};

class GradientRendererEx : public GradientRenderer
{
    Q_OBJECT
public:
    GradientRendererEx(QWidget *p) : GradientRenderer(p) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class GradientRendererPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    GradientRendererPlugin(QObject *parent = 0) : QObject(parent) { }
    QString includeFile() const { return "gradients.h"; }
    QString name() const { return "GradientRendererEx"; }

    QWidget *createWidget(QWidget *parent)
    {
        GradientRenderer *renderer = new GradientRendererEx(parent);
        renderer->setConicalGradient();
        return renderer;
    }
};

class PathStrokeRendererEx : public PathStrokeRenderer
{
    Q_OBJECT
public:
    PathStrokeRendererEx(QWidget *p) : PathStrokeRenderer(p) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class StrokeRenderPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    StrokeRenderPlugin(QObject *parent = 0) : QObject(parent) { }
    QString includeFile() const { return "pathstroke.h"; }
    QString name() const { return "PathStrokeRendererEx"; }

    QWidget *createWidget(QWidget *parent)
    {
        PathStrokeRenderer *stroke = new PathStrokeRendererEx(parent);
        return stroke;
    }
};


class CompositionModePlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    CompositionModePlugin(QObject *parent = 0) : QObject(parent) { }
    QString includeFile() const { return "composition.h"; }
    QString name() const { return "CompositionRenderer"; }

    QWidget *createWidget(QWidget *parent)
    {
        CompositionRenderer *renderer = new CompositionRenderer(parent);
        renderer->setAnimationEnabled(false);
        return renderer;
    }
};


class ArthurPlugins : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    ArthurPlugins(QObject *parent = 0);
    QList<QDesignerCustomWidgetInterface*> customWidgets() const { return m_plugins; }

private:
    QList<QDesignerCustomWidgetInterface *> m_plugins;
};

ArthurPlugins::ArthurPlugins(QObject *parent) :
    QObject(parent)
{
    m_plugins << new DeformPlugin(this)
              << new XFormPlugin(this)
              << new GradientEditorPlugin(this)
              << new GradientRendererPlugin(this)
              << new StrokeRenderPlugin(this)
              << new CompositionModePlugin(this);
}

#include "plugin.moc"

Q_EXPORT_PLUGIN(ArthurPlugins)
