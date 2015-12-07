/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef CMDLINEPARSER_H
#define CMDLINEPARSER_H

#include <QtCore/QStringList>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

class CmdLineParser
{
public:
    enum Result {Ok, Help, Error};
    enum ShowState {Untouched, Show, Hide, Activate};
    enum RegisterState {None, Register, Unregister};

    CmdLineParser();
    Result parse(const QStringList &arguments);

    void setCollectionFile(const QString &file);
    QString collectionFile() const;
    QString cloneFile() const;
    QUrl url() const;
    bool enableRemoteControl() const;
    ShowState contents() const;
    ShowState index() const;
    ShowState bookmarks() const;
    ShowState search() const;
    QString currentFilter() const;
    RegisterState registerRequest() const;
    QString helpFile() const;

    bool copy() const { return m_copy; }

    void showMessage(const QString &msg, bool error);

private:
    QString getFileName(const QString &fileName);

    QString m_helpMessage;
    QString m_collectionFile;
    QString m_cloneFile;
    QString m_helpFile;
    QUrl m_url;
    bool m_enableRemoteControl;

    ShowState m_contents;
    ShowState m_index;
    ShowState m_bookmarks;
    ShowState m_search;
    RegisterState m_register;
    QString m_currentFilter;
    bool m_copy;
    bool m_quiet;
};

QT_END_NAMESPACE

#endif
