/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>

/*
 This class exists for the solve purpose of creating a translation context.
*/
class XmlStreamLint
{
public:
    Q_DECLARE_TR_FUNCTIONS(XmlStreamLint)
};

int main(int argc, char *argv[])
{
    enum ExitCode
    {
        Success,
        ParseFailure,
        ArgumentError,
        WriteError,
        FileFailure
    };

    QCoreApplication app(argc, argv);

    QTextStream errorStream(stderr);

    if (argc != 2)
    {
        errorStream << XmlStreamLint::tr(
                       "Usage: xmlstreamlint <path to XML file>\n");
        return ArgumentError;
    }

    QString inputFilePath(QCoreApplication::arguments().at(1));
    QFile inputFile(inputFilePath);

    if (!QFile::exists(inputFilePath))
    {
        errorStream << XmlStreamLint::tr(
                       "File %1 does not exist.\n").arg(inputFilePath);
        return FileFailure;

    } else if (!inputFile.open(QIODevice::ReadOnly)) {
        errorStream << XmlStreamLint::tr(
                       "Failed to open file %1.\n").arg(inputFilePath);
        return FileFailure;
    }

    QFile outputFile;
    if (!outputFile.open(stdout, QIODevice::WriteOnly))
    {
        errorStream << XmlStreamLint::tr("Failed to open stdout.");
        return WriteError;
    }

//! [0]
    QXmlStreamReader reader(&inputFile);
    QXmlStreamWriter writer(&outputFile);
//! [0]

//! [1]
    while (!reader.atEnd())
    {
        reader.readNext();

        if (reader.error())
        {
            errorStream << XmlStreamLint::tr(
                           "Error: %1 in file %2 at line %3, column %4.\n").arg(
                               reader.errorString(), inputFilePath,
                               QString::number(reader.lineNumber()),
                               QString::number(reader.columnNumber()));
            return ParseFailure;
//! [1]

//! [2]
        } else
            writer.writeCurrentToken(reader);
    }
//! [2]

    return Success;
}
