/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "filewriter.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <ctype.h>

QT_BEGIN_NAMESPACE

FileWriter *FileWriter::theInstance  = 0;

FileWriter *FileWriter::instance()
{
     if(!theInstance)
        theInstance = new FileWriter();

        return theInstance;
}

void FileWriter::deleteInstance()
{
    if(theInstance) {
        delete theInstance;
        theInstance=0;
    }
}

FileWriter::FileWriter(OverWriteFiles overWrite, QString overwriteMsg)
:overWriteFiles(overWrite)
,overwriteMessage(overwriteMsg)
{
    if(overwriteMessage.isEmpty())
       overwriteMessage = QLatin1String("Convert file ");
}

FileWriter::WriteResult FileWriter::writeFileVerbously(QString filePath, QByteArray contents)
{
    const WriteResult result = writeFile(filePath, contents);
    if (result == WriteSucceeded) {
        QString cleanPath = QDir::cleanPath(filePath);
        printf("Wrote to file: %s \n", QDir::toNativeSeparators(cleanPath).toLocal8Bit().constData());
    }
    return result;
}

FileWriter::WriteResult FileWriter::writeFile(QString filePath, QByteArray contents)
{
    if(filePath.isEmpty())
        return WriteFailed;
    QString path = QFileInfo(filePath).path();
    if (!QDir().mkpath(path)){
         printf("Error creating path %s \n", QDir::toNativeSeparators(path).toLocal8Bit().constData());
    }

    QString cleanPath = QDir::cleanPath(filePath);
    QFile f(cleanPath);
    if (f.exists()) {
        if (overWriteFiles == DontOverWrite) {
            printf("Error writing file %s: It already exists \n",
                QDir::toNativeSeparators(cleanPath).toLatin1().constData());
            return WriteFailed;
        } else if(overWriteFiles == AskOnOverWrite) {
            printf("%s%s? (Y)es, (N)o, (A)ll ", overwriteMessage.toLatin1().constData(),
                QDir::toNativeSeparators(cleanPath).toLatin1().constData());
            
            char answer = 0;
            while (answer != 'y' && answer != 'n' && answer != 'a') {
#if defined(Q_OS_WIN) && defined(_MSC_VER) && _MSC_VER >= 1400
                scanf_s("%c", &answer);
#else
                scanf("%c", &answer);
#endif
                answer = tolower(answer);
            }

            if(answer == 'n')
                return WriteSkipped;
            else if(answer == 'a')
                overWriteFiles=AlwaysOverWrite;
        }
    }

    f.open(QFile::WriteOnly);
    if (f.isOpen() && f.write(contents) == contents.size())
        return WriteSucceeded;

    printf("Could not write to to file: %s. Is it write protected?\n",
        QDir::toNativeSeparators(filePath).toLatin1().constData());

    return WriteFailed;
}

/*
    Sets the write mode for the file writer. writeMode is one of
    DontOverWrite, AlwaysOverWrite, AskOnOverWrite.
*/
void FileWriter::setOverwriteFiles(OverWriteFiles writeMode)
{
    overWriteFiles = writeMode;
}

QByteArray detectLineEndings(const QByteArray &array)
{
    if (array.contains("\r\n")) {
        return "\r\n";
    } else {
        return "\n";
    }
}

QT_END_NAMESPACE
