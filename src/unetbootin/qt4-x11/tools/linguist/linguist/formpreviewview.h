/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#ifndef FORMPREVIEWVIEW_H
#define FORMPREVIEWVIEW_H

#include <quiloader_p.h>

#include <QtCore/QHash>
#include <QtCore/QList>

#include <QtGui/QMainWindow>

QT_BEGIN_NAMESPACE

class MultiDataModel;
class FormFrame;
class MessageItem;

class QComboBox;
class QListWidgetItem;
class QGridLayout;
class QMdiArea;
class QMdiSubWindow;
class QToolBox;
class QTableWidgetItem;
class QTreeWidgetItem;

enum TranslatableEntryType {
    TranslatableProperty,
    TranslatableToolItemText,
    TranslatableToolItemToolTip,
    TranslatableTabPageText,
    TranslatableTabPageToolTip,
    TranslatableTabPageWhatsThis,
    TranslatableListWidgetItem,
    TranslatableTableWidgetItem,
    TranslatableTreeWidgetItem,
    TranslatableComboBoxItem
};

struct TranslatableEntry {
    TranslatableEntryType type;
    union {
        QObject *object;
        QComboBox *comboBox;
        QTabWidget *tabWidget;
        QToolBox *toolBox;
        QListWidgetItem *listWidgetItem;
        QTableWidgetItem *tableWidgetItem;
        QTreeWidgetItem *treeWidgetItem;
    } target;
    union {
        char *name;
        int index;
        struct {
            short index; // Known to be below 1000
            short column;
        } treeIndex;
    } prop;
};

typedef QHash<QUiTranslatableStringValue, QList<TranslatableEntry> > TargetsHash;

class FormPreviewView : public QMainWindow
{
    Q_OBJECT
public:
    FormPreviewView(QWidget *parent, MultiDataModel *dataModel);

    void setSourceContext(int model, MessageItem *messageItem);

private:
    bool m_isActive;
    QString m_currentFileName;
    QMdiArea *m_mdiArea;
    QMdiSubWindow *m_mdiSubWindow;
    QWidget *m_form;
    TargetsHash m_targets;
    QList<TranslatableEntry> m_highlights;
    MultiDataModel *m_dataModel;

    QString m_lastFormName;
    QString m_lastClassName;
    int m_lastModel;
};

QT_END_NAMESPACE

#endif // FORMPREVIEWVIEW_H
