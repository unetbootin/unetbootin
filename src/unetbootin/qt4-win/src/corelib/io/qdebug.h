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

#ifndef QDEBUG_H
#define QDEBUG_H

#include <QtCore/qalgorithms.h>
#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qstring.h>
#include <QtCore/qvector.h>
#include <QtCore/qset.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class Q_CORE_EXPORT QDebug
{
    struct Stream {
        Stream(QIODevice *device) : ts(device), ref(1), type(QtDebugMsg), space(true), message_output(false) {}
        Stream(QString *string) : ts(string, QIODevice::WriteOnly), ref(1), type(QtDebugMsg), space(true), message_output(false) {}
        Stream(QtMsgType t) : ts(&buffer, QIODevice::WriteOnly), ref(1), type(t), space(true), message_output(true) {}
        QTextStream ts;
        QString buffer;
        int ref;
        QtMsgType type;
        bool space;
        bool message_output;
    } *stream;
public:
    inline QDebug(QIODevice *device) : stream(new Stream(device)) {}
    inline QDebug(QString *string) : stream(new Stream(string)) {}
    inline QDebug(QtMsgType t) : stream(new Stream(t)) {}
    inline QDebug(const QDebug &o):stream(o.stream) { ++stream->ref; }
    inline QDebug &operator=(const QDebug &other);
    inline ~QDebug() {
        if (!--stream->ref) {
            if(stream->message_output)
                qt_message_output(stream->type, stream->buffer.toLocal8Bit().data());
            delete stream;
        }
    }
    inline QDebug &space() { stream->space = true; stream->ts << " "; return *this; }
    inline QDebug &nospace() { stream->space = false; return *this; }
    inline QDebug &maybeSpace() { if (stream->space) stream->ts << " "; return *this; }

    inline QDebug &operator<<(QChar t) { stream->ts << "\'" << t << "\'"; return maybeSpace(); }
    inline QDebug &operator<<(bool t) { stream->ts << (t ? "true" : "false"); return maybeSpace(); }
    inline QDebug &operator<<(char t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(signed short t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(unsigned short t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(signed int t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(unsigned int t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(signed long t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(unsigned long t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(qint64 t)
        { stream->ts << QString::number(t); return maybeSpace(); }
    inline QDebug &operator<<(quint64 t)
        { stream->ts << QString::number(t); return maybeSpace(); }
    inline QDebug &operator<<(float t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(double t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(const char* t) { stream->ts << QString::fromAscii(t); return maybeSpace(); }
    inline QDebug &operator<<(const QString & t) { stream->ts << "\"" << t  << "\""; return maybeSpace(); }
    inline QDebug &operator<<(const QLatin1String &t) { stream->ts << "\""  << t.latin1() << "\""; return maybeSpace(); }
    inline QDebug &operator<<(const QByteArray & t) { stream->ts  << "\"" << t << "\""; return maybeSpace(); }
    inline QDebug &operator<<(const void * t) { stream->ts << t; return maybeSpace(); }
    inline QDebug &operator<<(QTextStreamFunction f) {
        stream->ts << f;
        return *this;
    }

    inline QDebug &operator<<(QTextStreamManipulator m)
    { stream->ts << m; return *this; }
};

Q_CORE_EXPORT_INLINE QDebug qCritical() { return QDebug(QtCriticalMsg); }

inline QDebug &QDebug::operator=(const QDebug &other)
{
    if (this != &other) {
        QDebug copy(other);
        qSwap(stream, copy.stream);
    }
    return *this;
}

#if defined(FORCE_UREF)
template <class T>
inline QDebug &operator<<(QDebug debug, const QList<T> &list)
#else
template <class T>
inline QDebug operator<<(QDebug debug, const QList<T> &list)
#endif
{
    debug.nospace() << "(";
    for (Q_TYPENAME QList<T>::size_type i = 0; i < list.count(); ++i) {
        if (i)
            debug << ", ";
        debug << list.at(i);
    }
    debug << ")";
    return debug.space();
}

#if defined(FORCE_UREF)
template <typename T>
inline QDebug &operator<<(QDebug debug, const QVector<T> &vec)
#else
template <typename T>
inline QDebug operator<<(QDebug debug, const QVector<T> &vec)
#endif
{
    debug.nospace() << "QVector";
    return operator<<(debug, vec.toList());
}

#if defined(FORCE_UREF)
template <class aKey, class aT>
inline QDebug &operator<<(QDebug debug, const QMap<aKey, aT> &map)
#else
template <class aKey, class aT>
inline QDebug operator<<(QDebug debug, const QMap<aKey, aT> &map)
#endif
{
    debug.nospace() << "QMap(";
    for (typename QMap<aKey, aT>::const_iterator it = map.constBegin();
         it != map.constEnd(); ++it) {
        debug << "(" << it.key() << ", " << it.value() << ")";
    }
    debug << ")";
    return debug.space();
}

#if defined(FORCE_UREF)
template <class aKey, class aT>
inline QDebug &operator<<(QDebug debug, const QHash<aKey, aT> &hash)
#else
template <class aKey, class aT>
inline QDebug operator<<(QDebug debug, const QHash<aKey, aT> &hash)
#endif
{
    debug.nospace() << "QHash(";
    for (typename QHash<aKey, aT>::const_iterator it = hash.constBegin();
            it != hash.constEnd(); ++it)
        debug << "(" << it.key() << ", " << it.value() << ")";
    debug << ")";
    return debug.space();
}

#if defined(FORCE_UREF)
template <class T1, class T2>
inline QDebug &operator<<(QDebug debug, const QPair<T1, T2> &pair)
#else
template <class T1, class T2>
inline QDebug operator<<(QDebug debug, const QPair<T1, T2> &pair)
#endif
{
    debug.nospace() << "QPair(" << pair.first << "," << pair.second << ")";
    return debug.space();
}

template <typename T>
inline QDebug operator<<(QDebug debug, const QSet<T> &set)
{
    debug.nospace() << "QSet";
    return operator<<(debug, set.toList());
}

#if !defined(QT_NO_DEBUG_STREAM)
Q_CORE_EXPORT_INLINE QDebug qDebug() { return QDebug(QtDebugMsg); }

#else // QT_NO_DEBUG_STREAM

class QNoDebug
{
public:
    inline QNoDebug(){}
    inline QNoDebug(const QDebug &){}
    inline ~QNoDebug(){}
#if !defined( QT_NO_TEXTSTREAM )
    inline QNoDebug &operator<<(QTextStreamFunction) { return *this; }
    inline QNoDebug &operator<<(QTextStreamManipulator) { return *this; }
#endif
    inline QNoDebug &space() { return *this; }
    inline QNoDebug &nospace() { return *this; }
    inline QNoDebug &maybeSpace() { return *this; }

#ifndef QT_NO_MEMBER_TEMPLATES
    template<typename T>
    inline QNoDebug &operator<<(const T &) { return *this; }
#endif
};
#undef qDebug
inline QNoDebug qDebug() { return QNoDebug(); }
#define qDebug QT_NO_QDEBUG_MACRO

#ifdef QT_NO_MEMBER_TEMPLATES
template<typename T>
inline QNoDebug operator<<(QNoDebug debug, const T &) { return debug; }
#endif

#endif

#if !defined(QT_NO_WARNING_OUTPUT)
Q_CORE_EXPORT_INLINE QDebug qWarning() { return QDebug(QtWarningMsg); }
#else
#undef qWarning
inline QNoDebug qWarning() { return QNoDebug(); }
#define qWarning QT_NO_QWARNING_MACRO
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDEBUG_H
