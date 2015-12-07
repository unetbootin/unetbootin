/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QREGEXP_H
#define QREGEXP_H

#ifndef QT_NO_REGEXP

#include <QtCore/qstring.h>
#ifdef QT3_SUPPORT
#include <new>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

struct QRegExpPrivate;
class QStringList;

class Q_CORE_EXPORT QRegExp
{
public:
    enum PatternSyntax { RegExp, Wildcard, FixedString, RegExp2 };
    enum CaretMode { CaretAtZero, CaretAtOffset, CaretWontMatch };

    QRegExp();
    explicit QRegExp(const QString &pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive,
		     PatternSyntax syntax = RegExp);
    QRegExp(const QRegExp &rx);
    ~QRegExp();
    QRegExp &operator=(const QRegExp &rx);

    bool operator==(const QRegExp &rx) const;
    inline bool operator!=(const QRegExp &rx) const { return !operator==(rx); }

    bool isEmpty() const;
    bool isValid() const;
    QString pattern() const;
    void setPattern(const QString &pattern);
    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity cs);
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT bool caseSensitive() const { return caseSensitivity() == Qt::CaseSensitive; }
    inline QT3_SUPPORT void setCaseSensitive(bool sensitive)
    { setCaseSensitivity(sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive); }
#endif
    PatternSyntax patternSyntax() const;
    void setPatternSyntax(PatternSyntax syntax);
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT bool wildcard() const { return patternSyntax() == Wildcard; }
    inline QT3_SUPPORT void setWildcard(bool aWildcard)
    { setPatternSyntax(aWildcard ? Wildcard : RegExp); }
#endif

    bool isMinimal() const;
    void setMinimal(bool minimal);
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT bool minimal() const { return isMinimal(); }
#endif

    bool exactMatch(const QString &str) const;

    int indexIn(const QString &str, int offset = 0, CaretMode caretMode = CaretAtZero) const;
    int lastIndexIn(const QString &str, int offset = -1, CaretMode caretMode = CaretAtZero) const;
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT int search(const QString &str, int from = 0,
                                CaretMode caretMode = CaretAtZero) const
    { return indexIn(str, from, caretMode); }
    inline QT3_SUPPORT int searchRev(const QString &str, int from = -1,
                                   CaretMode caretMode = CaretAtZero) const
    { return lastIndexIn(str, from, caretMode); }
#endif
    int matchedLength() const;
#ifndef QT_NO_REGEXP_CAPTURE
    int numCaptures() const;
    QStringList capturedTexts();
    QString cap(int nth = 0);
    int pos(int nth = 0);
    QString errorString();
#endif

    static QString escape(const QString &str);

#ifdef QT3_SUPPORT
    inline QT3_SUPPORT_CONSTRUCTOR QRegExp(const QString &aPattern, bool cs, bool aWildcard = false)
    {
        new (this)
            QRegExp(aPattern, cs ? Qt::CaseSensitive : Qt::CaseInsensitive,
                    aWildcard ? Wildcard : RegExp);
    }
#endif

private:
    QRegExpPrivate *priv;
};

Q_DECLARE_TYPEINFO(QRegExp, Q_MOVABLE_TYPE);

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out, const QRegExp &regExp);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in, QRegExp &regExp);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_REGEXP

#endif // QREGEXP_H
