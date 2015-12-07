/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef TOKENENGINE_H
#define TOKENENGINE_H

#include <QByteArray>
#include <QVector>
#include <QString>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QMap>

QT_BEGIN_NAMESPACE

namespace TokenEngine {

class TokenContainer;
/*
    A token is defined as a start-postion and a lenght. Since the actual text
    storage is not reffered to here, Token needs to be used together with
    a TokenContainer in order to be useful.
*/
class Token
{
public:
    Token()
    :start(0), length(0) {}
    Token(int p_start, int p_lenght)
    :start(p_start), length(p_lenght) {}
    int start;
    int length;
};

/*
    Each TokenContainer has a TypeInfo object with meta-information.
*/
class FileInfo;
class GeneratedInfo;
class TypeInfo
{
public:
    virtual ~TypeInfo() {};
    virtual FileInfo *toFileInfo() const {return 0;}
    virtual GeneratedInfo *toGeneratedInfo() const {return 0;}
};

/*
    MetaInfo for containers that contains tokens from a file
*/
class FileInfo: public TypeInfo
{
public:
    FileInfo *toFileInfo() const
    {return const_cast<FileInfo *>(this);}

    QString filename;
};

/*
    MetaInfo for containers that contains generated tokens.
*/
class GeneratedInfo: public TypeInfo
{
public:
    GeneratedInfo *toGeneratedInfo() const
    {return const_cast<GeneratedInfo *>(this);}

    //preprocessor tree pointer?
};

class TokenAttributes
{
public:
    void addAttribute(const QByteArray &name,  const QByteArray &value);
    QByteArray attribute(const QByteArray &name) const;
    void addAttribute(const int index, const QByteArray &name,  const QByteArray &value);
    QByteArray attribute(const int index, const QByteArray &name) const;

private:
    inline QByteArray makeKeyText(const int index, const QByteArray &name) const;
    QMap<QByteArray, QByteArray> attributes;
};


/*
    A TokenSequence that stores text and tokens referencing
    that text.
*/
class TokenContainerData : public QSharedData
{
public:
    TokenContainerData()
    {tokenAttributes = new TokenAttributes();}
    ~TokenContainerData()
    {delete tokenAttributes; delete typeInfo; }
    QByteArray text;
    QVector<Token> tokens;
    TypeInfo *typeInfo;
    TokenAttributes *tokenAttributes;
};
class TokenTempRef;
class TokenContainer
{
public:
    TokenContainer();
    TokenContainer(QByteArray text, QVector<Token> tokens, TypeInfo *typeInfo = 0);
    int count() const;
    QByteArray text(const int index) const;
    QByteArray tempText(const int index) const;
    QByteArray fullText() const;
    TokenContainer tokenContainer(const int index) const;
    inline int containerIndex(const int index) const
    { return index; }
    Token token(const int index) const;
    TypeInfo *typeInfo();
    TokenAttributes *tokenAttributes();
    const TokenAttributes *tokenAttributes() const;
    int line(int index) const;
    int column(int index) const;
    TokenTempRef tokenTempRef(const int index) const;
private:
    const QByteArray &textRef()
    { return d->text; }
    QExplicitlySharedDataPointer<TokenContainerData> d;
};

/*
    A reference to a single token in a container
*/
class TokenRef
{
public:
    TokenRef(): m_index(-1) {}
    TokenRef(TokenContainer tokenContainer, int containerIndex)
    : m_tokenContainer(tokenContainer), m_index(containerIndex) {}
    inline int count() const
    { return m_index == -1 ? 0 : 1; }
    inline QByteArray text(const int index = 0) const
    { Q_UNUSED(index); return m_tokenContainer.text(m_index);  }
    inline QByteArray tempText(const int index) const
    { Q_UNUSED(index); return m_tokenContainer.tempText(m_index);  }
    inline QByteArray fullText() const
    { return text(); }
    inline TokenContainer tokenContainer(const int index = 0) const
    { Q_UNUSED(index); return m_tokenContainer; }
    inline int containerIndex(const int index = 0) const
    { Q_UNUSED(index); return m_index; }
private:
    TokenContainer m_tokenContainer;
    int m_index;
};

/*
    A temporary reference to a single token in a container. This reference does
    not increase the refcount on the TokenContainer.
*/
class TokenTempRef
{
public:
    TokenTempRef(const char *text, int length)
    : m_text(text), m_length(length) {}
    inline const char *constData() const
    { return m_text; }
    inline int length() const
    { return m_length; }
    char at(int index) const
    { Q_ASSERT(index < m_length);  return m_text[index]; }
private:
    const char *m_text;
    int m_length;
};

/*
    Contains a selected range from a TokenSequence.
*/
class TokenSection
{
public:
    TokenSection() : m_start(0), m_count(0) {}
    TokenSection(TokenContainer tokenContainer,
        const int start, const int count)
    :m_tokenContainer(tokenContainer), m_start(start), m_count(count) {}

    inline int count() const
    { return m_count; }
    inline QByteArray text(const int index) const
    {
        const int cIndex = containerIndex(index);
        Q_ASSERT(cIndex < m_tokenContainer.count());
        return m_tokenContainer.text(cIndex);
    }
    inline QByteArray tempText(const int index) const
    {
        const int cIndex = containerIndex(index);
        Q_ASSERT(cIndex < m_tokenContainer.count());
        return m_tokenContainer.tempText(cIndex);
    }
    QByteArray fullText() const;
    inline TokenContainer tokenContainer(const int index = 0) const
    { Q_UNUSED(index); return m_tokenContainer; }
    inline int containerIndex(const int index) const
    { return m_start + index; }
    TokenTempRef tokenTempRef(const int index) const
    {
        const int cIndex = containerIndex(index);
        Q_ASSERT(cIndex < m_tokenContainer.count());
        return m_tokenContainer.tokenTempRef(cIndex);
    }
private:
    TokenContainer m_tokenContainer;
    int m_start;
    int m_count;
};

/*
    A list of tokens from a tokenContainer
*/
class TokenList
{
public:
    TokenList() {};
    TokenList(TokenContainer tokenContainer, QVector<int> tokenList)
    :m_tokenContainer(tokenContainer), m_tokenList(tokenList) {}
    inline int count() const
    { return m_tokenList.count(); }
    inline QByteArray text(const int index) const
    {
        const int cIndex = containerIndex(index);
        Q_ASSERT(cIndex < m_tokenContainer.count());
        return m_tokenContainer.text(cIndex);
    }
    inline QByteArray tempText(const int index) const
    {
        const int cIndex = containerIndex(index);
        Q_ASSERT(cIndex < m_tokenContainer.count());
        return m_tokenContainer.tempText(cIndex);
    }
    QByteArray fullText() const;
    inline TokenContainer tokenContainer(const int index) const
    { Q_UNUSED(index); return m_tokenContainer; }
    inline int containerIndex(const int index) const
    { return m_tokenList.at(index); }
    Token token(const int index) const
    {
        const int cIndex = containerIndex(index);
        Q_ASSERT(cIndex < m_tokenContainer.count());
        return m_tokenContainer.token(cIndex);
    }

private:
    TokenContainer m_tokenContainer;
    QVector<int> m_tokenList;
};

/*
    Combines a list of TokenSequences into one TokenSectionSequence
*/
class TokenSectionSequenceIterator;
class TokenSectionSequence
{
public:
    TokenSectionSequence() :m_count(0) {};
    TokenSectionSequence(QVector<TokenSection> tokenSections);

    QByteArray fullText() const;
    int count() const;
    QVector<TokenSection> tokenSections() const;

    //random access interface, access time is linear on the number of sections
    QByteArray text(const int index) const;
    QByteArray tempText(const int index) const;
    TokenContainer tokenContainer(const int index) const;
    int containerIndex(const int index) const;

protected:
    int findSection(const int index) const;
    int calculateInternalIndex(const int index, const int sectionIndex) const;
private:
    QVector<TokenSection> m_tokenSections;
    QVector<int> m_startIndexes;
    int m_count;
    friend class TokenSectionSequenceIterator;
};

//sequental access interface, constant access time.
class TokenSectionSequenceIterator
{
public:
    TokenSectionSequenceIterator(const TokenSectionSequence &tokenSectionSequence);
    void reset();
    bool nextToken();
    QByteArray text() const;
    QByteArray tempText() const;
    TokenContainer tokenContainer() const;
    int containerIndex() const;
    TokenTempRef tokenTempRef() const;
private:
    int m_currentSection;
    int m_currentToken;    // token index in currentTokenSequence;
    const int m_numSections;
    const TokenSectionSequence &m_tokenSectionSequence;
};

template <typename TokenSequence>
QByteArray getText(TokenSequence tokenSequence)
{
    QByteArray text;
    for (int t = 0; t<tokenSequence.count(); ++t) {
        text += tokenSequence.text(t);
    }
    return text;
}

/*
    Append the text and the tokens from the range [startToken, startToken + numTokens>
    to text and tokenList.
*/
template <typename TokenSequenceType>
void copy(QByteArray &text, QVector<TokenEngine::Token> &tokenList, const TokenSequenceType &tokenSequence, int startToken, int numTokens)
{
    const int endToken = startToken + numTokens;
    int textIndex = text.count();
    for(int t = startToken; t < endToken; ++t) {
        const QByteArray tokenText = tokenSequence.text(t);
        const int tokenLength = tokenText.count();
        TokenEngine::Token token(textIndex, tokenLength);
        tokenList.append(token);
        text += tokenSequence.text(t);
        textIndex += tokenText.count();
    }
}

/*
    Copy a the range [startToken, startToken + numTokens> from a tokenSequence to a new
    TokenConrtainer.
*/
template <typename TokenSequenceType>
TokenContainer copy(const TokenSequenceType &tokenSequence, int startToken, int numTokens)
{
    QByteArray containerText;
    QVector<Token> tokens;
    tokens.reserve(numTokens);
    TokenEngine::copy(containerText, tokens, tokenSequence, startToken, numTokens);
    return TokenContainer(containerText, tokens);
}

} //namespace TokenEngine

QT_END_NAMESPACE

#endif
