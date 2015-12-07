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

#ifndef QSTRINGLIST_H
#define QSTRINGLIST_H

#include <QtCore/qalgorithms.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qlist.h>
#include <QtCore/qregexp.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringmatcher.h>
#ifdef QT_INCLUDE_COMPAT
#include <Qt3Support/q3valuelist.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QRegExp;

typedef QListIterator<QString> QStringListIterator;
typedef QMutableListIterator<QString> QMutableStringListIterator;

class QStringList : public QList<QString>
{
public:
    inline QStringList() { }
    inline explicit QStringList(const QString &i) { append(i); }
    inline QStringList(const QStringList &l) : QList<QString>(l) { }
    inline QStringList(const QList<QString> &l) : QList<QString>(l) { }

    inline void sort();

    inline QString join(const QString &sep) const;

    inline QStringList filter(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline QBool contains(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline QStringList &replaceInStrings(const QString &before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    inline QStringList operator+(const QStringList &other) const
    { QStringList n = *this; n += other; return n; }
    inline QStringList &operator<<(const QString &str)
    { append(str); return *this; }
    inline QStringList &operator<<(const QStringList &l)
    { *this += l; return *this; }

#ifndef QT_NO_REGEXP
    inline QStringList filter(const QRegExp &rx) const;
    inline QStringList &replaceInStrings(const QRegExp &rx, const QString &after);
    inline int indexOf(const QRegExp &rx, int from = 0) const;
    inline int lastIndexOf(const QRegExp &rx, int from = -1) const;
#endif
#if !defined(Q_NO_USING_KEYWORD)
    using QList<QString>::indexOf;
    using QList<QString>::lastIndexOf;
#else
    inline int indexOf(const QString &str, int from = 0) const
    { return QList<QString>::indexOf(str, from); }
    inline int lastIndexOf(const QString &str, int from = -1) const
    { return QList<QString>::lastIndexOf(str, from); }
#endif
#ifdef QT3_SUPPORT
    static inline QT3_SUPPORT QStringList split(const QString &sep, const QString &str, bool allowEmptyEntries = false);
    static inline QT3_SUPPORT QStringList split(const QChar &sep, const QString &str, bool allowEmptyEntries = false);
    inline QT3_SUPPORT QStringList grep(const QString &str, bool cs = true) const
        { return filter(str, cs ? Qt::CaseSensitive : Qt::CaseInsensitive); }

#ifndef QT_NO_REGEXP
    static inline QT3_SUPPORT QStringList split(const QRegExp &sep, const QString &str, bool allowEmptyEntries = false);
    inline QT3_SUPPORT QStringList grep(const QRegExp &rx) const { return filter(rx); }
    inline QT3_SUPPORT QStringList &gres(const QRegExp &rx, const QString &after)
        { return replaceInStrings(rx, after); }
#endif
    inline QT3_SUPPORT QStringList &gres(const QString &before, const QString &after, bool cs = true)
        { return replaceInStrings(before, after, cs ? Qt::CaseSensitive : Qt::CaseInsensitive); }

    inline Iterator QT3_SUPPORT fromLast() { return (isEmpty() ? end() : --end()); }
    inline ConstIterator QT3_SUPPORT fromLast() const { return (isEmpty() ? end() : --end()); }
#endif
};

namespace QtPrivate {
    void Q_CORE_EXPORT QStringList_sort(QStringList *that);
    QString Q_CORE_EXPORT QStringList_join(const QStringList *that, const QString &sep);
    QStringList Q_CORE_EXPORT QStringList_filter(const QStringList *that, const QString &str,
                                               Qt::CaseSensitivity cs);

    QBool Q_CORE_EXPORT QStringList_contains(const QStringList *that, const QString &str, Qt::CaseSensitivity cs);
    void Q_CORE_EXPORT QStringList_replaceInStrings(QStringList *that, const QString &before, const QString &after,
                                      Qt::CaseSensitivity cs);

#ifndef QT_NO_REGEXP
    void Q_CORE_EXPORT QStringList_replaceInStrings(QStringList *that, const QRegExp &rx, const QString &after);
    QStringList Q_CORE_EXPORT QStringList_filter(const QStringList *that, const QRegExp &re);
    int Q_CORE_EXPORT QStringList_indexOf(const QStringList *that, const QRegExp &rx, int from);
    int Q_CORE_EXPORT QStringList_lastIndexOf(const QStringList *that, const QRegExp &rx, int from);
#endif
}

inline void QStringList::sort()
{
    QtPrivate::QStringList_sort(this);
}

inline QString QStringList::join(const QString &sep) const
{
    return QtPrivate::QStringList_join(this, sep);
}

inline QStringList QStringList::filter(const QString &str, Qt::CaseSensitivity cs) const
{
    return QtPrivate::QStringList_filter(this, str, cs);
}

inline QBool QStringList::contains(const QString &str, Qt::CaseSensitivity cs) const
{
    return QtPrivate::QStringList_contains(this, str, cs);
}

inline QStringList &QStringList::replaceInStrings(const QString &before, const QString &after, Qt::CaseSensitivity cs)
{
    QtPrivate::QStringList_replaceInStrings(this, before, after, cs);
    return *this;
}

#ifndef QT_NO_REGEXP
inline QStringList &QStringList::replaceInStrings(const QRegExp &rx, const QString &after)
{
    QtPrivate::QStringList_replaceInStrings(this, rx, after);
    return *this;
}

inline QStringList QStringList::filter(const QRegExp &rx) const
{
    return QtPrivate::QStringList_filter(this, rx);
}

inline int QStringList::indexOf(const QRegExp &rx, int from) const
{
    return QtPrivate::QStringList_indexOf(this, rx, from);
}

inline int QStringList::lastIndexOf(const QRegExp &rx, int from) const
{
    return QtPrivate::QStringList_lastIndexOf(this, rx, from);
}
#endif


#ifdef QT3_SUPPORT
inline QStringList QStringList::split(const QChar &sep, const QString &str, bool allowEmptyEntries)
{
    if (str.isEmpty())
        return QStringList();
    return str.split(sep, allowEmptyEntries ? QString::KeepEmptyParts
                                            : QString::SkipEmptyParts);
}

inline QStringList QStringList::split(const QString &sep, const QString &str, bool allowEmptyEntries)
{
    if (str.isEmpty())
        return QStringList();
    return str.split(sep, allowEmptyEntries ? QString::KeepEmptyParts
                                            : QString::SkipEmptyParts);
}

#ifndef QT_NO_REGEXP
inline QStringList QStringList::split(const QRegExp &sep, const QString &str, bool allowEmptyEntries)
{
    if (str.isEmpty())
        return QStringList();
    return str.split(sep, allowEmptyEntries ? QString::KeepEmptyParts
                                            : QString::SkipEmptyParts);
}
#endif // QT_NO_REGEXP

#endif // QT3_SUPPORT


#ifndef QT_NO_DATASTREAM
inline QDataStream &operator>>(QDataStream &in, QStringList &list)
{
    return operator>>(in, static_cast<QList<QString> &>(list));
}
inline QDataStream &operator<<(QDataStream &out, const QStringList &list)
{
    return operator<<(out, static_cast<const QList<QString> &>(list));
}
#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSTRINGLIST_H
