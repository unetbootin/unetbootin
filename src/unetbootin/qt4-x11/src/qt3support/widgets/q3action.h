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

#ifndef Q3ACTION_H
#define Q3ACTION_H

#include <QtGui/qicon.h>
#include <QtGui/qkeysequence.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_ACTION

class Q3ActionPrivate;
class Q3ActionGroupPrivate;
class QStatusBar;
class Q3PopupMenu;
class QToolTipGroup;
class QWidget;

class Q_COMPAT_EXPORT Q3Action : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool toggleAction READ isToggleAction WRITE setToggleAction)
    Q_PROPERTY(bool on READ isOn WRITE setOn)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(QIcon iconSet READ iconSet WRITE setIconSet)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString menuText READ menuText WRITE setMenuText)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)
    Q_PROPERTY(QString statusTip READ statusTip WRITE setStatusTip)
    Q_PROPERTY(QString whatsThis READ whatsThis WRITE setWhatsThis)
#ifndef QT_NO_ACCEL
    Q_PROPERTY(QKeySequence accel READ accel WRITE setAccel)
#endif
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

public:
    Q3Action(QObject* parent, const char* name = 0);
#ifndef QT_NO_ACCEL
    Q3Action(const QString& menuText, QKeySequence accel,
             QObject* parent, const char* name = 0);
    Q3Action(const QIcon& icon, const QString& menuText, QKeySequence accel,
             QObject* parent, const char* name = 0);

    Q3Action(const QString& text, const QIcon& icon, const QString& menuText, QKeySequence accel,
             QObject* parent, const char* name = 0, bool toggle = false); // obsolete
    Q3Action(const QString& text, const QString& menuText, QKeySequence accel, QObject* parent,
             const char* name = 0, bool toggle = false); // obsolete
#endif
    Q3Action(QObject* parent, const char* name , bool toggle); // obsolete
    ~Q3Action();

    virtual void setIconSet(const QIcon&);
    QIcon iconSet() const;
    virtual void setText(const QString&);
    QString text() const;
    virtual void setMenuText(const QString&);
    QString menuText() const;
    virtual void setToolTip(const QString&);
    QString toolTip() const;
    virtual void setStatusTip(const QString&);
    QString statusTip() const;
    virtual void setWhatsThis(const QString&);
    QString whatsThis() const;
#ifndef QT_NO_ACCEL
    virtual void setAccel(const QKeySequence& key);
    QKeySequence accel() const;
#endif
    virtual void setToggleAction(bool);

    bool isToggleAction() const;
    bool isOn() const;
    bool isEnabled() const;
    bool isVisible() const;
    virtual bool addTo(QWidget*);
    virtual bool removeFrom(QWidget*);

protected:
    virtual void addedTo(QWidget *actionWidget, QWidget *container);
    virtual void addedTo(int index, Q3PopupMenu *menu);

public Q_SLOTS:
    void activate();
    void toggle();
    virtual void setOn(bool);
    virtual void setEnabled(bool);
    void setDisabled(bool);
    virtual void setVisible(bool);

Q_SIGNALS:
    void activated();
    void toggled(bool);

private Q_SLOTS:
    void internalActivation();
    void toolButtonToggled(bool);
    void objectDestroyed();
    void menuStatusText(int id);
    void showStatusText(const QString&);
    void clearStatusText();

private:
    Q_DISABLE_COPY(Q3Action)

    void init();

    Q3ActionPrivate* d;

    friend class Q3ActionPrivate;
    friend class Q3ActionGroup;
    friend class Q3ActionGroupPrivate;
};

class Q_COMPAT_EXPORT Q3ActionGroup : public Q3Action
{
    Q_OBJECT
    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive)
    Q_PROPERTY(bool usesDropDown READ usesDropDown WRITE setUsesDropDown)

public:
    Q3ActionGroup(QObject* parent, const char* name = 0);
    Q3ActionGroup(QObject* parent, const char* name , bool exclusive ); // obsolete
    ~Q3ActionGroup();
    void setExclusive(bool);
    bool isExclusive() const;
    void add(Q3Action* a);
    void addSeparator();
    bool addTo(QWidget*);
    bool removeFrom(QWidget*);
    void setEnabled(bool);
    void setToggleAction(bool toggle);
    void setOn(bool on);
    void setVisible(bool);

    void setUsesDropDown(bool enable);
    bool usesDropDown() const;

    void setIconSet(const QIcon &);
    void setText(const QString&);
    void setMenuText(const QString&);
    void setToolTip(const QString&);
    void setWhatsThis(const QString&);

protected:
    void childEvent(QChildEvent*);
    virtual void addedTo(QWidget *actionWidget, QWidget *container, Q3Action *a);
    virtual void addedTo(int index, Q3PopupMenu *menu, Q3Action *a);
    virtual void addedTo(QWidget *actionWidget, QWidget *container);
    virtual void addedTo(int index, Q3PopupMenu *menu);

Q_SIGNALS:
    void selected(Q3Action*);
    void activated(Q3Action *);

private Q_SLOTS:
    void childToggled(bool);
    void childActivated();
    void childDestroyed();
    void internalComboBoxActivated(int);
    void internalComboBoxHighlighted(int);
    void internalToggle(Q3Action*);
    void objectDestroyed();

private:
    Q3ActionGroupPrivate* d;

public:
    void insert(Q3Action *a) { add(a); }

private:
    Q_DISABLE_COPY(Q3ActionGroup)
};

#endif // QT_NO_ACTION

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3ACTION_H
