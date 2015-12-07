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
** Definition of QXIMInputContext class
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

#ifndef QXIMINPUTCONTEXT_P_H
#define QXIMINPUTCONTEXT_P_H

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

#if !defined(Q_NO_IM)

#include "QtCore/qglobal.h"
#include "QtGui/qinputcontext.h"
#include "QtGui/qfont.h"
#include "QtCore/qhash.h"
#ifdef Q_WS_X11
#include "QtCore/qlist.h"
#include "QtCore/qbitarray.h"
#include "QtGui/qwindowdefs.h"
#include "private/qt_x11_p.h"
#endif

QT_BEGIN_NAMESPACE

class QKeyEvent;
class QWidget;
class QFont;
class QString;

class QXIMInputContext : public QInputContext
{
    Q_OBJECT
public:
    struct ICData {
        XIC ic;
        XFontSet fontset;
        QWidget *widget;
        QString text;
        QBitArray selectedChars;
        bool composing;
        void clear();
    };

    QXIMInputContext();
    ~QXIMInputContext();

    QString identifierName();
    QString language();

    void reset();

    void mouseHandler( int x, QMouseEvent *event);
    bool isComposing() const;

    void setFocusWidget( QWidget *w );
    void widgetDestroyed(QWidget *w);

    void create_xim();
    void close_xim();

    void update();

    ICData *icData() const;
protected:
    bool x11FilterEvent( QWidget *keywidget, XEvent *event );

private:
    static XIMStyle xim_style;

    QString _language;
    XIM xim;
    QHash<QWidget *, ICData *> ximData;

    ICData *createICData(QWidget *w);
};

QT_END_NAMESPACE

#endif // Q_NO_IM

#endif // QXIMINPUTCONTEXT_P_H
