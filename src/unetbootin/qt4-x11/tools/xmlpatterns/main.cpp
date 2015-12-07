/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Patternist project on Trolltech Labs.
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

#include <QtCore/QDir>
#include <QtCore/QtDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <QtXmlPatterns/QXmlFormatter>
#include <QtXmlPatterns/QXmlItem>
#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlSerializer>

#include "private/qautoptr_p.h"
#include "qapplicationargument_p.h"
#include "qapplicationargumentparser_p.h"
#include "qcoloringmessagehandler_p.h"

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
/* Needed for opening stdout with _fdopen & friends. io.h seems to not be
 * needed on MinGW though. */
#include <io.h>
#include <fcntl.h>
#endif

#include "main.h"

QT_USE_NAMESPACE

/* The two Q_DECLARE_METATYPE macros must appear before the code
 * on a couple of HPUX platforms. */

/*!
 \internal
 \since 4.4
 Represents the name and value found in "-param name=value".
 */
typedef QPair<QString, QString> Parameter;
Q_DECLARE_METATYPE(Parameter);

/*!
 \internal
 \since 4.4
 For the -output switch.
 */
Q_DECLARE_METATYPE(QIODevice *);

/*!
 \class PatternistApplicationParser
 \brief Subclass to handle -param name=value
 \internal
 \since 4.4
 \reentrant
 */
class PatternistApplicationParser : public QApplicationArgumentParser
{
public:
    inline PatternistApplicationParser(int argc, char **argv,
                                       const QXmlNamePool &np) : QApplicationArgumentParser(argc, argv)
                                                               , m_namePool(np)
#ifdef Q_OS_WIN
                                                               , m_stdout(0)
#endif
    {
    }

#ifdef Q_OS_WIN
    virtual ~PatternistApplicationParser()
    {
        /* QFile::~QFile() nor QFile::close() frees the handle when
         * we use QFile::open() so we have to do it manually.
         *
         * "If stream is NULL, the invalid parameter handler is invoked," so
         * lets try to avoid that. */
        if(m_stdout)
            fclose(m_stdout);
    }
#endif

protected:
    virtual QVariant convertToValue(const QApplicationArgument &arg,
                                    const QString &input) const
    {
        if(arg.name() == QLatin1String("param"))
        {
            const int assign = input.indexOf(QLatin1Char('='));

            if(assign == -1)
            {
                message(QXmlPatternistCLI::tr("Each binding must contain an equal sign."));
                return QVariant();
            }

            const QString name(input.left(assign));
            const QString value(input.mid(assign + 1));

            if(!QXmlName::isNCName(name))
            {
                message(QXmlPatternistCLI::tr("The variable name must be a valid NCName, which %1 isn't.").arg(name));
                return QVariant();
            }

            /* The value.isNull() check ensures we can bind variables whose value is an empty string. */
            return qVariantFromValue(Parameter(name, value.isNull() ? QString(QLatin1String("")) : value ));
        }
        else if(arg.name() == QLatin1String("output"))
        {
            QFile *const f = new QFile(input);

            if(f->open(QIODevice::WriteOnly))
                return qVariantFromValue(static_cast<QIODevice *>(f));
            else
            {
                message(QXmlPatternistCLI::tr("Failed to open file %1 for writing: %2").arg(f->fileName(), f->errorString()));
                return QVariant();
            }
        }
        else if(arg.name() == QLatin1String("initial-template"))
        {
            const QXmlName name(QXmlName::fromClarkName(input, m_namePool));
            if(name.isNull())
            {
                message(QXmlPatternistCLI::tr("%1 is an invalid Clark Name").arg(input));
                return QVariant();
            }
            else
                return qVariantFromValue(name);
        }
        else
            return QApplicationArgumentParser::convertToValue(arg, input);
    }

    virtual QString typeToName(const QApplicationArgument &argument) const
    {
        if(argument.name() == QLatin1String("param"))
            return QLatin1String("name=value");
        else if(argument.name() == QLatin1String("output"))
            return QLatin1String("local file");
        else
            return QApplicationArgumentParser::typeToName(argument);
    }

    virtual QVariant defaultValue(const QApplicationArgument &argument) const
    {
        if(argument.name() == QLatin1String("output"))
        {
            QFile *const out = new QFile();

#ifdef Q_OS_WIN
            /* If we don't open stdout in "binary" mode on Windows, it will translate
             * 0xA into 0xD 0xA. See Trolltech task 173619, for an example. */
            _setmode(_fileno(stdout), _O_BINARY);
            m_stdout = QT_WA_INLINE(_wfdopen(_fileno(stdout), L"wb"),_fdopen(_fileno(stdout), "wb"));
            out->open(m_stdout, QIODevice::WriteOnly);
#else
            out->open(stdout, QIODevice::WriteOnly);
#endif

            return qVariantFromValue(static_cast<QIODevice *>(out));
        }
        else
            return QApplicationArgumentParser::defaultValue(argument);
    }

private:
    QXmlNamePool    m_namePool;
#ifdef Q_OS_WIN
    mutable FILE *  m_stdout;
#endif
};

static inline QUrl finalizeURI(const QApplicationArgumentParser &parser,
                               const QApplicationArgument &isURI,
                               const QApplicationArgument &arg)
{
    QUrl userURI;
    {
        const QString stringURI(parser.value(arg).toString());

        if(parser.has(isURI))
            userURI = QUrl::fromEncoded(stringURI.toLatin1());
        else
            userURI = QUrl::fromLocalFile(stringURI);
    }

    return QUrl::fromLocalFile(QDir::current().absolutePath() + QLatin1Char('/')).resolved(userURI);
}

int main(int argc, char **argv)
{
    enum ExitCode
    {
        /**
         * We start from 2, because QApplicationArgumentParser
         * uses 1.
         */
        QueryFailure = 2,
        StdOutFailure
    };

    const QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QLatin1String("xmlpatterns"));

    QXmlNamePool namePool;
    PatternistApplicationParser parser(argc, argv, namePool);
    parser.setApplicationDescription(QLatin1String("A tool for running XQuery queries."));
    parser.setApplicationVersion(QLatin1String("0.1"));

    QApplicationArgument param(QLatin1String("param"),
                               QXmlPatternistCLI::tr("Binds an external variable. The value is directly available using the variable reference: $name."),
                               qMetaTypeId<Parameter>());
    param.setMaximumOccurrence(-1);
    parser.addArgument(param);

    const QApplicationArgument noformat(QLatin1String("no-format"),
                                        QXmlPatternistCLI::tr("By default output is formatted for readability. When specified, strict serialization is performed."));
    parser.addArgument(noformat);

    const QApplicationArgument isURI(QLatin1String("is-uri"),
                                     QXmlPatternistCLI::tr("If specified, all filenames on the command line are interpreted as URIs instead of a local filenames."));
    parser.addArgument(isURI);

    const QApplicationArgument initialTemplateName(QLatin1String("initial-template"),
                                                   QXmlPatternistCLI::tr("The name of the initial template to call as a Clark Name."),
                                                   QVariant::String);
    parser.addArgument(initialTemplateName);

    /* The temporary object is required to compile with g++ 3.3. */
    QApplicationArgument queryURI = QApplicationArgument(QLatin1String("query/stylesheet"),
                                                         QXmlPatternistCLI::tr("A local filename pointing to the query to run. If the name ends with .xsl it's assumed "
                                                                               "to be an XSL-T stylesheet. If it ends with .xq, it's assumed to be an XQuery query. (In "
                                                                               "other cases it's also assumed to be an XQuery query, but that interpretation may "
                                                                               "change in a future release of Qt.)"),
                                                         QVariant::String);
    queryURI.setMinimumOccurrence(1);
    queryURI.setNameless(true);
    parser.addArgument(queryURI);

    QApplicationArgument focus = QApplicationArgument(QLatin1String("focus"),
                                                      QXmlPatternistCLI::tr("The document to use as focus. Mandatory "
                                                                            "in case a stylesheet is used. This option is "
                                                                            "also affected by the is-uris option."),
                                                      QVariant::String);
    focus.setMinimumOccurrence(0);
    focus.setNameless(true);
    parser.addArgument(focus);

    QApplicationArgument output(QLatin1String("output"),
                                QXmlPatternistCLI::tr("A local file to which the output should be written. "
                                                      "The file is overwritten, or if not exist, created. "
                                                      "If absent, stdout is used."),
                                qMetaTypeId<QIODevice *>());
    parser.addArgument(output);

    if(!parser.parse())
        return parser.exitCode();

    /* Get the query URI. */
    const QUrl effectiveURI(finalizeURI(parser, isURI, queryURI));

    QXmlQuery::QueryLanguage lang;

    if(effectiveURI.toString().endsWith(QLatin1String(".xsl")))
         lang = QXmlQuery::XSLT20;
    else
         lang = QXmlQuery::XQuery10;

    if(lang == QXmlQuery::XQuery10 && parser.has(initialTemplateName))
    {
        parser.message(QXmlPatternistCLI::tr("An initial template name cannot be specified when running an XQuery."));
        return QApplicationArgumentParser::ParseError;
    }

    QXmlQuery query(lang, namePool);

    query.setInitialTemplateName(qVariantValue<QXmlName>(parser.value(initialTemplateName)));

    /* Bind external variables. */
    {
        const QVariantList parameters(parser.values(param));
        const int len = parameters.count();

        /* For tracking duplicates. */
        QSet<QString> usedParameters;

        for(int i = 0; i < len; ++i)
        {
            const Parameter p(qVariantValue<Parameter>(parameters.at(i)));

            if(usedParameters.contains(p.first))
            {
                parser.message(QXmlPatternistCLI::tr("Each parameter must be unique, %1 is specified at least twice.").arg(p.first));
                return QApplicationArgumentParser::ParseError;
            }
            else
            {
                usedParameters.insert(p.first);
                query.bindVariable(p.first, QXmlItem(p.second));
            }
        }
    }

    if(parser.has(focus))
    {
        if(!query.setFocus(finalizeURI(parser, isURI, focus)))
            return QueryFailure;
    }
    else if(lang == QXmlQuery::XSLT20 && !parser.has(initialTemplateName))
    {
        parser.message(QXmlPatternistCLI::tr("When a stylesheet is used, a "
                                             "document must be specified as a focus, or an "
                                             "initial template name must be specified, or both."));
        return QApplicationArgumentParser::ParseError;
    }

    query.setQuery(effectiveURI);

    const QPatternist::AutoPtr<QIODevice> outDevice(qVariantValue<QIODevice *>(parser.value(output)));
    Q_ASSERT(outDevice);
    Q_ASSERT(outDevice->isWritable());

    if(query.isValid())
    {
        typedef QPatternist::AutoPtr<QAbstractXmlReceiver> RecPtr;
        RecPtr receiver;

        if(parser.has(noformat))
            receiver = RecPtr(new QXmlSerializer(query, outDevice.data()));
        else
            receiver = RecPtr(new QXmlFormatter(query, outDevice.data()));

        const bool success = query.evaluateTo(receiver.data());

        if(success)
            return parser.exitCode();
        else
            return QueryFailure;
    }
    else
        return QueryFailure;
}

