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

#ifndef METATRANSLATOR_H
#define METATRANSLATOR_H

#include "translatormessage.h"

#include <QDir>
#include <QList>
#include <QLocale>
#include <QString>


QT_BEGIN_NAMESPACE

Q_DECLARE_TYPEINFO(TranslatorMessage, Q_MOVABLE_TYPE);

class QIODevice;

// A struct of "interesting" data passed to and from the load and save routines
class ConversionData
{
public:
    ConversionData() :
        m_verbose(false),
        m_ignoreUnfinished(false),
        m_sortContexts(false),
        m_noUiLines(false),
        m_saveMode(SaveEverything)
    {}

    // tag manipulation
    const QStringList &dropTags() const { return m_dropTags; }
    QStringList &dropTags() { return m_dropTags; }
    const QDir &targetDir() const { return m_targetDir; }
    bool isVerbose() const { return m_verbose; }
    bool ignoreUnfinished() const { return m_ignoreUnfinished; }
    bool sortContexts() const { return m_sortContexts; }

    void appendError(const QString &error) { m_errors.append(error); }
    QString error() const { return  m_errors.join(QLatin1String("\n")); }
    QStringList errors() const { return  m_errors; }
    void clearErrors() { m_errors.clear(); }

public:
    QString m_defaultContext;
    QByteArray m_codecForSource; // CPP specific
    QString m_sourceFileName;
    QString m_targetFileName;
    QDir m_sourceDir;
    QDir m_targetDir; // FIXME: TS spefic
    QStringList m_dropTags;  // tags to be dropped 
    QStringList m_errors;
    bool m_verbose;
    bool m_ignoreUnfinished;
    bool m_sortContexts;
    bool m_noUiLines;
    TranslatorSaveMode m_saveMode;
};

class Translator
{
public:
    Translator();

    bool load(const QString &filename, ConversionData &err, const QString &format /*= "auto"*/);
    bool save(const QString &filename, ConversionData &err, const QString &format /*= "auto"*/) const;
    bool release(QFile *iod, ConversionData &cd) const;

    bool contains(const QString &context, const QString &sourceText,
        const QString &comment) const;
    TranslatorMessage find(const QString &context,
        const QString &sourceText, const QString &comment) const;

    TranslatorMessage find(const QString &context,
        const QString &comment, const TranslatorMessage::References &refs) const;

    bool contains(const QString &context) const;
    TranslatorMessage find(const QString &context) const;

    void replace(const TranslatorMessage &msg);
    void replaceSorted(const TranslatorMessage &msg);
    void extend(const TranslatorMessage &msg); // Only for single-location messages
    void append(const TranslatorMessage &msg);
    void appendSorted(const TranslatorMessage &msg);

    void stripObsoleteMessages();
    void stripFinishedMessages();
    void stripEmptyContexts();
    void stripNonPluralForms();
    void stripIdenticalSourceTranslations();
    void dropTranslations();
    QList<TranslatorMessage> findDuplicates() const;
    void makeFileNamesAbsolute(const QDir &originalPath);

    void setCodecName(const QByteArray &name);
    QByteArray codecName() const { return m_codecName; }

    QString languageCode() const { return m_language; }
    QString sourceLanguageCode() const { return m_sourceLanguage; }

    enum LocationsType { NoLocations, RelativeLocations, AbsoluteLocations };
    void setLocationsType(LocationsType lt) { m_locationsType = lt; }
    LocationsType locationsType() const { return m_locationsType; }

    static QString makeLanguageCode(QLocale::Language language, QLocale::Country country);
    static void languageAndCountry(const QString &languageCode,
        QLocale::Language *lang, QLocale::Country *country);
    void setLanguageCode(const QString &languageCode) { m_language = languageCode; }
    void setSourceLanguageCode(const QString &languageCode) { m_sourceLanguage = languageCode; }
    static QString guessLanguageCodeFromFileName(const QString &fileName);
    QList<TranslatorMessage> messages() const;
    QList<TranslatorMessage> translatedMessages() const;
    static QStringList normalizedTranslations(const TranslatorMessage &m,
        QLocale::Language lang, QLocale::Country country);
    QStringList normalizedTranslations(const TranslatorMessage &m, ConversionData &cd, bool *ok) const;

    int messageCount() const { return m_messages.size(); }
    TranslatorMessage &message(int i) { return m_messages[i]; }
    const TranslatorMessage &message(int i) const { return m_messages.at(i); }
    void dump() const;

    // additional file format specific data
    // note: use '<fileformat>:' as prefix for file format specific members,
    // e.g. "po-flags", "po-msgid_plural"
    typedef TranslatorMessage::ExtraData ExtraData;
    QString extra(const QString &ba) const;
    void setExtra(const QString &ba, const QString &var);
    bool hasExtra(const QString &ba) const;
    const ExtraData &extras() const { return m_extra; }
    void setExtras(const ExtraData &extras) { m_extra = extras; }

    // registration of file formats
    typedef bool (*SaveFunction)(const Translator &, QIODevice &out, ConversionData &data);
    typedef bool (*LoadFunction)(Translator &, QIODevice &in, ConversionData &data);
    struct FileFormat {
        QString extension; // such as "ts", "xlf", ...
        QString description; // human-readable description
        LoadFunction loader;
        SaveFunction saver;
        enum FileType { SourceCode, TranslationSource, TranslationBinary } fileType;
        int priority; // 0 = highest, -1 = invisible
    };
    static void registerFileFormat(const FileFormat &format);
    static QList<FileFormat> &registeredFileFormats();

    enum VariantSeparators {
        DefaultVariantSeparator = 0x2762, // some weird character nobody ever heard of :-D
        InternalVariantSeparator = 0x9c // unicode "STRING TERMINATOR"
    };

private:
    typedef QList<TranslatorMessage> TMM;       // int stores the sequence position.

    TMM m_messages;
    QByteArray m_codecName;
    LocationsType m_locationsType;

    // A string beginning with a 2 or 3 letter language code (ISO 639-1
    // or ISO-639-2), followed by the optional country variant to distinguish
    //  between country-specific variations of the language. The language code
    // and country code are always separated by '_'
    // Note that the language part can also be a 3-letter ISO 639-2 code.
    // Legal examples:
    // 'pt'         portuguese, assumes portuguese from portugal
    // 'pt_BR'      Brazilian portuguese (ISO 639-1 language code)
    // 'por_BR'     Brazilian portuguese (ISO 639-2 language code)
    QString m_language;
    QString m_sourceLanguage;
    ExtraData m_extra;
};

bool getNumerusInfo(QLocale::Language language, QLocale::Country country,
                           QByteArray *rules, QStringList *forms);

/*
  This is a quick hack. The proper way to handle this would be
  to extend Translator's interface.
*/
#define ContextComment "QT_LINGUIST_INTERNAL_CONTEXT_COMMENT"

QT_END_NAMESPACE

#endif
