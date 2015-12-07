/****************************************************************************
 * ** * ** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
 * **
 * ** This file is part of the Patternist project on Trolltech Labs.
 * **
 * ** This file may be used under the terms of the GNU General Public
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
 * **
 * ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * **
 * ****************************************************************************/

#include <QXmlStreamReader>

#include "main.h"

#include "qcoloringmessagehandler_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ColoringMessageHandler::ColoringMessageHandler(QObject *parent) : QAbstractMessageHandler(parent)
{
    m_classToColor.insert(QLatin1String("XQuery-data"), Data);
    m_classToColor.insert(QLatin1String("XQuery-expression"), Keyword);
    m_classToColor.insert(QLatin1String("XQuery-function"), Keyword);
    m_classToColor.insert(QLatin1String("XQuery-keyword"), Keyword);
    m_classToColor.insert(QLatin1String("XQuery-type"), Keyword);
    m_classToColor.insert(QLatin1String("XQuery-uri"), Data);

    /* If you're tuning the colors, take it easy laddie. Take into account:
     *
     * - Get over your own taste, there's others too on this planet
     * - Make sure it works well on black & white
     * - Make sure it works well on white & black
     */
    insertMapping(Location, CyanForeground);
    insertMapping(ErrorCode, RedForeground);
    insertMapping(Keyword, BlueForeground);
    insertMapping(Data, BlueForeground);
    insertMapping(RunningText, DefaultColor);
}

void ColoringMessageHandler::handleMessage(QtMsgType type,
                                           const QString &description,
                                           const QUrl &identifier,
                                           const QSourceLocation &sourceLocation)
{
    const bool hasLine = sourceLocation.line() != -1;

    switch(type)
    {
        case QtWarningMsg:
        {
            if(hasLine)
            {
                writeUncolored(QXmlPatternistCLI::tr("Warning in %1, at line %2, column %3: %4").arg(QString::fromLatin1(sourceLocation.uri().toEncoded()),
                                                                                                 QString::number(sourceLocation.line()),
                                                                                                 QString::number(sourceLocation.column()),
                                                                                                 colorifyDescription(description)));
            }
            else
            {
                writeUncolored(QXmlPatternistCLI::tr("Warning in %1: %2").arg(QString::fromLatin1(sourceLocation.uri().toEncoded()),
                                                                          colorifyDescription(description)));
            }

            break;
        }
        case QtFatalMsg:
        {
            Q_ASSERT(!sourceLocation.isNull());
            const QString errorCode(identifier.fragment());
            Q_ASSERT(!errorCode.isEmpty());
            QUrl uri(identifier);
            uri.setFragment(QString());

            QString errorId;
            /* If it's a standard error code, we don't want to output the
             * whole URI. */
            if(uri.toString() == QLatin1String("http://www.w3.org/2005/xqt-errors"))
                errorId = errorCode;
            else
                errorId = QString::fromLatin1(identifier.toEncoded());

            if(hasLine)
            {
                writeUncolored(QXmlPatternistCLI::tr("Error %1 in %2, at line %3, column %4: %5").arg(colorify(errorId, ErrorCode),
                                                                                                  colorify(QString::fromLatin1(sourceLocation.uri().toEncoded()), Location),
                                                                                                  colorify(QString::number(sourceLocation.line()), Location),
                                                                                                  colorify(QString::number(sourceLocation.column()), Location),
                                                                                                  colorifyDescription(description)));
            }
            else
            {
                writeUncolored(QXmlPatternistCLI::tr("Error %1 in %2: %3").arg(colorify(errorId, ErrorCode),
                                                                           colorify(QString::fromLatin1(sourceLocation.uri().toEncoded()), Location),
                                                                           colorifyDescription(description)));
            }
            break;
        }
        case QtCriticalMsg:
        /* Fallthrough. */
        case QtDebugMsg:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "message() is not supposed to receive QtCriticalMsg or QtDebugMsg.");
            return;
        }
    }
}

QString ColoringMessageHandler::colorifyDescription(const QString &in) const
{
    QXmlStreamReader reader(in);
    QString result;
    result.reserve(in.size());
    ColorType currentColor = RunningText;

    while(!reader.atEnd())
    {
        reader.readNext();

        switch(reader.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if(reader.name() == QLatin1String("span"))
                {
                    Q_ASSERT(m_classToColor.contains(reader.attributes().value(QLatin1String("class")).toString()));
                    currentColor = m_classToColor.value(reader.attributes().value(QLatin1String("class")).toString());
                }

                continue;
            }
            case QXmlStreamReader::Characters:
            {
                result.append(colorify(reader.text().toString(), currentColor));
                continue;
            }
            case QXmlStreamReader::EndElement:
            {
                currentColor = RunningText;
                continue;
            }
            /* Fallthrough, */
            case QXmlStreamReader::StartDocument:
            /* Fallthrough, */
            case QXmlStreamReader::EndDocument:
                continue;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO,
                           "Unexpected node.");
        }
    }

    Q_ASSERT_X(!reader.hasError(), Q_FUNC_INFO,
               "The output from Patternist must be well-formed.");
    return result;
}

QT_END_NAMESPACE
