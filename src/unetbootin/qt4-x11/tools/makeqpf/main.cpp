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

#include <QtGui>

#include "qpf2.h"
#include "mainwindow.h"

#include <private/qfontengine_p.h>

QT_BEGIN_NAMESPACE

static void help()
{
    printf("usage:\n");
    printf("makeqpf fontname pixelsize [italic] [bold] [--exclude-cmap] [-v]\n");
    printf("makeqpf -dump [-v] file.qpf2\n");
    exit(0);
}

static int gui(const QString &customFont = QString())
{
    MainWindow mw(customFont);
    mw.show();
    return qApp->exec();
}

QT_END_NAMESPACE

int main(int argc, char **argv)
{
    QT_USE_NAMESPACE

    QApplication app(argc, argv);
    app.setOrganizationName(QLatin1String("Trolltech"));
    app.setApplicationName(QLatin1String("MakeQPF"));

    const QStringList arguments = app.arguments();

    if (arguments.count() <= 1) {
        return gui();
    } else if (arguments.count() == 2
               && QFile::exists(arguments.at(1))) {
        return gui(arguments.at(1));
    }

    const QString &firstArg = arguments.at(1);
    if (firstArg == QLatin1String("-h") || firstArg == QLatin1String("--help"))
        help();
    if (firstArg == QLatin1String("-dump")) {
        QString file;
        for (int i = 2; i < arguments.count(); ++i) {
            if (arguments.at(i).startsWith(QLatin1String("-v")))
                QPF::debugVerbosity += arguments.at(i).length() - 1;
            else if (file.isEmpty())
                file = arguments.at(i);
            else
                help();
        }

        if (file.isEmpty())
            help();

        QFile f(file);
        if (!f.open(QIODevice::ReadOnly)) {
            printf("cannot open %s\n", qPrintable(file));
            exit(1);
        }

        QByteArray qpf = f.readAll();
        f.close();

        QPF::dump(qpf);
        return 0;
    }

    if (arguments.count() < 3) help();

    QFont font;

    QString fontName = firstArg;
    if (QFile::exists(fontName)) {
        int id = QFontDatabase::addApplicationFont(fontName);
        if (id == -1) {
            printf("cannot open font %s", qPrintable(fontName));
            help();
        }
        QStringList families = QFontDatabase::applicationFontFamilies(id);
        if (families.isEmpty()) {
            printf("cannot find any font families in %s", qPrintable(fontName));
            help();
        }
        fontName = families.first();
    }
    font.setFamily(fontName);

    bool ok = false;
    int pixelSize = arguments.at(2).toInt(&ok);
    if (!ok) help();
    font.setPixelSize(pixelSize);

    int generationOptions = QPF::IncludeCMap | QPF::RenderGlyphs;

    for (int i = 3; i < arguments.count(); ++i) {
        const QString &arg = arguments.at(i);
        if (arg == QLatin1String("italic")) {
            font.setItalic(true);
        } else if (arg == QLatin1String("bold")) {
            font.setBold(true);
        } else if (arg == QLatin1String("--exclude-cmap")) {
            generationOptions &= ~QPF::IncludeCMap;
        } else if (arg == QLatin1String("--exclude-glyphs")) {
            generationOptions &= ~QPF::RenderGlyphs;
        } else if (arg == QLatin1String("-v")) {
            ++QPF::debugVerbosity;
        } else {
            printf("unknown option %s\n", qPrintable(arg));
            help();
        }
    }

    font.setStyleStrategy(QFont::NoFontMerging);

    QList<QPF::CharacterRange> ranges;
    ranges.append(QPF::CharacterRange()); // default range from 0 to 0xffff

    QString origFont;
    QByteArray qpf = QPF::generate(font, generationOptions, ranges, &origFont);

    QString fileName = QPF::fileNameForFont(font);
    QFile f(fileName);
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    f.write(qpf);
    f.close();

    if (generationOptions & QPF::IncludeCMap) {
        printf("Created %s from %s\n", qPrintable(fileName), qPrintable(origFont));
    } else {
        printf("Created %s from %s excluding the character-map\n", qPrintable(fileName), qPrintable(origFont));
        printf("The TrueType font file is therefore required for the font to work\n");
    }

    return 0;
}

