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

#ifndef QLABEL_P_H
#define QLABEL_P_H

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

#include "qlabel.h"

#include "../text/qtextdocumentlayout_p.h"
#include "private/qtextcontrol_p.h"
#include "qtextdocumentfragment.h"
#include "qframe_p.h"
#include "qtextdocument.h"
#include "qmovie.h"
#include "qimage.h"
#include "qbitmap.h"
#include "qpicture.h"
#include "qmenu.h"

QT_BEGIN_NAMESPACE

class QLabelPrivate : public QFramePrivate
{
    Q_DECLARE_PUBLIC(QLabel)
public:
    QLabelPrivate() {}

    void init();
    void clearContents();
    void updateLabel();
    QSize sizeForWidth(int w) const;

    mutable QSize sh;
    mutable QSize msh;
    mutable bool valid_hints;
    mutable QSizePolicy sizePolicy;
    int margin;
    QString text;
    QPixmap  *pixmap;
    QPixmap *scaledpixmap;
    QImage *cachedimage;
#ifndef QT_NO_PICTURE
    QPicture *picture;
#endif
#ifndef QT_NO_MOVIE
    QPointer<QMovie> movie;
    void _q_movieUpdated(const QRect&);
    void _q_movieResized(const QSize&);
#endif
#ifndef QT_NO_SHORTCUT
    void updateShortcut();
#endif
#ifndef QT_NO_SHORTCUT
    QPointer<QWidget> buddy;
    int shortcutId;
#endif
    ushort align;
    short indent;
    uint scaledcontents :1;
    mutable uint textLayoutDirty : 1;
    mutable uint textDirty : 1;
    mutable uint isRichText : 1;
    mutable uint isTextLabel : 1;
    mutable uint hasShortcut : 1;
    Qt::TextFormat textformat;
    mutable QTextControl *control;
    QTextCursor shortcutCursor;
    Qt::TextInteractionFlags textInteractionFlags;

    inline bool needTextControl() const {
        return isTextLabel
               && (isRichText
                   || (!isRichText && (textInteractionFlags & (Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard))));
    }

    void ensureTextPopulated() const;
    void ensureTextLayouted() const;
    void ensureTextControl() const;
    void sendControlEvent(QEvent *e);

    void _q_linkHovered(const QString &link);

    QRectF layoutRect() const;
    QRect documentRect() const;
    QPoint layoutPoint(const QPoint& p) const;
#ifndef QT_NO_CONTEXTMENU
    QMenu *createStandardContextMenu(const QPoint &pos);
#endif

    bool openExternalLinks;

#ifndef QT_NO_CURSOR
    uint validCursor : 1;
    uint onAnchor : 1;
    QCursor cursor;
#endif

    friend class QMessageBoxPrivate;
};

QT_END_NAMESPACE

#endif // QLABEL_P_H
