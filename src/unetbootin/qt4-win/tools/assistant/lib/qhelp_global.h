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

#ifndef QHELP_GLOBAL_H
#define QHELP_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QRegExp>
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

namespace {

    QString uniquifyConnectionName(const QString &name, void *pointer)
    {
        return QString::fromLatin1("%1-%2").arg(name).arg(long(pointer));
    };

    QString documentTitle(const QString &content)
    {
        QString title = QObject::tr("Untitled");
        if (!content.isEmpty()) {
            int start = content.indexOf(QLatin1String("<title>"), 0, Qt::CaseInsensitive) + 7;
            int end = content.indexOf(QLatin1String("</title>"), 0, Qt::CaseInsensitive);
            if ((end - start) > 0) {
                title = content.mid(start, end - start);
                if (Qt::mightBeRichText(title)) {
                    QTextDocument doc;
                    doc.setHtml(title);
                    title = doc.toPlainText();
                }
            }
        }
        return title;
    };

    QString charsetFromData(const QByteArray &data)
    {
        QString encoding;
        int start = data.indexOf("<meta", 0);
        if (start > 0) {
            int end = data.indexOf('>', start);
            QString meta = QString::fromLatin1(data.mid(start +5, end - start));
            meta = meta.toLower();
            QRegExp r(QLatin1String("charset=([^\"\\s]+)"));
            if (r.indexIn(meta) != -1) {
                encoding = r.cap(1);
            }
        }

        if (encoding.isEmpty())
            return QLatin1String("utf-8");

        return encoding;
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QHELP_GLOBAL_H
