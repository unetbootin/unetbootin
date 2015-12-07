/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QNETWORKACCESSDELEGATOR_P_H
#define QNETWORKACCESSDELEGATOR_P_H

#include <QObject>
#include <QPointer>
#include <QSharedData>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QNetworkAccessManager;
class QUrl;

namespace QPatternist
{
    /**
     * @short A value based class that hands out QNetworkAccessManager
     * appropriately for variable bindings.
     *
     * NetworkAccessDelegator is an indirection mechanism which takes care of
     * the fact that QIODevice instances are injected into the data model by
     * having them represented using a URI. managerFor() returns appropriately
     * the right network access manager depending on whether the URI refers to
     * a variable, or to something else.
     *
     * The constructor take a pointer to two NetworkAccessDelegator instances.
     * First is a generic one, the second is the one which handles variable
     * bindings. managerFor() then returns the appropriate one depending on the
     * URI.
     *
     * @since 4.5
     * @see AccelTreeResourceLoader::load()
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT NetworkAccessDelegator : public QObject
                                                   , public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<NetworkAccessDelegator> Ptr;
        NetworkAccessDelegator(QNetworkAccessManager *const genericManager,
                               QNetworkAccessManager *const variableURIManager);

        QNetworkAccessManager *managerFor(const QUrl &uri);

        QPointer<QNetworkAccessManager> m_genericManager;
        QPointer<QNetworkAccessManager> m_variableURIManager;
    private:
        Q_DISABLE_COPY(NetworkAccessDelegator)
    };
}

QT_END_NAMESPACE
QT_END_HEADER

#endif
