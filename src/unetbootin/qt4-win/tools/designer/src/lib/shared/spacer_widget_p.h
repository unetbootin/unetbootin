/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//


#ifndef SPACER_WIDGET_H
#define SPACER_WIDGET_H

#include "shared_global_p.h"

#include <QtGui/QWidget>
#include <QtGui/QSizePolicy>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

class QDESIGNER_SHARED_EXPORT Spacer: public QWidget
{
    Q_OBJECT

    Q_ENUMS(SizeType)
    // Special hack: Make name appear as "spacer name"
    Q_PROPERTY(QString spacerName  READ objectName WRITE setObjectName)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(QSizePolicy::Policy sizeType READ sizeType WRITE setSizeType)
    Q_PROPERTY(QSize sizeHint READ sizeHintProperty WRITE setSizeHintProperty DESIGNABLE true STORED true)

public:
    Spacer(QWidget *parent = 0);

    QSize sizeHint() const;

    QSize sizeHintProperty() const;
    void setSizeHintProperty(const QSize &s);

    QSizePolicy::Policy sizeType() const;
    void setSizeType(QSizePolicy::Policy t);

    Qt::Alignment alignment() const;
    Qt::Orientation orientation() const;

    void setOrientation(Qt::Orientation o);
    void setInteractiveMode(bool b) { m_interactive = b; };

    virtual bool event(QEvent *e);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent* e);
    void updateMask();

private:
    bool isInLayout() const;
    void updateToolTip();

    const QSize m_SizeOffset;
    QDesignerFormWindowInterface *m_formWindow;
    Qt::Orientation m_orientation;
    bool m_interactive;
    // Cache information about 'being in layout' which is expensive to calculate.
    enum LayoutState { InLayout, OutsideLayout, UnknownLayoutState };
    mutable LayoutState m_layoutState;
    QSize m_sizeHint;
};

QT_END_NAMESPACE

#endif // SPACER_WIDGET_H
