/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef DESIGNERWIDGETITEM_H
#define DESIGNERWIDGETITEM_H

#include "shared_global_p.h"

#include <QtGui/QLayoutItem>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// QDesignerWidgetItem: A Layout Item that prevents its widget item from being slammed to size 0
// in the directions specified if the widget returns an empty sizeHint()/ minimumSizeHint().
// Used for form editor Layout Items containing a QFrame or a QWidget for which this
// will happen if they are non-laid out

class QDESIGNER_SHARED_EXPORT QDesignerWidgetItem : public QWidgetItemV2 {
    Q_DISABLE_COPY(QDesignerWidgetItem)

public:
    QDesignerWidgetItem(QWidget *w, Qt::Orientations o = Qt::Horizontal|Qt::Vertical);

    virtual QSize minimumSize() const { return  expand(QWidgetItemV2::minimumSize()); }
    virtual QSize sizeHint()    const { return  expand(QWidgetItemV2::sizeHint());    }

    // Register itself using QLayoutPrivate's widget item factory method hook
    static void install();
    static void deinstall();

private:
    QSize expand(const QSize &s) const;
    const Qt::Orientations m_orientations;
};

// Helper class that ensures QDesignerWidgetItem is installed while an instance is in scope.

class QDESIGNER_SHARED_EXPORT QDesignerWidgetItemInstaller {
    Q_DISABLE_COPY(QDesignerWidgetItemInstaller)

public:
    QDesignerWidgetItemInstaller();
    ~QDesignerWidgetItemInstaller();

private:
    static int m_instanceCount;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif
