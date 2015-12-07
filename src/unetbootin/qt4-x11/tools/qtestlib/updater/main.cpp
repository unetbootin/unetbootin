/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QtCore/QtCore>

#include <stdio.h>

QT_USE_NAMESPACE

static void printHelp(char *argv[])
{
    qDebug("Usage: %s [-diff] FILES", argv[0] ? argv[0] : "qtest2to4");
    qDebug("updates files from QtTestLib 2.x to QTestLib 4.1");
    qDebug("\noptions:\n    -diff   Don't write any changes, output differences instead.");
    exit(2);
}

int main(int argc, char *argv[])
{
    bool printDiff = false;
    int i = 1;

    if (argc == 1)
        printHelp(argv);
    if (argv[1][0] == '-') {
        if (qstrcmp(argv[1], "-diff") == 0) {
            printDiff = true;
            ++i;
        } else {
            qDebug("Unknown option: %s\n", argv[1]);
            printHelp(argv);
        }
    }

    QRegExp dataHeaderRx(QLatin1String("_data(\\s*)\\((\\s*)QtTestTable\\s*\\&\\s*\\w*\\s*\\)"));
    QRegExp defElemRx(QLatin1String("\\w+\\.defineElement\\s*\\(\\s*\"(.+)\"\\s*,\\s*\"(.+)\"\\s*\\)"));
    defElemRx.setMinimal(true);
    QRegExp addDataRx(QLatin1String("\\*\\w+\\.newData(\\s*)(\\(\\s*\".*\"\\s*\\))"));
    addDataRx.setMinimal(true);
    QRegExp nsRx(QLatin1String("namespace(\\s+)QtTest"));
    QRegExp callRx(QLatin1String("QtTest(\\s*)::"));

    enum { MacroCount = 11 };
    static const char *macroNames[MacroCount] = {
        "VERIFY", "FAIL", "VERIFY2", "COMPARE", "SKIP", "VERIFY_EVENT",
        "EXPECT_FAIL", "FETCH", "FETCH_GLOBAL", "TEST", "WARN"
    };

    for (; i < argc; ++i) {
        QFile f(QString::fromLocal8Bit(argv[i]));
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
            qFatal("Unable to open file '%s' for reading: %s", argv[i], qPrintable(f.errorString()));

        if (printDiff)
            printf("diff %s\n", argv[i]);

        QStringList contents;
        int lineNumber = 0;
        int changedLines = 0;
        while (!f.atEnd()) {
            QString origLine = QString::fromLatin1(f.readLine());
            QString line = origLine;
            ++lineNumber;

            if (dataHeaderRx.indexIn(line) != -1) {
                QString ws = dataHeaderRx.cap(1);
                line.replace(dataHeaderRx, QString::fromLatin1("_data%1()").arg(ws));
            }
            if (defElemRx.indexIn(line) != -1) {
                QString type = defElemRx.cap(1);
                QString name = defElemRx.cap(2);
                if (type.endsWith(QLatin1Char('>')))
                    type.append(QLatin1Char(' '));
                line.replace(defElemRx, QString::fromLatin1("QTest::addColumn<%1>(\"%2\")").arg(
                               type).arg(name));
            }
            if (addDataRx.indexIn(line) != -1) {
                QString repl = QLatin1String("QTest::newRow");
                repl += addDataRx.cap(1);
                repl += addDataRx.cap(2);
                line.replace(addDataRx, repl);
            }
            if (nsRx.indexIn(line) != -1)
                line.replace(nsRx, QString::fromLatin1("namespace%1QTest").arg(nsRx.cap(1)));
            int pos = 0;
            while ((pos = callRx.indexIn(line, pos)) != -1) {
                line.replace(callRx, QString::fromLatin1("QTest%1::").arg(callRx.cap(1)));
                pos += callRx.matchedLength();
            }

            line.replace(QLatin1String("QTTEST_MAIN"), QLatin1String("QTEST_MAIN"));
            line.replace(QLatin1String("QTTEST_APPLESS_MAIN"), QLatin1String("QTEST_APPLESS_MAIN"));
            line.replace(QLatin1String("QTTEST_NOOP_MAIN"), QLatin1String("QTEST_NOOP_MAIN"));
            line.replace(QLatin1String("QtTestEventLoop"), QLatin1String("QTestEventLoop"));
            line.replace(QLatin1String("QtTestEventList"), QLatin1String("QTestEventList"));
            line.replace(QLatin1String("QtTestAccessibility"), QLatin1String("QTestAccessibility"));
            line.replace(QLatin1String("QTest::sleep"), QLatin1String("QTest::qSleep"));
            line.replace(QLatin1String("QTest::wait"), QLatin1String("QTest::qWait"));

            for (int m = 0; m < MacroCount; ++m) {
                QRegExp macroRe(QString::fromLatin1("\\b%1(\\s*\\()").arg(
                                 QLatin1String(macroNames[m])));
                QString newMacroName = QLatin1Char('Q') + QString::fromLatin1(macroNames[m]);
                int pos = 0;
                while ((pos = macroRe.indexIn(line)) != -1) {
                    line.replace(macroRe, newMacroName + macroRe.cap(1));
                    pos += macroRe.matchedLength();
                }
            }

            if (line != origLine) {
                if (printDiff) {
                    printf("%dc%d\n", lineNumber, lineNumber);
                    printf("<%s", qPrintable(origLine));
                    printf("---\n");
                    printf(">%s", qPrintable(line));
                }
                ++changedLines;
            }

            contents.append(line);
        }
        f.close();

        if (printDiff)
            continue;
        qDebug("%s: %d change%s made.", argv[i], changedLines, changedLines == 1 ? "" : "s");
        if (!changedLines)
            continue;

        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
            qFatal("Unable to open file '%s' for writing: %s", argv[i], qPrintable(f.errorString()));
        foreach (QString s, contents)
            f.write(s.toLatin1());
        f.close();
    }

    return 0;
}

