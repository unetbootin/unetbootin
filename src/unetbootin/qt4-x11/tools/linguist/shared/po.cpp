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

#include <QtCore/QDebug>
#include <QtCore/QIODevice>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include <ctype.h>

#define MAGIC_OBSOLETE_REFERENCE "Obsolete_PO_entries"

// Uncomment if you wish to hard wrap long lines in .po files. Note that this
// affects only msg strings, not comments.
//#define HARD_WRAP_LONG_WORDS

QT_BEGIN_NAMESPACE

static const int MAX_LEN = 79;

static QString poEscapedString(const QString &prefix, const QString &keyword,
                               bool noWrap, const QString &ba)
{
    QStringList lines;
    int off = 0;
    QString res;
    while (off < ba.length()) {
        ushort c = ba[off++].unicode();
        switch (c) {
        case '\n':
            res += QLatin1String("\\n");
            lines.append(res);
            res.clear();
            break;
        case '\r':
            res += QLatin1String("\\r");
            break;
        case '\t':
            res += QLatin1String("\\t");
            break;
        case '\v':
            res += QLatin1String("\\v");
            break;
        case '\a':
            res += QLatin1String("\\a");
            break;
        case '\b':
            res += QLatin1String("\\b");
            break;
        case '\f':
            res += QLatin1String("\\f");
            break;
        case '"':
            res += QLatin1String("\\\"");
            break;
        case '\\':
            res += QLatin1String("\\\\");
            break;
        default:
            if (c < 32) {
                res += QLatin1String("\\x");
                res += QString::number(c, 16);
                if (off < ba.length() && isxdigit(ba[off].unicode()))
                    res += QLatin1String("\"\"");
            } else {
                res += QChar(c);
            }
            break;
        }
    }
    if (!res.isEmpty())
        lines.append(res);
    if (!lines.isEmpty()) {
        if (!noWrap) {
            if (lines.count() != 1 ||
                lines.first().length() > MAX_LEN - keyword.length() - prefix.length() - 3)
            {
                QStringList olines = lines;
                lines = QStringList(QString());
                const int maxlen = MAX_LEN - prefix.length() - 2;
                foreach (const QString &line, olines) {
                    int off = 0;
                    while (off + maxlen < line.length()) {
                        int idx = line.lastIndexOf(QLatin1Char(' '), off + maxlen - 1) + 1;
                        if (idx == off) {
#ifdef HARD_WRAP_LONG_WORDS
                            // This doesn't seem too nice, but who knows ...
                            idx = off + maxlen;
#else
                            idx = line.indexOf(QLatin1Char(' '), off + maxlen) + 1;
                            if (!idx)
                                break;
#endif
                        }
                        lines.append(line.mid(off, idx - off));
                        off = idx;
                    }
                    lines.append(line.mid(off));
                }
            }
        } else if (lines.count() > 1) {
            lines.prepend(QString());
        }
    }
    return prefix + keyword + QLatin1String(" \"") +
           lines.join(QLatin1String("\"\n") + prefix + QLatin1Char('"')) +
           QLatin1String("\"\n");
}

static QString poEscapedLines(const QString &prefix, bool addSpace, const QStringList &lines)
{
    QString out;
    foreach (const QString &line, lines) {
        out += prefix;
        if (addSpace && !line.isEmpty())
            out += QLatin1Char(' ' );
        out += line;
        out += QLatin1Char('\n');
    }
    return out;
}

static QString poEscapedLines(const QString &prefix, bool addSpace, const QString &in0)
{
    QString in = in0;
    if (in.endsWith(QLatin1Char('\n')))
        in.chop(1);
    return poEscapedLines(prefix, addSpace, in.split(QLatin1Char('\n')));
}

static QString poWrappedEscapedLines(const QString &prefix, bool addSpace, const QString &line)
{
    const int maxlen = MAX_LEN - prefix.length();
    QStringList lines;
    int off = 0;
    while (off + maxlen < line.length()) {
        int idx = line.lastIndexOf(QLatin1Char(' '), off + maxlen - 1);
        if (idx < off) {
#if 0 //def HARD_WRAP_LONG_WORDS
            // This cannot work without messing up semantics, so do not even try.
#else
            idx = line.indexOf(QLatin1Char(' '), off + maxlen);
            if (idx < 0)
                break;
#endif
        }
        lines.append(line.mid(off, idx - off));
        off = idx + 1;
    }
    lines.append(line.mid(off));
    return poEscapedLines(prefix, addSpace, lines);
}

struct PoItem
{
public:
    PoItem()
      : isPlural(false), isFuzzy(false)
    {}


public:
    QString id;
    QString context;
    QString tscomment;
    QString oldTscomment;
    QString lineNumber;
    QString fileName;
    QString references;
    QString translatorComments;
    QString automaticComments;
    QString msgId;
    QString oldMsgId;
    QStringList msgStr;
    bool isPlural;
    bool isFuzzy;
    QHash<QString, QString> extra;
};


static bool isTranslationLine(const QString &line)
{
    return line.startsWith(QLatin1String("#~ msgstr"))
           || line.startsWith(QLatin1String("msgstr"));
}

static QString slurpEscapedString(const QStringList &lines, int & l,
        int offset, const QString &prefix, ConversionData &cd)
{
    QString msg;
    int stoff;

    for (; l < lines.size(); ++l) {
        const QString &line = lines.at(l);
        if (line.isEmpty() || !line.startsWith(prefix))
            break;
        while (line[offset].isSpace()) // No length check, as string has no trailing spaces.
            offset++;
        if (line[offset].unicode() != '"')
            break;
        offset++;
        forever {
            if (offset == line.length())
                goto premature_eol;
            ushort c = line[offset++].unicode();
            if (c == '"') {
                if (offset == line.length())
                    break;
                while (line[offset].isSpace())
                    offset++;
                if (line[offset++].unicode() != '"') {
                    cd.appendError(QString::fromLatin1(
                            "PO parsing error: extra characters on line %1.")
                            .arg(l + 1));
                    break;
                }
                continue;
            }
            if (c == '\\') {
                if (offset == line.length())
                    goto premature_eol;
                c = line[offset++].unicode();
                switch (c) {
                case 'r':
                    msg += QLatin1Char('\r'); // Maybe just throw it away?
                    break;
                case 'n':
                    msg += QLatin1Char('\n');
                    break;
                case 't':
                    msg += QLatin1Char('\t');
                    break;
                case 'v':
                    msg += QLatin1Char('\v');
                    break;
                case 'a':
                    msg += QLatin1Char('\a');
                    break;
                case 'b':
                    msg += QLatin1Char('\b');
                    break;
                case 'f':
                    msg += QLatin1Char('\f');
                    break;
                case '"':
                    msg += QLatin1Char('"');
                    break;
                case '\\':
                    msg += QLatin1Char('\\');
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    stoff = offset - 1;
                    while ((c = line[offset].unicode()) >= '0' && c <= '7')
                        if (++offset == line.length())
                            goto premature_eol;
                    msg += QChar(line.mid(stoff, offset - stoff).toUInt(0, 8));
                    break;
                case 'x':
                    stoff = offset;
                    while (isxdigit(line[offset].unicode()))
                        if (++offset == line.length())
                            goto premature_eol;
                    msg += QChar(line.mid(stoff, offset - stoff).toUInt(0, 16));
                    break;
                default:
                    cd.appendError(QString::fromLatin1(
                            "PO parsing error: invalid escape '\\%1' (line %2).")
                            .arg(QChar(c)).arg(l + 1));
                    msg += QLatin1Char('\\');
                    msg += QChar(c);
                    break;
                }
            } else {
                msg += QChar(c);
            }
        }
        offset = prefix.size();
    }
    --l;
    return msg;

premature_eol:
    cd.appendError(QString::fromLatin1(
            "PO parsing error: premature end of line %1.").arg(l + 1));
    return QString();
}

static void slurpComment(QString &msg, const QStringList &lines, int & l)
{
    const QChar newline = QLatin1Char('\n');
    QString prefix = lines.at(l);
    for (int i = 1; ; i++) {
        if (prefix.at(i).unicode() != ' ') {
            prefix.truncate(i);
            break;
        }
    }
    for (; l < lines.size(); ++l) {
        const QString &line = lines.at(l);
        if (line.startsWith(prefix))
            msg += line.mid(prefix.size());
        else if (line != QLatin1String("#"))
            break;
        msg += newline;
    }
    --l;
}

bool loadPO(Translator &translator, QIODevice &dev, ConversionData &cd)
{
    const QChar quote = QLatin1Char('"');
    const QChar newline = QLatin1Char('\n');
    QTextStream in(&dev);
    bool error = false;

    // format of a .po file entry:
    // white-space
    // #  translator-comments
    // #. automatic-comments
    // #: reference...
    // #, flag...
    // #~ msgctxt, msgid*, msgstr - used for obsoleted messages
    // #| msgctxt, msgid* previous untranslated-string - for fuzzy message
    // msgctx string-context
    // msgid untranslated-string
    // -- For singular:
    // msgstr translated-string
    // -- For plural:
    // msgid_plural untranslated-string-plural
    // msgstr[0] translated-string
    // ...

    // we need line based lookahead below.
    QStringList lines;
    while (!in.atEnd())
        lines.append(in.readLine().trimmed());
    lines.append(QString());

    int l = 0;
    PoItem item;
    for (; l != lines.size(); ++l) {
        QString line = lines.at(l);
        if (line.isEmpty())
           continue; 
        if (isTranslationLine(line)) {
            bool isObsolete = line.startsWith(QLatin1String("#~ msgstr"));
            const QString prefix = QLatin1String(isObsolete ? "#~ " : "");
            while (true) {
                int idx = line.indexOf(QLatin1Char(' '), prefix.length());
                item.msgStr.append(slurpEscapedString(lines, l, idx, prefix, cd));
                if (l + 1 >= lines.size() || !isTranslationLine(lines.at(l + 1)))
                    break;
                ++l;
                line = lines.at(l);
            }
            if (item.msgId.isEmpty()) {
                QRegExp rx(QLatin1String("\\bX-Language: ([^\n]*)\n"));
                int idx = rx.indexIn(item.msgStr.first());
                if (idx >= 0) {
                    translator.setLanguageCode(rx.cap(1));
                    item.msgStr.first().remove(idx, rx.matchedLength());
                }
                QRegExp rx2(QLatin1String("\\bX-Source-Language: ([^\n]*)\n"));
                int idx2 = rx2.indexIn(item.msgStr.first());
                if (idx2 >= 0) {
                    translator.setSourceLanguageCode(rx2.cap(1));
                    item.msgStr.first().remove(idx2, rx2.matchedLength());
                }
                if (item.msgStr.first().indexOf(
                        QRegExp(QLatin1String("\\bX-Virgin-Header:[^\n]*\n"))) >= 0) {
                    item = PoItem();
                    continue;
                }
            }
            // build translator message
            TranslatorMessage msg;
            msg.setContext(item.context);
            if (!item.references.isEmpty()) {
                foreach (const QString &ref,
                         item.references.split(QRegExp(QLatin1String("\\s")),
                                               QString::SkipEmptyParts)) {
                    int pos = ref.lastIndexOf(QLatin1Char(':'));
                    if (pos != -1)
                        msg.addReference(ref.left(pos), ref.mid(pos + 1).toInt());
                }
            } else if (isObsolete) {
                msg.setFileName(QLatin1String(MAGIC_OBSOLETE_REFERENCE));
            }
            msg.setId(item.id);
            msg.setSourceText(item.msgId);
            msg.setOldSourceText(item.oldMsgId);
            msg.setComment(item.tscomment);
            msg.setOldComment(item.oldTscomment);
            msg.setExtraComment(item.automaticComments);
            msg.setTranslatorComment(item.translatorComments);
            msg.setPlural(item.isPlural || item.msgStr.size() > 1);
            msg.setTranslations(item.msgStr);
            if (isObsolete)
                msg.setType(TranslatorMessage::Obsolete);
            else if (item.isFuzzy)
                msg.setType(TranslatorMessage::Unfinished);
            else
                msg.setType(TranslatorMessage::Finished);
            msg.setExtras(item.extra);

            //qDebug() << "WRITE: " << context;
            //qDebug() << "SOURCE: " << msg.sourceText();
            //qDebug() << flags << msg.m_extra;
            translator.append(msg);
            item = PoItem();
        } else if (line.startsWith(QLatin1Char('#'))) {
            switch(line.size() < 2 ? 0 : line.at(1).unicode()) {
                case ':':
                    item.references += line.mid(3);
                    item.references += newline;
                    break;
                case ',': {
                    QStringList flags =
                            line.mid(2).split(QRegExp(QLatin1String("[, ]")),
                                              QString::SkipEmptyParts);
                    if (flags.removeOne(QLatin1String("fuzzy")))
                        item.isFuzzy = true;
                    TranslatorMessage::ExtraData::const_iterator it =
                            item.extra.find(QLatin1String("po-flags"));
                    if (it != item.extra.end())
                        flags.prepend(*it);
                    if (!flags.isEmpty())
                        item.extra[QLatin1String("po-flags")] = flags.join(QLatin1String(", "));
                    break;
                }
                case 0:
                    item.translatorComments += newline;
                    break;
                case ' ':
                    slurpComment(item.translatorComments, lines, l);
                    break;
                case '.':
                    if (line.startsWith(QLatin1String("#. ts-context "))) {
                        item.context = line.mid(14);
                    } else if (line.startsWith(QLatin1String("#. ts-id "))) {
                        item.id = line.mid(9);
                    } else {
                        item.automaticComments += line.mid(3);
                        item.automaticComments += newline;
                    }
                    break;
                case '|':
                    if (line.startsWith(QLatin1String("#| msgid "))) {
                        item.oldMsgId = slurpEscapedString(lines, l, 9, QLatin1String("#| "), cd);
                    } else if (line.startsWith(QLatin1String("#| msgid_plural "))) {
                        QString extra = slurpEscapedString(lines, l, 16, QLatin1String("#| "), cd);
                        if (extra != item.oldMsgId)
                            item.extra[QLatin1String("po-old_msgid_plural")] = extra;
                    } else if (line.startsWith(QLatin1String("#| msgctxt "))) {
                        item.oldTscomment = slurpEscapedString(lines, l, 11, QLatin1String("#| "), cd);
                    } else {
                        cd.appendError(QString(QLatin1String("PO-format parse error in line %1: '%2'\n"))
                            .arg(l + 1).arg(lines[l]));
                        error = true;
                    }
                    break;
                case '~':
                    if (line.startsWith(QLatin1String("#~ msgid "))) {
                        item.msgId = slurpEscapedString(lines, l, 9, QLatin1String("#~ "), cd);
                    } else if (line.startsWith(QLatin1String("#~ msgid_plural "))) {
                        QString extra = slurpEscapedString(lines, l, 16, QLatin1String("#~ "), cd);
                        if (extra != item.msgId)
                            item.extra[QLatin1String("po-msgid_plural")] = extra;
                        item.isPlural = true;
                    } else if (line.startsWith(QLatin1String("#~ msgctxt "))) {
                        item.tscomment = slurpEscapedString(lines, l, 11, QLatin1String("#~ "), cd);
                    } else {
                        cd.appendError(QString(QLatin1String("PO-format parse error in line %1: '%2'\n"))
                            .arg(l + 1).arg(lines[l]));
                        error = true;
                    }
                    break;
                default:
                    cd.appendError(QString(QLatin1String("PO-format parse error in line %1: '%2'\n"))
                        .arg(l + 1).arg(lines[l]));
                    error = true;
                    break;
            }
        } else if (line.startsWith(QLatin1String("msgctxt "))) {
            item.tscomment = slurpEscapedString(lines, l, 8, QString(), cd);
        } else if (line.startsWith(QLatin1String("msgid "))) {
            item.msgId = slurpEscapedString(lines, l, 6, QString(), cd);
        } else if (line.startsWith(QLatin1String("msgid_plural "))) {
            QString extra = slurpEscapedString(lines, l, 13, QString(), cd);
            if (extra != item.msgId)
                item.extra[QLatin1String("po-msgid_plural")] = extra;
            item.isPlural = true;
        } else {
            cd.appendError(QString(QLatin1String("PO-format error in line %1: '%2'\n"))
                .arg(l + 1).arg(lines[l]));
            error = true;
        }
    }
    return !error && cd.errors().isEmpty();
}

bool savePO(const Translator &translator, QIODevice &dev, ConversionData &cd)
{
    bool ok = true;
    QTextStream out(&dev);
    //qDebug() << "OUT CODEC: " << out.codec()->name();

    bool first = true;
    if (translator.messages().isEmpty() || !translator.messages().first().sourceText().isEmpty()) {
        out <<
            "# SOME DESCRIPTIVE TITLE.\n"
            "# Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER\n"
            "# This file is distributed under the same license as the PACKAGE package.\n"
            "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n"
            "#\n"
            "#, fuzzy\n"
            "msgid \"\"\n"
            "msgstr \"\"\n"
            "\"X-Virgin-Header: remove this line if you change anything in the header.\\n\"\n";
        if (!translator.languageCode().isEmpty())
            out << "\"X-Language: " << translator.languageCode() << "\\n\"\n";
        if (!translator.sourceLanguageCode().isEmpty())
            out << "\"X-Source-Language: " << translator.sourceLanguageCode() << "\\n\"\n";
        first = false;
    }
    foreach (const TranslatorMessage &msg, translator.messages()) {
        if (!first)
            out << endl;

        if (!msg.translatorComment().isEmpty())
            out << poEscapedLines(QLatin1String("#"), true, msg.translatorComment());

        if (!msg.extraComment().isEmpty())
            out << poEscapedLines(QLatin1String("#."), true, msg.extraComment());

        if (!msg.context().isEmpty())
            out << QLatin1String("#. ts-context ") << msg.context() << '\n';
        if (!msg.id().isEmpty())
            out << QLatin1String("#. ts-id ") << msg.id() << '\n';

        if (!msg.fileName().isEmpty() && msg.fileName() != QLatin1String(MAGIC_OBSOLETE_REFERENCE)) {
            QStringList refs;
            foreach (const TranslatorMessage::Reference &ref, msg.allReferences())
                refs.append(QString(QLatin1String("%2:%1"))
                                    .arg(ref.lineNumber()).arg(ref.fileName()));
            out << poWrappedEscapedLines(QLatin1String("#:"), true, refs.join(QLatin1String(" ")));
        }

        bool noWrap = false;
        QStringList flags;
        if (msg.type() == TranslatorMessage::Unfinished)
            flags.append(QLatin1String("fuzzy"));
        TranslatorMessage::ExtraData::const_iterator itr =
                msg.extras().find(QLatin1String("po-flags"));
        if (itr != msg.extras().end()) {
            if (itr->split(QLatin1String(", ")).contains(QLatin1String("no-wrap")))
                noWrap = true;
            flags.append(*itr);
        }
        if (!flags.isEmpty())
            out << "#, " << flags.join(QLatin1String(", ")) << '\n';

        QString prefix = QLatin1String("#| ");
        if (!msg.oldComment().isEmpty())
            out << poEscapedString(prefix, QLatin1String("msgctxt"), noWrap, msg.oldComment());
        if (!msg.oldSourceText().isEmpty())
            out << poEscapedString(prefix, QLatin1String("msgid"), noWrap, msg.oldSourceText());
        QString plural = msg.extra(QLatin1String("po-old_msgid_plural"));
        if (!plural.isEmpty())
            out << poEscapedString(prefix, QLatin1String("msgid_plural"), noWrap, plural);
        prefix = QLatin1String((msg.type() == TranslatorMessage::Obsolete) ? "#~ " : "");
        if (!msg.comment().isEmpty())
            out << poEscapedString(prefix, QLatin1String("msgctxt"), noWrap, msg.comment());
        out << poEscapedString(prefix, QLatin1String("msgid"), noWrap, msg.sourceText());
        if (!msg.isPlural()) {
            QString transl = msg.translation();
            if (first) {
                transl.remove(QRegExp(QLatin1String("\\bX-Language:[^\n]*\n")));
                if (!translator.languageCode().isEmpty())
                    transl += QLatin1String("X-Language: ") + translator.languageCode() + QLatin1Char('\n');
            }
            out << poEscapedString(prefix, QLatin1String("msgstr"), noWrap, transl);
        } else {
            QString plural = msg.extra(QLatin1String("po-msgid_plural"));
            if (plural.isEmpty())
                plural = msg.sourceText();
            out << poEscapedString(prefix, QLatin1String("msgid_plural"), noWrap, plural);
            QStringList translations = translator.normalizedTranslations(msg, cd, &ok);
            for (int i = 0; i != translations.size(); ++i) {
                out << poEscapedString(prefix, QString::fromLatin1("msgstr[%1]").arg(i), noWrap,
                                       translations.at(i));
            }
        }
        first = false;
    }
    return ok;
}

int initPO()
{
    Translator::FileFormat format;
    format.extension = QLatin1String("po");
    format.description = QObject::tr("GNU Gettext localization files");
    format.loader = &loadPO;
    format.saver = &savePO;
    format.fileType = Translator::FileFormat::TranslationSource;
    format.priority = 1;
    Translator::registerFileFormat(format);
    return 1;
}

Q_CONSTRUCTOR_FUNCTION(initPO)

QT_END_NAMESPACE
