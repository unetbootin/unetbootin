/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
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

#include "qscriptcompletiontask_p.h"
#include "qscriptcompletiontaskinterface_p_p.h"
#include "qscriptdebuggerconsole_p.h"
#include "qscriptdebuggerconsolecommand_p.h"
#include "qscriptdebuggerconsolecommandmanager_p.h"

#include "qscriptenginedebuggerfrontend_p.h" // ### kill
#include "qscriptdebuggerbackend_p.h" // ### kill
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptvalueiterator.h>

#include "private/qobject_p.h"

#include <QtCore/qset.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QScriptCompletionTaskPrivate
    : public QScriptCompletionTaskInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptCompletionTask)
public:
    QScriptCompletionTaskPrivate();
    ~QScriptCompletionTaskPrivate();

    void completeScriptExpression();

    QString contents;
    int cursorPosition;
    int frameIndex;
    QScriptDebuggerFrontend *frontend;
    QScriptDebuggerConsole *console;
};

QScriptCompletionTaskPrivate::QScriptCompletionTaskPrivate()
{
}

QScriptCompletionTaskPrivate::~QScriptCompletionTaskPrivate()
{
}

QScriptCompletionTask::QScriptCompletionTask(
    const QString &contents, int cursorPosition,
    int frameIndex, QScriptDebuggerFrontend *frontend,
    QScriptDebuggerConsole *console,
    QObject *parent)
    : QScriptCompletionTaskInterface(
        *new QScriptCompletionTaskPrivate, parent)
{
    Q_D(QScriptCompletionTask);
    d->contents = contents;
    d->cursorPosition = cursorPosition;
    if ((frameIndex == -1) && console)
        d->frameIndex = console->currentFrameIndex();
    else
        d->frameIndex = frameIndex;
    d->frontend = frontend;
    d->console = console;
}

QScriptCompletionTask::~QScriptCompletionTask()
{
}

namespace {

static bool isIdentChar(const QChar &ch)
{
    static QChar underscore = QLatin1Char('_');
    return ch.isLetter() || (ch == underscore);
}

static bool isPrefixOf(const QString &prefix, const QString &what)
{
    return ((what.length() > prefix.length())
            && what.startsWith(prefix));
}

} // namespace

void QScriptCompletionTaskPrivate::completeScriptExpression()
{
    int pos = cursorPosition;
    if ((pos > 0) && contents.at(pos-1).isNumber()) {
        // completion of numbers is pointless
        return;
    }

    while ((pos > 0) && isIdentChar(contents.at(pos-1)))
        --pos;
    int pos2 = cursorPosition;
    while ((pos2 < contents.size()-1) && isIdentChar(contents.at(pos2+1)))
        ++pos2;
    QString ident = contents.mid(pos, pos2 - pos + 1);
    position = pos;

    QStringList path;
    path.append(ident);
    while ((pos > 0) && (contents.at(pos-1) == QLatin1Char('.'))) {
        --pos;
        pos2 = pos;
        while ((pos > 0) && isIdentChar(contents.at(pos-1)))
            --pos;
        path.prepend(contents.mid(pos, pos2 - pos));
    }

    // ### super-cheating for now; have to use the async API
    QScriptEngineDebuggerFrontend *edf = static_cast<QScriptEngineDebuggerFrontend*>(frontend);
    QScriptDebuggerBackend *backend = edf->backend();
    QScriptContext *ctx = backend->context(frameIndex);
    QScriptValueList objects;
    QString prefix = path.last();
    QSet<QString> matches;
    if (path.size() > 1) {
        const QString &topLevelIdent = path.at(0);
        QScriptValue obj;
        if (topLevelIdent == QString::fromLatin1("this")) {
            obj = ctx->thisObject();
        } else {
            QScriptValueList scopeChain;
#if QT_VERSION >= 0x040500
            scopeChain = ctx->scopeChain();
#else
            scopeChain.append(ctx->activationObject());
#endif
            for (int i = 0; i < scopeChain.size(); ++i) {
                QScriptValue oo = scopeChain.at(i).property(topLevelIdent);
                if (oo.isObject()) {
                    obj = oo;
                    break;
                }
            }
        }
        for (int i = 1; obj.isObject() && (i < path.size()-1); ++i)
            obj = obj.property(path.at(i));
        if (obj.isValid())
            objects.append(obj);
    } else {
#if QT_VERSION >= 0x040500
        objects << ctx->scopeChain();
#else
        objects.append(ctx->activationObject());
#endif
        QStringList keywords;
        keywords.append(QString::fromLatin1("this"));
        keywords.append(QString::fromLatin1("true"));
        keywords.append(QString::fromLatin1("false"));
        keywords.append(QString::fromLatin1("null"));
        for (int i = 0; i < keywords.size(); ++i) {
            const QString &kwd = keywords.at(i);
            if (isPrefixOf(prefix, kwd))
                matches.insert(kwd);
        }
    }

    for (int i = 0; i < objects.size(); ++i) {
        QScriptValue obj = objects.at(i);
        while (obj.isObject()) {
            QScriptValueIterator it(obj);
            while (it.hasNext()) {
                it.next();
                QString propertyName = it.name();
                if (isPrefixOf(prefix, propertyName))
                    matches.insert(propertyName);
            }
            obj = obj.prototype();
        }
    }
    results = matches.toList();
    qStableSort(results);

    length = prefix.length();
    type = QScriptCompletionTask::ScriptIdentifierCompletion;
}

void QScriptCompletionTask::start()
{
    Q_D(QScriptCompletionTask);
    d->type = NoCompletion;
    // see if we're typing a command
    // ### don't hardcode the command prefix
    QRegExp cmdRx(QString::fromLatin1("^\\s*\\.([a-zA-Z]*)"));
    int cmdIndex = cmdRx.indexIn(d->contents);
    if ((cmdIndex != -1) && d->console) {
        int len = cmdRx.matchedLength();
        QString prefix = cmdRx.capturedTexts().at(1);
        if ((d->cursorPosition >= cmdIndex) && (d->cursorPosition <= (cmdIndex+len))) {
            // editing command --> get command completions
            d->results = d->console->commandManager()->completions(prefix);
            qStableSort(d->results);
            d->position = cmdRx.pos(1);
            d->length = prefix.length();
            d->type = CommandNameCompletion;
            d->appendix = QString::fromLatin1(" ");
            emit finished();
        } else {
            QScriptDebuggerConsoleCommand *cmd = d->console->commandManager()->findCommand(prefix);
            if (!cmd) {
                emit finished();
                return;
            }
            // editing an argument
            int argNum = 0;
            QString arg;
            int pos = cmdIndex + len;
            while (pos < d->contents.size()) {
                while ((pos < d->contents.size()) && d->contents.at(pos).isSpace())
                    ++pos;
                if (pos < d->contents.size()) {
                    int pos2 = pos + 1;
                    while ((pos2 < d->contents.size()) && !d->contents.at(pos2).isSpace())
                        ++pos2;
                    if ((d->cursorPosition >= pos) && (d->cursorPosition <= pos2)) {
                        arg = d->contents.mid(pos, pos2 - pos);
                        break;
                    }
                    pos = pos2;
                    ++argNum;
                }
            }
            QString argType = cmd->argumentTypes().value(argNum);
            if (!argType.isEmpty()) {
                if (argType == QString::fromLatin1("command-or-group-name")) {
                    d->results = d->console->commandManager()->completions(arg);
                } else if (argType == QString::fromLatin1("script-filename")) {
                    // ### super-cheating for now; have to use the async API
                    QScriptEngineDebuggerFrontend *edf = static_cast<QScriptEngineDebuggerFrontend*>(d->frontend);
                    QScriptDebuggerBackend *backend = edf->backend();
                    QScriptScriptMap scripts = backend->scripts();
                    QScriptScriptMap::const_iterator it;
                    for (it = scripts.constBegin(); it != scripts.constEnd(); ++it) {
                        QString fileName = it.value().fileName();
                        if (isPrefixOf(arg, fileName))
                            d->results.append(fileName);
                    }
                } else if (argType == QString::fromLatin1("subcommand-name")) {
                    for (int i = 0; i < cmd->subCommands().size(); ++i) {
                        QString name = cmd->subCommands().at(i);
                        if (isPrefixOf(arg, name))
                            d->results.append(name);
                    }
                } else if (argType == QString::fromLatin1("script")) {
                    d->completeScriptExpression();
                }
                if ((d->type == NoCompletion) && !d->results.isEmpty()) {
                    qStableSort(d->results);
                    d->position = pos;
                    d->length = arg.length();
                    d->type = CommandArgumentCompletion;
                }
            }
            emit finished();
        }
    } else {
        // assume it's an eval expression
        d->completeScriptExpression();
        emit finished();
    }
}

QT_END_NAMESPACE
