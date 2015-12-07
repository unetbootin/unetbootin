/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef GRADIENTVIEW_H
#define GRADIENTVIEW_H

#include <QtGui/QWidget>
#include <QtCore/QMap>
#include "ui_qtgradientview.h"

QT_BEGIN_NAMESPACE

class QtGradientManager;
class QListViewItem;
class QAction;

class QtGradientView : public QWidget
{
    Q_OBJECT
public:
    QtGradientView(QWidget *parent = 0);

    void setGradientManager(QtGradientManager *manager);
    QtGradientManager *gradientManager() const;

    void setCurrentGradient(const QString &id);
    QString currentGradient() const;

signals:
    void currentGradientChanged(const QString &id);
    void gradientActivated(const QString &id);

private slots:
    void slotGradientAdded(const QString &id, const QGradient &gradient);
    void slotGradientRenamed(const QString &id, const QString &newId);
    void slotGradientChanged(const QString &id, const QGradient &newGradient);
    void slotGradientRemoved(const QString &id);
    void slotNewGradient();
    void slotEditGradient();
    void slotRemoveGradient();
    void slotRenameGradient();
    void slotRenameGradient(QListWidgetItem *item);
    void slotCurrentItemChanged(QListWidgetItem *item);
    void slotGradientActivated(QListWidgetItem *item);

private:
    QMap<QString, QListWidgetItem *> m_idToItem;
    QMap<QListWidgetItem *, QString> m_itemToId;

    QAction *m_newAction;
    QAction *m_editAction;
    QAction *m_renameAction;
    QAction *m_removeAction;

    QtGradientManager *m_manager;
    Ui::QtGradientView m_ui;
};

QT_END_NAMESPACE

#endif
