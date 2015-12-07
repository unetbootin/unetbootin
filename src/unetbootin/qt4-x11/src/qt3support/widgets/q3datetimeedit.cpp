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

#include "q3datetimeedit.h"

#ifndef QT_NO_DATETIMEEDIT

#include <private/q3richtext_p.h>
#include "qevent.h"
#include "q3rangecontrol.h"
#include "qapplication.h"
#include "qpixmap.h"
#include "qlist.h"
#include "qstring.h"
#include "qstyle.h"

#if defined(Q_WS_WIN)
#include "qt_windows.h"
#endif

QT_BEGIN_NAMESPACE

#define QDATETIMEEDIT_HIDDEN_CHAR QLatin1Char('0')

class Q_COMPAT_EXPORT QNumberSection
{
public:
    QNumberSection(int selStart = 0, int selEnd = 0, bool separat = true, int actual = -1)
        : selstart(selStart), selend(selEnd), act(actual), sep(separat)
    {}
    int selectionStart() const { return selstart; }
    void setSelectionStart(int s) { selstart = s; }
    int selectionEnd() const { return selend; }
    void setSelectionEnd(int s) { selend = s; }
    int width() const { return selend - selstart; }
    int index() const { return act; }
    bool separator() const { return sep; }
    Q_DUMMY_COMPARISON_OPERATOR(QNumberSection)
private:
    signed int selstart :12;
    signed int selend         :12;
    signed int act         :7;
    bool sep         :1;
};

static QString        *lDateSep = 0;
static QString        *lTimeSep = 0;
static bool        lAMPM          = false;
static QString        *lAM          = 0;
static QString        *lPM          = 0;
static Q3DateEdit::Order        lOrder = Q3DateEdit::YMD;
static int refcount = 0;

static void cleanup()
{
    delete lDateSep;
    lDateSep = 0;
    delete lTimeSep;
    lTimeSep = 0;
    delete lAM;
    lAM = 0;
    delete lPM;
    lPM = 0;
}

/*!
\internal
try to get the order of DMY and the date/time separator from the locale settings
*/
static void readLocaleSettings()
{
    int dpos, mpos, ypos;
    cleanup();

    lDateSep = new QString();
    lTimeSep = new QString();

#if defined(Q_WS_WIN)
    QT_WA({
        TCHAR data[10];
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDATE, data, 10);
        *lDateSep = QString::fromUtf16((ushort*)data);
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIME, data, 10);
        *lTimeSep = QString::fromUtf16((ushort*)data);
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, data, 10);
        lAMPM = QString::fromUtf16((ushort*)data).toInt()==0;
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_S1159, data, 10);
        QString am = QString::fromUtf16((ushort*)data);
        if (!am.isEmpty())
            lAM = new QString(am);
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_S2359, data, 10);
        QString pm = QString::fromUtf16((ushort*)data);
        if (!pm.isEmpty() )
            lPM = new QString(pm);
    } , {
        char data[10];
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SDATE, (char*)&data, 10);
        *lDateSep = QString::fromLocal8Bit(data);
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_STIME, (char*)&data, 10);
        *lTimeSep = QString::fromLocal8Bit(data);
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_ITIME, (char*)&data, 10);
        lAMPM = QString::fromLocal8Bit(data).toInt()==0;
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_S1159, (char*)&data, 10);
        QString am = QString::fromLocal8Bit(data);
        if (!am.isEmpty())
            lAM = new QString(am);
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_S2359, (char*)&data, 10);
        QString pm = QString::fromLocal8Bit(data);
        if (!pm.isEmpty())
            lPM = new QString(pm);
    });
#else
    *lDateSep = QLatin1Char('-');
    *lTimeSep = QLatin1Char(':');
#endif
    QString d = QDate(1999, 11, 22).toString(Qt::LocalDate);
    dpos = d.indexOf(QLatin1String("22"));
    mpos = d.indexOf(QLatin1String("11"));
    ypos = d.indexOf(QLatin1String("99"));
    if (dpos > -1 && mpos > -1 && ypos > -1) {
        // test for DMY, MDY, YMD, YDM
        if (dpos < mpos && mpos < ypos) {
            lOrder = Q3DateEdit::DMY;
        } else if (mpos < dpos && dpos < ypos) {
            lOrder = Q3DateEdit::MDY;
        } else if (ypos < mpos && mpos < dpos) {
            lOrder = Q3DateEdit::YMD;
        } else if (ypos < dpos && dpos < mpos) {
            lOrder = Q3DateEdit::YDM;
        } else {
            // cannot determine the dateformat - use the default
            return;
        }

        // this code needs to change if new formats are added

#ifndef Q_WS_WIN
        QString sep = d.mid(qMin(dpos, mpos) + 2, QABS(dpos - mpos) - 2);
        if (d.count(sep) == 2) {
            *lDateSep = sep;
        }
#endif
    }

#ifndef Q_WS_WIN
    QString t = QTime(11, 22, 33).toString(Qt::LocalDate);
    dpos = t.indexOf(QLatin1String("11"));
    mpos = t.indexOf(QLatin1String("22"));
    ypos = t.indexOf(QLatin1String("33"));
    // We only allow hhmmss
    if (dpos > -1 && dpos < mpos && mpos < ypos) {
        QString sep = t.mid(dpos + 2, mpos - dpos - 2);
        if (sep == t.mid(mpos + 2, ypos - mpos - 2)) {
            *lTimeSep = sep;
        }
    }
#endif
}

static Q3DateEdit::Order localOrder() {
    if (!lDateSep) {
        readLocaleSettings();
    }
    return lOrder;
}

static QString localDateSep() {
    if (!lDateSep) {
        readLocaleSettings();
    }
    return *lDateSep;
}

static QString localTimeSep() {
    if (!lTimeSep) {
        readLocaleSettings();
    }
    return *lTimeSep;
}

class Q3DateTimeEditorPrivate
{
public:
    Q3DateTimeEditorPrivate()
        : frm(true),
          parag(new Q3TextParagraph(0, 0, 0, false)),
          focusSec(0)
    {
        parag->formatter()->setWrapEnabled(false);
        cursor = new Q3TextCursor(0);
        cursor->setParagraph(parag);
        offset = 0;
        sep = localDateSep();
        refcount++;
    }
    ~Q3DateTimeEditorPrivate()
    {
        delete parag;
        delete cursor;
        if (!--refcount)
            cleanup();
    }

    void appendSection(const QNumberSection& sec)
    {
        sections.append(sec);

    }
    void clearSections()
    {
        sections.clear();
    }
    void setSectionSelection(int sec, int selstart, int selend)
    {
        if (sec < 0 || sec >= sections.count())
            return;
        sections[sec].setSelectionStart(selstart);
        sections[sec].setSelectionEnd(selend);
    }
    uint sectionCount() const { return (uint)sections.count(); }
    void setSeparator(const QString& s) { sep = s; }
    QString separator() const { return sep; }

    void setFrame(bool f) { frm = f; }
    bool frame() const { return frm; }

    int focusSection() const { return focusSec; }
    int section(const QPoint& p)
    {
        cursor->place(p + QPoint(offset, 0), parag);
        int idx = cursor->index();
        for (int i = 0; i < sections.count(); ++i) {
            if (idx >= sections[i].selectionStart() &&
                 idx <= sections[i].selectionEnd())
                return i;
        }
        return -1;
    }
    QNumberSection section(int idx) const
    {
        return sections[idx];
    }
    bool setFocusSection(int idx)
    {
        if (idx > (int)sections.count()-1 || idx < 0)
            return false;
        if (idx != focusSec) {
            focusSec = idx;
            applyFocusSelection();
            return true;
        }
        return false;
    }

    bool inSectionSelection(int idx)
    {
        for (int i = 0; i < sections.count(); ++i) {
            if (idx >= sections[i].selectionStart() &&
                 idx <= sections[i].selectionEnd())
                return true;
        }
        return false;
    }

    void paint(const QString& txt, bool focus, QPainter& p,
                const QPalette&pal, const QRect& rect, QStyle *style)
    {
        int fw = 0;
        if (frm)
            fw = style->pixelMetric(QStyle::PM_DefaultFrameWidth);

        parag->truncate(0);
        parag->append(txt);
        if (!focus)
            parag->removeSelection(Q3TextDocument::Standard);
        else {
            applyFocusSelection();
        }

        /* color all QDATETIMEEDIT_HIDDEN_CHAR chars to background color */
        Q3TextFormat *fb = parag->formatCollection()->format(p.font(),
                                                             pal.base().color());
        Q3TextFormat *nf = parag->formatCollection()->format(p.font(),
                                                             pal.text().color());
        for (int i = 0; i < txt.length(); ++i) {
            parag->setFormat(i, 1, nf);
            if (inSectionSelection(i))
                continue;
            if (txt.at(i) == QDATETIMEEDIT_HIDDEN_CHAR)
                parag->setFormat(i, 1, fb);
            else
                parag->setFormat(i, 1, nf);
        }
        fb->removeRef();
        nf->removeRef();

        QRect r(rect.x(), rect.y(), rect.width() - 2 * (2 + fw), rect.height());
        parag->pseudoDocument()->docRect = r;
        parag->invalidate(0);
        parag->format();

        int xoff = 2 + fw - offset;
        int yoff = (rect.height() - parag->rect().height() + 1) / 2;
        if (yoff < 0)
            yoff = 0;

        p.translate(xoff, yoff);
        parag->paint(p, pal, 0, true);
        if (frm)
            p.translate(-xoff, -yoff);
    }

    void resize(const QSize& size) { sz = size; }

    int mapSection(int sec)
    {
        return (sec >= 0 && sec < sections.count() ? sections[sec].index() : -1);
    }

protected:
    void applyFocusSelection()
    {
        if (focusSec > -1 && focusSec < sections.count()) {
            int selstart = sections[focusSec].selectionStart();
            int selend = sections[focusSec].selectionEnd();
            parag->setSelection(Q3TextDocument::Standard, selstart, selend);
            parag->format();
            if (parag->at(selstart)->x < offset ||
                 parag->at(selend)->x + parag->string()->width(selend) > offset + sz.width()) {
                offset = parag->at(selstart)->x;
            }
        }
    }
private:
    bool frm;
    Q3TextParagraph *parag;
    Q3TextCursor *cursor;
    QSize sz;
    int focusSec;
    QList< QNumberSection > sections;
    QString sep;
    int offset;
};

class Q3DateTimeEditor : public QWidget
{
    Q_OBJECT
public:
    Q3DateTimeEditor(Q3DateTimeEditBase *widget, QWidget *parent, const char* name=0);
    ~Q3DateTimeEditor();

    void setControlWidget(Q3DateTimeEditBase * widget);
    Q3DateTimeEditBase * controlWidget() const;

    void setSeparator(const QString& s);
    QString separator() const;

    int  focusSection() const;
    bool setFocusSection(int s);
    void appendSection(const QNumberSection& sec);
    void clearSections();
    void setSectionSelection(int sec, int selstart, int selend);
    bool eventFilter(QObject *o, QEvent *e);
    int  sectionAt(const QPoint &p);
    int mapSection(int sec);

protected:
    void init();
    bool event(QEvent *e);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);

private:
    Q3DateTimeEditBase* cw;
    Q3DateTimeEditorPrivate* d;
};

class QDateTimeSpinWidget : public Q3SpinWidget
{
    Q_OBJECT
public:
    QDateTimeSpinWidget(QWidget *parent, const char *name)
        : Q3SpinWidget(parent, name)
    {
    }

    void changeEvent(QEvent *e)
    {
	if (e->type() == QEvent::EnabledChange && isEnabled()) {
	    Q3DateEdit *de = qobject_cast<Q3DateEdit*>(parentWidget());
	    if (de) {
		setUpEnabled(de->date() < de->maxValue());
		setDownEnabled(de->date() > de->minValue());
	    } else {
		setUpEnabled(true);
		setDownEnabled(true);
	    }
	}
    }
    void enabledChange(bool notenabled)
    {
	Q3DateEdit *de = qobject_cast<Q3DateEdit*>(parentWidget());
	if (de && !notenabled) {
	    setUpEnabled(de->date() < de->maxValue());
	    setDownEnabled(de->date() > de->minValue());
	} else {
	    setUpEnabled(!notenabled);
	    setDownEnabled(!notenabled);
	}
    }


protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *e)
    {
        Q3DateTimeEditor *editor = qobject_cast<Q3DateTimeEditor*>(editWidget());
        Q_ASSERT(editor);
        if (!editor)
            return;

        int section = editor->sectionAt(e->pos());
        editor->setFocusSection(section);

        if (section == -1)
            return;
        Q3SpinWidget::wheelEvent(e);
    }
#endif
};

/*!
    Constructs an empty datetime editor with parent \a parent and
    called \a name.
*/
Q3DateTimeEditor::Q3DateTimeEditor(Q3DateTimeEditBase *widget, QWidget *parent, const char * name)
    : QWidget(parent, name)
{
    d = new Q3DateTimeEditorPrivate();
    cw = widget;
    init();
}

/*!
    Destroys the object and frees any allocated resources.
*/

Q3DateTimeEditor::~Q3DateTimeEditor()
{
    delete d;
}

/*! \internal

*/

void Q3DateTimeEditor::init()
{
    setBackgroundRole(QPalette::Base);
    setFocusSection(-1);
    installEventFilter(this);
    setFocusPolicy(Qt::WheelFocus);
}


/*! \reimp

*/

bool Q3DateTimeEditor::event(QEvent *e)
{
    if (e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut) {
        if (e->type() == QEvent::FocusOut)
            qApp->sendEvent(cw, e);
        update(rect());
    } else if (e->type() == QEvent::ShortcutOverride) {
        QKeyEvent* ke = (QKeyEvent*) e;
        switch (ke->key()) {
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
            ke->accept();
        default:
            break;
        }
    }
    return QWidget::event(e);
}

/*! \reimp

*/

void Q3DateTimeEditor::resizeEvent(QResizeEvent *e)
{
    d->resize(e->size());
    QWidget::resizeEvent(e);
}


/*! \reimp

*/

void Q3DateTimeEditor::paintEvent(QPaintEvent *)
{
    QString txt;
    for (uint i = 0; i < d->sectionCount(); ++i) {
        txt += cw->sectionFormattedText(i);
        if (i < d->sectionCount()-1) {
            if (d->section(i+1).separator())
                txt += d->separator();
            else
                txt += QLatin1Char(' ');
        }
    }

    QPainter p(this);
    const QBrush &bg = palette().brush(isEnabled() ? QPalette::Base : QPalette::Window);
    p.fillRect(0, 0, width(), height(), bg);
    d->paint(txt, hasFocus(), p, palette(), rect(), style());
}


/*!
    Returns the section index at point \a p.
*/
int Q3DateTimeEditor::sectionAt(const QPoint &p)
{
    return d->section(p);
}

int Q3DateTimeEditor::mapSection(int sec)
{
    return d->mapSection(sec);
}


/*! \reimp

*/

void Q3DateTimeEditor::mousePressEvent(QMouseEvent *e)
{
    QPoint p(e->pos().x(), 0);
    int sec = sectionAt(p);
    if (sec != -1) {
        cw->setFocusSection(sec);
        repaint(rect());
    }
}

/*! \reimp

*/
bool Q3DateTimeEditor::eventFilter(QObject *o, QEvent *e)
{
    if (o == this) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ke = (QKeyEvent*)e;
            switch (ke->key()) {
            case Qt::Key_Right:
                if (d->focusSection() < (int)d->sectionCount()-1) {
                    if (cw->setFocusSection(focusSection()+1))
                        repaint(rect());
                }
                return true;
            case Qt::Key_Left:
                if (d->focusSection() > 0) {
                    if (cw->setFocusSection(focusSection()-1))
                        repaint(rect());
                }
                return true;
            case Qt::Key_Up:
                cw->stepUp();
                return true;
            case Qt::Key_Down:
                cw->stepDown();
                return true;
            case Qt::Key_Backspace:
                if (qobject_cast<Q3DateEdit*>(cw))
                    ((Q3DateEdit*)cw)->removeFirstNumber(d->focusSection());
                else if (qobject_cast<Q3TimeEdit*>(cw))
                    ((Q3TimeEdit*)cw)->removeFirstNumber(d->focusSection());
                return true;
            case Qt::Key_Delete:
                cw->removeLastNumber(d->focusSection());
                return true;
            case Qt::Key_Tab:
            case Qt::Key_BackTab: {
                if (ke->state() == Qt::ControlButton)
                    return false;
                QWidget *w = this;
                bool hadDateEdit = false;
                while (w) {
                    if (qobject_cast<QDateTimeSpinWidget*>(w) || qobject_cast<Q3DateTimeEdit*>(w))
                        break;
                    hadDateEdit = hadDateEdit || qobject_cast<Q3DateEdit*>(w);
                    w = w->parentWidget();
                }
                if (w) {
                    if (!qobject_cast<Q3DateTimeEdit*>(w)) {
                        w = w->parentWidget();
                    } else {
                        Q3DateTimeEdit *ed = (Q3DateTimeEdit*)w;
                        if (hadDateEdit && ke->key() == Qt::Key_Tab) {
                            ed->timeEdit()->setFocus();
                            return true;
                        } else if (!hadDateEdit && ke->key() == Qt::Key_BackTab) {
                            ed->dateEdit()->setFocus();
                            return true;
                        } else {
                            while (w && !qobject_cast<Q3DateTimeEdit*>(w))
                                w = w->parentWidget();
                        }
                    }
                    qApp->sendEvent(w, e);
                    return true;
                }
            } break;
            default:
                QString txt = ke->text().toLower();
                if (!txt.isEmpty() && !separator().isEmpty() && txt[0] == separator()[0]) {
                    // do the same thing as KEY_RIGHT when the user presses the separator key
                    if (d->focusSection() < 2) {
                        if (cw->setFocusSection(focusSection()+1))
                            repaint(rect());
                    }
                    return true;
                } else if (!txt.isEmpty() && qobject_cast<Q3TimeEdit*>(cw) && focusSection() == (int) d->sectionCount()-1) {
                    // the first character of the AM/PM indicator toggles if the section has focus
                    Q3TimeEdit *te = (Q3TimeEdit*)cw;
                    QTime time = te->time();
                    if (lAMPM && lAM && lPM && (te->display()&Q3TimeEdit::AMPM)) {
                        if (txt[0] == (*lAM).toLower()[0] && time.hour() >= 12) {
                            time.setHMS(time.hour()-12, time.minute(), time.second(), time.msec());
                            te->setTime(time);
                        } else if (txt[0] == (*lPM).toLower()[0] && time.hour() < 12) {
                            time.setHMS(time.hour()+12, time.minute(), time.second(), time.msec());
                            te->setTime(time);
                        }
                    }
                }

                int num = txt[0].digitValue();
                if (num != -1) {
                    cw->addNumber(d->focusSection(), num);
                    return true;
                }
            }
        }
    }
    return false;
}


/*!
    Appends the number section \a sec to the editor.
*/

void Q3DateTimeEditor::appendSection(const QNumberSection& sec)
{
    d->appendSection(sec);
}

/*!
    Removes all sections from the editor.
*/

void Q3DateTimeEditor::clearSections()
{
    d->clearSections();
}

/*!
    Sets the selection of \a sec to start at \a selstart and end at \a
    selend.
*/

void Q3DateTimeEditor::setSectionSelection(int sec, int selstart, int selend)
{
    d->setSectionSelection(sec, selstart, selend);
}

/*!
    Sets the separator for all numbered sections to \a s. Note that
    currently, only the first character of \a s is used.
*/

void Q3DateTimeEditor::setSeparator(const QString& s)
{
    d->setSeparator(s);
    update();
}


/*!
    Returns the editor's separator.
*/

QString Q3DateTimeEditor::separator() const
{
    return d->separator();
}

/*!
    Returns the number of the section that has focus.
*/

int Q3DateTimeEditor::focusSection() const
{
    return d->focusSection();
}


/*!
    Sets the focus to section \a sec. If \a sec does not exist,
    nothing happens.
*/

bool Q3DateTimeEditor::setFocusSection(int sec)
{
    return d->setFocusSection(sec);
}

/*!
    \class Q3DateTimeEditBase
    \brief The Q3DateTimeEditBase class provides an abstraction for date and edit editors.

    \compat

    Small abstract class that provides some functions that are common
    for both Q3DateEdit and Q3TimeEdit. It is used internally by
    Q3DateTimeEditor.
*/

/*!
    \fn Q3DateTimeEditBase::Q3DateTimeEditBase(QWidget *, const char*)
    \internal
*/

/*!
    \fn Q3DateTimeEditBase::setFocusSection(int)
    \internal
*/

/*! \fn QString Q3DateTimeEditBase::sectionFormattedText(int sec)
    \internal

  Pure virtual function which returns the formatted text of section \a
  sec.

*/

/*! \fn void Q3DateTimeEditBase::stepUp()
    \internal

  Pure virtual slot which is called whenever the user increases the
  number in a section by pressing the widget's arrow buttons or the
  keyboard's arrow keys.
*/

/*! \fn void Q3DateTimeEditBase::stepDown()
    \internal

  Pure virtual slot which is called whenever the user decreases the
  number in a section by pressing the widget's arrow buttons or the
  keyboard's arrow keys.

*/

/*! \fn void Q3DateTimeEditBase::addNumber(int sec, int num)
    \internal

  Pure virtual function which is called whenever the user types a number.
  \a sec indicates the section where the number should be added. \a
  num is the number that was pressed.
*/

/*! \fn void Q3DateTimeEditBase::removeLastNumber(int sec)
    \internal

  Pure virtual function which is called whenever the user tries to
  remove the last number from \a sec by pressing the delete key.
*/

////////////////

class Q3DateEditPrivate
{
public:
    int y;
    int m;
    int d;
    // remembers the last entry for the day.
    // if the day is 31 and you cycle through the months,
    // the day will be 31 again if you reach a month with 31 days
    // otherwise it will be the highest day in the month
    int dayCache;
    int yearSection;
    int monthSection;
    int daySection;
    Q3DateEdit::Order ord;
    bool overwrite;
    bool adv;
    int timerId;
    bool typing;
    QDate min;
    QDate max;
    bool changed;
    Q3DateTimeEditor *ed;
    Q3SpinWidget *controls;
};


/*!
    \class Q3DateEdit q3datetimeedit.h
    \brief The Q3DateEdit class provides a date editor.

    \compat

    Q3DateEdit allows the user to edit dates by using the keyboard or
    the arrow keys to increase/decrease date values. The arrow keys
    can be used to move from section to section within the Q3DateEdit
    box. Dates appear in accordance with the local date/time settings
    or in year, month, day order if the system doesn't provide this
    information. It is recommended that the Q3DateEdit be initialised
    with a date, e.g.

    \snippet doc/src/snippets/code/src_qt3support_widgets_q3datetimeedit.cpp 0

    Here we've created a new Q3DateEdit object initialised with today's
    date and restricted the valid date range to today plus or minus
    365 days. We've set the order to month, day, year. If the auto
    advance property is true (as we've set it here) when the user
    completes a section of the date, e.g. enters two digits for the
    month, they are automatically taken to the next section.

    The maximum and minimum values for a date value in the date editor
    default to the maximum and minimum values for a QDate. You can
    change this by calling setMinValue(), setMaxValue() or setRange().

    Terminology: A Q3DateEdit widget comprises three 'sections', one
    each for the year, month and day. You can change the separator
    character using Q3DateTimeEditor::setSeparator(), by default the
    separator will be taken from the systems settings. If that is
    not possible, it defaults to "-".

    \img datetimewidgets.png Date Time Widgets

    \sa QDate Q3TimeEdit Q3DateTimeEdit
*/

/*!
    \enum Q3DateEdit::Order

    This enum defines the order in which the sections that comprise a
    date appear.

    \value MDY month-day-year
    \value DMY day-month-year
    \value YMD year-month-day (the default)
    \omitvalue YDM
*/

/*!
    \enum Q3TimeEdit::Display

    This enum defines the sections that comprise a time

    \value Hours The hours section
    \value Minutes The minutes section
    \value Seconds The seconds section
    \value AMPM The AM/PM section

    The values can be or'ed together to show any combination.
*/

/*!
    Constructs an empty date editor which is a child of \a parent and
    called name \a name.
*/

Q3DateEdit::Q3DateEdit(QWidget * parent, const char * name)
    : Q3DateTimeEditBase(parent, name)
{
    init();
    updateButtons();
}

/*!
    \overload

    Constructs a date editor with the initial value \a date, parent \a
    parent and called \a name.

    The date editor is initialized with \a date.
*/

Q3DateEdit::Q3DateEdit(const QDate& date, QWidget * parent, const char * name)
    : Q3DateTimeEditBase(parent, name)
{
    init();
    setDate(date);
}

/*! \internal
*/
void Q3DateEdit::init()
{
    d = new Q3DateEditPrivate();
    d->controls = new QDateTimeSpinWidget(this, 0);
    d->ed = new Q3DateTimeEditor(this, d->controls);
    d->controls->setEditWidget(d->ed);
    setFocusProxy(d->ed);
    connect(d->controls, SIGNAL(stepUpPressed()), SLOT(stepUp()));
    connect(d->controls, SIGNAL(stepDownPressed()), SLOT(stepDown()));
    connect(this, SIGNAL(valueChanged(QDate)), SLOT(updateButtons()));
    d->ed->appendSection(QNumberSection(0,4));
    d->ed->appendSection(QNumberSection(5,7));
    d->ed->appendSection(QNumberSection(8,10));

    d->yearSection = -1;
    d->monthSection = -1;
    d->daySection = -1;

    d->y = 0;
    d->m = 0;
    d->d = 0;
    d->dayCache = 0;
    setOrder(localOrder());
    setFocusSection(0);
    d->overwrite = true;
    d->adv = false;
    d->timerId = 0;
    d->typing = false;
    d->min = QDate(1752, 9, 14);
    d->max = QDate(8000, 12, 31);
    d->changed = false;

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    refcount++;
}

/*!
    Destroys the object and frees any allocated resources.
*/

Q3DateEdit::~Q3DateEdit()
{
    delete d;
    if (!--refcount)
        cleanup();
}

/*!
    \property Q3DateEdit::minValue

    \brief the editor's minimum value

    Setting the minimum date value is equivalent to calling
    Q3DateEdit::setRange(\e d, maxValue()), where \e d is the minimum
    date. The default minimum date is 1752-09-14.

    \sa maxValue setRange()
*/

QDate Q3DateEdit::minValue() const
{
    return d->min;
}

/*!
    \property Q3DateEdit::maxValue

    \brief the editor's maximum value

    Setting the maximum date value for the editor is equivalent to
    calling Q3DateEdit::setRange(minValue(), \e d), where \e d is the
    maximum date. The default maximum date is 8000-12-31.

    \sa minValue setRange()
*/

QDate Q3DateEdit::maxValue() const
{
    return d->max;
}


/*!
    Sets the valid input range for the editor to be from \a min to \a
    max inclusive. If \a min is invalid no minimum date will be set.
    Similarly, if \a max is invalid no maximum date will be set.
*/

void Q3DateEdit::setRange(const QDate& min, const QDate& max)
{
    if (min.isValid())
        d->min = min;
    if (max.isValid())
        d->max = max;
}

/*!
    Sets the separator to \a s. Note that currently only the first
    character of \a s is used.
*/

void Q3DateEdit::setSeparator(const QString& s)
{
    d->ed->setSeparator(s);
}

/*!
    Returns the editor's separator.
*/

QString Q3DateEdit::separator() const
{
    return d->ed->separator();
}


/*!
    Enables/disables the push buttons according to the min/max date
    for this widget.
*/

void Q3DateEdit::updateButtons()
{
    if (!isEnabled())
        return;

    bool upEnabled = date() < maxValue();
    bool downEnabled = date() > minValue();

    d->controls->setUpEnabled(upEnabled);
    d->controls->setDownEnabled(downEnabled);
}

/*! \reimp
 */
void Q3DateEdit::resizeEvent(QResizeEvent *)
{
    d->controls->resize(width(), height());
}

/*! \reimp

*/
QSize Q3DateEdit::sizeHint() const
{
    ensurePolished();
    QFontMetrics fm(font());
    int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
    int h = qMax(fm.lineSpacing(), 14) + 2;
    int w = 2 + fm.width(QLatin1Char('9')) * 8 + fm.width(d->ed->separator()) * 2 + d->controls->upRect().width() + fw * 4;

    return QSize(w, qMax(h + fw * 2,20)).expandedTo(QApplication::globalStrut());
}

/*! \reimp

*/
QSize Q3DateEdit::minimumSizeHint() const
{
    return sizeHint();
}


/*!
    Returns the formatted number for section \a sec. This will
    correspond to either the year, month or day section, depending on
    the current display order.

    \sa setOrder()
*/

QString Q3DateEdit::sectionFormattedText(int sec)
{
    QString txt;
    txt = sectionText(sec);
    if (d->typing && sec == d->ed->focusSection())
        d->ed->setSectionSelection(sec, sectionOffsetEnd(sec) - txt.length(),
                             sectionOffsetEnd(sec));
    else
        d->ed->setSectionSelection(sec, sectionOffsetEnd(sec) - sectionLength(sec),
                             sectionOffsetEnd(sec));
    txt = txt.rightJustified(sectionLength(sec), QDATETIMEEDIT_HIDDEN_CHAR);
    return txt;
}


/*!
    Returns the desired length (number of digits) of section \a sec.
    This will correspond to either the year, month or day section,
    depending on the current display order.

    \sa setOrder()
*/

int Q3DateEdit::sectionLength(int sec) const
{
    int val = 0;
    if (sec == d->yearSection) {
        val = 4;
    } else if (sec == d->monthSection) {
        val = 2;
    } else if (sec == d->daySection) {
        val = 2;
    }
    return val;
}

/*!
    Returns the text of section \a sec. This will correspond to either
    the year, month or day section, depending on the current display
    order.

    \sa setOrder()
*/

QString Q3DateEdit::sectionText(int sec) const
{
    int val = 0;
    if (sec == d->yearSection) {
        val = d->y;
    } else if (sec == d->monthSection) {
        val = d->m;
    } else if (sec == d->daySection) {
        val = d->d;
    }
    return QString::number(val);
}

/*! \internal

  Returns the end of the section offset \a sec.

*/

int Q3DateEdit::sectionOffsetEnd(int sec) const
{
    if (sec == d->yearSection) {
        switch(d->ord) {
        case DMY:
        case MDY:
            return sectionOffsetEnd(sec-1) + separator().length() + sectionLength(sec);
        case YMD:
        case YDM:
            return sectionLength(sec);
        }
    } else if (sec == d->monthSection) {
        switch(d->ord) {
        case DMY:
        case YDM:
        case YMD:
            return sectionOffsetEnd(sec-1) + separator().length() + sectionLength(sec);
        case MDY:
            return sectionLength(sec);
        }
    } else if (sec == d->daySection) {
        switch(d->ord) {
        case DMY:
            return sectionLength(sec);
        case YMD:
        case MDY:
        case YDM:
            return sectionOffsetEnd(sec-1) + separator().length() + sectionLength(sec);
        }
    }
    return 0;
}


/*!
    \property Q3DateEdit::order
    \brief the order in which the year, month and day appear

    The default order is locale dependent.

    \sa Order
*/

void Q3DateEdit::setOrder(Q3DateEdit::Order order)
{
    d->ord = order;
    switch(d->ord) {
    case DMY:
        d->yearSection = 2;
        d->monthSection = 1;
        d->daySection = 0;
        break;
    case MDY:
        d->yearSection = 2;
        d->monthSection = 0;
        d->daySection = 1;
        break;
    case YMD:
        d->yearSection = 0;
        d->monthSection = 1;
        d->daySection = 2;
        break;
    case YDM:
        d->yearSection = 0;
        d->monthSection = 2;
        d->daySection = 1;
        break;
    }
    if (isVisible())
        d->ed->repaint(d->ed->rect());
}


Q3DateEdit::Order Q3DateEdit::order() const
{
    return d->ord;
}


/*! \reimp

*/
void Q3DateEdit::stepUp()
{
    int sec = d->ed->focusSection();
    bool accepted = false;
    if (sec == d->yearSection) {
        if (!outOfRange(d->y+1, d->m, d->d)) {
            accepted = true;
            setYear(d->y+1);
        }
    } else if (sec == d->monthSection) {
        if (!outOfRange(d->y, d->m+1, d->d)) {
            accepted = true;
            setMonth(d->m+1);
        }
    } else if (sec == d->daySection) {
        if (!outOfRange(d->y, d->m, d->d+1)) {
            accepted = true;
            setDay(d->d+1);
        }
    }
    if (accepted) {
        d->changed = false;
        emit valueChanged(date());
    }
    d->ed->repaint(d->ed->rect());
}



/*! \reimp

*/

void Q3DateEdit::stepDown()
{
    int sec = d->ed->focusSection();
    bool accepted = false;
    if (sec == d->yearSection) {
        if (!outOfRange(d->y-1, d->m, d->d)) {
            accepted = true;
            setYear(d->y-1);
        }
    } else if (sec == d->monthSection) {
        if (!outOfRange(d->y, d->m-1, d->d)) {
            accepted = true;
            setMonth(d->m-1);
        }
    } else if (sec == d->daySection) {
        if (!outOfRange(d->y, d->m, d->d-1)) {
            accepted = true;
            setDay(d->d-1);
        }
    }
    if (accepted) {
        d->changed = false;
        emit valueChanged(date());
    }
    d->ed->repaint(d->ed->rect());
}

/*!
    Sets the year to \a year, which must be a valid year. The range
    currently supported is from 1752 to 8000.

    \sa QDate
*/

void Q3DateEdit::setYear(int year)
{
    if (year < 1752)
        year = 1752;
    if (year > 8000)
        year = 8000;
    if (!outOfRange(year, d->m, d->d)) {
        d->y = year;
        setMonth(d->m);
        int tmp = d->dayCache;
        setDay(d->dayCache);
        d->dayCache = tmp;
    }
}


/*!
    Sets the month to \a month, which must be a valid month, i.e.
    between 1 and 12.
*/

void Q3DateEdit::setMonth(int month)
{
    if (month < 1)
        month = 1;
    if (month > 12)
        month = 12;
    if (!outOfRange(d->y, month, d->d)) {
        d->m = month;
        int tmp = d->dayCache;
        setDay(d->dayCache);
        d->dayCache = tmp;
    }
}


/*!
    Sets the day to \a day, which must be a valid day. The function
    will ensure that the \a day set is valid for the month and year.
*/

void Q3DateEdit::setDay(int day)
{
    if (day < 1)
        day = 1;
    if (day > 31)
        day = 31;
    if (d->m > 0 && d->y > 1752) {
        while (!QDate::isValid(d->y, d->m, day))
            --day;
        if (!outOfRange(d->y, d->m, day))
            d->d = day;
    } else if (d->m > 0) {
        if (day > 0 && day < 32) {
            if (!outOfRange(d->y, d->m, day))
                d->d = day;
        }
    }
    d->dayCache = d->d;
}


/*!
    \property Q3DateEdit::date
    \brief the editor's date value.

    If the date property is not valid, the editor displays all zeroes
    and Q3DateEdit::date() will return an invalid date. It is strongly
    recommended that the editor is given a default date value (e.g.
    currentDate()). That way, attempts to set the date property to an
    invalid date will fail.

    When changing the date property, if the date is less than
    minValue(), or is greater than maxValue(), nothing happens.
*/

void Q3DateEdit::setDate(const QDate& date)
{
    if (!date.isValid()) {
        d->y = 0;
        d->m = 0;
        d->d = 0;
        d->dayCache = 0;
    } else {
        if (date > maxValue() || date < minValue())
            return;
        d->y = date.year();
        d->m = date.month();
        d->d = date.day();
        d->dayCache = d->d;
        emit valueChanged(date);
    }
    d->changed = false;
    d->ed->repaint(d->ed->rect());
}

QDate Q3DateEdit::date() const
{
    if (QDate::isValid(d->y, d->m, d->d))
        return QDate(d->y, d->m, d->d);
    return QDate();
}

/*!  \internal

  Returns true if \a y, \a m, \a d is out of range, otherwise returns
  false.

  \sa setRange()

*/

bool Q3DateEdit::outOfRange(int y, int m, int d) const
{
    if (QDate::isValid(y, m, d)) {
        QDate currentDate(y, m, d);
        if (currentDate > maxValue() ||
             currentDate < minValue()) {
            //## outOfRange should set overwrite?
            return true;
        }
        return false;
    }
    return false; /* assume ok */
}

/*!  \reimp

*/

void Q3DateEdit::addNumber(int sec, int num)
{
    if (sec == -1)
        return;
    if (d->timerId)
        killTimer(d->timerId);
    d->timerId = 0;
    bool overwrite = false;
    bool accepted = false;
    d->typing = true;
    QString txt;
    if (sec == d->yearSection) {
        txt = QString::number(d->y);
        if (d->overwrite || txt.length() == 4) {
            accepted = true;
            d->y = num;
        } else {
            txt += QString::number(num);
            if (txt.length() == 4 ) {
                const int val = qBound(1792, txt.toInt(), 8000);
                if (outOfRange(val, d->m, d->d)) {
                    txt = QString::number(d->y);
                } else {
                    accepted = true;
                    d->y = val;
                }
            } else {
                accepted = true;
                d->y = txt.toInt();
            }
            if (d->adv && txt.length() == 4) {
                d->ed->setFocusSection(d->ed->focusSection()+1);
                overwrite = true;
            }
        }
    } else if (sec == d->monthSection) {
        txt = QString::number(d->m);
        if (d->overwrite || txt.length() == 2) {
            accepted = true;
            d->m = num;
        } else {
            txt += QString::number(num);
            int temp = txt.toInt();
            if (temp > 12)
                temp = num;
            if (outOfRange(d->y, temp, d->d))
                txt = QString::number(d->m);
            else {
                accepted = true;
                d->m = temp;
            }
            if (d->adv && txt.length() == 2) {
                d->ed->setFocusSection(d->ed->focusSection()+1);
                overwrite = true;
            }
        }
    } else if (sec == d->daySection) {
        txt = QString::number(d->d);
        if (d->overwrite || txt.length() == 2) {
            accepted = true;
            d->d = num;
            d->dayCache = d->d;
        } else {
            txt += QString::number(num);
            int temp = txt.toInt();
            if (temp > 31)
                temp = num;
            if (outOfRange(d->y, d->m, temp))
                txt = QString::number(d->d);
            else {
                accepted = true;
                d->d = temp;
                d->dayCache = d->d;
            }
            if (d->adv && txt.length() == 2) {
                d->ed->setFocusSection(d->ed->focusSection()+1);
                overwrite = true;
            }
        }
    }
    if (accepted) {
        d->changed = false;
        emit valueChanged(date());
    }
    d->overwrite = overwrite;
    d->timerId = startTimer(qApp->doubleClickInterval()*4);
    d->ed->repaint(d->ed->rect());
}


/*! \reimp

*/

bool Q3DateEdit::setFocusSection(int s)
{
    if (s != d->ed->focusSection()) {
        if (d->timerId)
            killTimer(d->timerId);
        d->timerId = 0;
        d->overwrite = true;
        d->typing = false;
        fix(); // will emit valueChanged if necessary
    }
    return d->ed->setFocusSection(s);
}


/*!
    Attempts to fix any invalid date entries.

    The rules applied are as follows:

    \list
    \i If the year has four digits it is left unchanged.
    \i If the year has two digits, the year will be changed to four
    digits in the range current year - 70 to current year + 29.
    \i If the year has three digits in the range 100..999, the
    current millennium, i.e. 2000, will be added giving a year
    in the range 2100..2999.
    \i If the day or month is 0 then it will be set to 1 or the
    minimum valid day/month in the range.
    \endlist
*/

void Q3DateEdit::fix()
{
    bool changed = false;
    int currentYear = QDate::currentDate().year();
    int year = d->y;
    if (year < 100) {
        int currentCentury = currentYear / 100;
        year += currentCentury * 100;
        if (currentYear > year) {
            if (currentYear > year + 70)
                year += 100;
        } else {
            if (year >= currentYear + 30)
                year -= 100;
        }
        changed = true;
    } else if (year < 1000) {
        int currentMillennium = currentYear / 10;
        year += currentMillennium * 10;
        changed = true;
    } else if (d->d == 0) {
	d->d = 1;
	changed = true;
    } else if (d->m == 0) {
	d->m = 1;
	changed = true;
    }
    if (outOfRange(year, d->m, d->d)) {
        if (minValue().isValid() && date() < minValue()) {
            d->d =  minValue().day();
            d->dayCache = d->d;
            d->m = minValue().month();
            d->y = minValue().year();
        }
        if (date() > maxValue()) {
            d->d =  maxValue().day();
            d->dayCache = d->d;
            d->m = maxValue().month();
            d->y = maxValue().year();
        }
	changed = true;
    } else if (changed)
        setYear(year);
    if (changed) {
        emit valueChanged(date());
        d->changed = false;
    }
}


/*! \reimp

*/

bool Q3DateEdit::event(QEvent *e)
{
    if(e->type() == QEvent::FocusOut) {
        d->typing = false;
        fix();
        // the following can't be done in fix() because fix() called
        // from all over the place and it will break the old behaviour
        if (!QDate::isValid(d->y, d->m, d->d)) {
            d->dayCache = d->d;
            int i = d->d;
            for (; i > 0; i--) {
                d->d = i;
                if (QDate::isValid(d->y, d->m, d->d))
                    break;
            }
            d->changed = true;
        }
        if (d->changed) {
            emit valueChanged(date());
            d->changed = false;
        }
    } else if (e->type() == QEvent::LocaleChange) {
        readLocaleSettings();
        d->ed->setSeparator(localDateSep());
        setOrder(localOrder());
    }
    return Q3DateTimeEditBase::event(e);
}

/*!
  \internal

  Function which is called whenever the user tries to
  remove the first number from \a sec by pressing the backspace key.
*/

void Q3DateEdit::removeFirstNumber(int sec)
{
    if (sec == -1)
        return;
    QString txt;
    if (sec == d->yearSection) {
        txt = QString::number(d->y);
        txt = txt.mid(1, txt.length()) + QLatin1Char('0');
        d->y = txt.toInt();
    } else if (sec == d->monthSection) {
        txt = QString::number(d->m);
        txt = txt.mid(1, txt.length()) + QLatin1Char('0');
        d->m = txt.toInt();
    } else if (sec == d->daySection) {
        txt = QString::number(d->d);
        txt = txt.mid(1, txt.length()) + QLatin1Char('0');
        d->d = txt.toInt();
        d->dayCache = d->d;
    }
    d->ed->repaint(d->ed->rect());
}

/*! \reimp

*/

void Q3DateEdit::removeLastNumber(int sec)
{
    if (sec == -1)
        return;
    QString txt;
    if (sec == d->yearSection) {
        txt = QString::number(d->y);
        txt = txt.mid(0, txt.length()-1);
        d->y = txt.toInt();
    } else if (sec == d->monthSection) {
        txt = QString::number(d->m);
        txt = txt.mid(0, txt.length()-1);
        d->m = txt.toInt();
    } else if (sec == d->daySection) {
        txt = QString::number(d->d);
        txt = txt.mid(0, txt.length()-1);
        d->d = txt.toInt();
        d->dayCache = d->d;
    }
    d->ed->repaint(d->ed->rect());
}

/*!
    \property Q3DateEdit::autoAdvance
    \brief whether the editor automatically advances to the next
    section

    If autoAdvance is true, the editor will automatically advance
    focus to the next date section if a user has completed a section.
    The default is false.
*/

void Q3DateEdit::setAutoAdvance(bool advance)
{
    d->adv = advance;
}


bool Q3DateEdit::autoAdvance() const
{
    return d->adv;
}

/*! \reimp
*/

void Q3DateEdit::timerEvent(QTimerEvent *)
{
    d->overwrite = true;
}

/*!
    \fn void Q3DateEdit::valueChanged(const QDate& date)

    This signal is emitted whenever the editor's value changes. The \a
    date parameter is the new value.
*/

///////////

class Q3TimeEditPrivate
{
public:
    int h;
    int m;
    int s;
    uint display;
    bool adv;
    bool overwrite;
    int timerId;
    bool typing;
    QTime min;
    QTime max;
    bool changed;
    Q3DateTimeEditor *ed;
    Q3SpinWidget *controls;
};

/*!
    \class Q3TimeEdit q3datetimeedit.h
    \brief The Q3TimeEdit class provides a time editor.

    \compat

    Q3TimeEdit allows the user to edit times by using the keyboard or
    the arrow keys to increase/decrease time values. The arrow keys
    can be used to move from section to section within the Q3TimeEdit
    box. The user can automatically be moved to the next section once
    they complete a section using setAutoAdvance(). Times appear in
    hour, minute, second order. It is recommended that the Q3TimeEdit
    is initialised with a time, e.g.
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3datetimeedit.cpp 1
    Here we've created a Q3TimeEdit widget set to the current time.
    We've also set the minimum value to the current time and the
    maximum time to one hour from now.

    The maximum and minimum values for a time value in the time editor
    default to the maximum and minimum values for a QTime. You can
    change this by calling setMinValue(), setMaxValue() or setRange().

    Terminology: A QTimeWidget consists of three sections, one each
    for the hour, minute and second. You can change the separator
    character using setSeparator(), by default the separator is read
    from the system's settings.

    \img datetimewidgets.png Date Time Widgets

    \sa QTime Q3DateEdit Q3DateTimeEdit
*/


/*!
    Constructs an empty time edit with parent \a parent and called \a
    name.
*/

Q3TimeEdit::Q3TimeEdit(QWidget * parent, const char * name)
    : Q3DateTimeEditBase(parent, name)
{
    init();
}

/*!
    \overload

    Constructs a time edit with the initial time value, \a time,
    parent \a parent and called \a name.
*/

Q3TimeEdit::Q3TimeEdit(const QTime& time, QWidget * parent, const char * name)
    : Q3DateTimeEditBase(parent, name)
{
    init();
    setTime(time);
}

/*! \internal
 */

void Q3TimeEdit::init()
{
    d = new Q3TimeEditPrivate();
    d->controls = new QDateTimeSpinWidget(this, 0);
    d->ed = new Q3DateTimeEditor(this, d->controls, "time edit base");
    d->controls->setEditWidget(d->ed);
    setFocusProxy(d->ed);
    connect(d->controls, SIGNAL(stepUpPressed()), SLOT(stepUp()));
    connect(d->controls, SIGNAL(stepDownPressed()), SLOT(stepDown()));

    d->ed->appendSection(QNumberSection(0,0, true, 0));
    d->ed->appendSection(QNumberSection(0,0, true, 1));
    d->ed->appendSection(QNumberSection(0,0, true, 2));
    d->ed->setSeparator(localTimeSep());

    d->h = 0;
    d->m = 0;
    d->s = 0;
    d->display = Hours | Minutes | Seconds;
    if (lAMPM) {
        d->display |= AMPM;
        d->ed->appendSection(QNumberSection(0,0, false, 3));
    }
    d->adv = false;
    d->overwrite = true;
    d->timerId = 0;
    d->typing = false;
    d->min = QTime(0, 0, 0);
    d->max = QTime(23, 59, 59);
    d->changed = false;

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    refcount++;
}

/*!
    Destroys the object and frees any allocated resources.
*/

Q3TimeEdit::~Q3TimeEdit()
{
    delete d;
    if (!--refcount)
        cleanup();
}

/*!
    \property Q3TimeEdit::minValue
    \brief the minimum time value

    Setting the minimum time value is equivalent to calling
    Q3TimeEdit::setRange(\e t, maxValue()), where \e t is the minimum
    time. The default minimum time is 00:00:00.

    \sa maxValue setRange()
*/

QTime Q3TimeEdit::minValue() const
{
    return d->min;
}

/*!
    \property Q3TimeEdit::maxValue
    \brief the maximum time value

    Setting the maximum time value is equivalent to calling
    Q3TimeEdit::setRange(minValue(), \e t), where \e t is the maximum
    time. The default maximum time is 23:59:59.

    \sa minValue setRange()
*/

QTime Q3TimeEdit::maxValue() const
{
    return d->max;
}


/*!
    Sets the valid input range for the editor to be from \a min to \a
    max inclusive. If \a min is invalid no minimum time is set.
    Similarly, if \a max is invalid no maximum time is set.
*/

void Q3TimeEdit::setRange(const QTime& min, const QTime& max)
{
    if (min.isValid())
        d->min = min;
    if (max.isValid())
        d->max = max;
}

/*!
  \property Q3TimeEdit::display
  \brief the sections that are displayed in the time edit

  The value can be any combination of the values in the Display enum.
  By default, the widget displays hours, minutes and seconds.
*/
void Q3TimeEdit::setDisplay(uint display)
{
    if (d->display == display)
        return;

    d->ed->clearSections();
    d->display = display;
    if (d->display & Hours)
        d->ed->appendSection(QNumberSection(0,0, true, 0));
    if (d->display & Minutes)
        d->ed->appendSection(QNumberSection(0,0, true, 1));
    if (d->display & Seconds)
        d->ed->appendSection(QNumberSection(0,0, true, 2));
    if (d->display & AMPM)
        d->ed->appendSection(QNumberSection(0,0, false, 3));

    d->ed->setFocusSection(0);
    d->ed->update();
}

uint Q3TimeEdit::display() const
{
    return d->display;
}

/*!
    \property Q3TimeEdit::time
    \brief the editor's time value.

    When changing the time property, if the time is less than
    minValue(), or is greater than maxValue(), nothing happens.
*/

void Q3TimeEdit::setTime(const QTime& time)
{
    if (!time.isValid()) {
        d->h = 0;
        d->m = 0;
        d->s = 0;
    } else {
        if (time > maxValue() || time < minValue())
            return;
        d->h = time.hour();
        d->m = time.minute();
        d->s = time.second();
        emit valueChanged(time);
    }
    d->changed = false;
    d->ed->repaint(d->ed->rect());
}

QTime Q3TimeEdit::time() const
{
    if (QTime::isValid(d->h, d->m, d->s))
        return QTime(d->h, d->m, d->s);
    return QTime();
}

/*!
    \property Q3TimeEdit::autoAdvance
    \brief whether the editor automatically advances to the next
    section

    If autoAdvance is true, the editor will automatically advance
    focus to the next time section if a user has completed a section.
    The default is false.
*/

void Q3TimeEdit::setAutoAdvance(bool advance)
{
    d->adv = advance;
}

bool Q3TimeEdit::autoAdvance() const
{
    return d->adv;
}

/*!
    Sets the separator to \a s. Note that currently only the first
    character of \a s is used.
*/

void Q3TimeEdit::setSeparator(const QString& s)
{
    d->ed->setSeparator(s);
}

/*!
    Returns the editor's separator.
*/

QString Q3TimeEdit::separator() const
{
    return d->ed->separator();
}


/*!
    \fn void Q3TimeEdit::valueChanged(const QTime& time)

    This signal is emitted whenever the editor's value changes. The \a
    time parameter is the new value.
*/

/*! \reimp

*/

bool Q3TimeEdit::event(QEvent *e)
{
    if (e->type() == QEvent::FocusOut) {
        d->typing = false;
        if (d->changed) {
            emit valueChanged(time());
            d->changed = false;
        }
    } else if (e->type() == QEvent::LocaleChange) {
        readLocaleSettings();
        d->ed->setSeparator(localTimeSep());
    }
    return Q3DateTimeEditBase::event(e);
}

/*! \reimp

*/

void Q3TimeEdit::timerEvent(QTimerEvent *)
{
    d->overwrite = true;
}


/*! \reimp

*/

void Q3TimeEdit::stepUp()
{
    int sec = d->ed->mapSection(d->ed->focusSection());
    bool accepted = true;
    switch(sec) {
    case 0:
        if (!outOfRange(d->h+1, d->m, d->s))
            setHour(d->h+1);
        else
            setHour(d->min.hour());
        break;
    case 1:
        if (!outOfRange(d->h, d->m+1, d->s))
            setMinute(d->m+1);
        else
            setMinute(d->min.minute());
        break;
    case 2:
        if (!outOfRange(d->h, d->m, d->s+1))
            setSecond(d->s+1);
        else
            setSecond(d->min.second());
        break;
    case 3:
        if (d->h < 12)
            setHour(d->h+12);
        else
            setHour(d->h-12);
        break;
    default:
        accepted = false;
        qWarning("Q3TimeEdit::stepUp: Focus section out of range!");
        break;
    }
    if (accepted) {
        d->changed = false;
        emit valueChanged(time());
    }
    d->ed->repaint(d->ed->rect());
}


/*! \reimp

*/

void Q3TimeEdit::stepDown()
{
    int sec = d->ed->mapSection(d->ed->focusSection());

    bool accepted = true;
    switch(sec) {
    case 0:
        if (!outOfRange(d->h-1, d->m, d->s))
            setHour(d->h-1);
        else
            setHour(d->max.hour());
        break;
    case 1:
        if (!outOfRange(d->h, d->m-1, d->s))
            setMinute(d->m-1);
        else
            setMinute(d->max.minute());
        break;
    case 2:
        if (!outOfRange(d->h, d->m, d->s-1))
            setSecond(d->s-1);
        else
            setSecond(d->max.second());
        break;
    case 3:
        if (d->h > 11)
            setHour(d->h-12);
        else
            setHour(d->h+12);
        break;
    default:
        accepted = false;
        qWarning("Q3TimeEdit::stepDown: Focus section out of range!");
        break;
    }
    if (accepted) {
        d->changed = false;
        emit valueChanged(time());
    }
    d->ed->repaint(d->ed->rect());
}


/*!
    Returns the formatted number for section \a sec. This will
    correspond to either the hour, minute or second section, depending
    on \a sec.
*/

QString Q3TimeEdit::sectionFormattedText(int sec)
{
    QString txt;
    txt = sectionText(sec);
    txt = txt.rightJustified(2, QDATETIMEEDIT_HIDDEN_CHAR);
    int offset = sec*2+sec*separator().length() + txt.length();
    if (d->typing && sec == d->ed->focusSection())
        d->ed->setSectionSelection(sec, offset - txt.length(), offset);
    else
        d->ed->setSectionSelection(sec, offset - txt.length(), offset);

    return txt;
}


/*! \reimp

*/

bool Q3TimeEdit::setFocusSection(int sec)
{
    if (sec != d->ed->focusSection()) {
        if (d->timerId)
            killTimer(d->timerId);
        d->timerId = 0;
        d->overwrite = true;
        d->typing = false;
        QString txt = sectionText(sec);
        txt = txt.rightJustified(2, QDATETIMEEDIT_HIDDEN_CHAR);
        int offset = sec*2+sec*separator().length() + txt.length();
        d->ed->setSectionSelection(sec, offset - txt.length(), offset);
        if (d->changed) {
            emit valueChanged(time());
            d->changed = false;
        }
    }
    return d->ed->setFocusSection(sec);
}


/*!
    Sets the hour to \a h, which must be a valid hour, i.e. in the
    range 0..24.
*/

void Q3TimeEdit::setHour(int h)
{
    if (h < 0)
        h = 0;
    if (h > 23)
        h = 23;
    d->h = h;
}


/*!
    Sets the minute to \a m, which must be a valid minute, i.e. in the
    range 0..59.
*/

void Q3TimeEdit::setMinute(int m)
{
    if (m < 0)
        m = 0;
    if (m > 59)
        m = 59;
    d->m = m;
}


/*!
    Sets the second to \a s, which must be a valid second, i.e. in the
    range 0..59.
*/

void Q3TimeEdit::setSecond(int s)
{
    if (s < 0)
        s = 0;
    if (s > 59)
        s = 59;
    d->s = s;
}


/*! \internal

  Returns the text of section \a sec.

*/

QString Q3TimeEdit::sectionText(int sec)
{
    sec = d->ed->mapSection(sec);

    QString txt;
    switch(sec) {
    case 0:
        if (!(d->display & AMPM) || (d->h < 13 && d->h)) {    // I wished the day stared at 0:00 for everybody
            txt = QString::number(d->h);
        } else {
            if (d->h)
                txt = QString::number(d->h - 12);
            else
                txt = QLatin1String("12");
        }
        break;
    case 1:
        txt = QString::number(d->m);
        break;
    case 2:
        txt = QString::number(d->s);
        break;
    case 3:
        if (d->h < 12) {
            if (lAM)
                txt = *lAM;
            else
                txt = QString::fromLatin1("AM");
        } else {
            if (lPM)
                txt = *lPM;
            else
                txt = QString::fromLatin1("PM");
        }
        break;
    default:
        break;
    }
    return txt;
}


/*! \internal
 Returns true if \a h, \a m, and \a s are out of range.
 */

bool Q3TimeEdit::outOfRange(int h, int m, int s) const
{
    if (QTime::isValid(h, m, s)) {
        QTime currentTime(h, m, s);
        if (currentTime > maxValue() ||
             currentTime < minValue())
            return true;
        else
            return false;
    }
    return true;
}

/*! \reimp

*/

void Q3TimeEdit::addNumber(int sec, int num)
{
    if (sec == -1)
        return;
    sec = d->ed->mapSection(sec);
    if (d->timerId)
        killTimer(d->timerId);
    d->timerId = 0;
    bool overwrite = false;
    bool accepted = false;
    d->typing = true;
    QString txt;

    switch(sec) {
    case 0:
        txt = (d->display & AMPM && d->h > 12) ?
            QString::number(d->h - 12) : QString::number(d->h);

        if (d->overwrite || txt.length() == 2) {
            if (d->display & AMPM && num == 0)
                break; // Don't process 0 in 12 hour clock mode
            if (d->display & AMPM && d->h > 11)
                num += 12;
            if (!outOfRange(num, d->m, d->s)) {
                accepted = true;
                d->h = num;
            }
        } else {
            txt += QString::number(num);
            int temp = txt.toInt();

            if (d->display & AMPM) {
                if (temp == 12) {
                    if (d->h < 12) {
                        temp = 0;
                    }
                    accepted = true;
                } else if (outOfRange(temp + 12, d->m, d->s)) {
                    txt = QString::number(d->h);
                } else {
                    if (d->h > 11) {
                        temp += 12;
                    }
                    accepted = true;
                }
            } else if (!(d->display & AMPM) && outOfRange(temp, d->m, d->s)) {
                txt = QString::number(d->h);
            } else {
                accepted = true;
            }

            if (accepted)
                d->h = temp;

            if (d->adv && txt.length() == 2) {
                setFocusSection(d->ed->focusSection()+1);
                overwrite = true;
            }
        }
        break;

    case 1:
        txt = QString::number(d->m);
        if (d->overwrite || txt.length() == 2) {
            if (!outOfRange(d->h, num, d->s)) {
                accepted = true;
                d->m = num;
            }
        } else {
            txt += QString::number(num);
            int temp = txt.toInt();
            if (temp > 59)
                temp = num;
            if (outOfRange(d->h, temp, d->s))
                txt = QString::number(d->m);
            else {
                accepted = true;
                d->m = temp;
            }
            if (d->adv && txt.length() == 2) {
                setFocusSection(d->ed->focusSection()+1);
                overwrite = true;
            }
        }
        break;

    case 2:
        txt = QString::number(d->s);
        if (d->overwrite || txt.length() == 2) {
            if (!outOfRange(d->h, d->m, num)) {
                accepted = true;
                d->s = num;
            }
        } else {
            txt += QString::number(num);
            int temp = txt.toInt();
            if (temp > 59)
                temp = num;
            if (outOfRange(d->h, d->m, temp))
                txt = QString::number(d->s);
            else {
                accepted = true;
                d->s = temp;
            }
            if (d->adv && txt.length() == 2) {
                setFocusSection(d->ed->focusSection()+1);
                overwrite = true;
            }
        }
        break;

    case 3:
        break;

    default:
        break;
    }
    d->changed = !accepted;
    if (accepted)
        emit valueChanged(time());
    d->overwrite = overwrite;
    d->timerId = startTimer(qApp->doubleClickInterval()*4);
    d->ed->repaint(d->ed->rect());
}


/*!
  \internal

  Function which is called whenever the user tries to
  remove the first number from \a sec by pressing the backspace key.
*/

void Q3TimeEdit::removeFirstNumber(int sec)
{
    if (sec == -1)
        return;
    sec = d->ed->mapSection(sec);
    QString txt;
    switch(sec) {
    case 0:
        txt = QString::number(d->h);
        break;
    case 1:
        txt = QString::number(d->m);
        break;
    case 2:
        txt = QString::number(d->s);
        break;
    }
    txt = txt.mid(1, txt.length()) + QLatin1Char('0');
    switch(sec) {
    case 0:
        d->h = txt.toInt();
        break;
    case 1:
        d->m = txt.toInt();
        break;
    case 2:
        d->s = txt.toInt();
        break;
    }
    d->ed->repaint(d->ed->rect());
}

/*! \reimp

*/
void Q3TimeEdit::removeLastNumber(int sec)
{
    if (sec == -1)
        return;
    sec = d->ed->mapSection(sec);
    QString txt;
    switch(sec) {
    case 0:
        txt = QString::number(d->h);
        break;
    case 1:
        txt = QString::number(d->m);
        break;
    case 2:
        txt = QString::number(d->s);
        break;
    }
    txt = txt.mid(0, txt.length()-1);
    switch(sec) {
    case 0:
        d->h = txt.toInt();
        break;
    case 1:
        d->m = txt.toInt();
        break;
    case 2:
        d->s = txt.toInt();
        break;
    }
    d->ed->repaint(d->ed->rect());
}

/*! \reimp
 */
void Q3TimeEdit::resizeEvent(QResizeEvent *)
{
    d->controls->resize(width(), height());
}

/*! \reimp
*/
QSize Q3TimeEdit::sizeHint() const
{
    ensurePolished();
    QFontMetrics fm(font());
    int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
    int h = fm.lineSpacing() + 2;
    int w = 2 + fm.width(QLatin1Char('9')) * 6 + fm.width(d->ed->separator()) * 2 +
        d->controls->upRect().width() + fw * 4;
    if (d->display & AMPM) {
        if (lAM)
            w += fm.width(*lAM) + 4;
        else
            w += fm.width(QString::fromLatin1("AM")) + 4;
    }

    return QSize(w, qMax(h + fw * 2,20)).expandedTo(QApplication::globalStrut());
}

/*! \reimp
*/
QSize Q3TimeEdit::minimumSizeHint() const
{
    return sizeHint();
}

/*!
    \internal
    Enables/disables the push buttons according to the min/max time
    for this widget.
*/

void Q3TimeEdit::updateButtons()
{
    if (!isEnabled())
        return;

    bool upEnabled = time() < maxValue();
    bool downEnabled = time() > minValue();

    d->controls->setUpEnabled(upEnabled);
    d->controls->setDownEnabled(downEnabled);
}


class Q3DateTimeEditPrivate
{
public:
    bool adv;
};

/*!
    \class Q3DateTimeEdit q3datetimeedit.h
    \brief The Q3DateTimeEdit class combines a Q3DateEdit and Q3TimeEdit
    widget into a single widget for editing datetimes.

    \compat

    Q3DateTimeEdit consists of a Q3DateEdit and Q3TimeEdit widget placed
    side by side and offers the functionality of both. The user can
    edit the date and time by using the keyboard or the arrow keys to
    increase/decrease date or time values. The Tab key can be used to
    move from section to section within the Q3DateTimeEdit widget, and
    the user can be moved automatically when they complete a section
    using setAutoAdvance(). The datetime can be set with
    setDateTime().

    The date format is read from the system's locale settings. It is
    set to year, month, day order if that is not possible. See
    Q3DateEdit::setOrder() to change this. Times appear in the order
    hours, minutes, seconds using the 24 hour clock.

    It is recommended that the Q3DateTimeEdit is initialised with a
    datetime, e.g.
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3datetimeedit.cpp 2
    Here we've created a new Q3DateTimeEdit set to the current date and
    time, and set the date to have a minimum date of now and a maximum
    date of a week from now.

    Terminology: A Q3DateEdit widget consists of three 'sections', one
    each for the year, month and day. Similarly a Q3TimeEdit consists
    of three sections, one each for the hour, minute and second. The
    character that separates each date section is specified with
    setDateSeparator(); similarly setTimeSeparator() is used for the
    time sections.

    \img datetimewidgets.png Date Time Widgets

    \sa Q3DateEdit Q3TimeEdit
*/

/*!
    Constructs an empty datetime edit with parent \a parent and called
    \a name.
*/
Q3DateTimeEdit::Q3DateTimeEdit(QWidget * parent, const char * name)
    : QWidget(parent, name)
{
    init();
}


/*!
    \overload

    Constructs a datetime edit with the initial value \a datetime,
    parent \a parent and called \a name.
*/
Q3DateTimeEdit::Q3DateTimeEdit(const QDateTime& datetime,
                              QWidget * parent, const char * name)
    : QWidget(parent, name)
{
    init();
    setDateTime(datetime);
}



/*!
    Destroys the object and frees any allocated resources.
*/

Q3DateTimeEdit::~Q3DateTimeEdit()
{
    delete d;
}


/*!
    \reimp

    Intercepts and handles resize events which have special meaning
    for the Q3DateTimeEdit.
*/

void Q3DateTimeEdit::resizeEvent(QResizeEvent *)
{
    int dw = de->sizeHint().width();
    int tw = te->sizeHint().width();
    int w = width();
    int h = height();
    int extra = w - (dw + tw);

    if (tw + extra < 0) {
        dw = w;
    } else {
        dw += 9 * extra / 16;
    }
    tw = w - dw;

    de->setGeometry(0, 0, dw, h);
    te->setGeometry(dw, 0, tw, h);
}

/*! \reimp
*/

QSize Q3DateTimeEdit::minimumSizeHint() const
{
    QSize dsh = de->minimumSizeHint();
    QSize tsh = te->minimumSizeHint();
    return QSize(dsh.width() + tsh.width(),
                  qMax(dsh.height(), tsh.height()));
}

/*!  \internal
 */

void Q3DateTimeEdit::init()
{
    d = new Q3DateTimeEditPrivate();
    de = new Q3DateEdit(this, "qt_datetime_dateedit");
    te = new Q3TimeEdit(this, "qt_datetime_timeedit");
    d->adv = false;
    connect(de, SIGNAL(valueChanged(QDate)), this, SLOT(newValue(QDate)));
    connect(te, SIGNAL(valueChanged(QTime)), this, SLOT(newValue(QTime)));
    setFocusProxy(de);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

/*! \reimp
 */

QSize Q3DateTimeEdit::sizeHint() const
{
    ensurePolished();
    QSize dsh = de->sizeHint();
    QSize tsh = te->sizeHint();
    return QSize(dsh.width() + tsh.width(),
                  qMax(dsh.height(), tsh.height()));
}

/*!
    \property Q3DateTimeEdit::dateTime
    \brief the editor's datetime value

    The datetime edit's datetime which may be an invalid datetime.
*/

void Q3DateTimeEdit::setDateTime(const QDateTime & dt)
{
    if (dt.isValid()) {
        de->setDate(dt.date());
        te->setTime(dt.time());
        emit valueChanged(dt);
    }
}

QDateTime Q3DateTimeEdit::dateTime() const
{
    return QDateTime(de->date(), te->time());
}

/*!
    \fn void Q3DateTimeEdit::valueChanged(const QDateTime& datetime)

    This signal is emitted every time the date or time changes. The \a
    datetime argument is the new datetime.
*/


/*! \internal

  Re-emits the value \a d.
 */

void Q3DateTimeEdit::newValue(const QDate&)
{
    QDateTime dt = dateTime();
    emit valueChanged(dt);
}

/*! \internal
  \overload
  Re-emits the value \a t.
 */

void Q3DateTimeEdit::newValue(const QTime&)
{
    QDateTime dt = dateTime();
    emit valueChanged(dt);
}


/*!
    Sets the auto advance property of the editor to \a advance. If set
    to true, the editor will automatically advance focus to the next
    date or time section if the user has completed a section.
*/

void Q3DateTimeEdit::setAutoAdvance(bool advance)
{
    de->setAutoAdvance(advance);
    te->setAutoAdvance(advance);
}

/*!
    Returns true if auto-advance is enabled, otherwise returns false.

    \sa setAutoAdvance()
*/

bool Q3DateTimeEdit::autoAdvance() const
{
    return de->autoAdvance();
}

/*!
    \fn Q3DateEdit* Q3DateTimeEdit::dateEdit()

    Returns the internal widget used for editing the date part of the
    datetime.
*/

/*!
    \fn Q3TimeEdit* Q3DateTimeEdit::timeEdit()

    Returns the internal widget used for editing the time part of the
    datetime.
*/

QT_END_NAMESPACE

#include "q3datetimeedit.moc"

#endif
