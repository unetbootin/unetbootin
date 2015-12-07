/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#ifndef QIODEVICEDELEGATE_P_H
#define QIODEVICEDELEGATE_P_H

#include <QtCore/QTimer>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * This is read-only currently.
     */
    class QIODeviceDelegate : public QNetworkReply
    {
        Q_OBJECT
    public:
        QIODeviceDelegate(QIODevice *const source);

        virtual void abort();

        virtual bool atEnd() const;
        virtual qint64 bytesAvailable() const;
        virtual qint64 bytesToWrite() const;
        virtual bool canReadLine() const;
        virtual void close();
        virtual bool isSequential() const;
        virtual bool open(OpenMode mode);
        virtual qint64 pos() const;
        virtual bool reset();
        virtual bool seek(qint64 pos);
        virtual qint64 size() const;
        virtual bool waitForBytesWritten(int msecs);
        virtual bool waitForReadyRead(int msecs);

    protected:
        virtual qint64 readData(char *data, qint64 maxSize);

    private Q_SLOTS:
        void networkTimeout();
    private:
        enum
        {
            /**
             * 20 seconds expressed in milliseconds.
             */
            Timeout = 20000
        };

        QIODevice *const m_source;
        QTimer m_timeout;
    };
};

QT_END_NAMESPACE

#endif
