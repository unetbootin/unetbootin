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

#ifndef TRANSLATORMESSAGE_H
#define TRANSLATORMESSAGE_H

#ifndef QT_NO_TRANSLATION

#include <QString>
#include <QStringList>
#include <QHash>


QT_BEGIN_NAMESPACE

enum TranslatorSaveMode { SaveEverything, SaveStripped };

class TranslatorMessage
{
public:
    enum Type { Unfinished, Finished, Obsolete };
    typedef QHash<QString, QString> ExtraData;
    class Reference
    {
        QString m_fileName;
        int m_lineNumber;
    public:
        Reference(const QString &n, int l) : m_fileName(n), m_lineNumber(l) {}
        bool operator==(const Reference &other) const
            { return fileName() == other.fileName() && lineNumber() == other.lineNumber(); }
        QString fileName() const { return m_fileName; }
        int lineNumber() const { return m_lineNumber; }
    };
    typedef QList<Reference> References;

    TranslatorMessage();
    TranslatorMessage(const QString &context, const QString &sourceText,
        const QString &comment, const QString &userData,
        const QString &fileName, int lineNumber,
        const QStringList &translations = QStringList(),
        Type type = Unfinished, bool plural = false);

    uint hash() const;

    QString id() const { return m_id; }
    void setId(const QString &id) { m_id = id; }

    QString context() const { return m_context; }
    void setContext(const QString &context) { m_context = context; }

    QString sourceText() const { return m_sourcetext; }
    void setSourceText(const QString &sourcetext) { m_sourcetext = sourcetext; }
    QString oldSourceText() const { return m_oldsourcetext; }
    void setOldSourceText(const QString &oldsourcetext) { m_oldsourcetext = oldsourcetext; }

    QString comment() const { return m_comment; }
    void setComment(const QString &comment) { m_comment = comment; }
    QString oldComment() const { return m_oldcomment; }
    void setOldComment(const QString &oldcomment) { m_oldcomment = oldcomment; }

    QStringList translations() const { return m_translations; }
    void setTranslations(const QStringList &translations) { m_translations = translations; }
    QString translation() const { return m_translations.value(0); }
    void setTranslation(const QString &translation) { m_translations = QStringList(translation); }
    void appendTranslation(const QString &translation) { m_translations.append(translation); }
    bool isTranslated() const
    {
        foreach (const QString &trans, m_translations)
            if (!trans.isEmpty())
                return true;
        return false;
    }

    bool operator==(const TranslatorMessage& m) const;
    bool operator<(const TranslatorMessage& m) const;

    QString fileName() const { return m_fileName; }
    void setFileName(const QString &fileName) { m_fileName = fileName; }
    int lineNumber() const { return m_lineNumber; }
    void setLineNumber(int lineNumber) { m_lineNumber = lineNumber; }
    void clearReferences();
    void setReferences(const References &refs);
    void addReference(const QString &fileName, int lineNumber);
    void addReference(const Reference &ref) { addReference(ref.fileName(), ref.lineNumber()); }
    void addReferenceUniq(const QString &fileName, int lineNumber);
    References extraReferences() const { return m_extraRefs; }
    References allReferences() const;
    QString userData() const { return m_userData; }
    void setUserData(const QString &userData) { m_userData = userData; }
    QString extraComment() const { return m_extraComment; }
    void setExtraComment(const QString &extraComment) { m_extraComment = extraComment; }
    QString translatorComment() const { return m_translatorComment; }
    void setTranslatorComment(const QString &translatorComment) { m_translatorComment = translatorComment; }

    bool isNull() const { return m_sourcetext.isNull() && m_lineNumber == -1 && m_translations.isEmpty(); }

    Type type() const { return m_type; }
    void setType(Type t) { m_type = t; }
    bool isUtf8() const { return m_utf8; } // codecForTr override
    void setUtf8(bool on) { m_utf8 = on; }
    bool isPlural() const { return m_plural; }
    void setPlural(bool isplural) { m_plural = isplural; }

    // note: use '<fileformat>:' as prefix for file format specific members,
    // e.g. "po-msgid_plural"
    QString extra(const QString &ba) const;
    void setExtra(const QString &ba, const QString &var);
    bool hasExtra(const QString &ba) const;
    const ExtraData &extras() const { return m_extra; }
    void setExtras(const ExtraData &extras) { m_extra = extras; }
    void unsetExtra(const QString &key);

    bool needs8Bit() const;
    void dump() const;

private:
    QString     m_id;
    QString     m_context;
    QString     m_sourcetext;
    QString     m_oldsourcetext;
    QString     m_comment;
    QString     m_oldcomment;
    QString     m_userData;
    ExtraData   m_extra; // PO flags, PO plurals
    QString     m_extraComment;
    QString     m_translatorComment;
    QStringList m_translations;
    QString     m_fileName;
    int         m_lineNumber;
    References  m_extraRefs;

    Type m_type;
    bool m_utf8;
    bool m_plural;
};

int qHash(const TranslatorMessage &msg);

QT_END_NAMESPACE

#endif // QT_NO_TRANSLATION

#endif // TRANSLATORMESSAGE_H
