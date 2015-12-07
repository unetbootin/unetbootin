/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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


#include "quiloader.h"
#include "customwidget.h"

#include <formbuilder.h>
#include <ui4_p.h>

#include <QtCore/qdebug.h>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtGui/QLayout>
#include <QtGui/QWidget>
#include <QtCore/QMap>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QListWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBox>
#include <QtGui/QComboBox>
#include <QtGui/QFontComboBox>

QT_BEGIN_NAMESPACE

typedef QMap<QString, bool> widget_map;
Q_GLOBAL_STATIC(widget_map, g_widgets)

class QUiLoader;
class QUiLoaderPrivate;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal
{
#endif

class FormBuilderPrivate: public QFormBuilder
{
    friend class QT_PREPEND_NAMESPACE(QUiLoader);
    friend class QT_PREPEND_NAMESPACE(QUiLoaderPrivate);
    typedef QFormBuilder ParentClass;

public:
    QUiLoader *loader;

    FormBuilderPrivate(): loader(0) {}

    QWidget *defaultCreateWidget(const QString &className, QWidget *parent, const QString &name)
    {
        return ParentClass::createWidget(className, parent, name);
    }

    QLayout *defaultCreateLayout(const QString &className, QObject *parent, const QString &name)
    {
        return ParentClass::createLayout(className, parent, name);
    }

    QAction *defaultCreateAction(QObject *parent, const QString &name)
    {
        return ParentClass::createAction(parent, name);
    }

    QActionGroup *defaultCreateActionGroup(QObject *parent, const QString &name)
    {
        return ParentClass::createActionGroup(parent, name);
    }

    virtual QWidget *createWidget(const QString &className, QWidget *parent, const QString &name)
    {
        if (QWidget *widget = loader->createWidget(className, parent, name)) {
            widget->setObjectName(name);
            return widget;
        }

        return 0;
    }

    virtual QLayout *createLayout(const QString &className, QObject *parent, const QString &name)
    {
        if (QLayout *layout = loader->createLayout(className, parent, name)) {
            layout->setObjectName(name);
            return layout;
        }

        return 0;
    }

    virtual QActionGroup *createActionGroup(QObject *parent, const QString &name)
    {
        if (QActionGroup *actionGroup = loader->createActionGroup(parent, name)) {
            actionGroup->setObjectName(name);
            return actionGroup;
        }

        return 0;
    }

    virtual QAction *createAction(QObject *parent, const QString &name)
    {
        if (QAction *action = loader->createAction(parent, name)) {
            action->setObjectName(name);
            return action;
        }

        return 0;
    }

    virtual void applyProperties(QObject *o, const QList<DomProperty*> &properties);
    virtual QWidget *create(DomUI *ui, QWidget *parentWidget);
    virtual QWidget *create(DomWidget *ui_widget, QWidget *parentWidget);

private:
    QString m_class;
};

void FormBuilderPrivate::applyProperties(QObject *o, const QList<DomProperty*> &properties)
{
    typedef QList<DomProperty*> DomPropertyList;

    QFormBuilder::applyProperties(o, properties);

    if (properties.empty())
        return;

    // translate string properties
    const DomPropertyList::const_iterator cend = properties.constEnd();
    for (DomPropertyList::const_iterator it = properties.constBegin(); it != cend; ++it) {
        const DomProperty *p = *it;
        if (p->kind() != DomProperty::String)
            continue;
        const DomString *dom_str = p->elementString();
        if (dom_str->hasAttributeNotr()) {
            const QString notr = dom_str->attributeNotr();
            if (notr == QLatin1String("yes") || notr == QLatin1String("true"))
                continue;
        }
        const QByteArray name = p->attributeName().toUtf8();
        const QVariant v = o->property(name);
        if (v.type() != QVariant::String)
            continue;
        const QString text = QApplication::translate(m_class.toUtf8(),
                                                     v.toString().toUtf8(),
                                                     dom_str->attributeComment().toUtf8(),
                                                     QCoreApplication::UnicodeUTF8);
        o->setProperty(name, text);
    }
}

QWidget *FormBuilderPrivate::create(DomUI *ui, QWidget *parentWidget)
{
    m_class = ui->elementClass();
    return QFormBuilder::create(ui, parentWidget);
}

static void recursiveTranslate(QTreeWidgetItem *item, const QString &class_name)
{
    int cnt = item->columnCount();
    for (int i = 0; i < cnt; ++i) {
        QString text = QApplication::translate(class_name.toUtf8(),
                                                item->text(i).toUtf8(),
                                                "",
                                                QCoreApplication::UnicodeUTF8);
        item->setText(i, text);
    }

    cnt = item->childCount();
    for (int i = 0; i < cnt; ++i)
        recursiveTranslate(item->child(i), class_name);
}

QWidget *FormBuilderPrivate::create(DomWidget *ui_widget, QWidget *parentWidget)
{
    QWidget *w = QFormBuilder::create(ui_widget, parentWidget);
    if (w == 0)
        return 0;

    if (0) {
#ifndef QT_NO_TABWIDGET
    } else if (QTabWidget *tabw = qobject_cast<QTabWidget*>(w)) {
        const int cnt = tabw->count();
        for (int i = 0; i < cnt; ++i) {
            const QString text = QApplication::translate(m_class.toUtf8(),
                                                    tabw->tabText(i).toUtf8(),
                                                    "",
                                                    QCoreApplication::UnicodeUTF8);

            tabw->setTabText(i, text);
        }
#endif
#ifndef QT_NO_LISTWIDGET
    } else if (QListWidget *listw = qobject_cast<QListWidget*>(w)) {
        const int cnt = listw->count();
        for (int i = 0; i < cnt; ++i) {
            QListWidgetItem *item = listw->item(i);
            const QString text = QApplication::translate(m_class.toUtf8(),
                                                    item->text().toUtf8(),
                                                    "",
                                                    QCoreApplication::UnicodeUTF8);
            item->setText(text);
        }
#endif
#ifndef QT_NO_TREEWIDGET
    } else if (QTreeWidget *treew = qobject_cast<QTreeWidget*>(w)) {
        const int cnt = treew->topLevelItemCount();
        for (int i = 0; i < cnt; ++i) {
            QTreeWidgetItem *item = treew->topLevelItem(i);
            recursiveTranslate(item, m_class);
        }
#endif
#ifndef QT_NO_TABLEWIDGET
    } else if (QTableWidget *tablew = qobject_cast<QTableWidget*>(w)) {
        const int row_cnt = tablew->rowCount();
        const int col_cnt = tablew->columnCount();
        for (int i = 0; i < row_cnt; ++i) {
            for (int j = 0; j < col_cnt; ++j) {
                QTableWidgetItem *item = tablew->item(i, j);
                if (item == 0)
                    continue;
                const QString text = QApplication::translate(m_class.toUtf8(),
                                                        item->text().toUtf8(),
                                                        "",
                                                        QCoreApplication::UnicodeUTF8);
                item->setText(text);
            }
        }
#endif
#ifndef QT_NO_COMBOBOX
    } else if (QComboBox *combow = qobject_cast<QComboBox*>(w)) {
        if (!qobject_cast<QFontComboBox*>(w)) {
            const int cnt = combow->count();
            for (int i = 0; i < cnt; ++i) {
                const QString text = QApplication::translate(m_class.toUtf8(),
                        combow->itemText(i).toUtf8(),
                        "",
                        QCoreApplication::UnicodeUTF8);
                combow->setItemText(i, text);
            }
        }
#endif
#ifndef QT_NO_TOOLBOX
    } else if (QToolBox *toolw = qobject_cast<QToolBox*>(w)) {
        const int cnt = toolw->count();
        for (int i = 0; i < cnt; ++i) {
            const QString text = QApplication::translate(m_class.toUtf8(),
                                                    toolw->itemText(i).toUtf8(),
                                                    "",
                                                    QCoreApplication::UnicodeUTF8);
            toolw->setItemText(i, text);
        }
#endif
    }

    return w;
}

#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

class QUiLoaderPrivate
{
public:
#ifdef QFORMINTERNAL_NAMESPACE
    QFormInternal::FormBuilderPrivate builder;
#else
    FormBuilderPrivate builder;
#endif

    void setupWidgetMap() const;
};

void QUiLoaderPrivate::setupWidgetMap() const
{
    if (!g_widgets()->isEmpty())
        return;

#define DECLARE_WIDGET(a, b) g_widgets()->insert(QLatin1String(#a), true);
#define DECLARE_LAYOUT(a, b)

#include "widgets.table"

#undef DECLARE_WIDGET
#undef DECLARE_WIDGET_1
#undef DECLARE_LAYOUT
}

/*!
    \class QUiLoader
    \inmodule QtUiTools

    \brief The QUiLoader class allows standalone applications dynamically
    create user interfaces at run-time using the information stored in
    .ui files or specified plugin paths.

    In addition, you can customize of creating an user interface by
    deriving your own loader class.

    If you have a custom component or an application that embeds Qt
    Designer, you can also use the QFormBuilder class provided by the
    QtDesigner module to create user interfaces from .ui files.

    The QUiLoader class provides a collection of functions that allows
    you to create widgets based on the information stored in \c .ui
    files (created with Qt Designer) or available in the specified
    plugin paths. The specified plugin paths can be retrieved using
    the pluginPaths() function. You can retrieve the contents of an \c
    .ui file using the load() function. For example:

    \snippet doc/src/snippets/quiloader/mywidget.cpp 0

    By including the user interface in the form's resources (\c myform.qrc),
    we ensure that it will be present at run-time:

    \snippet doc/src/snippets/quiloader/mywidget.qrc 0

    The availableWidgets() function returns a QStringList with the
    class names of the widgets available in the specified plugin
    paths. You can create any of these widgets using the
    createWidget() function. For example:

    \snippet doc/src/snippets/quiloader/main.cpp 0

    You can make a custom widget available to the loader using the
    addPluginPath() function, and you can remove all the available widgets
    by calling the clearPluginPaths() function.

    The createAction(), createActionGroup(), createLayout() and
    createWidget() functions are used internally by the QUiLoader class
    whenever it has to create an action, action group, layout or
    widget respectively. For that reason, you can subclass the QUiLoader
    class and reimplement these functions to intervene the process of
    constructing an user interface. For example, you might want to
    create a list of the actions created when loading a form or
    creating a custom widget.

    For a complete example using the QUiLoader class, see the \l
    {designer/calculatorbuilder}{Calculator Builder} example.

    \sa QtUiTools, QFormBuilder
*/

/*!
    Creates a form loader with the given \a parent.
*/
QUiLoader::QUiLoader(QObject *parent)
    : QObject(parent), d_ptr(new QUiLoaderPrivate)
{
    Q_D(QUiLoader);

    d->builder.loader = this;

    QStringList paths;
    foreach (const QString &path, QApplication::libraryPaths()) {
        QString libPath = path;
        libPath  += QDir::separator();
        libPath  += QLatin1String("designer");
        paths.append(libPath);
    }

    d->builder.setPluginPath(paths);
}

/*!
    Destroys the loader.
*/
QUiLoader::~QUiLoader()
{
    delete d_ptr;
}

/*!
    Loads a form from the given \a device and creates a new widget with the given
    \a parentWidget to hold its contents.

    \sa createWidget()
*/
QWidget *QUiLoader::load(QIODevice *device, QWidget *parentWidget)
{
    Q_D(QUiLoader);
    return d->builder.load(device, parentWidget);
}

/*!
    Returns a list naming the paths the loader searches when locating
    custom widget plugins.

    \sa addPluginPath(), clearPluginPaths()
*/
QStringList QUiLoader::pluginPaths() const
{
    Q_D(const QUiLoader);
    return d->builder.pluginPaths();
}

/*!
    Clears the list of paths the loader searches when locating
    plugins.

    \sa addPluginPath(), pluginPaths()
*/
void QUiLoader::clearPluginPaths()
{
    Q_D(QUiLoader);
    d->builder.clearPluginPaths();
}

/*!
    Adds the given \a path to the list of paths the loader searches
    when locating plugins.

    \sa pluginPaths(), clearPluginPaths()
*/
void QUiLoader::addPluginPath(const QString &path)
{
    Q_D(QUiLoader);
    d->builder.addPluginPath(path);
}

/*!
  Creates a new widget with the given \a parent and \a name
  using the class specified by \a className. You can use this
  function to create any of the widgets returned by the
  availableWidgets() function.

  The function is also used internally by the QUiLoader class whenever
  it has to create a widget. For that reason, you can subclass the
  QUiLoader class and reimplement this function to intervene in the
  process of constructing a user interface or widget.

  \sa availableWidgets(), load()
*/
QWidget *QUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
    Q_D(QUiLoader);
    return d->builder.defaultCreateWidget(className, parent, name);
}

/*!
    Creates a new layout with the given \a parent and \a name
    using the class specified by \a className.

    The function is used internally by the QUiLoader class whenever it
    has to create a layout. For that reason, you can subclass the
    QUiLoader class and reimplement this function to intervene the
    process of constructing an user interface or widget.

    \sa createWidget(), load()
*/
QLayout *QUiLoader::createLayout(const QString &className, QObject *parent, const QString &name)
{
    Q_D(QUiLoader);
    return d->builder.defaultCreateLayout(className, parent, name);
}

/*!
    Creates a new action group with the given \a parent and \a name.

    The function is used internally by the QUiLoader class whenever it
    has to create an action group. For that reason, you can subclass
    the QUiLoader class and reimplement this function to intervene the
    process of constructing an user interface or widget.

    \sa createAction(), createWidget(), load()
 */
QActionGroup *QUiLoader::createActionGroup(QObject *parent, const QString &name)
{
    Q_D(QUiLoader);
    return d->builder.defaultCreateActionGroup(parent, name);
}

/*!
    Creates a new action with the given \a parent and \a name.

    The function is used internally by the QUiLoader class whenever it
    has to create an action. For that reason, you can subclass the
    QUiLoader class and reimplement this function to intervene the
    process of constructing an user interface or widget.

    \sa createActionGroup(), createWidget(), load()
*/
QAction *QUiLoader::createAction(QObject *parent, const QString &name)
{
    Q_D(QUiLoader);
    return d->builder.defaultCreateAction(parent, name);
}

/*!
    Returns a list naming the available widgets that can be built
    using the createWidget() function, i.e all the widgets specified
    within the given plugin paths.

    \sa pluginPaths(), createWidget()

*/
QStringList QUiLoader::availableWidgets() const
{
    Q_D(const QUiLoader);

    d->setupWidgetMap();
    widget_map available = *g_widgets();

    foreach (QDesignerCustomWidgetInterface *plugin, d->builder.customWidgets()) {
        available.insert(plugin->name(), true);
    }

    return available.keys();
}

/*!
    Sets the working directory of the loader to \a dir. The loader
    looks for other resources, such as icons and resource files,
    in paths relative to this directory.

    \sa workingDirectory()
*/

void QUiLoader::setWorkingDirectory(const QDir &dir)
{
    Q_D(QUiLoader);
    d->builder.setWorkingDirectory(dir);
}

/*!
    Returns the working directory of the loader.

    \sa setWorkingDirectory()
*/

QDir QUiLoader::workingDirectory() const
{
    Q_D(const QUiLoader);
    return d->builder.workingDirectory();
}

/*!
    Sets whether the execution of scripts is enabled to \a enabled.
    \since 4.3
    \internal
*/

void QUiLoader::setScriptingEnabled(bool enabled)
{
    Q_D(QUiLoader);
    d->builder.setScriptingEnabled(enabled);
}

/*!
   Returns whether the execution of scripts is enabled.
  \sa setScriptingEnabled()
  \since 4.3
  \internal
*/

bool QUiLoader::isScriptingEnabled() const
{
    Q_D(const QUiLoader);
    return d->builder.isScriptingEnabled();
}

QT_END_NAMESPACE
