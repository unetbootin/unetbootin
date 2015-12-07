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

#include "recentfiles.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

const QString &settingsPrefix();

static QString configKey()
{
    return settingsPrefix() + QLatin1String("RecentlyOpenedFiles");
}


RecentFiles::RecentFiles(const int maxEntries)
  : m_groupOpen(false),
    m_clone1st(false),
    m_maxEntries(maxEntries)
{
    m_timer.setSingleShot(true);
    m_timer.setInterval(3 * 60 * 1000);
    connect(&m_timer, SIGNAL(timeout()), SLOT(closeGroup()));
}

/*
 * The logic is as follows:
 * - The most recent (i.e., topmost) item can be open ("in flux")
 * - The item is closed by either a timeout (3 min or so) or a
 *   "terminal action" (e.g., closing all files)
 * - While the item is open, modifications to the set of open files
 *   will modify that item instead of creating new items
 * - If the open item is modified to be equal to an existing item,
 *   the existing item is deleted, but will be re-created when the
 *   open item is modified even further
 * Cases (actions in parentheses are no-ops):
 * - identical to top item => (do nothing)
 * - closed, new item => insert at top, (clear marker)
 * - closed, existing item => move to top, mark for cloning
 * - open, new item, not marked => replace top, (clear marker)
 * - open, new item, marked => insert at top, clear marker
 * - open, existing item, not marked => replace top, delete copy, mark for cloning
 * - open, existing item, marked => insert at top, delete copy, (mark for cloning)
 * - closing clears marker
 */
void RecentFiles::addFiles(const QStringList &names)
{
    if (m_strLists.isEmpty() || names != m_strLists.first()) {
        if (m_groupOpen && !m_clone1st)
            // Group being open implies at least one item in the list
            m_strLists.removeFirst();
        m_groupOpen = true;

        // We do *not* sort the actual entries, as that would destroy the user's
        // chosen arrangement. However, we do the searching on sorted lists, so
        // we throw out (probably) obsolete arrangements.
        QList<QStringList> sortedLists = m_strLists;
        for (int i = 0; i < sortedLists.size(); ++i)
            sortedLists[i].sort();
        QStringList sortedNames = names;
        sortedNames.sort();

        int index = sortedLists.indexOf(sortedNames);
        if (index >= 0) {
            m_strLists.removeAt(index);
            m_clone1st = true;
        } else {
            if (m_strLists.count() >= m_maxEntries)
                m_strLists.removeLast();
            m_clone1st = false;
        }
        m_strLists.prepend(names);
    }
    m_timer.start();
}

void RecentFiles::closeGroup()
{
    m_timer.stop();
    m_groupOpen = false;
}

void RecentFiles::readConfig()
{
    m_strLists.clear();
    QVariant val = QSettings().value(configKey());
    if (val.type() == QVariant::StringList) // Backwards compat to Qt < 4.5
        foreach (const QString &s, val.toStringList())
            m_strLists << QStringList(QFileInfo(s).canonicalFilePath());
    else
        foreach (const QVariant &v, val.toList())
            m_strLists << v.toStringList();
}

void RecentFiles::writeConfig() const
{
    QList<QVariant> vals;
    foreach (const QStringList &sl, m_strLists)
        vals << sl;
    QSettings().setValue(configKey(), vals);
}

QT_END_NAMESPACE
