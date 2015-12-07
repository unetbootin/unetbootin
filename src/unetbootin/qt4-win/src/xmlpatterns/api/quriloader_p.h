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

#ifndef QURILOADER_P_H
#define QURILOADER_P_H

//#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtXmlPatterns/QXmlName>

#include "qnamepool_p.h"

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class URILoader : public QNetworkAccessManager
    {
    public:
        URILoader(QObject *const parent,
                  const QHash<QXmlName, QIODevice *> &deviceBindings,
                  const QPatternist::NamePool::Ptr &np);
        virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest & req, QIODevice *outgoingData = 0);

    private:
        const QString                       m_variableNS;
        const NamePool::Ptr                 m_namePool;
        const QHash<QXmlName, QIODevice *>  m_deviceBindings;

    };

#if 0
    class NetworkReplyTimeout : public QTimer
    {
    public:
        NetworkReplyTimeout(QNetworkReply *const parent) : QTimer(parent)
        {
            Q_ASSERT(parent);
            connect(this, SIGNAL(timeout()), this, SLOT(networkTimeout()));

            setInterval(Timeout);
            setSingleShot(true);
        }

    public slots:
        void networkTimeout()
        {
            static_cast<QNetworkReply *>(parent())->error(QNetworkReply::TimeoutError);
        }

    private:
    };
#endif
};

QT_END_NAMESPACE

#endif
