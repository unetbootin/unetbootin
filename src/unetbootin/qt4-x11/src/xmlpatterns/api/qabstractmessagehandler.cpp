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

#include <QMutex>

#include "private/qobject_p.h"
#include "qabstractmessagehandler.h"

QT_BEGIN_NAMESPACE

class QAbstractMessageHandlerPrivate : public QObjectPrivate
{
public:
    QMutex mutex;
};

/*!
  \class QAbstractMessageHandler
  \threadsafe
  \since 4.4
  \ingroup xml-tools

  \brief The QAbstractMessageHandler class provides a callback interface for handling messages.

  QAbstractMessageHandler is an abstract base class that provides a
  callback interface for handling messages. For example, class
  QXmlQuery parses and runs an XQuery. When it detects a compile
  or runtime error, it generates an appropriate error message,
  but rather than output the message itself, it passes the message to
  the message() function of its QAbstractMessageHandler.
  See QXmlQuery::setMessageHandler().

  You create a message handler by subclassing QAbstractMessageHandler
  and implementing handleMessage(). You then pass a pointer to an
  instance of your subclass to any classes that must generate
  messages. The messages are sent to the message handler via the
  message() function, which forwards them to your handleMessge().
  The effect is to serialize the handling of all messages, which
  means your QAbstractMessageHandler subclass is thread safe.

  A single instance of QAbstractMessageHandler can be called on to
  handle messages from multiple sources. Hence, the content of a
  message, which is the \e description parameter passed to message()
  and handleMessage(), must be interpreted in light of the context
  that required the message to be sent. That context is specified by
  the \e identifier and \e sourceLocation parameters to message()
  handleMessage().
 */

/*!
  Constructs a QAbstractMessageHandler. The \a parent is passed
  to the QObject base class constructor.
 */
QAbstractMessageHandler::QAbstractMessageHandler(QObject *parent) : QObject(*new QAbstractMessageHandlerPrivate(), parent)
{
}

/*!
  Destructs this QAbstractMessageHandler.
 */
QAbstractMessageHandler::~QAbstractMessageHandler()
{
}

/*!
  Sends a message to this message handler. \a type is the kind of
  message being sent. \a description is the message content. The \a
  identifier is a URI that identifies the message and is the key to
  interpreting the other arguments.

  Typically, this class is used for reporting errors, as is the case
  for QXmlQuery, which uses a QAbstractMessageHandler to report
  compile and runtime XQuery errors. Hence, using a QUrl as the
  message \a identifier is was inspired by the explanation of \l{error
  handling in the XQuery language}. Because the \a identifier is
  composed of a namespace URI and a local part, identifiers with the
  same local part are unique.  The caller is responsible for ensuring
  that \a identifier is either a valid QUrl or a default constructed
  QUrl.

  \a sourceLocation identifies a location in a resource (i.e., file or
  document) where the need for reporting a message was detected.

  This function unconditionally calls handleMessage(), passing all
  its parameters unmodified.

  \sa {http://www.w3.org/TR/xquery/#errors}
 */
void QAbstractMessageHandler::message(QtMsgType type,
                                      const QString &description,
                                      const QUrl &identifier,
                                      const QSourceLocation &sourceLocation)
{
    Q_D(QAbstractMessageHandler);
    QMutexLocker(&d->mutex);
    handleMessage(type, description, identifier, sourceLocation);
}

/*!
  \fn void QAbstractMessageHandler::handleMessage(QtMsgType type,
                                                  const QString &description,
                                                  const QUrl &identifier = QUrl(),
                                                  const QSourceLocation &sourceLocation = QSourceLocation()) = 0

  This function must be implemented by the sub-class. message() will
  call this function, passing in its parameters, \a type,
  \a description, \a identifier and \a sourceLocation unmodified.
 */

QT_END_NAMESPACE

