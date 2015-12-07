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
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef ORDERDIALOG_P_H
#define ORDERDIALOG_P_H

#include "shared_global_p.h"

#include <QtGui/QDialog>
#include <QtCore/QMap>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;

namespace qdesigner_internal {

namespace Ui {
    class OrderDialog;
}

class QDESIGNER_SHARED_EXPORT OrderDialog: public QDialog
{
    Q_OBJECT
public:
    OrderDialog(QWidget *parent);
    virtual ~OrderDialog();

    static QWidgetList pagesOfContainer(const QDesignerFormEditorInterface *core, QWidget *container);

    void setPageList(const QWidgetList &pages);
    QWidgetList pageList() const;

    void setDescription(const QString &d);

    enum Format {        // Display format
        PageOrderFormat, // Container pages, ranging 0..[n-1]
        TabOrderFormat   // List of widgets,  ranging 1..1
    };

    void setFormat(Format f)  { m_format = f; }
    Format format() const     { return m_format; }

private slots:
    void on_upButton_clicked();
    void on_downButton_clicked();
    void on_pageList_currentRowChanged(int row);
    void slotEnableButtonsAfterDnD();
    void slotReset();

private:
    void buildList();
    void enableButtons(int r);

    typedef QMap<int, QWidget*> OrderMap;
    OrderMap m_orderMap;
    Ui::OrderDialog* m_ui;
    Format m_format;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // ORDERDIALOG_P_H
