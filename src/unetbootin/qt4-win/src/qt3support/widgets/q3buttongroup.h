/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3BUTTONGROUP_H
#define Q3BUTTONGROUP_H

#include <QtGui/qbuttongroup.h>
#include <Qt3Support/q3groupbox.h>
#include <QtCore/qmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class QAbstractButton;

class Q_COMPAT_EXPORT Q3ButtonGroup : public Q3GroupBox
{
    Q_OBJECT
    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive)
    Q_PROPERTY(bool radioButtonExclusive READ isRadioButtonExclusive WRITE setRadioButtonExclusive)
    Q_PROPERTY(int selectedId READ selectedId WRITE setButton)

public:
    Q3ButtonGroup(QWidget* parent=0, const char* name=0);
    Q3ButtonGroup(const QString &title,
                  QWidget* parent=0, const char* name=0);
    Q3ButtonGroup(int columns, Qt::Orientation o,
                  QWidget* parent=0, const char* name=0);
    Q3ButtonGroup(int columns, Qt::Orientation o, const QString &title,
                  QWidget* parent=0, const char* name=0);
    ~Q3ButtonGroup();

    bool isExclusive() const;
    bool isRadioButtonExclusive() const { return radio_excl; }
    void setExclusive(bool);
    void setRadioButtonExclusive(bool);

public:
    int insert(QAbstractButton *, int id=-1);
    void remove(QAbstractButton *);
    QAbstractButton    *find(int id) const;
    int id(QAbstractButton *) const;
    int count() const;

    void setButton(int id);

    QAbstractButton *selected() const;
    int selectedId() const;

Q_SIGNALS:
    void pressed(int id);
    void released(int id);
    void clicked(int id);

protected Q_SLOTS:
    void buttonPressed();
    void buttonReleased();
    void buttonClicked();

protected:
    bool event(QEvent * e);

private Q_SLOTS:
    void buttonDestroyed();

private:
    Q_DISABLE_COPY(Q3ButtonGroup)

    void init();
    void fixChildren() const;
    int insert_helper(QAbstractButton* , int id=-1);

    bool excl_grp; // Not used.
    bool radio_excl;
    QMap<int, QAbstractButton*> buttonIds;
    QButtonGroup group;
};

class Q_COMPAT_EXPORT Q3VButtonGroup : public Q3ButtonGroup
{
    Q_OBJECT
public:
    inline Q3VButtonGroup(QWidget* parent=0, const char* name=0)
        : Q3ButtonGroup(1, Qt::Horizontal /* sic! */, parent, name) {}
    inline Q3VButtonGroup(const QString &title, QWidget* parent=0, const char* name=0)
        : Q3ButtonGroup(1, Qt::Horizontal /* sic! */, title, parent, name) {}

private:
    Q_DISABLE_COPY(Q3VButtonGroup)
};


class Q_COMPAT_EXPORT Q3HButtonGroup : public Q3ButtonGroup
{
    Q_OBJECT
public:
    inline Q3HButtonGroup(QWidget* parent=0, const char* name=0)
        : Q3ButtonGroup(1, Qt::Vertical /* sic! */, parent, name) {}
    inline Q3HButtonGroup(const QString &title, QWidget* parent=0, const char* name=0)
        : Q3ButtonGroup(1, Qt::Vertical /* sic! */, title, parent, name) {}

private:
    Q_DISABLE_COPY(Q3HButtonGroup)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3BUTTONGROUP_H
