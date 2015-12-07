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

#ifndef QHELP_GLOBAL_H
#define QHELP_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QRegExp>
#include <QtCore/QMutexLocker>
#include <QtGui/QTextDocument>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Help)

#if !defined(QT_SHARED) && !defined(QT_DLL)
#   define QHELP_EXPORT
#elif defined(QHELP_LIB)
#   define QHELP_EXPORT Q_DECL_EXPORT
#else
#   define QHELP_EXPORT Q_DECL_IMPORT
#endif

class QHelpGlobal {
public:
    static QString uniquifyConnectionName(const QString &name, void *pointer)
    {
        static int counter = 0;
        static QMutex mutex;

        QMutexLocker locker(&mutex);
        if (++counter > 1000)
            counter = 0;
        
        return QString::fromLatin1("%1-%2-%3")
            .arg(name).arg(long(pointer)).arg(counter);
    };

    static QString documentTitle(const QString &content)
    {
        QString title = QObject::tr("Untitled");
        if (!content.isEmpty()) {
            int start = content.indexOf(QLatin1String("<title>"), 0, Qt::CaseInsensitive) + 7;
            int end = content.indexOf(QLatin1String("</title>"), 0, Qt::CaseInsensitive);
            if ((end - start) > 0) {
                title = content.mid(start, end - start);
                if (Qt::mightBeRichText(title) || title.contains(QLatin1Char('&'))) {
                    QTextDocument doc;
                    doc.setHtml(title);
                    title = doc.toPlainText();
                }
            }
        }
        return title;
    };

    static QString charsetFromData(const QByteArray &data)
    {
        QString content = QString::fromUtf8(data);
        int start =
            content.indexOf(QLatin1String("<meta"), 0, Qt::CaseInsensitive);
        if (start > 0) {
            int end;
            QRegExp r(QLatin1String("charset=([^\"\\s]+)"));
            while (start != -1) {
                end = content.indexOf(QLatin1Char('>'), start) + 1;
                const QString &meta = content.mid(start, end - start).toLower();
                if (r.indexIn(meta) != -1)
                    return r.cap(1);
                start = content.indexOf(QLatin1String("<meta"), end,
                    Qt::CaseInsensitive);
            }
        }
        return QLatin1String("utf-8");
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QHELP_GLOBAL_H
