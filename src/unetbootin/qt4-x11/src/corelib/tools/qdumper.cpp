/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <qdatetime.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qmap.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qstring.h>
#include <qvariant.h>
#include <qvector.h>

#if !defined(Q_OS_WINCE) && !defined(QT_NO_DUMPER)

#include <stdlib.h>
#include <stdio.h>

#ifdef Q_OS_WIN
#   include <windows.h>
#endif

QT_BEGIN_NAMESPACE

namespace {

// This is used to abort evaluation of custom data dumpers in a "coordinated"
// way. Abortion will happen anyway when we try to access a non-initialized
// non-trivial object, so there is no way to prevent this from occuring at all
// conceptionally.  Gdb will catch SIGSEGV and return to the calling frame.
// This is just fine provided we only _read_ memory in the custom handlers
// below.

volatile int qProvokeSegFaultHelper;

static void qCheckAccess(const void *d)
{
    // provoke segfault when address is not readable
    qProvokeSegFaultHelper = *(char*)d;
}

static void qCheckPointer(const void *d)
{
    if (!d)
        return;
    qProvokeSegFaultHelper = *(char*)d;
}

static void qProvokeSegFault()
{
    // provoke segfault unconditionally
    qCheckAccess(0);
}

static char qDumpInBuffer[100];
static char qDumpBuffer[1000];
#ifdef Q_OS_WIN
static char qDumpBuffer2[sizeof(qDumpBuffer) + 100];
#endif

static char toHex(int n)
{
    return n < 10 ? '0' + n : 'a' - 10 + n;
}


struct QDumper
{
    explicit QDumper();
    ~QDumper();
    void flush();
    QDumper &operator<<(long c);
    QDumper &operator<<(int i);
    QDumper &operator<<(unsigned long c);
    QDumper &operator<<(unsigned int i);
    QDumper &operator<<(const void *p);
    void put(char c);
    void addCommaIfNeeded();
    void putEncoded(unsigned c);
    QDumper &operator<<(const char *str);
    QDumper &operator<<(const QString &str);
    void disarm();

    void beginHash(); // start of data hash output
    void endHash(); // start of data hash output

    // the dumper arguments
    int protocolVersion;   // dumper protocol version
    int token;             // some token to show on success
    const char *outertype; // object type
    const char *iname;     // object name used for display
    const char *exp;       // object expression
    const char *innertype; // 'inner type' for class templates
    const void *data;      // pointer to raw data
    bool dumpChildren;     // do we want to see children?

    // handling of nested templates
    void setupTemplateParameters();
    enum { maxTemplateParameters = 10 };
    const char *templateParameters[maxTemplateParameters + 1];
    int templateParametersCount;

    // internal state
    bool success;          // are we finished?
    size_t pos;
};


QDumper::QDumper()
{
    success = false;
    pos = 0;
}

QDumper::~QDumper()
{
    flush();
    put(0); // our end marker
#ifdef Q_OS_WIN
    sprintf(qDumpBuffer2, "@@CDD/%d/done\n", token);
    OutputDebugStringA(qDumpBuffer2);
#else
    fprintf(stderr, "%d/done\n", token);
#endif
    qDumpInBuffer[0] = 0;
}

void QDumper::flush()
{
    qDumpBuffer[pos++] = 0;
#ifdef Q_OS_WIN
    sprintf(qDumpBuffer2, "@@CDD#%d#%d,%s\n", token, int(pos - 1), qDumpBuffer);
    OutputDebugStringA(qDumpBuffer2);
#else
    fprintf(stderr, "%d#%d,%s\n", token, int(pos - 1), qDumpBuffer);
#endif
    pos = 0;
}

void QDumper::setupTemplateParameters()
{
    char *s = const_cast<char *>(innertype);

    templateParametersCount = 1;
    templateParameters[0] = s;
    for (int i = 1; i != maxTemplateParameters + 1; ++i)
        templateParameters[i] = 0;

    while (*s) {
        while (*s && *s != '@')
            ++s;
        if (*s) {
            *s = '\0';
            ++s;
            templateParameters[templateParametersCount++] = s;
        }
    }
}

QDumper &QDumper::operator<<(unsigned long c)
{
    static char buf[100];
    sprintf(buf, "%lu", c);
    return (*this) << buf;
}

QDumper &QDumper::operator<<(unsigned int i)
{
    static char buf[100];
    sprintf(buf, "%u", i);
    return (*this) << buf;
}

QDumper &QDumper::operator<<(long c)
{
    static char buf[100];
    sprintf(buf, "%ld", c);
    return (*this) << buf;
}

QDumper &QDumper::operator<<(int i)
{
    static char buf[100];
    sprintf(buf, "%d", i);
    return (*this) << buf;
}

QDumper &QDumper::operator<<(const void *p)
{
    static char buf[100];
    sprintf(buf, "%p", p);
    // we get a '0x' prefix only on some implementations.
    // if it isn't there, write it out manually.
    if (buf[1] != 'x') {
        put('0');
        put('x');
    }
    return (*this) << buf;
}

void QDumper::put(char c)
{
    if (pos >= sizeof(qDumpBuffer) - 100)
        flush();
    qDumpBuffer[pos++] = c;
}

void QDumper::addCommaIfNeeded()
{
    if (pos == 0)
        return;
    if (qDumpBuffer[pos - 1] == '}' || qDumpBuffer[pos - 1] == '"')
        put(',');
}

void QDumper::putEncoded(unsigned c)
{
    if (c >= 32 && c <= 126 && c != '"' && c != '\\') {
        put(c);
    } else {
        put('\\');
        put('u');
        put(toHex((c >> 12) & 0xf));
        put(toHex((c >>  8) & 0xf));
        put(toHex((c >>  4) & 0xf));
        put(toHex( c        & 0xf));
    }
}

QDumper &QDumper::operator<<(const char *str)
{
    while (*str)
        put(*(str++));
    return *this;
}

QDumper &QDumper::operator<<(const QString &str)
{
    int n = str.size();
    if (n < 0) {
        qProvokeSegFault();
    } else {
        //(*this) << "[" << n << "]";
        if (n > 1000000)
            n = 1000000;
        //put(' ');
        put('\\');
        put('"');
        for (int i = 0; i != n; ++i)
            putEncoded(str[i].unicode());
        put('\\');
        put('"');
        if (n < str.size())
            (*this) << "<incomplete string>";
    }
    return *this;
}

void QDumper::disarm()
{
    flush();
    success = true;
}

void QDumper::beginHash()
{
    addCommaIfNeeded();
    put('{');
}

void QDumper::endHash()
{
    put('}');
}


//
// Some helpers to keep the dumper code short
//

// dump property=value pair
#undef P
#define P(dumper,name,value) \
    do { \
        dumper.addCommaIfNeeded(); \
        dumper << (name) << "=\"" << value << "\""; \
    } while (0)

// simple string property
#undef S
#define S(dumper, name, value) \
    dumper.beginHash(); \
    P(dumper, "name", name); \
    P(dumper, "value", value); \
    P(dumper, "type", "QString"); \
    P(dumper, "numchild", "0"); \
    dumper.endHash();

// simple integer property
#undef I
#define I(dumper, name, value) \
    dumper.beginHash(); \
    P(dumper, "name", name); \
    P(dumper, "value", value); \
    P(dumper, "type", "int"); \
    P(dumper, "numchild", "0"); \
    dumper.endHash();

// simple boolean property
#undef BL
#define BL(dumper, name, value) \
    dumper.beginHash(); \
    P(dumper, "name", name); \
    P(dumper, "value", (value ? "true" : "false")); \
    P(dumper, "type", "bool"); \
    P(dumper, "numchild", "0"); \
    dumper.endHash();

#undef TT
#define TT(type, value) \
    "<tr><td>" << type << "</td><td> : </td><td>" << value << "</td></tr>"

static void qDumpUnknown(QDumper &d)
{
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "<internal error>");
    P(d, "type", d.outertype);
    P(d, "numchild", "0");
    d.disarm();
}

static void qDumpQPropertyList(QDumper &d)
{
    const QObject *ob = (const QObject *)d.data;
    const QMetaObject *mo = ob->metaObject();
    P(d, "iname", d.iname);
    P(d, "addr", "<synthetic>");
    P(d, "type", "QObject");
    P(d, "numchild", mo->propertyCount());
    if (d.dumpChildren) {
        d << ",children=[";
        for (int i = mo->propertyCount(); --i >= 0; ) {
            const QMetaProperty & prop = mo->property(i);
            d.beginHash();
            P(d, "name", prop.name());
            if (QLatin1String(prop.typeName()) == QLatin1String("QString")) {
                P(d, "value", prop.read(ob).toString());
                P(d, "numchild", "0");
            } else if (QLatin1String(prop.typeName()) == QLatin1String("bool")) {
                P(d, "value", (prop.read(ob).toBool() ? "true" : "false"));
                P(d, "numchild", "0");
            } else if (QLatin1String(prop.typeName()) == QLatin1String("int")) {
                P(d, "value", prop.read(ob).toInt());
                P(d, "numchild", "0");
            } else {
                P(d, "exp", "((" << mo->className() << "*)" << ob
                            << ")->" << prop.name() << "()");
            }
            P(d, "type", prop.typeName());
            P(d, "numchild", "1");
            d.endHash();
        }
        d << "]";
    }
    d.disarm();
}

static void qDumpQObject(QDumper &d)
{
    const QObject *ob = reinterpret_cast<const QObject *>(d.data);
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", (void*)d.data);
    P(d, "type", "QObject");
    P(d, "numchild", 4);
    if (d.dumpChildren) {
        const QMetaObject *mo = ob->metaObject();
        const QObjectList &children = ob->children();
        d << ",children=[";
        S(d, "objectName", ob->objectName());
        d.beginHash();
            P(d, "name", "properties");
            // FIXME: Note that when simply using '(QObject*)'
            // in the cast below, Gdb/MI _sometimes misparses
            // expressions further down in the tree.
            P(d, "exp", "*(class QObject*)" << d.data);
            P(d, "type", "QPropertyList");
            P(d, "value", "<" << mo->propertyCount() << " items>");
            P(d, "numchild", mo->propertyCount());
        d.endHash();
        d.beginHash();
            P(d, "name", "children");
            P(d, "exp", "((class QObject*)" << d.data << ")->children()");
            P(d, "type", "QList<QObject *>");
            P(d, "value", "<" << children.size() << " items>");
            P(d, "numchild", children.size());
        d.endHash();
        d.beginHash();
            P(d, "name", "parent");
            P(d, "exp", "((class QObject*)" << d.data << ")->parent()");
            P(d, "type", "QObject *");
            P(d, "numchild", (ob->parent() ? "1" : "0"));
        d.endHash();
        d << "]";
    }
    d.disarm();
}

static void qDumpQDir(QDumper &d)
{
    const QDir &dir = *reinterpret_cast<const QDir *>(d.data);
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", dir.path());
    P(d, "type", "QDir");
    P(d, "numchild", "3");
    if (d.dumpChildren) {
        d << ",children=[";
        S(d, "absolutePath", dir.absolutePath());
        S(d, "canonicalPath", dir.canonicalPath());
        d << "]";
    }
    d.disarm();
}

static void qDumpQFileInfo(QDumper &d)
{
    const QFileInfo &info = *reinterpret_cast<const QFileInfo *>(d.data);
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", info.filePath());
    P(d, "type", "QDir");
    P(d, "numchild", "3");
    if (d.dumpChildren) {
        d << ",children=[";
        S(d, "absolutePath", info.absolutePath());
        S(d, "absoluteFilePath", info.absoluteFilePath());
        S(d, "canonicalPath", info.canonicalPath());
        S(d, "canonicalFilePath", info.canonicalFilePath());
        S(d, "completeBaseName", info.completeBaseName());
        S(d, "completeSuffix", info.completeSuffix());
        S(d, "baseName", info.baseName());
#ifdef Q_OS_MACX
        BL(d, "isBundle", info.isBundle());
        S(d, "bundleName", info.bundleName());
#endif
        S(d, "completeSuffix", info.completeSuffix());
        S(d, "fileName", info.fileName());
        S(d, "filePath", info.filePath());
        S(d, "group", info.group());
        S(d, "owner", info.owner());
        S(d, "path", info.path());

        I(d, "groupid", (long)info.groupId());
        I(d, "ownerid", (long)info.ownerId());
        //QFile::Permissions permissions () const
        I(d, "permissions", info.permissions());

        //QDir absoluteDir () const
        //QDir dir () const

        BL(d, "caching", info.caching());
        BL(d, "exists", info.exists());
        BL(d, "isAbsolute", info.isAbsolute());
        BL(d, "isDir", info.isDir());
        BL(d, "isExecutable", info.isExecutable());
        BL(d, "isFile", info.isFile());
        BL(d, "isHidden", info.isHidden());
        BL(d, "isReadable", info.isReadable());
        BL(d, "isRelative", info.isRelative());
        BL(d, "isRoot", info.isRoot());
        BL(d, "isSymLink", info.isSymLink());
        BL(d, "isWritable", info.isWritable());

#ifndef QT_NO_DATESTRING
        d.beginHash();
        P(d, "name", "created");
        P(d, "value", info.created().toString());
        P(d, "exp", "((QFileInfo*)" << d.data << ")->created()");
        P(d, "type", "QDateTime");
        P(d, "numchild", "1");
        d.endHash();

        d.beginHash();
        P(d, "name", "lastModified");
        P(d, "value", info.lastModified().toString());
        P(d, "exp", "((QFileInfo*)" << d.data << ")->lastModified()");
        P(d, "type", "QDateTime");
        P(d, "numchild", "1");
        d.endHash();

        d.beginHash();
        P(d, "name", "lastRead");
        P(d, "value", info.lastRead().toString());
        P(d, "exp", "((QFileInfo*)" << d.data << ")->lastRead()");
        P(d, "type", "QDateTime");
        P(d, "numchild", "1");
        d.endHash();
#endif

        d << "]";
    }
    d.disarm();
}

static void qDumpQDateTime(QDumper &d)
{
#ifdef QT_NO_DATESTRING
    qDumpUnknown(d);
#else
    const QDateTime &date = *reinterpret_cast<const QDateTime *>(d.data);
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", date.toString());
    P(d, "type", "QDateTime");
    P(d, "numchild", "3");
    if (d.dumpChildren) {
        d << ",children=[";
        BL(d, "isNull", date.isNull());
        I(d, "toTime_t", (long)date.toTime_t());
        S(d, "toString", date.toString());
        S(d, "toString_(ISO)", date.toString(Qt::ISODate));
        S(d, "toString_(SystemLocale)", date.toString(Qt::SystemLocaleDate));
        S(d, "toString_(Locale)", date.toString(Qt::LocaleDate));
        S(d, "toString", date.toString());

        d.beginHash();
        P(d, "name", "toUTC");
        P(d, "exp", "((QDateTime*)" << d.data << ")->toTimeSpec(Qt::UTC)");
        P(d, "type", "QDateTime");
        P(d, "numchild", "1");
        d.endHash();

        d.beginHash();
        P(d, "name", "toLocalTime");
        P(d, "exp", "((QDateTime*)" << d.data << ")->toTimeSpec(Qt::LocalTime)");
        P(d, "type", "QDateTime");
        P(d, "numchild", "1");
        d.endHash();

        d << "]";
    }
    d.disarm();
#endif // ifdef QT_NO_DATESTRING
}

static void qDumpQString(QDumper &d)
{
    const QString &str = *reinterpret_cast<const QString *>(d.data);

    // Try to provoke segfaults early to prevent the frontend
    // from asking for unavailable child details
    if (!str.isEmpty()) {
        volatile ushort dummy = 0;
        dummy += str.at(0).unicode();
        dummy += str.at(str.size() - 1).unicode();
    }

    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", str);
    P(d, "type", "QString");
    P(d, "numchild", "0");
    d.disarm();
}

static void qDumpQStringList(QDumper &d)
{
    const QStringList &list = *reinterpret_cast<const QStringList *>(d.data);
    int n = list.size();
    if (n < 0)
        qProvokeSegFault();
    if (n > 0) {
        qCheckAccess(&list.front());
        qCheckAccess(&list.back());
    }

    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "<" << n << " items>");
    P(d, "valuedisabled", "true");
    P(d, "numchild", n);
    if (d.dumpChildren) {
        if (n > 100)
            n = 100;
        d << ",children=[";
        for (int i = 0; i != n; ++i) {
            S(d, "[" << i << "]", list[i]);
        }
        if (n < list.size()) {
            d.beginHash();
            P(d, "value", "<incomplete>");
            P(d, "type", " ");
            P(d, "numchild", "0");
            d.endHash();
        }
        d << "]";
    }
    d.disarm();
}

static void qDumpQVariantHelper(const void *data, QString *value,
    QString *exp, int *numchild)
{
    const QVariant &v = *reinterpret_cast<const QVariant *>(data);
    switch (v.type()) {
    case QVariant::Invalid:
        *value = QLatin1String("<invalid>");
        *numchild = 0;
        break;
    case QVariant::String:
        *value = QLatin1Char('"') + v.toString() + QLatin1Char('"');
        *numchild = 0;
        break;
    case QVariant::StringList:
        *exp = QString(QLatin1String("((QVariant*)%1)->d.data.c"))
                    .arg((qulonglong)data);
        *numchild = v.toStringList().size();
        break;
    case QVariant::Int:
        *value = QString::number(v.toInt());
        *numchild= 0;
        break;
    case QVariant::Double:
        *value = QString::number(v.toDouble());
        *numchild = 0;
        break;
    default:
        // FIXME
        //*exp = QString("qVariantValue<" << v.typeName() << ">"
        //             << "(*(QVariant*)" << data << ")");
        break;
    }
}

static void qDumpQVariant(QDumper &d)
{
    const QVariant &v = *reinterpret_cast<const QVariant *>(d.data);
    QString value;
    QString exp;
    int numchild = 0;
    qDumpQVariantHelper(d.data, &value, &exp, &numchild);
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "(" << v.typeName() << ") " << qPrintable(value));
    P(d, "type", "QVariant");
    P(d, "numchild", 1);
    if (d.dumpChildren) {
        d << ",children=[";
        d.beginHash();
        P(d, "name", "value");
        if (!exp.isEmpty())
            P(d, "exp", qPrintable(exp));
        if (!value.isEmpty())
            P(d, "value", qPrintable(value));
        P(d, "type", v.typeName());
        P(d, "numchild", numchild);
        d.endHash();
        d << "]";
    }
    d.disarm();
}

static void qDumpQList(QDumper &d)
{
    // This uses the knowledge that QList<T> has only a single member
    // of type  union { QListData p; QListData::Data *d; };
    const QListData &ldata = *reinterpret_cast<const QListData*>(d.data);
    const QListData::Data *pdata = *reinterpret_cast<const QListData::Data* const*>(d.data);
    int nn = ldata.size();
    if (nn < 0)
        qProvokeSegFault();
    if (nn > 0) {
        qCheckAccess(ldata.d->array);
        //qCheckAccess(ldata.d->array[0]);
        //qCheckAccess(ldata.d->array[nn - 1]);
    }

    int n = nn;
    P(d, "iname", d.iname);
    P(d, "value", "<" << n << " items>");
    P(d, "valuedisabled", "true");
    P(d, "numchild", n);
    if (d.dumpChildren) {
        if (n > 100)
            n = 100;
        d << ",children=[";
        for (int i = 0; i != n; ++i) {
            d.beginHash();
            P(d, "name", "[" << i << "]");
            // The exact condition here is:
            //  QTypeInfo<T>::isLarge || QTypeInfo<T>::isStatic
            // but this data is not available in the compiled binary.
            // So as first approximation only do the 'isLarge' check:
            void *p = &(ldata.d->array[i + pdata->begin]);
            unsigned long voidpsize = sizeof(void*);
            P(d, "exp", "(sizeof(" << d.innertype << ")>" << voidpsize <<
                "?(**(" << d.innertype << "**)(" << p << "))" 
                ":(*(" << d.innertype << "*)(" << p << ")))");
            P(d, "type", d.innertype);
            d.endHash();
        }
        if (n < nn) {
            d << ",{";
            P(d, "value", "<incomplete>");
            d.endHash();
        }
        d << "]";
    }
    d.disarm();
}

static void qDumpQVector(QDumper &d)
{
    // Use 'int' as representative value. No way (and no need)
    // to deduce proper type here.
    const QVector<int> &vec = *reinterpret_cast<const QVector<int> *>(d.data);
    const int nn = vec.size();

    // Try to provoke segfaults early to prevent the frontend
    // from asking for unavailable child details
    if (nn < 0)
        qProvokeSegFault();
    if (nn > 0) {
        qCheckAccess(&vec.front());
        qCheckAccess(&vec.back());
    }

    //int innersize = 0;
    //scanf(qDumpInBuffer, "%d", &innersize);
    
    int n = nn;
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "<" << n << " items>");
    P(d, "valuedisabled", "true");
    P(d, "numchild", n);
    if (d.dumpChildren) {
        if (n > 100)
            n = 100;
        d << ",children=[";
        for (int i = 0; i != n; ++i) {
            if (i)
                d << ",";
            d.beginHash();
            P(d, "name", "[" << i << "]");
            P(d, "exp", "(" << d.exp << ".d->array[" << i << "])");
            P(d, "type", d.innertype);
            d.endHash();
        }
        if (n < nn) {
            d << ",{";
            P(d, "value", "<incomplete>");
            d.endHash();
        }
        d << "]";
    }
    d.disarm();
}

static void qDumpQHashNode(QDumper &d)
{
    struct NodeOS { void *next; uint k; uint  v; } nodeOS; // int-key optimization, small value
    struct NodeOL { void *next; uint k; void *v; } nodeOL; // int-key optimiatzion, large value
    struct NodeNS { void *next; uint h; uint  k; uint  v; } nodeNS; // no optimization, small value
    struct NodeNL { void *next; uint h; uint  k; void *v; } nodeNL; // no optimization, large value
    struct NodeL  { void *next; uint h; void *k; void *v; } nodeL; // complex key

    // offsetof(...,...) not yet in Standard C++
    const ulong nodeOSk ( (char *)&nodeOS.k - (char *)&nodeOS );
    const ulong nodeOSv ( (char *)&nodeOS.v - (char *)&nodeOS );
    const ulong nodeOLk ( (char *)&nodeOL.k - (char *)&nodeOL );
    const ulong nodeOLv ( (char *)&nodeOL.v - (char *)&nodeOL );
    const ulong nodeNSk ( (char *)&nodeNS.k - (char *)&nodeNS );
    const ulong nodeNSv ( (char *)&nodeNS.v - (char *)&nodeNS );
    const ulong nodeNLk ( (char *)&nodeNL.k - (char *)&nodeNL );
    const ulong nodeNLv ( (char *)&nodeNL.v - (char *)&nodeNL );
    const ulong nodeLk  ( (char *)&nodeL.k  - (char *)&nodeL );
    const ulong nodeLv  ( (char *)&nodeL.v  - (char *)&nodeL );

    const QHashData *h = reinterpret_cast<const QHashData *>(d.data);
    const char *keyType   = d.templateParameters[0];
    const char *valueType = d.templateParameters[1];

    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "");
    P(d, "numchild", 2);
    if (d.dumpChildren) {
        // there is a hash specialization in cast the key are integers or shorts
        bool isOptimizedIntKey = qstrcmp(keyType, "int") == 0
#if defined(Q_BYTE_ORDER) && Q_BYTE_ORDER == Q_LITTLE_ENDIAN
            || qstrcmp(keyType, "short") == 0
            || qstrcmp(keyType, "ushort") == 0
#endif
            || qstrcmp(keyType, "uint") == 0;

        d << ",children=[";
        d.beginHash();
        P(d, "name", "key");
        P(d, "type", keyType);
        unsigned long intsize = sizeof(int);
        if (isOptimizedIntKey) {
            P(d, "exp", "*(" << keyType << "*)"
                "(((sizeof(" << valueType << ")>" << intsize << ")?"
              << nodeOLk << ":" << nodeOSk <<
                ")+(char*)" << h << ")");
        } else {
            P(d, "exp", "*(" << keyType << "*)"
              "(((sizeof(" << keyType << ")>" << intsize << ")?"
                << nodeLk << ":"
                    "((sizeof(" << valueType << ")>" << intsize << ")?"
              << nodeNLk << ":" << nodeNSk << "))+(char*)" << h << ")");
        }
        d.endHash();
        d.beginHash();
        P(d, "name", "value");
        P(d, "type", valueType);
        if (isOptimizedIntKey) {
            P(d, "exp", "*(" << valueType << "*)"
                "(((sizeof(" << valueType << ")>" << intsize << ")?"
              << nodeOLv << ":" << nodeOSv << ")+(char*)" << h << ")");
        } else {
            P(d, "exp", "*(" << valueType << "*)"
              "(((sizeof(" << keyType << ")>" << intsize << ")?" << nodeLv << ":"
                    "((sizeof(" << valueType << ")>" << intsize << ")?"
              << nodeNLv << ":" << nodeNSv << "))+(char*)" << h << ")");
        }
        d.endHash();
        d << "]";
    }
    d.disarm();
}

static void qDumpQHash(QDumper &d)
{
    QHashData *h = *reinterpret_cast<QHashData *const*>(d.data);
    const char *keyType   = d.templateParameters[0];
    const char *valueType = d.templateParameters[1];

    qCheckPointer(h->fakeNext);
    qCheckPointer(h->buckets);

    int n = h->size;

    if (n < 0)
        qProvokeSegFault();
    if (n > 0) {
        qCheckPointer(h->fakeNext);
        qCheckPointer(*h->buckets);
    }
    
    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "<" << n << " items>");
    P(d, "numchild", n);
    if (d.dumpChildren) {
        if (n > 100)
            n = 100;
        d << ",children=[";

        QHashData::Node *node = h->firstNode();
        QHashData::Node *end = reinterpret_cast<QHashData::Node *>(h);
        int i = 0;

        while (node != end) {
            d.beginHash();
                P(d, "name", "[" << i << "]");
                P(d, "type", "QHashNode<" << keyType << "," << valueType << " >");
                P(d, "exp", "*(QHashNode<" << keyType << "," << valueType << " >*)" << node);
            d.endHash();

            ++i;
            node = QHashData::nextNode(node);
        }
        d << "]";
    }
    d.disarm();
}

static void qDumpQMapNode(QDumper &d)
{
    const QMapData *h = reinterpret_cast<const QMapData *>(d.data);
    const char *keyType   = d.templateParameters[0];
    const char *valueType = d.templateParameters[1];
 
    qCheckAccess(h->backward);
    qCheckAccess(h->forward[0]);

    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "");
    P(d, "numchild", 2);
    if (d.dumpChildren) {
        unsigned long voidpsize = sizeof(void*);
        d << ",children=[";
        d.beginHash();
        P(d, "name", "key");
        P(d, "type", keyType);
        P(d, "exp", "*(" << keyType << "*)"
            << "("
            << 2 * voidpsize
            << "-sizeof('QMap<" << keyType << "," << valueType << ">::Node')"
            << "+(char*)" << h
            << ")");
        d.endHash();
        d.beginHash();
        P(d, "name", "value");
        P(d, "type", valueType);
        P(d, "exp", "*(" << valueType << "*)"
            << "("
            << "(size_t)&(('QMap<" << keyType << "," << valueType << ">::Node'*)0)->value"
            << "+" << 2 * voidpsize
            << "-sizeof('QMap<" << keyType << "," << valueType << ">::Node')"
            << "+(char*)" << h
            << ")");
        d.endHash();
        d << "]";
    }

    d.disarm();
}

static void qDumpQMap(QDumper &d)
{ 
    QMapData *h = *reinterpret_cast<QMapData *const*>(d.data);
    const char *keyType   = d.templateParameters[0];
    const char *valueType = d.templateParameters[1];

    int n = h->size;

    if (n < 0)
        qProvokeSegFault();
    if (n > 0) {
        qCheckAccess(h->backward);
        qCheckAccess(h->forward[0]);
        qCheckPointer(h->backward->backward);
        qCheckPointer(h->forward[0]->backward);
    }

    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "<" << n << " items>");
    P(d, "numchild", n);
    if (d.dumpChildren) {
        if (n > 100)
            n = 100;
        d << ",children=[";

        QMapData::Node *node = reinterpret_cast<QMapData::Node *>(h->forward[0]);
        QMapData::Node *end = reinterpret_cast<QMapData::Node *>(h);
        int i = 0;

        while (node != end) {
            d.beginHash();
                P(d, "name", "[" << i << "]");
                P(d, "type", "QMap<" << keyType << "," << valueType << ">::Node");
                P(d, "exp", "*('QMap<" << keyType << "," << valueType << ">::Node'*)" << node);
            d.endHash();

            ++i;
            node = node->forward[0];
        }
        d << "]";
    }

    d.disarm();
}

static void qDumpQSet(QDumper &d)
{
    // This uses the knowledge that QHash<T> has only a single member
    // of  union { QHashData *d; QHashNode<Key, T> *e; };
    QHashData *hd = *(QHashData**)d.data;
    QHashData::Node *node = hd->firstNode();

    int n = hd->size;
    if (n < 0)
        qProvokeSegFault();
    if (n > 0) {
        qCheckAccess(node);
        qCheckPointer(node->next);
    }

    P(d, "iname", d.iname);
    P(d, "addr", d.data);
    P(d, "value", "<" << n << " items>");
    P(d, "valuedisabled", "true");
    P(d, "numchild", 2 * n);
    if (d.dumpChildren) {
        if (n > 100)
            n = 100;
        d << ",children=[";
        int i = 0;
        for (int bucket = 0; bucket != hd->numBuckets; ++bucket) {
            for (node = hd->buckets[bucket]; node->next; node = node->next) {
                d.beginHash();
                P(d, "name", "[" << i << "]");
                P(d, "type", d.innertype);
                P(d, "exp", "(('QHashNode<" << d.innertype
                    << ",QHashDummyValue>'*)"
                    << static_cast<const void*>(node) << ")->key"
                );
                d.endHash();
                ++i;
            }
        }
        d << "]";
    }
    d.disarm();
}

static void handleProtocolVersion2(QDumper & d)
{
    if (!d.outertype[0]) {
        qDumpUnknown(d);
        return;
    }

    d.setupTemplateParameters();
    // d.outertype[0] is usally 'Q', so don't use it
    switch (d.outertype[1]) {
        case 'D':
            if (qstrcmp(d.outertype, "QDateTime") == 0)
                qDumpQDateTime(d);
            else if (qstrcmp(d.outertype, "QDir") == 0)
                qDumpQDir(d);
            break;
        case 'F':
            if (qstrcmp(d.outertype, "QFileInfo") == 0)
                qDumpQFileInfo(d);
            break;
        case 'H':
            if (qstrcmp(d.outertype, "QHash") == 0)
                qDumpQHash(d);
            else if (qstrcmp(d.outertype, "QHashNode") == 0)
                qDumpQHashNode(d);
            break;
        case 'L':
            if (qstrcmp(d.outertype, "QList") == 0)
                qDumpQList(d);
            break;
        case 'M':
            if (qstrcmp(d.outertype, "QMap") == 0)
                qDumpQMap(d);
            else if (qstrcmp(d.outertype, "QMap::Node") == 0)
                qDumpQMapNode(d);
            break;
        case 'O':
            if (qstrcmp(d.outertype, "QObject") == 0)
                qDumpQObject(d);
            break;
        case 'P':
            if (qstrcmp(d.outertype, "QPropertyList") == 0)
                qDumpQPropertyList(d);
            break;
        case 'S':
            if (qstrcmp(d.outertype, "QSet") == 0)
                qDumpQSet(d);
            else if (qstrcmp(d.outertype, "QString") == 0)
                qDumpQString(d);
            else if (qstrcmp(d.outertype, "QStringList") == 0)
                qDumpQStringList(d);
            break;
        case 'V':
            if (qstrcmp(d.outertype, "QVariant") == 0)
                qDumpQVariant(d);
            else if (qstrcmp(d.outertype, "QVector") == 0)
                qDumpQVector(d);
            break;
    }

    if (!d.success)
        qDumpUnknown(d);
}

} // anonymous namespace


extern "C" Q_CORE_EXPORT void qDumpObjectData(
    int protocolVersion,
    int token,
    const char *outertype,
    const char *iname,
    const char *exp,
    const char *innertype,
    const void *data,
    bool dumpChildren)
{
    if (protocolVersion == 1) {
        // used to test whether error output gets through
        //fprintf(stderr, "using stderr, qDebug follows: %d\n", token);
        //qDebug() << "using qDebug, stderr already used: " << token;
    }

    else if (protocolVersion == 2) {
        QDumper d;
        d.protocolVersion = protocolVersion;
        d.token           = token;
        d.outertype       = outertype ? outertype : "";
        d.iname           = iname ? iname : "";
        d.exp             = exp ? exp : "";
        d.innertype       = innertype ? innertype : "";
        d.data            = data ? data : "";
        d.dumpChildren    = dumpChildren;
        handleProtocolVersion2(d);
    }

    else {
        qDebug() << "Unsupported protocol version" << protocolVersion;
    }
}

QT_END_NAMESPACE

#endif // !Q_OS_WINCE && !QT_NO_QDUMPER
