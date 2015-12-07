/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QTEDITORFACTORY_H
#define QTEDITORFACTORY_H

#include "qtpropertymanager.h"

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

class QtSpinBoxFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
    Q_OBJECT
public:
    QtSpinBoxFactory(QObject *parent = 0);
    ~QtSpinBoxFactory();
protected:
    void connectPropertyManager(QtIntPropertyManager *manager);
    QWidget *createEditor(QtIntPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtIntPropertyManager *manager);
private:
    class QtSpinBoxFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtSpinBoxFactory)
    Q_DISABLE_COPY(QtSpinBoxFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, int, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtSliderFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
    Q_OBJECT
public:
    QtSliderFactory(QObject *parent = 0);
    ~QtSliderFactory();
protected:
    void connectPropertyManager(QtIntPropertyManager *manager);
    QWidget *createEditor(QtIntPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtIntPropertyManager *manager);
private:
    class QtSliderFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtSliderFactory)
    Q_DISABLE_COPY(QtSliderFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, int, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtScrollBarFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
    Q_OBJECT
public:
    QtScrollBarFactory(QObject *parent = 0);
    ~QtScrollBarFactory();
protected:
    void connectPropertyManager(QtIntPropertyManager *manager);
    QWidget *createEditor(QtIntPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtIntPropertyManager *manager);
private:
    class QtScrollBarFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtScrollBarFactory)
    Q_DISABLE_COPY(QtScrollBarFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, int, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtCheckBoxFactory : public QtAbstractEditorFactory<QtBoolPropertyManager>
{
    Q_OBJECT
public:
    QtCheckBoxFactory(QObject *parent = 0);
    ~QtCheckBoxFactory();
protected:
    void connectPropertyManager(QtBoolPropertyManager *manager);
    QWidget *createEditor(QtBoolPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtBoolPropertyManager *manager);
private:
    class QtCheckBoxFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtCheckBoxFactory)
    Q_DISABLE_COPY(QtCheckBoxFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, bool))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(bool))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtDoubleSpinBoxFactory : public QtAbstractEditorFactory<QtDoublePropertyManager>
{
    Q_OBJECT
public:
    QtDoubleSpinBoxFactory(QObject *parent = 0);
    ~QtDoubleSpinBoxFactory();
protected:
    void connectPropertyManager(QtDoublePropertyManager *manager);
    QWidget *createEditor(QtDoublePropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtDoublePropertyManager *manager);
private:
    class QtDoubleSpinBoxFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtDoubleSpinBoxFactory)
    Q_DISABLE_COPY(QtDoubleSpinBoxFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, double, double))
    Q_PRIVATE_SLOT(d_func(), void slotDecimalsChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(double))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtLineEditFactory : public QtAbstractEditorFactory<QtStringPropertyManager>
{
    Q_OBJECT
public:
    QtLineEditFactory(QObject *parent = 0);
    ~QtLineEditFactory();
protected:
    void connectPropertyManager(QtStringPropertyManager *manager);
    QWidget *createEditor(QtStringPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtStringPropertyManager *manager);
private:
    class QtLineEditFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtLineEditFactory)
    Q_DISABLE_COPY(QtLineEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QString &))
    Q_PRIVATE_SLOT(d_func(), void slotRegExpChanged(QtProperty *, const QRegExp &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QString &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtDateEditFactory : public QtAbstractEditorFactory<QtDatePropertyManager>
{
    Q_OBJECT
public:
    QtDateEditFactory(QObject *parent = 0);
    ~QtDateEditFactory();
protected:
    void connectPropertyManager(QtDatePropertyManager *manager);
    QWidget *createEditor(QtDatePropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtDatePropertyManager *manager);
private:
    class QtDateEditFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtDateEditFactory)
    Q_DISABLE_COPY(QtDateEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *,
                        const QDate &, const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtTimeEditFactory : public QtAbstractEditorFactory<QtTimePropertyManager>
{
    Q_OBJECT
public:
    QtTimeEditFactory(QObject *parent = 0);
    ~QtTimeEditFactory();
protected:
    void connectPropertyManager(QtTimePropertyManager *manager);
    QWidget *createEditor(QtTimePropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtTimePropertyManager *manager);
private:
    class QtTimeEditFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtTimeEditFactory)
    Q_DISABLE_COPY(QtTimeEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QTime &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QTime &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtDateTimeEditFactory : public QtAbstractEditorFactory<QtDateTimePropertyManager>
{
    Q_OBJECT
public:
    QtDateTimeEditFactory(QObject *parent = 0);
    ~QtDateTimeEditFactory();
protected:
    void connectPropertyManager(QtDateTimePropertyManager *manager);
    QWidget *createEditor(QtDateTimePropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtDateTimePropertyManager *manager);
private:
    class QtDateTimeEditFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtDateTimeEditFactory)
    Q_DISABLE_COPY(QtDateTimeEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QDateTime &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QDateTime &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtKeySequenceEditorFactory : public QtAbstractEditorFactory<QtKeySequencePropertyManager>
{
    Q_OBJECT
public:
    QtKeySequenceEditorFactory(QObject *parent = 0);
    ~QtKeySequenceEditorFactory();
protected:
    void connectPropertyManager(QtKeySequencePropertyManager *manager);
    QWidget *createEditor(QtKeySequencePropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtKeySequencePropertyManager *manager);
private:
    class QtKeySequenceEditorFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtKeySequenceEditorFactory)
    Q_DISABLE_COPY(QtKeySequenceEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QKeySequence &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QKeySequence &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtCharEditorFactory : public QtAbstractEditorFactory<QtCharPropertyManager>
{
    Q_OBJECT
public:
    QtCharEditorFactory(QObject *parent = 0);
    ~QtCharEditorFactory();
protected:
    void connectPropertyManager(QtCharPropertyManager *manager);
    QWidget *createEditor(QtCharPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtCharPropertyManager *manager);
private:
    class QtCharEditorFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtCharEditorFactory)
    Q_DISABLE_COPY(QtCharEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QChar &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QChar &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtEnumEditorFactory : public QtAbstractEditorFactory<QtEnumPropertyManager>
{
    Q_OBJECT
public:
    QtEnumEditorFactory(QObject *parent = 0);
    ~QtEnumEditorFactory();
protected:
    void connectPropertyManager(QtEnumPropertyManager *manager);
    QWidget *createEditor(QtEnumPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtEnumPropertyManager *manager);
private:
    class QtEnumEditorFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtEnumEditorFactory)
    Q_DISABLE_COPY(QtEnumEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotEnumNamesChanged(QtProperty *,
                        const QStringList &))
    Q_PRIVATE_SLOT(d_func(), void slotEnumIconsChanged(QtProperty *,
                        const QMap<int, QIcon> &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtCursorEditorFactory : public QtAbstractEditorFactory<QtCursorPropertyManager>
{
    Q_OBJECT
public:
    QtCursorEditorFactory(QObject *parent = 0);
    ~QtCursorEditorFactory();
protected:
    void connectPropertyManager(QtCursorPropertyManager *manager);
    QWidget *createEditor(QtCursorPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtCursorPropertyManager *manager);
private:
    class QtCursorEditorFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtCursorEditorFactory)
    Q_DISABLE_COPY(QtCursorEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QCursor &))
    Q_PRIVATE_SLOT(d_func(), void slotEnumChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtColorEditorFactory : public QtAbstractEditorFactory<QtColorPropertyManager>
{
    Q_OBJECT
public:
    QtColorEditorFactory(QObject *parent = 0);
    ~QtColorEditorFactory();
protected:
    void connectPropertyManager(QtColorPropertyManager *manager);
    QWidget *createEditor(QtColorPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtColorPropertyManager *manager);
private:
    class QtColorEditorFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtColorEditorFactory)
    Q_DISABLE_COPY(QtColorEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QColor &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QColor &))
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#endif
