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

#include "simtexth.h"

#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include <private/qtranslator_p.h>

QT_BEGIN_NAMESPACE

Translator::Translator() :
    m_codecName("ISO-8859-1"),
    m_locationsType(AbsoluteLocations)
{
}

void Translator::registerFileFormat(const FileFormat &format)
{
    //qDebug() << "Translator: Registering format " << format.extension;
    QList<Translator::FileFormat> &formats = registeredFileFormats();
    for (int i = 0; i < formats.size(); ++i)
        if (format.fileType == formats[i].fileType && format.priority < formats[i].priority) {
            formats.insert(i, format);
            return;
        }
    formats.append(format);
}

QList<Translator::FileFormat> &Translator::registeredFileFormats()
{
    static QList<Translator::FileFormat> theFormats;
    return theFormats;
}

void Translator::replace(const TranslatorMessage &msg)
{
    int index = m_messages.indexOf(msg);
    if (index == -1)
        m_messages.append(msg);
    else
        m_messages[index] = msg;
}

void Translator::replaceSorted(const TranslatorMessage &msg)
{
    int index = m_messages.indexOf(msg);
    if (index == -1)
        appendSorted(msg);
    else
        m_messages[index] = msg;
}

void Translator::extend(const TranslatorMessage &msg)
{
    int index = m_messages.indexOf(msg);
    if (index == -1) {
        m_messages.append(msg);
    } else {
        m_messages[index].addReferenceUniq(msg.fileName(), msg.lineNumber());
        if (!msg.extraComment().isEmpty()) {
            QString cmt = m_messages[index].extraComment();
            if (!cmt.isEmpty())
                cmt.append(QLatin1String("\n----------\n"));
            cmt.append(msg.extraComment());
            m_messages[index].setExtraComment(cmt);
        }
    }
}

void Translator::append(const TranslatorMessage &msg)
{
    m_messages.append(msg);
}

void Translator::appendSorted(const TranslatorMessage &msg)
{
    int msgLine = msg.lineNumber();
    if (msgLine < 0) {
        m_messages.append(msg);
        return;
    }

    int bestIdx = 0; // Best insertion point found so far
    int bestScore = 0; // Its category: 0 = no hit, 1 = pre or post, 2 = middle
    int bestSize = 0; // The length of the region. Longer is better within one category.

    // The insertion point to use should this region turn out to be the best one so far
    int thisIdx = 0;
    int thisScore = 0;
    int thisSize = 0;
    // Working vars
    int prevLine = 0;
    int curIdx = 0;
    foreach (const TranslatorMessage &mit, m_messages) {
        bool sameFile = mit.fileName() == msg.fileName();
        int curLine;
        if (sameFile && (curLine = mit.lineNumber()) >= prevLine) {
            if (msgLine >= prevLine && msgLine < curLine) {
                thisIdx = curIdx;
                thisScore = thisSize ? 2 : 1;
            }
            ++thisSize;
            prevLine = curLine;
        } else {
            if (thisSize) {
                if (!thisScore) {
                    thisIdx = curIdx;
                    thisScore = 1;
                }
                if (thisScore > bestScore || (thisScore == bestScore && thisSize > bestSize)) {
                    bestIdx = thisIdx;
                    bestScore = thisScore;
                    bestSize = thisSize;
                }
                thisScore = 0;
                thisSize = sameFile ? 1 : 0;
                prevLine = 0;
            }
        }
        ++curIdx;
    }
    if (thisSize && !thisScore) {
        thisIdx = curIdx;
        thisScore = 1;
    }
    if (thisScore > bestScore || (thisScore == bestScore && thisSize > bestSize))
        m_messages.insert(thisIdx, msg);
    else if (bestScore)
        m_messages.insert(bestIdx, msg);
    else
        m_messages.append(msg);
}

static QString guessFormat(const QString &filename, const QString &format)
{
    if (format != QLatin1String("auto"))
        return format;

    foreach (const Translator::FileFormat &fmt, Translator::registeredFileFormats()) {
        if (filename.endsWith(QLatin1Char('.') + fmt.extension, Qt::CaseInsensitive))
            return fmt.extension;
    }

    // the default format.
    // FIXME: change to something more widely distributed later.
    return QLatin1String("ts");
}

bool Translator::load(const QString &filename, ConversionData &cd, const QString &format)
{
    cd.m_sourceDir = QFileInfo(filename).absoluteDir();
    cd.m_sourceFileName = filename;

    QFile file;
    if (filename.isEmpty() || filename == QLatin1String("-")) {
        if (!file.open(stdin, QIODevice::ReadOnly)) {
            cd.appendError(QString::fromLatin1("Cannot open stdin!? (%1)")
                .arg(file.errorString()));
            return false;
        }
    } else {
        file.setFileName(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            cd.appendError(QString::fromLatin1("Cannot open %1: %2")
                .arg(filename, file.errorString()));
            return false;
        }
    }

    QString fmt = guessFormat(filename, format);

    foreach (const FileFormat &format, registeredFileFormats()) {
        if (fmt == format.extension) {
            if (format.loader)
                return (*format.loader)(*this, file, cd);
            cd.appendError(QString(QLatin1String("No loader for format %1 found"))
                .arg(fmt));
            return false;
        }
    }

    cd.appendError(QString(QLatin1String("Unknown format %1 for file %2"))
        .arg(format, filename));
    return false;
}


bool Translator::save(const QString &filename, ConversionData &cd, const QString &format) const
{
    QFile file;
    if (filename.isEmpty() || filename == QLatin1String("-")) {
        if (!file.open(stdout, QIODevice::WriteOnly)) {
            cd.appendError(QString::fromLatin1("Cannot open stdout!? (%1)")
                .arg(file.errorString()));
            return false;
        }
    } else {
        file.setFileName(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            cd.appendError(QString::fromLatin1("Cannot create %1: %2")
                .arg(filename, file.errorString()));
            return false;
        }
    }

    QString fmt = guessFormat(filename, format);
    cd.m_targetDir = QFileInfo(filename).absoluteDir();

    foreach (const FileFormat &format, registeredFileFormats()) {
        if (fmt == format.extension) {
            if (format.saver)
                return (*format.saver)(*this, file, cd);
            cd.appendError(QString(QLatin1String("No saver for format %1 found"))
                .arg(fmt));
            return false;
        }
    }

    cd.appendError(QString(QLatin1String("Unknown format %1 for file %2"))
        .arg(format).arg(filename));
    return false;
}

QString Translator::makeLanguageCode(QLocale::Language language, QLocale::Country country)
{
    QLocale locale(language, country);
    if (country == QLocale::AnyCountry) {
        QString languageCode = locale.name().section(QLatin1Char('_'), 0, 0);
        if (languageCode.length() <= 3)
            return languageCode;
        return QString();
    } else {
        return locale.name();
    }
}

void Translator::languageAndCountry(const QString &languageCode,
    QLocale::Language *lang, QLocale::Country *country)
{
    QLocale locale(languageCode);
    if (lang)
        *lang = locale.language();

    if (country) {
        if (languageCode.indexOf(QLatin1Char('_')) != -1)
            *country = locale.country();
        else
            *country = QLocale::AnyCountry;
    }
}

bool Translator::release(QFile *iod, ConversionData &cd) const
{
    foreach (const FileFormat &format, registeredFileFormats()) {
        if (format.extension == QLatin1String("qm"))
            return (*format.saver)(*this, *iod, cd);
    }
    cd.appendError(QLatin1String("No .qm saver available."));
    return false;
}

bool Translator::contains(const QString &context,
    const QString &sourceText, const QString &comment) const
{
    return m_messages.contains(TranslatorMessage(context, sourceText, comment,
        QString(), QString(), 0));
}

TranslatorMessage Translator::find(const QString &context,
    const QString &sourceText, const QString &comment) const
{
    TranslatorMessage needle(context, sourceText, comment, QString(), QString(), 0);
    int index = m_messages.indexOf(needle);
    return index == -1 ? TranslatorMessage() : m_messages.at(index);
}

TranslatorMessage Translator::find(const QString &context,
    const QString &comment, const TranslatorMessage::References &refs) const
{
    if (!refs.isEmpty()) {
        for (TMM::ConstIterator it = m_messages.constBegin(); it != m_messages.constEnd(); ++it) {
            if (it->context() == context && it->comment() == comment)
                foreach (const TranslatorMessage::Reference &itref, it->allReferences())
                    foreach (const TranslatorMessage::Reference &ref, refs)
                        if (itref == ref)
                            return *it;
        }
    }
    return TranslatorMessage();
}

bool Translator::contains(const QString &context) const
{
    foreach (const TranslatorMessage &msg, m_messages)
        if (msg.context() == context && msg.sourceText().isEmpty())
            return true;
    return false;
}

TranslatorMessage Translator::find(const QString &context) const
{
    foreach (const TranslatorMessage &msg, m_messages)
        if (msg.context() == context && msg.sourceText().isEmpty())
            return msg;
    return TranslatorMessage();
}

void Translator::stripObsoleteMessages()
{
    TMM newmm;
    for (TMM::ConstIterator it = m_messages.begin(); it != m_messages.end(); ++it)
        if (it->type() != TranslatorMessage::Obsolete)
            newmm.append(*it);
    m_messages = newmm;
}

void Translator::stripFinishedMessages()
{
    TMM newmm;
    for (TMM::ConstIterator it = m_messages.begin(); it != m_messages.end(); ++it)
        if (it->type() != TranslatorMessage::Finished)
            newmm.append(*it);
    m_messages = newmm;
}

void Translator::stripEmptyContexts()
{
    TMM newmm;
    for (TMM::ConstIterator it = m_messages.begin(); it != m_messages.end(); ++it)
        if (it->sourceText() != QLatin1String(ContextComment))
            newmm.append(*it);
    m_messages = newmm;
}

void Translator::stripNonPluralForms()
{
    TMM newmm;
    for (TMM::ConstIterator it = m_messages.begin(); it != m_messages.end(); ++it)
        if (it->isPlural())
            newmm.append(*it);
    m_messages = newmm;
}

void Translator::stripIdenticalSourceTranslations()
{
    TMM newmm;
    for (TMM::ConstIterator it = m_messages.begin(); it != m_messages.end(); ++it) {
        // we need to have just one translation, and it be equal to the source
        if (it->translations().count() != 1)
            newmm.append(*it);
        else if (it->translation() != it->sourceText())
            newmm.append(*it);
    }
    m_messages = newmm;
}

void Translator::dropTranslations()
{
    for (TMM::Iterator it = m_messages.begin(); it != m_messages.end(); ++it) {
        if (it->type() == TranslatorMessage::Finished)
            it->setType(TranslatorMessage::Unfinished);
        it->setTranslation(QString());
    }
}

QList<TranslatorMessage> Translator::findDuplicates() const
{
    QHash<TranslatorMessage, int> dups;
    foreach (const TranslatorMessage &msg, m_messages)
        dups[msg]++;
    QList<TranslatorMessage> ret;
    QHash<TranslatorMessage, int>::ConstIterator it = dups.constBegin(), end = dups.constEnd();
    for (; it != end; ++it)
        if (it.value() > 1)
            ret.append(it.key());
    return ret;
}

// Used by lupdate to be able to search using absolute paths during merging
void Translator::makeFileNamesAbsolute(const QDir &originalPath)
{
    TMM newmm;
    for (TMM::iterator it = m_messages.begin(); it != m_messages.end(); ++it) {
        TranslatorMessage msg = *it;
        msg.setReferences(TranslatorMessage::References());
        foreach (const TranslatorMessage::Reference &ref, it->allReferences()) {
            QString fileName = ref.fileName();
            QFileInfo fi (fileName);
            if (fi.isRelative())
                fileName = originalPath.absoluteFilePath(fileName);
            msg.addReference(fileName, ref.lineNumber());
        }
        newmm.append(msg);
    }
    m_messages = newmm;
}

QList<TranslatorMessage> Translator::messages() const
{
    return m_messages;
}

QList<TranslatorMessage> Translator::translatedMessages() const
{
    TMM result;
    for (TMM::ConstIterator it = m_messages.begin(); it != m_messages.end(); ++it)
        if (it->type() == TranslatorMessage::Finished)
            result.append(*it);
    return result;
}

QStringList Translator::normalizedTranslations(const TranslatorMessage &msg,
    QLocale::Language language, QLocale::Country country)
{
    QStringList translations = msg.translations();
    int numTranslations = 1;
    if (msg.isPlural() && language != QLocale::C) {
        QStringList forms;
        getNumerusInfo(language, country, 0, &forms);
        numTranslations = forms.count(); // includes singular
    }

    // make sure that the stringlist always have the size of the
    // language's current numerus, or 1 if its not plural
    if (translations.count() > numTranslations) {
        for (int i = translations.count(); i > numTranslations; --i)
            translations.removeLast();
    } else if (translations.count() < numTranslations) {
        for (int i = translations.count(); i < numTranslations; ++i)
            translations.append(QString());
    }
    return translations;
}

QStringList Translator::normalizedTranslations(const TranslatorMessage &msg,
    ConversionData &cd, bool *ok) const
{
    QLocale::Language l;
    QLocale::Country c;
    languageAndCountry(languageCode(), &l, &c);
    QStringList translns = normalizedTranslations(msg, l, c);
    if (msg.translations().size() > translns.size() && ok) {
        cd.appendError(QLatin1String(
            "Removed plural forms as the target language has less "
            "forms.\nIf this sounds wrong, possibly the target language is "
            "not set or recognized.\n"));
        *ok = false;
    }
    return translns;
}

QString Translator::guessLanguageCodeFromFileName(const QString &filename)
{
    QString str = filename;
    foreach (const FileFormat &format, registeredFileFormats()) {
        if (str.endsWith(format.extension)) {
            str = str.left(str.size() - format.extension.size() - 1);
            break;
        }
    }
    static QRegExp re(QLatin1String("[\\._]"));
    while (true) {
        QLocale locale(str);
        //qDebug() << "LANGUAGE FROM " << str << "LANG: " << locale.language();
        if (locale.language() != QLocale::C) {
            //qDebug() << "FOUND " << locale.name();
            return locale.name();
        }
        int pos = str.indexOf(re);
        if (pos == -1)
            break;
        str = str.mid(pos + 1);
    }
    //qDebug() << "LANGUAGE GUESSING UNSUCCESSFUL";
    return QString();
}

bool Translator::hasExtra(const QString &key) const
{
    return m_extra.contains(key);
}

QString Translator::extra(const QString &key) const
{
    return m_extra[key];
}

void Translator::setExtra(const QString &key, const QString &value)
{
    m_extra[key] = value;
}

void Translator::setCodecName(const QByteArray &name)
{
    QTextCodec *codec = QTextCodec::codecForName(name);
    if (!codec) {
        if (!name.isEmpty())
            qWarning("No QTextCodec for %s available. Using Latin1\n", name.constData());
        m_codecName.clear();
    } else {
        m_codecName = codec->name();
    }
}

void Translator::dump() const
{
    for (int i = 0; i != messageCount(); ++i)
        message(i).dump();
}

QT_END_NAMESPACE
