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

#ifndef QDESIGNER_WIDGET_H
#define QDESIGNER_WIDGET_H

#include "shared_global_p.h"
#include <QtGui/QDialog>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

namespace qdesigner_internal {
    class FormWindowBase;
}

class QDESIGNER_SHARED_EXPORT QDesignerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDesignerWidget(QDesignerFormWindowInterface* formWindow, QWidget *parent = 0);
    virtual ~QDesignerWidget();

    QDesignerFormWindowInterface* formWindow() const;

    void updatePixmap();

    virtual QSize minimumSizeHint() const
    { return QWidget::minimumSizeHint().expandedTo(QSize(16, 16)); }

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    qdesigner_internal::FormWindowBase* m_formWindow;
};

class QDESIGNER_SHARED_EXPORT QDesignerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QDesignerDialog(QDesignerFormWindowInterface *fw, QWidget *parent);

    virtual QSize minimumSizeHint() const
    { return QWidget::minimumSizeHint().expandedTo(QSize(16, 16)); }

protected:
    void paintEvent(QPaintEvent *e);

private:
    qdesigner_internal::FormWindowBase* m_formWindow;
};

class QDESIGNER_SHARED_EXPORT Line : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
public:
    explicit Line(QWidget *parent) : QFrame(parent)
    { setAttribute(Qt::WA_MouseNoMask); setFrameStyle(HLine | Sunken); }

    inline void setOrientation(Qt::Orientation orient)
    { setFrameShape(orient == Qt::Horizontal ? HLine : VLine); }

    inline Qt::Orientation orientation() const
    { return frameShape() == HLine ? Qt::Horizontal : Qt::Vertical; }
};

QT_END_NAMESPACE

#endif // QDESIGNER_WIDGET_H
