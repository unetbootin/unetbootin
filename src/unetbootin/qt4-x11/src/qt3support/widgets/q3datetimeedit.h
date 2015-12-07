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

#ifndef Q3DATETIMEEDIT_H
#define Q3DATETIMEEDIT_H

#include <QtGui/qwidget.h>
#include <QtCore/qstring.h>
#include <QtCore/qdatetime.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_DATETIMEEDIT

class Q_COMPAT_EXPORT Q3DateTimeEditBase : public QWidget
{
    Q_OBJECT
public:
    Q3DateTimeEditBase(QWidget* parent=0, const char* name=0)
        : QWidget(parent) { setObjectName(QString::fromAscii(name)); }

    virtual bool setFocusSection(int sec) = 0;
    virtual QString sectionFormattedText(int sec) = 0;
    virtual void addNumber(int sec, int num) = 0;
    virtual void removeLastNumber(int sec) = 0;

public Q_SLOTS:
    virtual void stepUp() = 0;
    virtual void stepDown() = 0;

private:
    Q_DISABLE_COPY(Q3DateTimeEditBase)
};

class Q3DateEditPrivate;

class Q_COMPAT_EXPORT Q3DateEdit : public Q3DateTimeEditBase
{
    Q_OBJECT
    Q_ENUMS(Order)
    Q_PROPERTY(Order order READ order WRITE setOrder)
    Q_PROPERTY(QDate date READ date WRITE setDate USER true)
    Q_PROPERTY(bool autoAdvance READ autoAdvance WRITE setAutoAdvance)
    Q_PROPERTY(QDate maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(QDate minValue READ minValue WRITE setMinValue)

public:
    Q3DateEdit(QWidget* parent=0, const char* name=0);
    Q3DateEdit(const QDate& date, QWidget* parent=0, const char* name=0);
    ~Q3DateEdit();

    enum Order { DMY, MDY, YMD, YDM };

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public Q_SLOTS:
    virtual void setDate(const QDate& date);

public:
    QDate date() const;
    virtual void setOrder(Order order);
    Order order() const;
    virtual void setAutoAdvance(bool advance);
    bool autoAdvance() const;

    virtual void setMinValue(const QDate& d) { setRange(d, maxValue()); }
    QDate minValue() const;
    virtual void setMaxValue(const QDate& d) { setRange(minValue(), d); }
    QDate maxValue() const;
    virtual void setRange(const QDate& min, const QDate& max);
    QString separator() const;
    virtual void setSeparator(const QString& s);

    // Make removeFirstNumber() virtual in Q3DateTimeEditBase in 4.0
    void removeFirstNumber(int sec);

Q_SIGNALS:
    void valueChanged(const QDate& date);

protected:
    bool event(QEvent *e);
    void timerEvent(QTimerEvent *);
    void resizeEvent(QResizeEvent *);
    void stepUp();
    void stepDown();
    QString sectionFormattedText(int sec);
    void addNumber(int sec, int num);

    void removeLastNumber(int sec);
    bool setFocusSection(int s);

    virtual void setYear(int year);
    virtual void setMonth(int month);
    virtual void setDay(int day);
    virtual void fix();
    virtual bool outOfRange(int y, int m, int d) const;

protected Q_SLOTS:
    void updateButtons();

private:
    Q_DISABLE_COPY(Q3DateEdit)

    void init();
    int sectionOffsetEnd(int sec) const;
    int sectionLength(int sec) const;
    QString sectionText(int sec) const;
    Q3DateEditPrivate* d;
};

class Q3TimeEditPrivate;

class Q_COMPAT_EXPORT Q3TimeEdit : public Q3DateTimeEditBase
{
    Q_OBJECT
    Q_FLAGS(Display)
    Q_PROPERTY(QTime time READ time WRITE setTime USER true)
    Q_PROPERTY(bool autoAdvance READ autoAdvance WRITE setAutoAdvance)
    Q_PROPERTY(QTime maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(QTime minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(Display display READ display WRITE setDisplay)

public:
    enum Display {
        Hours        = 0x01,
        Minutes        = 0x02,
        Seconds        = 0x04,
        /*Reserved = 0x08,*/
        AMPM        = 0x10
    };

    Q3TimeEdit(QWidget* parent=0, const char* name=0);
    Q3TimeEdit(const QTime& time, QWidget* parent=0, const char* name=0);
    ~Q3TimeEdit();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public Q_SLOTS:
    virtual void setTime(const QTime& time);

public:
    QTime time() const;
    virtual void setAutoAdvance(bool advance);
    bool autoAdvance() const;

    virtual void setMinValue(const QTime& d) { setRange(d, maxValue()); }
    QTime minValue() const;
    virtual void setMaxValue(const QTime& d) { setRange(minValue(), d); }
    QTime maxValue() const;
    virtual void setRange(const QTime& min, const QTime& max);
    QString separator() const;
    virtual void setSeparator(const QString& s);

    uint display() const;
    void setDisplay(uint disp);

    // Make removeFirstNumber() virtual in Q3DateTimeEditBase in 4.0
    void removeFirstNumber(int sec);

Q_SIGNALS:
    void valueChanged(const QTime& time);

protected:
    bool event(QEvent *e);
    void timerEvent(QTimerEvent *e);
    void resizeEvent(QResizeEvent *);
    void stepUp();
    void stepDown();
    QString sectionFormattedText(int sec);
    void addNumber(int sec, int num);
    void removeLastNumber(int sec);
    bool setFocusSection(int s);

    virtual bool outOfRange(int h, int m, int s) const;
    virtual void setHour(int h);
    virtual void setMinute(int m);
    virtual void setSecond(int s);

protected Q_SLOTS:
    void updateButtons();

private:
    Q_DISABLE_COPY(Q3TimeEdit)

    void init();
    QString sectionText(int sec);
    Q3TimeEditPrivate* d;
};


class Q3DateTimeEditPrivate;

class Q_COMPAT_EXPORT Q3DateTimeEdit : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime USER true)

public:
    Q3DateTimeEdit(QWidget* parent=0, const char* name=0);
    Q3DateTimeEdit(const QDateTime& datetime, QWidget* parent=0,
                   const char* name=0);
    ~Q3DateTimeEdit();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public Q_SLOTS:
    virtual void setDateTime(const QDateTime & dt);

public:
    QDateTime dateTime() const;

    Q3DateEdit* dateEdit() { return de; }
    Q3TimeEdit* timeEdit() { return te; }

    virtual void setAutoAdvance(bool advance);
    bool autoAdvance() const;

Q_SIGNALS:
    void valueChanged(const QDateTime& datetime);

protected:
    void init();
    void resizeEvent(QResizeEvent *);

protected Q_SLOTS:
    void newValue(const QDate& d);
    void newValue(const QTime& t);

private:
    Q_DISABLE_COPY(Q3DateTimeEdit)

    Q3DateEdit* de;
    Q3TimeEdit* te;
    Q3DateTimeEditPrivate* d;
};

#endif // QT_NO_DATETIMEEDIT

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3DATETIMEEDIT_H
