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

#include "q3textbrowser.h"
#ifndef QT_NO_TEXTBROWSER
#include <private/q3richtext_p.h>

#include "qevent.h"
#include "qdesktopwidget.h"
#include "qapplication.h"
#include "qlayout.h"
#include "qpainter.h"
#include "qstack.h"
#include "stdio.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qbitmap.h"
#include "qtimer.h"
#include "qimage.h"
#include "q3simplerichtext.h"
#include "q3dragobject.h"
#include "qurl.h"
#include "qcursor.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3TextBrowser q3textbrowser.h
    \brief The Q3TextBrowser class provides a rich text browser with hypertext navigation.

    \compat

    This class extends Q3TextEdit (in read-only mode), adding some
    navigation functionality so that users can follow links in
    hypertext documents. The contents of Q3TextEdit is set with
    setText(), but Q3TextBrowser has an additional function,
    setSource(), which makes it possible to set the text to a named
    document. The name is looked up in the text view's mime source
    factory. If a document name ends with an anchor (for example, "\c
    #anchor"), the text browser automatically scrolls to that position
    (using scrollToAnchor()). When the user clicks on a hyperlink, the
    browser will call setSource() itself, with the link's \c href
    value as argument. You can track the current source by connetion
    to the sourceChanged() signal.

    Q3TextBrowser provides backward() and forward() slots which you can
    use to implement Back and Forward buttons. The home() slot sets
    the text to the very first document displayed. The linkClicked()
    signal is emitted when the user clicks a link.

    By using Q3TextEdit::setMimeSourceFactory() you can provide your
    own subclass of Q3MimeSourceFactory. This makes it possible to
    access data from anywhere, for example from a network or from a
    database. See Q3MimeSourceFactory::data() for details.

    If you intend using the mime factory to read the data directly
    from the file system, you may have to specify the encoding for the
    file extension you are using. For example:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textbrowser.cpp 0
    This is to ensure that the factory is able to resolve the document
    names.

    Q3TextBrowser interprets the tags it processes in accordance with
    the default style sheet. Change the style sheet with
    \l{setStyleSheet()}; see QStyleSheet for details.

    If you want to provide your users with editable rich text use
    Q3TextEdit. If you want a text browser without hypertext navigation
    use Q3TextEdit, and use Q3TextEdit::setReadOnly() to disable
    editing. If you just need to display a small piece of rich text
    use QSimpleRichText or QLabel.
*/

class Q3TextBrowserData
{
public:
    Q3TextBrowserData():textOrSourceChanged(false) {}

    QStack<QString> stack;
    QStack<QString> forwardStack;
    QString home;
    QString curmain;
    QString curmark;

    /*flag necessary to give the linkClicked() signal some meaningful
      semantics when somebody connected to it calls setText() or
      setSource() */
    bool textOrSourceChanged;
};


/*!
    Constructs an empty Q3TextBrowser called \a name, with parent \a
    parent.
*/
Q3TextBrowser::Q3TextBrowser(QWidget *parent, const char *name)
    : Q3TextEdit(parent, name)
{
    setReadOnly(true);
    d = new Q3TextBrowserData;

    viewport()->setMouseTracking(true);
}

/*!
    \internal
*/
Q3TextBrowser::~Q3TextBrowser()
{
    delete d;
}


/*!
    \property Q3TextBrowser::source
    \brief the name of the displayed document.

    This is a an empty string if no document is displayed or if the
    source is unknown.

    Setting this property uses the mimeSourceFactory() to lookup the
    named document. It also checks for optional anchors and scrolls
    the document accordingly.

    If the first tag in the document is \c{<qt type=detail>}, the
    document is displayed as a popup rather than as new document in
    the browser window itself. Otherwise, the document is displayed
    normally in the text browser with the text set to the contents of
    the named document with setText().

    If you are using the filesystem access capabilities of the mime
    source factory, you must ensure that the factory knows about the
    encoding of specified files; otherwise no data will be available.
    The default factory handles a couple of common file extensions
    such as \c *.html and \c *.txt with reasonable defaults. See
    Q3MimeSourceFactory::data() for details.
*/

QString Q3TextBrowser::source() const
{
    if (d->stack.isEmpty())
        return QString();
    else
        return d->stack.top();
}

/*!
    Reloads the current set source.
*/

void Q3TextBrowser::reload()
{
    QString s = d->curmain;
    d->curmain = QLatin1String("");
    setSource(s);
}


void Q3TextBrowser::setSource(const QString& name)
{
#ifndef QT_NO_CURSOR
    if (isVisible())
        qApp->setOverrideCursor(Qt::WaitCursor);
#endif
    d->textOrSourceChanged = true;
    QString source = name;
    QString mark;
    int hash = name.indexOf(QLatin1Char('#'));
    if (hash != -1) {
        source = name.left(hash);
        mark = name.mid(hash+1);
    }

    if (source.left(5) == QLatin1String("file:"))
        source = source.mid(6);

    QString url = mimeSourceFactory()->makeAbsolute(source, context());
    QString txt;
    bool dosettext = false;

    if (!source.isEmpty() && url != d->curmain) {
        const QMimeSource* m =
                    mimeSourceFactory()->data(source, context());
        if (!m){
            qWarning("Q3TextBrowser: no mimesource for %s", source.latin1());
        }
        else {
            if (!Q3TextDrag::decode(m, txt)) {
                qWarning("Q3TextBrowser: cannot decode %s", source.latin1());
            }
        }
        if (isVisible()) {
            QString firstTag = txt.left(txt.indexOf(QLatin1Char('>')) + 1);
            if (firstTag.left(3) == QLatin1String("<qt") && firstTag.contains(QLatin1String("type")) && firstTag.contains(QLatin1String("detail"))) {
                popupDetail(txt, QCursor::pos());
#ifndef QT_NO_CURSOR
                qApp->restoreOverrideCursor();
#endif
                return;
            }
        }

        d->curmain = url;
        dosettext = true;
    }

    d->curmark = mark;

    if (!mark.isEmpty()) {
        url += QLatin1Char('#');
        url += mark;
    }
    if (d->home.count() == 0)
        d->home = url;

    if (d->stack.isEmpty() || d->stack.top() != url)
        d->stack.push(url);

    int stackCount = (int)d->stack.count();
    if (d->stack.top() == url)
        stackCount--;
    emit backwardAvailable(stackCount > 0);
    stackCount = (int)d->forwardStack.count();
    if (d->forwardStack.isEmpty() || d->forwardStack.top() == url)
        stackCount--;
    emit forwardAvailable(stackCount > 0);

    if (dosettext)
        Q3TextEdit::setText(txt, url);

    if (!mark.isEmpty())
        scrollToAnchor(mark);
    else
        setContentsPos(0, 0);

#ifndef QT_NO_CURSOR
    if (isVisible())
        qApp->restoreOverrideCursor();
#endif

    emit sourceChanged(url);
}

/*!
    \fn void Q3TextBrowser::backwardAvailable(bool available)

    This signal is emitted when the availability of backward()
    changes. \a available is false when the user is at home();
    otherwise it is true.
*/

/*!
    \fn void Q3TextBrowser::forwardAvailable(bool available)

    This signal is emitted when the availability of forward() changes.
    \a available is true after the user navigates backward() and false
    when the user navigates or goes forward().
*/

/*!
    \fn void Q3TextBrowser::sourceChanged(const QString& src)

    This signal is emitted when the mime source has changed, \a src
    being the new source.

    Source changes happen both programmatically when calling
    setSource(), forward(), backword() or home() or when the user
    clicks on links or presses the equivalent key sequences.
*/

/*!  \fn void Q3TextBrowser::highlighted (const QString &link)

    This signal is emitted when the user has selected but not
    activated a link in the document. \a link is the value of the \c
    href i.e. the name of the target document.
*/

/*!
    \fn void Q3TextBrowser::linkClicked(const QString& link)

    This signal is emitted when the user clicks a link. The \a link is
    the value of the \c href i.e. the name of the target document.

    The \a link will be the absolute location of the document, based
    on the value of the anchor's href tag and the current context of
    the document.

    \sa anchorClicked()
*/

/*!
    \fn void Q3TextBrowser::anchorClicked(const QString& name, const QString &link)

    This signal is emitted when the user clicks an anchor. The \a link is
    the value of the \c href i.e. the name of the target document.  The \a name
    is the name of the anchor.

    \sa linkClicked()
*/

/*!
    Changes the document displayed to the previous document in the
    list of documents built by navigating links. Does nothing if there
    is no previous document.

    \sa forward(), backwardAvailable()
*/
void Q3TextBrowser::backward()
{
    if (d->stack.count() <= 1)
        return;
    d->forwardStack.push(d->stack.pop());
    setSource(d->stack.pop());
    emit forwardAvailable(true);
}

/*!
    Changes the document displayed to the next document in the list of
    documents built by navigating links. Does nothing if there is no
    next document.

    \sa backward(), forwardAvailable()
*/
void Q3TextBrowser::forward()
{
    if (d->forwardStack.isEmpty())
        return;
    setSource(d->forwardStack.pop());
    emit forwardAvailable(!d->forwardStack.isEmpty());
}

/*!
    Changes the document displayed to be the first document the
    browser displayed.
*/
void Q3TextBrowser::home()
{
    if (!d->home.isNull())
        setSource(d->home);
}

/*!
    The event \a e is used to provide the following keyboard shortcuts:
    \table
    \header \i Keypress            \i Action
    \row \i Alt+Left Arrow  \i \l backward()
    \row \i Alt+Right Arrow \i \l forward()
    \row \i Alt+Up Arrow    \i \l home()
    \endtable
*/
void Q3TextBrowser::keyPressEvent(QKeyEvent * e)
{
    if (e->state() & Qt::AltButton) {
        switch (e->key()) {
        case Qt::Key_Right:
            forward();
            return;
        case Qt::Key_Left:
            backward();
            return;
        case Qt::Key_Up:
            home();
            return;
        }
    }
    Q3TextEdit::keyPressEvent(e);
}

class QTextDetailPopup : public QWidget
{
public:
    QTextDetailPopup()
        : QWidget (0, "automatic QText detail widget", Qt::WType_Popup)
    {
	setAttribute(Qt::WA_DeleteOnClose, true);
    }

protected:
    void mousePressEvent(QMouseEvent *)
    {
        close();
    }
};


void Q3TextBrowser::popupDetail(const QString& contents, const QPoint& pos)
{

    const int shadowWidth = 6;   // also used as '5' and '6' and even '8' below
    const int vMargin = 8;
    const int hMargin = 12;

    QWidget* popup = new QTextDetailPopup;
    popup->setAttribute(Qt::WA_NoSystemBackground, true);

    Q3SimpleRichText* doc = new Q3SimpleRichText(contents, popup->font());
    doc->adjustSize();
    QRect r(0, 0, doc->width(), doc->height());

    int w = r.width() + 2*hMargin;
    int h = r.height() + 2*vMargin;

    popup->resize(w + shadowWidth, h + shadowWidth);

    // okay, now to find a suitable location
    //###### we need a global fancy popup positioning somewhere
    popup->move(pos - popup->rect().center());
    if (popup->geometry().right() > QApplication::desktop()->width())
        popup->move(QApplication::desktop()->width() - popup->width(),
                     popup->y());
    if (popup->geometry().bottom() > QApplication::desktop()->height())
        popup->move(popup->x(),
                     QApplication::desktop()->height() - popup->height());
    if (popup->x() < 0)
        popup->move(0, popup->y());
    if (popup->y() < 0)
        popup->move(popup->x(), 0);


    popup->show();

    // now for super-clever shadow stuff.  super-clever mostly in
    // how many window system problems it skirts around.

    QPainter p(popup);
    p.setPen(QApplication::palette().color(QPalette::Active, QPalette::WindowText));
    p.drawRect(0, 0, w, h);
    p.setPen(QApplication::palette().color(QPalette::Active, QPalette::Mid));
    p.setBrush(QColor(255, 255, 240));
    p.drawRect(1, 1, w-2, h-2);
    p.setPen(Qt::black);

    doc->draw(&p, hMargin, vMargin, r, popup->palette(), 0);
    delete doc;

    p.drawPoint(w + 5, 6);
    p.drawLine(w + 3, 6,
                w + 5, 8);
    p.drawLine(w + 1, 6,
                w + 5, 10);
    int i;
    for(i=7; i < h; i += 2)
        p.drawLine(w, i,
                    w + 5, i + 5);
    for(i = w - i + h; i > 6; i -= 2)
        p.drawLine(i, h,
                    i + 5, h + 5);
    for(; i > 0 ; i -= 2)
        p.drawLine(6, h + 6 - i,
                    i + 5, h + 5);
}

/*!
    \fn void Q3TextBrowser::setText(const QString &txt)

    \overload

    Sets the text to \a txt.
*/

/*!
    \reimp
*/

void Q3TextBrowser::setText(const QString &txt, const QString &context)
{
    d->textOrSourceChanged = true;
    d->curmark = QLatin1String("");
    d->curmain = QLatin1String("");
    Q3TextEdit::setText(txt, context);
}

void Q3TextBrowser::emitHighlighted(const QString &s)
{
    emit highlighted(s);
}

void Q3TextBrowser::emitLinkClicked(const QString &s)
{
    d->textOrSourceChanged = false;
    emit linkClicked(s);
    if (!d->textOrSourceChanged)
        setSource(s);
}

QT_END_NAMESPACE

#endif  // QT_NO_TEXTBROWSER
