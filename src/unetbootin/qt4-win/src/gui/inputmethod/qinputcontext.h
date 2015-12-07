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

/****************************************************************************
**
** Definition of QInputContext class
**
** Copyright (C) 2003-2004 immodule for Qt Project.  All rights reserved.
**
** This file is written to contribute to Trolltech ASA under their own
** license. You may use this file under your Qt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef QINPUTCONTEXT_H
#define QINPUTCONTEXT_H

#include <QtCore/qobject.h>
#include <QtCore/qglobal.h>
#include <QtGui/qevent.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtGui/qaction.h>

#ifndef QT_NO_IM

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QWidget;
class QFont;
class QPopupMenu;
class QInputContextPrivate;


class Q_GUI_EXPORT QInputContext : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QInputContext)
public:
    explicit QInputContext(QObject* parent = 0);
    virtual ~QInputContext();

    virtual QString identifierName() = 0;
    virtual QString language() = 0;

    virtual void reset() = 0;
    virtual void update();

    virtual void mouseHandler( int x, QMouseEvent *event);
    virtual QFont font() const;
    virtual bool isComposing() const = 0;

    QWidget *focusWidget() const;
    virtual void setFocusWidget( QWidget *w );

    virtual void widgetDestroyed(QWidget *w);

    virtual QList<QAction *> actions();

#if defined(Q_WS_X11)
    virtual bool x11FilterEvent( QWidget *keywidget, XEvent *event );
#endif // Q_WS_X11
    virtual bool filterEvent( const QEvent *event );

    void sendEvent(const QInputMethodEvent &event);

    enum StandardFormat {
        PreeditFormat,
        SelectionFormat
    };
    QTextFormat standardFormat(StandardFormat s) const;
private:
    friend class QWidget;
    friend class QInputContextFactory;
    friend class QApplication;

private:   // Disabled copy constructor and operator=
    QInputContext( const QInputContext & );
    QInputContext &operator=( const QInputContext & );

};

QT_END_NAMESPACE

QT_END_HEADER

#endif //Q_NO_IM

#endif // QINPUTCONTEXT_H
