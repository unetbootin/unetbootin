/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
    bool m_copy;
    bool m_quiet;
};

QT_END_NAMESPACE

#endif
