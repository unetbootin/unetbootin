/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <QList>
#include <QMap>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QTime>
#include <QDebug>

#include <qtconcurrentmap.h>

#ifndef QT_NO_CONCURRENT

using namespace QtConcurrent;

/*
    Utility function that recursivily searches for files.
*/
QStringList findFiles(const QString &startDir, QStringList filters)
{
    QStringList names;
    QDir dir(startDir);

    foreach (QString file, dir.entryList(filters, QDir::Files))
        names += startDir + "/" + file;

    foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
        names += findFiles(startDir + "/" + subdir, filters);
    return names;
}

typedef QMap<QString, int> WordCount;

/*
    Single threaded word counter function.
*/
WordCount singleThreadedWordCount(QStringList files)
{
    WordCount wordCount;
    foreach (QString file, files) {
        QFile f(file);
        f.open(QIODevice::ReadOnly);
        QTextStream textStream(&f);
        while (textStream.atEnd() == false)
            foreach(QString word, textStream.readLine().split(" "))
                wordCount[word] += 1;

    }
    return wordCount;
}


// countWords counts the words in a single file. This function is
// called in parallel by several threads and must be thread
// safe.
WordCount countWords(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QTextStream textStream(&f);
    WordCount wordCount;

    while (textStream.atEnd() == false)
        foreach (QString word, textStream.readLine().split(" "))
            wordCount[word] += 1;

    return wordCount;
}

// reduce adds the results from map to the final
// result. This functor will only be called by one thread
// at a time.
void reduce(WordCount &result, const WordCount &w)
{
    QMapIterator<QString, int> i(w);
    while (i.hasNext()) {
        i.next();
        result[i.key()] += i.value();
    }
}

int main()
{
    qDebug() << "finding files...";
    QStringList files = findFiles("../../", QStringList() << "*.cpp" << "*.h");
    qDebug() << files.count() << "files";

    qDebug() << "warmup";
    {
        QTime time;
        time.start();
        WordCount total = singleThreadedWordCount(files);
    }

    qDebug() << "warmup done";

    int singleThreadTime = 0;
    {
        QTime time;
        time.start();
        WordCount total = singleThreadedWordCount(files);
        singleThreadTime = time.elapsed();
        qDebug() << "single thread" << singleThreadTime;
    }

    int mapReduceTime = 0;
    {
        QTime time;
        time.start();
        WordCount total = mappedReduced(files, countWords, reduce);
        mapReduceTime = time.elapsed();
        qDebug() << "MapReduce" << mapReduceTime;
    }
    qDebug() << "MapReduce speedup x" << ((double)singleThreadTime - (double)mapReduceTime) / (double)mapReduceTime + 1;
}

#else

int main()
{
	qDebug() << "Qt Concurrent is not yet supported on this platform";
}

#endif
