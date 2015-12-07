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
    void remove_helper(QAbstractButton *);

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
