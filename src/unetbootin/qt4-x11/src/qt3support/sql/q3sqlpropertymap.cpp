/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3sqlpropertymap.h"

#ifndef QT_NO_SQL_FORM

#include "qwidget.h"
#include "q3cleanuphandler.h"
#include "qmetaobject.h"
#include "qmap.h"

QT_BEGIN_NAMESPACE

class Q3SqlPropertyMapPrivate
{
public:
    Q3SqlPropertyMapPrivate() {}
    QMap<QByteArray, QByteArray> propertyMap;
};

/*!
    \class Q3SqlPropertyMap qsqlpropertymap.h
    \brief The Q3SqlPropertyMap class is used to map widgets to SQL fields.

    \compat

    The SQL module uses Qt \link properties.html object
    properties\endlink to insert and extract values from editor
    widgets.

    This class is used to map editors to SQL fields. This works by
    associating SQL editor class names to the properties used to
    insert and extract values to/from the editor.

    For example, a QLineEdit can be used to edit text strings and
    other data types in Q3DataTables or Q3SqlForms. Several properties
    are defined in QLineEdit, but only the \e text property is used to
    insert and extract text from a QLineEdit. Both Q3DataTable and
    Q3SqlForm use the global Q3SqlPropertyMap for inserting and
    extracting values to and from an editor widget. The global
    property map defines several common widgets and properties that
    are suitable for many applications. You can add and remove widget
    properties to suit your specific needs.

    If you want to use custom editors with your Q3DataTable or
    Q3SqlForm, you must install your own Q3SqlPropertyMap for that table
    or form. Example:

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlpropertymap.cpp 0

    You can also replace the global Q3SqlPropertyMap that is used by
    default. (Bear in mind that Q3SqlPropertyMap takes ownership of the
    new default map.)

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlpropertymap.cpp 1

    \sa Q3DataTable, Q3SqlForm, Q3SqlEditorFactory
*/

/*!

Constructs a Q3SqlPropertyMap.

The default property mappings used by Qt widgets are:
\table
\header \i Widgets \i Property
\row \i \l QCheckBox,
        \l QRadioButton
     \i checked
\row \i \l QComboBox,
        \l Q3ListBox
     \i currentItem
\row \i \l Q3DateEdit
     \i date
\row \i \l Q3DateTimeEdit
        \l QDateTimeEdit
     \i dateTime
\row \i \l QTextBrowser
     \i source
\row \i \l QAbstractButton,
        \l QDial,
        \l QLabel,
        \l QLineEdit,
        \l Q3MultiLineEdit,
        \l QPushButton,
        \l QTextEdit,
     \i text
\row \i \l Q3TimeEdit
     \i time
\row \i \l QLCDNumber,
        \l QScrollBar
        \l QSlider,
        \l QSpinBox
     \i value
\endtable
*/

Q3SqlPropertyMap::Q3SqlPropertyMap()
{
    d = new Q3SqlPropertyMapPrivate();
    const struct MapData {
        const char *classname;
        const char *property;
    } mapData[] = {
        { "Q3DateEdit", "date" },
        { "Q3DateTimeEdit", "dateTime" },
        { "Q3ListBox", "currentItem" },
        { "Q3TimeEdit", "time" },
        { "QAbstractButton", "text" },
        { "QCheckBox", "checked" },
        { "QRadioButton", "checked" },
        { "QComboBox", "currentIndex" },
        { "QDateTimeEdit", "dateTime" },
        { "QDial", "value" },
        { "QLabel", "text" },
        { "QLCDNumber", "value" },
        { "QLineEdit", "text" },
        { "QPushButton", "text" },
        { "QScrollBar", "value" },
        { "QSlider", "value" },
        { "QSpinBox", "value" },
        { "QTabBar", "currentTab" },
        { "QTabWidget", "currentPage" },
        { "QTextBrowser", "source" },
        { "QTextEdit", "text" },
        { "QGroupBox", "checked" }
    };

    const MapData *m = mapData;
    for (uint i = 0; i < sizeof(mapData)/sizeof(MapData); i++, m++)
        d->propertyMap.insert(m->classname, m->property);
}

/*!
    Destroys the Q3SqlPropertyMap.

    Note that if the Q3SqlPropertyMap is installed with
    installPropertyMap() the object it was installed into, e.g. the
    Q3SqlForm, takes ownership and will delete the Q3SqlPropertyMap when
    necessary.
*/
Q3SqlPropertyMap::~Q3SqlPropertyMap()
{
    delete d;
}

/*!
    Returns the mapped property of \a widget as a QVariant.
*/
QVariant Q3SqlPropertyMap::property(QWidget * widget)
{
    if(!widget) return QVariant();
    const QMetaObject* mo = widget->metaObject();
    while (mo && !d->propertyMap.contains(mo->className()))
        mo = mo->superClass();

    if (!mo) {
        qWarning("Q3SqlPropertyMap::property: %s does not exist", widget->metaObject()->className());
        return QVariant();
    }
    return widget->property(d->propertyMap[mo->className()]);
}

/*!
    Sets the property of \a widget to \a value.
*/
void Q3SqlPropertyMap::setProperty(QWidget * widget, const QVariant & value)
{
    if(!widget) return;

    const QMetaObject* mo = widget->metaObject();
    while (mo && !d->propertyMap.contains(mo->className()))
        mo = mo->superClass();
    if (!mo) {
        qWarning("Q3SqlPropertyMap::setProperty: %s not handled by Q3SqlPropertyMap", widget->metaObject()->className());
        return;
    }

    widget->setProperty(d->propertyMap[mo->className()], value);
}

/*!
  Insert a new classname/property pair, which is used for custom SQL
  field editors. There \e must be a Q_PROPERTY() clause in the \a
  classname class declaration for the \a property.
*/
void Q3SqlPropertyMap::insert(const QString & classname,
                              const QString & property)
{
    d->propertyMap[classname.latin1()] = property.latin1();
}

/*!
    Removes \a classname from the map.
*/
void Q3SqlPropertyMap::remove(const QString & classname)
{
    d->propertyMap.remove(classname.latin1());
}

static Q3SqlPropertyMap * defaultmap = 0;
static Q3CleanupHandler< Q3SqlPropertyMap > qsql_cleanup_property_map;

/*!
    Returns the application global Q3SqlPropertyMap.
*/
Q3SqlPropertyMap * Q3SqlPropertyMap::defaultMap()
{
    if(defaultmap == 0){
        defaultmap = new Q3SqlPropertyMap();
        qsql_cleanup_property_map.add(&defaultmap);
    }
    return defaultmap;
}

/*!
    Replaces the global default property map with \a map. All
    Q3DataTable and Q3SqlForm instantiations will use this new map for
    inserting and extracting values to and from editors.
    \e{Q3SqlPropertyMap takes ownership of \a map, and destroys it
    when it is no longer needed.}
*/
void Q3SqlPropertyMap::installDefaultMap(Q3SqlPropertyMap * map)
{
    if(map == 0) return;

    if(defaultmap != 0){
        qsql_cleanup_property_map.remove(&defaultmap);
        delete defaultmap;
    }
    defaultmap = map;
    qsql_cleanup_property_map.add(&defaultmap);
}

QT_END_NAMESPACE

#endif // QT_NO_SQL_FORM
