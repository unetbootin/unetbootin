/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
** ** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QApplicationArgumentParser_H
#define QApplicationArgumentParser_H

#include <QtCore/QVariant> /* Needed, because we can't forward declare QVariantList. */

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QApplicationArgument;
class QApplicationArgumentParserPrivate;
class QStringList;
template<typename A, typename B> struct QPair;
template<typename T> class QList;
template<typename Value> class QList;

class QApplicationArgumentParser
{
public:
    enum ExitCode
    {
        Success = 0,
        ParseError = 1
    };

    QApplicationArgumentParser(int argc, char **argv);
    QApplicationArgumentParser(const QStringList &input);
    virtual ~QApplicationArgumentParser();
    void addArgument(const QApplicationArgument &argument);
    void setDeclaredArguments(const QList<QApplicationArgument> &arguments);
    QList<QApplicationArgument> declaredArguments() const;

    int count(const QApplicationArgument &argument) const;
    bool has(const QApplicationArgument &argument) const;

    virtual bool parse();
    ExitCode exitCode() const;
    QVariant value(const QApplicationArgument &argument) const;
    QVariantList values(const QApplicationArgument &argument) const;
    void setApplicationDescription(const QString &description);
    void setApplicationVersion(const QString &version);

protected:
    void setExitCode(ExitCode code);
    void setUsedArguments(const QList<QPair<QApplicationArgument, QVariant> > &arguments);
    QList<QPair<QApplicationArgument, QVariant> > usedArguments() const;
    QStringList input() const;
    virtual QVariant convertToValue(const QApplicationArgument &argument,
                                    const QString &value) const;
    virtual QString typeToName(const QApplicationArgument &argument) const;
    virtual QVariant defaultValue(const QApplicationArgument &argument) const;
    virtual void message(const QString &message) const;

private:
    friend class QApplicationArgumentParserPrivate;
    QApplicationArgumentParserPrivate *d;
    Q_DISABLE_COPY(QApplicationArgumentParser)
};

QT_END_NAMESPACE
QT_END_HEADER
#endif
