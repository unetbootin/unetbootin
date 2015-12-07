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

#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

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

#include "ui_connectdialog.h"
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QPushButton;

namespace qdesigner_internal {

class ConnectDialog : public QDialog
{
    Q_OBJECT
public:
    ConnectDialog(QDesignerFormWindowInterface *formWindow, QWidget *sender, QWidget *receiver, QWidget *parent = 0);

    QString signal() const;
    QString slot() const;

    void setSignalSlot(const QString &signal, const QString &slot);

    bool showAllSignalsSlots() const;
    void setShowAllSignalsSlots(bool showIt);

private slots:
    void populateLists();
    void selectSignal(QListWidgetItem *item);
    void selectSlot(QListWidgetItem *item);
    void populateSignalList();
    void populateSlotList(const QString &signal = QString());
    void editSignals();
    void editSlots();

private:
    enum WidgetMode { NormalWidget, MainContainer, PromotedWidget };

    static WidgetMode widgetMode(QWidget *w,  QDesignerFormWindowInterface *formWindow);
    QPushButton *okButton();
    void setOkButtonEnabled(bool);
    void editSignalsSlots(QWidget *w, WidgetMode mode, int signalSlotDialogMode);

    QWidget *m_source;
    QWidget *m_destination;
    const WidgetMode m_sourceMode;
    const WidgetMode m_destinationMode;
    QDesignerFormWindowInterface *m_formWindow;
    Ui::ConnectDialog m_ui;
};

}

QT_END_NAMESPACE

#endif // CONNECTDIALOG_H
