/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFONTDATABASE_H
#define QFONTDATABASE_H

#include <QtGui/qwindowdefs.h>
#include <QtCore/qstring.h>
#include <QtGui/qfont.h>
#ifdef QT3_SUPPORT
#include <QtCore/qstringlist.h>
#include <QtCore/qlist.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QStringList;
template <class T> class QList;
struct QFontDef;
class QFontEngine;

class QFontDatabasePrivate;

class Q_GUI_EXPORT QFontDatabase
{
    Q_GADGET
    Q_ENUMS(WritingSystem)
public:
    // do not re-order or delete entries from this enum without updating the
    // QPF2 format and makeqpf!!
    enum WritingSystem {
        Any,

        Latin,
        Greek,
        Cyrillic,
        Armenian,
        Hebrew,
        Arabic,
        Syriac,
        Thaana,
        Devanagari,
        Bengali,
        Gurmukhi,
        Gujarati,
        Oriya,
        Tamil,
        Telugu,
        Kannada,
        Malayalam,
        Sinhala,
        Thai,
        Lao,
        Tibetan,
        Myanmar,
        Georgian,
        Khmer,
        SimplifiedChinese,
        TraditionalChinese,
        Japanese,
        Korean,
        Vietnamese,

        Symbol,
        Other = Symbol,

        Ogham,
        Runic,

        WritingSystemsCount
    };

    static QList<int> standardSizes();

    QFontDatabase();

    QList<WritingSystem> writingSystems() const;
    QList<WritingSystem> writingSystems(const QString &family) const;

    QStringList families(WritingSystem writingSystem = Any) const;
    QStringList styles(const QString &family) const;
    QList<int> pointSizes(const QString &family, const QString &style = QString());
    QList<int> smoothSizes(const QString &family, const QString &style);
    QString styleString(const QFont &font);
    QString styleString(const QFontInfo &fontInfo);

    QFont font(const QString &family, const QString &style, int pointSize) const;

    bool isBitmapScalable(const QString &family, const QString &style = QString()) const;
    bool isSmoothlyScalable(const QString &family, const QString &style = QString()) const;
    bool isScalable(const QString &family, const QString &style = QString()) const;
    bool isFixedPitch(const QString &family, const QString &style = QString()) const;

    bool italic(const QString &family, const QString &style) const;
    bool bold(const QString &family, const QString &style) const;
    int weight(const QString &family, const QString &style) const;

    static QString writingSystemName(WritingSystem writingSystem);
    static QString writingSystemSample(WritingSystem writingSystem);

    static int addApplicationFont(const QString &fileName);
    static int addApplicationFontFromData(const QByteArray &fontData);
    static QStringList applicationFontFamilies(int id);
    static bool removeApplicationFont(int id);
    static bool removeAllApplicationFonts();

    static bool supportsThreadedFontRendering();

private:
    static void createDatabase();
    static void parseFontName(const QString &name, QString &foundry, QString &family);
#if defined(Q_WS_QWS)
    static QFontEngine *findFont(int script, const QFontPrivate *fp, const QFontDef &request);
#endif
    static void load(const QFontPrivate *d, int script);
#ifdef Q_WS_X11
    static QFontEngine *loadXlfd(int screen, int script, const QFontDef &request, int force_encoding_id = -1);
#endif

    friend struct QFontDef;
    friend class QFontPrivate;
    friend class QFontDialog;
    friend class QFontEngineMultiXLFD;
    friend class QFontEngineMultiQWS;

    QFontDatabasePrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFONTDATABASE_H
