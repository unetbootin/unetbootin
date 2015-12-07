/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "translator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QTextCodec>

QT_BEGIN_NAMESPACE

// magic number for the file
static const int MagicLength = 16;
static const uchar magic[MagicLength] = {
    0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
    0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};


namespace {

enum Tag {
    Tag_End          = 1,
    Tag_SourceText16 = 2,
    Tag_Translation  = 3,
    Tag_Context16    = 4,
    Tag_Obsolete1    = 5,
    Tag_SourceText   = 6,
    Tag_Context      = 7,
    Tag_Comment      = 8,
    Tag_Obsolete2    = 9
};

enum Prefix {
    NoPrefix,
    Hash,
    HashContext,
    HashContextSourceText,
    HashContextSourceTextComment
};

} // namespace anon

static uint elfHash(const QByteArray &ba)
{
    const uchar *k = (const uchar *)ba.data();
    uint h = 0;
    uint g;

    if (k) {
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

class Releaser
{
public:
    struct Offset {
        Offset()
            : h(0), o(0)
        {}
        Offset(uint hash, uint offset)
            : h(hash), o(offset)
        {}

        bool operator<(const Offset &other) const {
            return (h != other.h) ? h < other.h : o < other.o;
        }
        bool operator==(const Offset &other) const {
            return h == other.h && o == other.o;
        }
        uint h;
        uint o;
    };

    enum { Contexts = 0x2f, Hashes = 0x42, Messages = 0x69, NumerusRules = 0x88 };

    Releaser() : m_codec(0) {}

    void setCodecName(const QByteArray &codecName)
    {
        m_codec = QTextCodec::codecForName(codecName);
    }

    TranslatorMessage findMessage(const QString &context,
        const QString &sourceText, const QString &comment,
        const QString &fileName = QString(), int lineNumber = -1) const;

    bool save(QIODevice *iod);

    void insert(const TranslatorMessage &);
    void remove(const TranslatorMessage &);

    bool contains(const QString &context, const QString &sourceText,
        const QString & comment) const;

    bool contains(const QString &context, const QString &comment,
        const QString &fileName, int lineNumber) const;

    void squeeze(TranslatorSaveMode mode);

    QList<TranslatorMessage> messages() const;

    bool isEmpty() const;

    void setNumerusRules(const QByteArray &rules);

private:
    Q_DISABLE_COPY(Releaser)

    // This should reproduce the byte array fetched from the source file, which
    // on turn should be the same as passed to the actual tr(...) calls
    QByteArray originalBytes(const QString &str, bool isUtf8) const;

    Prefix commonPrefix(const TranslatorMessage &m1, const TranslatorMessage &m2) const;

    uint msgHash(const TranslatorMessage &msg) const;

    void writeMessage(const TranslatorMessage & msg, QDataStream & stream,
        TranslatorSaveMode strip, Prefix prefix) const;

    // for squeezed but non-file data, this is what needs to be deleted
    QByteArray m_messageArray;
    QByteArray m_offsetArray;
    QByteArray m_contextArray;
    QMap<TranslatorMessage, void *> m_messages;
    QByteArray m_numerusRules;

    // Used to reproduce the original bytes
    QTextCodec *m_codec;
};

QByteArray Releaser::originalBytes(const QString &str, bool isUtf8) const
{
    if (str.isEmpty()) {
        // Do not use QByteArray() here as the result of the serialization
        // will be different.
        return QByteArray("");
    }
    if (isUtf8)
        return str.toUtf8();
    return m_codec ? m_codec->fromUnicode(str) : str.toLatin1();
}

uint Releaser::msgHash(const TranslatorMessage &msg) const
{
    return elfHash(originalBytes(msg.sourceText() + msg.comment(), msg.isUtf8()));
}

Prefix Releaser::commonPrefix(const TranslatorMessage &m1, const TranslatorMessage &m2) const
{
    if (msgHash(m1) != msgHash(m2))
        return NoPrefix;
    if (m1.context() != m2.context())
        return Hash;
    if (m1.sourceText() != m2.sourceText())
        return HashContext;
    if (m1.comment() != m2.comment())
        return HashContextSourceText;
    return HashContextSourceTextComment;
}

void Releaser::writeMessage(const TranslatorMessage & msg, QDataStream & stream,
    TranslatorSaveMode mode, Prefix prefix) const
{
    for (int i = 0; i < msg.translations().count(); ++i) {
        QString str = msg.translations().at(i);
        str.replace(QChar(Translator::DefaultVariantSeparator),
                    QChar(Translator::InternalVariantSeparator));
        stream << quint8(Tag_Translation) << str;
    }

    if (mode == SaveEverything)
        prefix = HashContextSourceTextComment;

    // lrelease produces "wrong" .qm files for QByteArrays that are .isNull().
    switch (prefix) {
    default:
    case HashContextSourceTextComment:
        stream << quint8(Tag_Comment) << originalBytes(msg.comment(), msg.isUtf8());
        // fall through
    case HashContextSourceText:
        stream << quint8(Tag_SourceText) << originalBytes(msg.sourceText(), msg.isUtf8());
        // fall through
    case HashContext:
        stream << quint8(Tag_Context) << originalBytes(msg.context(), msg.isUtf8());
        ;
    }

    stream << quint8(Tag_End);
}


bool Releaser::save(QIODevice *iod)
{
    QDataStream s(iod);
    s.writeRawData((const char *)magic, MagicLength);

    if (!m_offsetArray.isEmpty()) {
        quint32 oas = quint32(m_offsetArray.size());
        s << quint8(Hashes) << oas;
        s.writeRawData(m_offsetArray.constData(), oas);
    }
    if (!m_messageArray.isEmpty()) {
        quint32 mas = quint32(m_messageArray.size());
        s << quint8(Messages) << mas;
        s.writeRawData(m_messageArray.constData(), mas);
    }
    if (!m_contextArray.isEmpty()) {
        quint32 cas = quint32(m_contextArray.size());
        s << quint8(Contexts) << cas;
        s.writeRawData(m_contextArray.constData(), cas);
    }
    if (!m_numerusRules.isEmpty()) {
        quint32 nrs = m_numerusRules.size();
        s << quint8(NumerusRules) << nrs;
        s.writeRawData(m_numerusRules.constData(), nrs);
    }
    return true;
}

void Releaser::squeeze(TranslatorSaveMode mode)
{
    if (m_messages.isEmpty() && mode == SaveEverything)
        return;

    QMap<TranslatorMessage, void *> messages = m_messages;

    // re-build contents
    m_messageArray.clear();
    m_offsetArray.clear();
    m_contextArray.clear();
    m_messages.clear();

    QMap<Offset, void *> offsets;

    QDataStream ms(&m_messageArray, QIODevice::WriteOnly);
    QMap<TranslatorMessage, void *>::const_iterator it, next;
    int cpPrev = 0, cpNext = 0;
    for (it = messages.constBegin(); it != messages.constEnd(); ++it) {
        cpPrev = cpNext;
        next = it;
        ++next;
        if (next == messages.constEnd())
            cpNext = 0;
        else
            cpNext = commonPrefix(it.key(), next.key());
        offsets.insert(Offset(msgHash(it.key()), ms.device()->pos()), (void *)0);
        writeMessage(it.key(), ms, mode, Prefix(qMax(cpPrev, cpNext + 1)));
    }

    QMap<Offset, void *>::Iterator offset;
    offset = offsets.begin();
    QDataStream ds(&m_offsetArray, QIODevice::WriteOnly);
    while (offset != offsets.end()) {
        Offset k = offset.key();
        ++offset;
        ds << quint32(k.h) << quint32(k.o);
    }

    if (mode == SaveStripped) {
        QMap<QString, int> contextSet;
        for (it = messages.constBegin(); it != messages.constEnd(); ++it)
            ++contextSet[it.key().context()];

        quint16 hTableSize;
        if (contextSet.size() < 200)
            hTableSize = (contextSet.size() < 60) ? 151 : 503;
        else if (contextSet.size() < 2500)
            hTableSize = (contextSet.size() < 750) ? 1511 : 5003;
        else
            hTableSize = (contextSet.size() < 10000) ? 15013 : 3 * contextSet.size() / 2;

        QMultiMap<int, QString> hashMap;
        QMap<QString, int>::const_iterator c;
        for (c = contextSet.constBegin(); c != contextSet.constEnd(); ++c)
            hashMap.insert(elfHash(originalBytes(c.key(), false /*FIXME*/)) % hTableSize, c.key());

        /*
          The contexts found in this translator are stored in a hash
          table to provide fast lookup. The context array has the
          following format:

              quint16 hTableSize;
              quint16 hTable[hTableSize];
              quint8  contextPool[...];

          The context pool stores the contexts as Pascal strings:

              quint8  len;
              quint8  data[len];

          Let's consider the look-up of context "FunnyDialog".  A
          hash value between 0 and hTableSize - 1 is computed, say h.
          If hTable[h] is 0, "FunnyDialog" is not covered by this
          translator. Else, we check in the contextPool at offset
          2 * hTable[h] to see if "FunnyDialog" is one of the
          contexts stored there, until we find it or we meet the
          empty string.
        */
        m_contextArray.resize(2 + (hTableSize << 1));
        QDataStream t(&m_contextArray, QIODevice::WriteOnly);

        quint16 *hTable = new quint16[hTableSize];
        memset(hTable, 0, hTableSize * sizeof(quint16));

        t << hTableSize;
        t.device()->seek(2 + (hTableSize << 1));
        t << quint16(0); // the entry at offset 0 cannot be used
        uint upto = 2;

        QMap<int, QString>::const_iterator entry = hashMap.constBegin();
        while (entry != hashMap.constEnd()) {
            int i = entry.key();
            hTable[i] = quint16(upto >> 1);

            do {
                QString context = entry.value();
                QByteArray ba = context.toUtf8();
                const char *con = ba.data();
                uint len = uint(qstrlen(con));
                len = qMin(len, 255u);
                t << quint8(len);
                t.writeRawData(con, len);
                upto += 1 + len;
                ++entry;
            } while (entry != hashMap.constEnd() && entry.key() == i);
            if (upto & 0x1) {
                // offsets have to be even
                t << quint8(0); // empty string
                ++upto;
            }
        }
        t.device()->seek(2);
        for (int j = 0; j < hTableSize; j++)
            t << hTable[j];
        delete [] hTable;

        if (upto > 131072) {
            qWarning("Releaser::squeeze: Too many contexts");
            m_contextArray.clear();
        }
    }
}

bool Releaser::contains(const QString &context, const QString &sourceText,
    const QString &comment) const
{
    return !findMessage(context, sourceText, comment).translation().isNull();
}

bool Releaser::contains(const QString &context, const QString &comment,
    const QString &fileName, int lineNumber) const
{
    return !findMessage(context, QString(), comment, fileName, lineNumber).isNull();
}

void Releaser::insert(const TranslatorMessage &message)
{
    m_messages.insert(message, 0);
}

void Releaser::remove(const TranslatorMessage &message)
{
    m_messages.remove(message);
}


TranslatorMessage Releaser::findMessage(const QString &context,
    const QString &sourceText, const QString &comment,
    const QString &fileName, int lineNumber) const
{
    if (m_messages.isEmpty()) 
        return TranslatorMessage();

    QMap<TranslatorMessage, void *>::const_iterator it;

    // Either we want to find an item that matches context, sourcetext
    // (and optionally comment) Or we want to find an item that
    // matches context, filename, linenumber (and optionally comment)
    TranslatorMessage msg(context, sourceText, comment, QString(), fileName, lineNumber);
    it = m_messages.constFind(msg);
    if (it != m_messages.constEnd())
        return it.key();

    if (!comment.isEmpty()) {
        it = m_messages.constFind(TranslatorMessage(context, sourceText, QString(), QString(), fileName, lineNumber));
        if (it != m_messages.constEnd())
            return it.key();
    }

    it = m_messages.constFind(TranslatorMessage(context, QString(), comment, QString(), fileName, lineNumber));
    if (it != m_messages.constEnd())
        return it.key();
    if (comment.isEmpty())
        return TranslatorMessage();

    it = m_messages.constFind(TranslatorMessage(context, QString(), QString(), QString(), fileName, lineNumber));
    if (it != m_messages.constEnd())
        return it.key();
    return TranslatorMessage();
}

bool Releaser::isEmpty() const
{
    return m_messageArray.isEmpty() && m_offsetArray.isEmpty()
        && m_contextArray.isEmpty() && m_messages.isEmpty();
}

void Releaser::setNumerusRules(const QByteArray &rules)
{
    m_numerusRules = rules;
}

QList<TranslatorMessage> Releaser::messages() const
{
    return m_messages.keys();
}

static quint8 read8(const uchar *data)
{
    return *data;
}

static quint32 read32(const uchar *data)
{
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
}


bool loadQM(Translator &translator, QIODevice &dev, ConversionData &cd)
{
    QByteArray ba = dev.readAll();
    const uchar *data = (uchar*)ba.data();
    int len = ba.size();
    if (len < MagicLength || memcmp(data, magic, MagicLength) != 0) {
        cd.appendError(QLatin1String("QM-Format error: magic marker missing"));
        return false;
    }

    enum { Contexts = 0x2f, Hashes = 0x42, Messages = 0x69, NumerusRules = 0x88 };

    // for squeezed but non-file data, this is what needs to be deleted
    const uchar *messageArray = 0;
    const uchar *offsetArray = 0;
    const uchar *contextArray = 0;
    const uchar *numerusRulesArray = 0;
    uint messageLength = 0;
    uint offsetLength = 0;
    uint contextLength = 0;
    uint numerusRulesLength = 0;

    bool ok = true;
    const uchar *end = data + len;

    data += MagicLength;

    while (data < end - 4) {
        quint8 tag = read8(data++);
        quint32 blockLen = read32(data);
        //qDebug() << "TAG:" << tag <<  "BLOCKLEN:" << blockLen;
        data += 4;
        if (!tag || !blockLen)
            break;
        if (data + blockLen > end) {
            ok = false;
            break;
        }

        if (tag == Contexts) {
            contextArray = data;
            contextLength = blockLen;
            //qDebug() << "CONTEXTS: " << contextLength << QByteArray((const char *)contextArray, contextLength).toHex();
        } else if (tag == Hashes) {
            offsetArray = data;
            offsetLength = blockLen;
            //qDebug() << "HASHES: " << offsetLength << QByteArray((const char *)offsetArray, offsetLength).toHex();
        } else if (tag == Messages) {
            messageArray = data;
            messageLength = blockLen;
            //qDebug() << "MESSAGES: " << messageLength << QByteArray((const char *)messageArray, messageLength).toHex();
        } else if (tag == NumerusRules) {
            numerusRulesArray = data;
            numerusRulesLength = blockLen;
            //qDebug() << "NUMERUSRULES: " << numerusRulesLength << QByteArray((const char *)numerusRulesArray, numerusRulesLength).toHex();
        }

        data += blockLen;
    }


    size_t numItems = offsetLength / (2 * sizeof(quint32));
    //qDebug() << "NUMITEMS: " << numItems;

    TranslatorMessage msg;

    // FIXME: that's just a guess, the original locale data is lost...
    QTextCodec *codec = QTextCodec::codecForLocale();

    for (const uchar *start = offsetArray; start != offsetArray + (numItems << 3); start += 8) {
        //quint32 hash = read32(start);
        quint32 ro = read32(start + 4);
        //qDebug() << "\nHASH:" << hash;
        const uchar *m = messageArray + ro;

        for (;;) {
            uchar tag = read8(m++);
            //qDebug() << "Tag:" << tag << " ADDR: " << m;
            switch(tag) {
            case Tag_End:
                goto end;
            case Tag_Translation: {
                int len = read32(m);
                if (len % 1) {
                    cd.appendError(QLatin1String("QM-Format error"));
                    return false;
                }
                m += 4;
                QString str = QString::fromUtf16((const ushort *)m, len/2);
                if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
                    for (int i = 0; i < str.length(); ++i)
                        str[i] = QChar((str.at(i).unicode() >> 8) +
                            ((str.at(i).unicode() << 8) & 0xff00));
                }
                str.replace(QChar(Translator::InternalVariantSeparator),
                            QChar(Translator::DefaultVariantSeparator));
                msg.appendTranslation(str);
                m += len;
                break;
            }
            case Tag_Obsolete1:
                m += 4;
                //qDebug() << "OBSOLETE";
                break;
            case Tag_SourceText: {
                quint32 len = read32(m);
                m += 4;
                //qDebug() << "SOURCE LEN: " << len;
                //qDebug() << "SOURCE: " << QByteArray((const char*)m, len);
                msg.setSourceText(codec->toUnicode(QByteArray((const char*)m, len)));
                m += len;
                break;
            }
            case Tag_Context: {
                quint32 len = read32(m);
                m += 4;
                //qDebug() << "CONTEXT LEN: " << len;
                //qDebug() << "CONTEXT: " << QByteArray((const char*)m, len);
                msg.setContext(codec->toUnicode(QByteArray((const char*)m, len)));
                m += len;
                break;
            }
            case Tag_Comment: {
                quint32 len = read32(m);
                m += 4;
                //qDebug() << "COMMENT LEN: " << len;
                //qDebug() << "COMMENT: " << QByteArray((const char*)m, len);
                msg.setComment(codec->toUnicode(QByteArray((const char*)m, len)));
                m += len;
                break;
            }
            default:
                //qDebug() << "UNKNOWN TAG" << tag;
                break;
            }
        }
    end:;
        msg.setType(TranslatorMessage::Finished);
        translator.append(msg);
        //qDebug() << "\nHASH:" << hash << msg.sourceText() << msg.context();
        msg.setTranslations(QStringList());
    }
    return ok;
}



static bool saveQM(const Translator &translator, QIODevice &dev, ConversionData &cd)
{
    Releaser releaser;
    QLocale::Language l;
    QLocale::Country c;
    Translator::languageAndCountry(translator.languageCode(), &l, &c);
    QByteArray rules;
    if (getNumerusInfo(l, c, &rules, 0))
        releaser.setNumerusRules(rules);
    releaser.setCodecName(translator.codecName());

    int finished = 0;
    int unfinished = 0;
    int untranslated = 0;

    for (int i = 0; i != translator.messageCount(); ++i) {
        const TranslatorMessage &msg = translator.message(i);
        TranslatorMessage::Type typ = msg.type();
        if (typ != TranslatorMessage::Obsolete) {
            if (typ == TranslatorMessage::Unfinished) {
                if (msg.translation().isEmpty())
                    ++untranslated;
                else
                    ++unfinished;
            } else {
                ++finished;
            }
            QString context = msg.context();
            QString sourceText = msg.sourceText();
            QString comment = msg.comment();
            QStringList translations = msg.translations();

            if (!cd.ignoreUnfinished() || typ != TranslatorMessage::Unfinished) {
                /*
                Drop the comment in (context, sourceText, comment),
                unless the context is empty,
                unless (context, sourceText, "") already exists or
                unless we already dropped the comment of (context,
                sourceText, comment0).
                */
                if (comment.isEmpty()
                    || context.isEmpty()
                    || translator.contains(context, sourceText, QString())
                    || !releaser.findMessage(context, sourceText, QString()).translation()
                            .isNull() ) {
                    releaser.insert(msg);
                } else {
                    TranslatorMessage tm(context, sourceText, QString(),
                    QString(), QString(), -1, translations);
                    //filename and lineNumbers will be ignored from now.
                    releaser.insert(tm);
                }
            }
        }
    }

    releaser.squeeze(cd.m_saveMode);
    bool saved = releaser.save(&dev);
    if (saved && cd.isVerbose()) {
        int generatedCount = finished + unfinished;
        cd.appendError(QCoreApplication::translate("LRelease",
            "    Generated %n translation(s) (%1 finished and %2 unfinished)\n", 0,
            QCoreApplication::CodecForTr, generatedCount).arg(finished).arg(unfinished));
        if (untranslated)
            cd.appendError(QCoreApplication::translate("LRelease",
                "    Ignored %n untranslated source text(s)\n", 0,
                QCoreApplication::CodecForTr, untranslated));
    }
    return saved;
}

int initQM()
{
    Translator::FileFormat format;

    format.extension = QLatin1String("qm");
    format.description = QObject::tr("Compiled Qt translations");
    format.fileType = Translator::FileFormat::TranslationBinary;
    format.priority = 0;
    format.loader = &loadQM;
    format.saver = &saveQM;
    Translator::registerFileFormat(format);

    return 1;
}

Q_CONSTRUCTOR_FUNCTION(initQM)

QT_END_NAMESPACE
