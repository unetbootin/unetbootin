/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QDATETIMEEDIT_P_H
#define QDATETIMEEDIT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qcombobox.h"
#include "QtGui/qcalendarwidget.h"
#include "QtGui/qspinbox.h"
#include "QtGui/qtoolbutton.h"
#include "QtGui/qmenu.h"
#include "QtGui/qlabel.h"
#include "QtGui/qdatetimeedit.h"
#include "QtGui/private/qabstractspinbox_p.h"
#include "QtCore/private/qdatetime_p.h"

#include "qdebug.h"

#ifndef QT_NO_DATETIMEEDIT

QT_BEGIN_NAMESPACE

class QCalendarPopup;
class QDateTimeEditPrivate : public QAbstractSpinBoxPrivate, public QDateTimeParser
{
    Q_DECLARE_PUBLIC(QDateTimeEdit)
public:
    QDateTimeEditPrivate();

    void init(const QVariant &var);
    void readLocaleSettings();

    void emitSignals(EmitPolicy ep, const QVariant &old);
    QString textFromValue(const QVariant &f) const;
    QVariant valueFromText(const QString &f) const;
    virtual void _q_editorCursorPositionChanged(int oldpos, int newpos);
    virtual void interpret(EmitPolicy ep);
    virtual void clearCache() const;

    QDateTime validateAndInterpret(QString &input, int &, QValidator::State &state,
                                   bool fixup = false) const;
    void clearSection(int index);
    virtual QString displayText() const { return edit->displayText(); } // this is from QDateTimeParser

    int absoluteIndex(QDateTimeEdit::Section s, int index) const;
    int absoluteIndex(const SectionNode &s) const;
    void updateEdit();
    QDateTime stepBy(int index, int steps, bool test = false) const;
    int sectionAt(int pos) const;
    int closestSection(int index, bool forward) const;
    int nextPrevSection(int index, bool forward) const;
    void setSelected(int index, bool forward = false);

    void updateCache(const QVariant &val, const QString &str) const;

    void updateTimeSpec();
    virtual QDateTime getMinimum() const { return minimum.toDateTime(); }
    virtual QDateTime getMaximum() const { return maximum.toDateTime(); }
    virtual QLocale locale() const { return q_func()->locale(); }
    QString valueToText(const QVariant &var) const { return textFromValue(var); }
    QString getAmPmText(AmPm ap, Case cs) const;
    int cursorPosition() const { return edit ? edit->cursorPosition() : -1; }

    virtual QStyle::SubControl newHoverControl(const QPoint &pos);
    virtual void updateEditFieldGeometry();
    virtual QVariant getZeroVariant() const;
    virtual void setRange(const QVariant &min, const QVariant &max);

    void _q_resetButton();
    void updateArrow(QStyle::StateFlag state);
    bool calendarPopupEnabled() const;
    void syncCalendarWidget();

    bool isSeparatorKey(const QKeyEvent *k) const;

    static QDateTimeEdit::Sections convertSections(QDateTimeParser::Sections s);
    static QDateTimeEdit::Section convertToPublic(QDateTimeParser::Section s);

    void initCalendarPopup(QCalendarWidget *cw = 0);
    void positionCalendarPopup();

    QDateTimeEdit::Sections sections;
    mutable bool cacheGuard;

    QString defaultDateFormat, defaultTimeFormat, defaultDateTimeFormat, unreversedFormat;
    Qt::LayoutDirection layoutDirection;
    mutable QVariant conflictGuard;
    bool hasHadFocus, formatExplicitlySet, calendarPopup;
    QStyle::StateFlag arrowState;
    QCalendarPopup *monthCalendar;

#ifdef QT_KEYPAD_NAVIGATION
    bool focusOnButton;
#endif
};


class QCalendarPopup : public QWidget
{
    Q_OBJECT
public:
    QCalendarPopup(QWidget *parent = 0, QCalendarWidget *cw = 0);
    QDate selectedDate() { return calendar->selectedDate(); }
    void setDate(const QDate &date);
    void setDateRange(const QDate &min, const QDate &max);
    void setFirstDayOfWeek(Qt::DayOfWeek dow) { calendar->setFirstDayOfWeek(dow); }
    QCalendarWidget *calendarWidget() const { return calendar; }
    void setCalendarWidget(QCalendarWidget *cw);
Q_SIGNALS:
    void activated(const QDate &date);
    void newDateSelected(const QDate &newDate);
    void hidingCalendar(const QDate &oldDate);
    void resetButton();

private Q_SLOTS:
    void dateSelected(const QDate &date);
    void dateSelectionChanged();

protected:
    void hideEvent(QHideEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    bool event(QEvent *e);

private:
    QCalendarWidget *calendar;
    QDate oldDate;
    bool dateChanged;
};

QT_END_NAMESPACE

#endif // QT_NO_DATETIMEEDIT

#endif // QDATETIMEEDIT_P_H
