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

#include "qplatformdefs.h"

#include "qtranslator.h"

#ifndef QT_NO_TRANSLATION

#include "qfileinfo.h"
#include "qstring.h"
#include "qcoreapplication.h"
#include "qcoreapplication_p.h"
#include "qdatastream.h"
#include "qfile.h"
#include "qmap.h"
#include "qalgorithms.h"
#include "qhash.h"
#include "qtranslator_p.h"

#if defined(Q_OS_UNIX)
#define QT_USE_MMAP
#endif

// most of the headers below are already included in qplatformdefs.h
// also this lacks Large File support but that's probably irrelevant
#if defined(QT_USE_MMAP)
// for mmap
#include <sys/mman.h>
#include <errno.h>
#endif

#include <stdlib.h>

#include "qobject_p.h"

QT_BEGIN_NAMESPACE

enum Tag { Tag_End = 1, Tag_SourceText16, Tag_Translation, Tag_Context16, Tag_Obsolete1,
           Tag_SourceText, Tag_Context, Tag_Comment, Tag_Obsolete2 };
/*
$ mcookie
3cb86418caef9c95cd211cbf60a1bddd
$
*/

// magic number for the file
static const int MagicLength = 16;
static const uchar magic[MagicLength] = {
    0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
    0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};

static bool match(const uchar* found, const char* target, uint len)
{
    // catch the case if \a found has a zero-terminating symbol and \a len includes it.
    // (normalize it to be without the zero-terminating symbol)
    if (len > 0 && found[len-1] == '\0')
        --len;
    // 0 means anything, "" means empty
    return !found || qstrncmp((const char *)found, target, len) == 0 && target[len] == '\0';
}

static uint elfHash(const char *name)
{
    const uchar *k;
    uint h = 0;
    uint g;

    if (name) {
        k = (const uchar *) name;
        while (*k) {
            h = (h << 4) + *k++;
            if ((g = (h & 0xf0000000)) != 0)
                h ^= g >> 24;
            h &= ~g;
        }
    }
    if (!h)
        h = 1;
    return h;
}

static int numerusHelper(int n, const uchar *rules, int rulesSize)
{
#define CHECK_RANGE \
    do { \
        if (i >= rulesSize) \
            return -1; \
    } while (0)

    int result = 0;
    int i = 0;

    if (rulesSize == 0)
        return 0;

    for (;;) {
        bool orExprTruthValue = false;

        for (;;) {
            bool andExprTruthValue = true;

            for (;;) {
                bool truthValue = true;

                CHECK_RANGE;
                int opcode = rules[i++];

                int leftOperand = n;
                if (opcode & MOD_10) {
                    leftOperand %= 10;
                } else if (opcode & MOD_100) {
                    leftOperand %= 100;
                }

                int op = opcode & OP_MASK;

                CHECK_RANGE;
                int rightOperand = rules[i++];

                switch (op) {
                default:
                    return -1;
                case EQ:
                    truthValue = (leftOperand == rightOperand);
                    break;
                case LT:
                    truthValue = (leftOperand < rightOperand);
                    break;
                case LEQ:
                    truthValue = (leftOperand <= rightOperand);
                    break;
		case BETWEEN:
                    int bottom = rightOperand;
                    CHECK_RANGE;
                    int top = rules[i++];
                    truthValue = (leftOperand >= bottom && leftOperand <= top);
                }

                if (opcode & NOT)
                    truthValue = !truthValue;

                andExprTruthValue = andExprTruthValue && truthValue;

                if (i == rulesSize || rules[i] != AND)
                    break;
                ++i;
            }

            orExprTruthValue = orExprTruthValue || andExprTruthValue;

            if (i == rulesSize || rules[i] != OR)
                break;
            ++i;
        }

        if (orExprTruthValue)
            return result;

        ++result;

        if (i == rulesSize)
            return result;

        if (rules[i++] != NEWRULE)
            break;
    }
    return -1;
}

extern bool qt_detectRTLLanguage();

class QTranslatorPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QTranslator)
public:
    enum { Contexts = 0x2f, Hashes = 0x42, Messages = 0x69, NumerusRules = 0x88 };

    QTranslatorPrivate()
        : used_mmap(0), unmapPointer(0), unmapLength(0), messageArray(0), offsetArray(0),
          contextArray(0), numerusRulesArray(0), messageLength(0), offsetLength(0),
          contextLength(0), numerusRulesLength(0) {}

    // for mmap'ed files, this is what needs to be unmapped.
    uint used_mmap : 1;
    char *unmapPointer;
    unsigned int unmapLength;

    // for squeezed but non-file data, this is what needs to be deleted
    const uchar *messageArray;
    const uchar *offsetArray;
    const uchar *contextArray;
    const uchar *numerusRulesArray;
    uint messageLength;
    uint offsetLength;
    uint contextLength;
    uint numerusRulesLength;

    bool do_load(const uchar *data, int len);
    QString do_translate(const char *context, const char *sourceText, const char *comment,
                         int n) const;
    void clear();
};

/*!
    \class QTranslator

    \brief The QTranslator class provides internationalization support for text
    output.

    \ingroup i18n
    \ingroup environment
    \mainclass

    An object of this class contains a set of translations from a
    source language to a target language. QTranslator provides
    functions to look up translations in a translation file.
    Translation files are created using \l{Qt Linguist}.

    The most common use of QTranslator is to: load a translation
    file, install it using QApplication::installTranslator(), and use
    it via QObject::tr(). Here's the \c main() function from the
    \l{linguist/hellotr}{Hello tr()} example:

    \snippet examples/linguist/hellotr/main.cpp 2

    Note that the translator must be created \e before the
    application's widgets.

    Most applications will never need to do anything else with this
    class. The other functions provided by this class are useful for
    applications that work on translator files.

    It is possible to lookup a translation using translate() (as tr()
    and QApplication::translate() do). The translate() function takes
    up to three parameters:

    \list
    \o The \e context - usually the class name for the tr() caller.
    \o The \e {source text} - usually the argument to tr().
    \o The \e comment - an optional comment that helps disambiguate
       different uses of the same text in the same context.
    \endlist

    For example, the "Cancel" in a dialog might have "Anuluj" when the
    program runs in Polish (in this case the source text would be
    "Cancel"). The context would (normally) be the dialog's class
    name; there would normally be no comment, and the translated text
    would be "Anuluj".

    But it's not always so simple. The Spanish version of a printer
    dialog with settings for two-sided printing and binding would
    probably require both "Activado" and "Activada" as translations
    for "Enabled". In this case the source text would be "Enabled" in
    both cases, and the context would be the dialog's class name, but
    the two items would have disambiguating comments such as
    "two-sided printing" for one and "binding" for the other. The
    comment enables the translator to choose the appropriate gender
    for the Spanish version, and enables Qt to distinguish between
    translations.

    \sa QApplication::installTranslator(), QApplication::removeTranslator(),
        QObject::tr(), QApplication::translate(), {I18N Example},
	{Hello tr() Example}, {Arrow Pad Example}, {Troll Print Example}
*/

/*!
    Constructs an empty message file object with parent \a parent that
    is not connected to any file.
*/

QTranslator::QTranslator(QObject * parent)
    : QObject(*new QTranslatorPrivate, parent)
{
}

#ifdef QT3_SUPPORT
/*!
    \overload
    \obsolete
 */
QTranslator::QTranslator(QObject * parent, const char * name)
    : QObject(*new QTranslatorPrivate, parent)
{
    setObjectName(QString::fromAscii(name));
}
#endif

/*!
    Destroys the object and frees any allocated resources.
*/

QTranslator::~QTranslator()
{
    if (QCoreApplication::instance())
        QCoreApplication::instance()->removeTranslator(this);
    Q_D(QTranslator);
    d->clear();
}

/*!
    Loads \a filename + \a suffix (".qm" if the \a suffix is
    not specified), which may be an absolute file name or relative
    to \a directory. Returns true if the translation is successfully
    loaded; otherwise returns false.

    The previous contents of this translator object are discarded.

    If the file name does not exist, other file names are tried
    in the following order:

    \list 1
    \o File name without \a suffix appended.
    \o File name with text after a character in \a search_delimiters
       stripped ("_." is the default for \a search_delimiters if it is
       an empty string) and \a suffix.
    \o File name stripped without \a suffix appended.
    \o File name stripped further, etc.
    \endlist

    For example, an application running in the fr_CA locale
    (French-speaking Canada) might call load("foo.fr_ca",
    "/opt/foolib"). load() would then try to open the first existing
    readable file from this list:

    \list 1
    \o \c /opt/foolib/foo.fr_ca.qm
    \o \c /opt/foolib/foo.fr_ca
    \o \c /opt/foolib/foo.fr.qm
    \o \c /opt/foolib/foo.fr
    \o \c /opt/foolib/foo.qm
    \o \c /opt/foolib/foo
    \endlist
*/

bool QTranslator::load(const QString & filename, const QString & directory,
                       const QString & search_delimiters,
                       const QString & suffix)
{
    Q_D(QTranslator);
    d->clear();

    QString prefix;
    if (QFileInfo(filename).isRelative()) {
        prefix = directory;
	if (prefix.length() && !prefix.endsWith(QLatin1Char('/')))
	    prefix += QLatin1Char('/');
    }

    QString fname = filename;
    QString realname;
    QString delims;
    delims = search_delimiters.isNull() ? QString::fromLatin1("_.") : search_delimiters;

    for (;;) {
        QFileInfo fi;

        realname = prefix + fname + (suffix.isNull() ? QString::fromLatin1(".qm") : suffix);
        fi.setFile(realname);
        if (fi.isReadable())
            break;

        realname = prefix + fname;
        fi.setFile(realname);
        if (fi.isReadable())
            break;

        int rightmost = 0;
        for (int i = 0; i < (int)delims.length(); i++) {
            int k = fname.lastIndexOf(delims[i]);
            if (k > rightmost)
                rightmost = k;
        }

        // no truncations? fail
        if (rightmost == 0)
            return false;

        fname.truncate(rightmost);
    }

    // realname is now the fully qualified name of a readable file.

    bool ok = false;

#ifdef QT_USE_MMAP

#ifndef MAP_FILE
#define MAP_FILE 0
#endif
#ifndef MAP_FAILED
#define MAP_FAILED -1
#endif

    int fd = -1;
    if (!realname.startsWith(QLatin1Char(':')))
        fd = QT_OPEN(QFile::encodeName(realname), O_RDONLY,
#if defined(Q_OS_WIN)
                     _S_IREAD | _S_IWRITE
#else
                     0666
#endif
            );
    if (fd >= 0) {
        QT_STATBUF st;
        if (!QT_FSTAT(fd, &st)) {
            char *ptr;
            ptr = reinterpret_cast<char *>(
                mmap(0, st.st_size,             // any address, whole file
                     PROT_READ,                 // read-only memory
                     MAP_FILE | MAP_PRIVATE,    // swap-backed map from file
                     fd, 0));                   // from offset 0 of fd
            if (ptr && ptr != reinterpret_cast<char *>(MAP_FAILED)) {
                d->used_mmap = true;
                d->unmapPointer = ptr;
                d->unmapLength = st.st_size;
                ok = true;
            }
        }
        ::close(fd);
    }
#endif // QT_USE_MMAP

    if (!ok) {
        QFile file(realname);
        d->unmapLength = file.size();
        if (!d->unmapLength)
            return false;
        d->unmapPointer = new char[d->unmapLength];

        if (file.open(QIODevice::ReadOnly))
            ok = (d->unmapLength == (uint)file.read(d->unmapPointer, d->unmapLength));

        if (!ok) {
            delete [] d->unmapPointer;
            d->unmapPointer = 0;
            d->unmapLength = 0;
            return false;
        }
    }

    return d->do_load(reinterpret_cast<const uchar *>(d->unmapPointer), d->unmapLength);
}

/*!
  \overload
  \fn bool QTranslator::load(const uchar *data, int len)

  Loads the .qm file data \a data of length \a len into the
  translator.

  The data is not copied. The caller must be able to guarantee that \a data
  will not be deleted or modified.
*/
bool QTranslator::load(const uchar *data, int len)
{
    Q_D(QTranslator);
    d->clear();
    return d->do_load(data, len);
}

static quint8 read8(const uchar *data)
{
    return *data;
}

static quint16 read16(const uchar *data)
{
    return (data[0] << 8) | (data[1]);
}

static quint32 read32(const uchar *data)
{
    return (data[0] << 24)
        | (data[1] << 16)
        | (data[2] << 8)
        | (data[3]);
}

bool QTranslatorPrivate::do_load(const uchar *data, int len)
{
    if (!data || len < MagicLength || memcmp(data, magic, MagicLength) != 0)
        return false;

    bool ok = true;
    const uchar *end = data + len;

    data += MagicLength;

    while (data < end - 4) {
        quint8 tag = read8(data++);
        quint32 blockLen = read32(data);
        data += 4;
        if (!tag || !blockLen)
            break;
        if (data + blockLen > end) {
            ok = false;
            break;
        }

        if (tag == QTranslatorPrivate::Contexts) {
            contextArray = data;
            contextLength = blockLen;
        } else if (tag == QTranslatorPrivate::Hashes) {
            offsetArray = data;
            offsetLength = blockLen;
        } else if (tag == QTranslatorPrivate::Messages) {
            messageArray = data;
            messageLength = blockLen;
        } else if (tag == QTranslatorPrivate::NumerusRules) {
            numerusRulesArray = data;
            numerusRulesLength = blockLen;
        }

        data += blockLen;
    }

    return ok;
}

static QString getMessage(const uchar *m, const uchar *end, const char *context,
                          const char *sourceText, const char *comment, int numerus)
{
    const uchar *tn = 0;
    uint tn_length = 0;
    int currentNumerus = -1;

    for (;;) {
        uchar tag = 0;
        if (m < end)
            tag = read8(m++);
        switch((Tag)tag) {
        case Tag_End:
            goto end;
        case Tag_Translation: {
            int len = read32(m);
            if (len % 1)
                return QString();
            m += 4;
            if (++currentNumerus == numerus) {
                tn_length = len;
                tn = m;
            }
            m += len;
            break;
        }
        case Tag_Obsolete1:
            m += 4;
            break;
        case Tag_SourceText: {
            quint32 len = read32(m);
            m += 4;
            if (!match(m, sourceText, len))
                return QString();
            m += len;
        }
            break;
        case Tag_Context: {
            quint32 len = read32(m);
            m += 4;
            if (!match(m, context, len))
                return QString();
            m += len;
        }
            break;
        case Tag_Comment: {
            quint32 len = read32(m);
            m += 4;
            if (*m && !match(m, comment, len))
                return QString();
            m += len;
        }
            break;
        default:
            return QString();
        }
    }
end:
    if (!tn)
        return QString();
    QString str = QString::fromUtf16((const ushort *)tn, tn_length/2);
    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
        for (int i = 0; i < str.length(); ++i)
            str[i] = QChar((str.at(i).unicode() >> 8) + ((str.at(i).unicode() << 8) & 0xff00));
    }
    return str;
}

QString QTranslatorPrivate::do_translate(const char *context, const char *sourceText,
                                         const char *comment, int n) const
{
    if (context == 0)
        context = "";
    if (sourceText == 0)
        sourceText = "";
    if (comment == 0)
        comment = "";

    if (!offsetLength)
        return QString();

    /*
        Check if the context belongs to this QTranslator. If many
        translators are installed, this step is necessary.
    */
    if (contextLength) {
        quint16 hTableSize = read16(contextArray);
        uint g = elfHash(context) % hTableSize;
        const uchar *c = contextArray + 2 + (g << 1);
        quint16 off = read16(c);
        c += 2;
        if (off == 0)
            return QString();
        c = contextArray + (2 + (hTableSize << 1) + (off << 1));

        for (;;) {
            quint8 len = read8(c++);
            if (len == 0)
                return QString();
            if (match(c, context, len))
                break;
            c += len;
        }
    }

    size_t numItems = offsetLength / (2 * sizeof(quint32));
    if (!numItems)
        return QString();

    int numerus = 0;
    if (n >= 0)
        numerus = numerusHelper(n, numerusRulesArray, numerusRulesLength);

    for (;;) {
        quint32 h = elfHash(QByteArray(sourceText) + comment);

        const uchar *start = offsetArray;
        const uchar *end = start + ((numItems-1) << 3);
        while (start <= end) {
            const uchar *middle = start + (((end - start) >> 4) << 3);
            uint hash = read32(middle);
            if (h == hash) {
                start = middle;
                break;
            } else if (hash < h) {
                start = middle + 8;
            } else {
                end = middle - 8;
            }
        }

        if (start <= end) {
            // go back on equal key
            while (start != offsetArray && read32(start) == read32(start-8))
                start -= 8;

            while (start < offsetArray + offsetLength) {
                quint32 rh = read32(start);
                start += 4;
                if (rh != h)
                    break;
                quint32 ro = read32(start);
                start += 4;
                QString tn = getMessage(messageArray + ro, messageArray + messageLength, context,
                                        sourceText, comment, numerus);
                if (!tn.isNull())
                    return tn;
            }
        }
        if (!comment[0])
            break;
        comment = "";
    }
    return QString();
}

/*!
    Empties this translator of all contents.

    This function works with stripped translator files.
*/

void QTranslatorPrivate::clear()
{
    Q_Q(QTranslator);
    if (unmapPointer && unmapLength) {
#if defined(QT_USE_MMAP)
        if (used_mmap)
            munmap(unmapPointer, unmapLength);
        else
#endif
            delete [] unmapPointer;
    }

    unmapPointer = 0;
    unmapLength = 0;
    messageArray = 0;
    contextArray = 0;
    offsetArray = 0;
    numerusRulesArray = 0;
    messageLength = 0;
    contextLength = 0;
    offsetLength = 0;
    numerusRulesLength = 0;

    if (QCoreApplicationPrivate::isTranslatorInstalled(q))
        QCoreApplication::postEvent(QCoreApplication::instance(),
                                    new QEvent(QEvent::LanguageChange));
}

/*!
    Returns the translation for the key (\a context, \a sourceText,
    \a comment). If none is found, also tries (\a context, \a
    sourceText, ""). If that still fails, returns an empty string.

    If you need to programatically insert translations in to a
    QTranslator, this function can be reimplemented.

    \sa load()
*/
QString QTranslator::translate(const char *context, const char *sourceText, const char *comment) const
{
    Q_D(const QTranslator);
    return d->do_translate(context, sourceText, comment, -1);
}


/*!
    \overload

    Returns the translation for the key (\a context, \a sourceText,
    \a comment). If none is found, also tries (\a context, \a
    sourceText, ""). If that still fails, returns an empty string.

    If \a n is not -1, it is used to choose an appropriate form for
    the translation (e.g. "%n file found" vs. "%n files found").

    \sa load()
*/
QString QTranslator::translate(const char *context, const char *sourceText, const char *comment,
                               int n) const
{
    Q_D(const QTranslator);
    // this step is necessary because the 3-parameter translate() overload is virtual
    if (n == -1)
        return translate(context, sourceText, comment);
    return d->do_translate(context, sourceText, comment, n);
}

/*!
    Returns true if this translator is empty, otherwise returns false.
    This function works with stripped and unstripped translation files.
*/
bool QTranslator::isEmpty() const
{
    Q_D(const QTranslator);
    return !d->unmapPointer && !d->unmapLength && !d->messageArray &&
           !d->offsetArray && !d->contextArray;
}

/*!
    \fn QString QTranslator::find(const char *context, const char *sourceText, const char * comment = 0) const

    Use translate(\a context, \a sourceText, \a comment) instead.
*/

#endif // QT_NO_TRANSLATION

QT_END_NAMESPACE
