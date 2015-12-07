/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTENGINEAGENT_H
#define QSCRIPTENGINEAGENT_H

#include <QtCore/qobjectdefs.h>

#ifndef QT_NO_SCRIPT

#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

class QScriptEngine;
class QScriptValue;

class QScriptEngineAgentPrivate;
class Q_SCRIPT_EXPORT QScriptEngineAgent
{
public:
    enum Extension {
    };

    QScriptEngineAgent(QScriptEngine *engine);
    virtual ~QScriptEngineAgent();

    virtual void scriptLoad(qint64 id, const QString &program,
                            const QString &fileName, int baseLineNumber);
    virtual void scriptUnload(qint64 id);

    virtual void contextPush();
    virtual void contextPop();

    virtual void functionEntry(qint64 scriptId);
    virtual void functionExit(qint64 scriptId,
                              const QScriptValue &returnValue);

    virtual void positionChange(qint64 scriptId,
                                int lineNumber, int columnNumber);

    virtual void exceptionThrow(qint64 scriptId,
                                const QScriptValue &exception,
                                bool hasHandler);
    virtual void exceptionCatch(qint64 scriptId,
                                const QScriptValue &exception);

    virtual bool supportsExtension(Extension extension) const;
    virtual QVariant extension(Extension extension,
                               const QVariant &argument = QVariant());

    QScriptEngine *engine() const;

protected:
    QScriptEngineAgent(QScriptEngineAgentPrivate &dd, QScriptEngine *engine);
    QScriptEngineAgentPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QScriptEngineAgent)
    Q_DISABLE_COPY(QScriptEngineAgent)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT

#endif
