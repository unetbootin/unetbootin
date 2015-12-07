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

#include "tokenengine.h"

QT_BEGIN_NAMESPACE

namespace TokenEngine {

/*
    Add a container-global attribute.
*/
void TokenAttributes::addAttribute(const QByteArray &name,  const QByteArray &value)
{
    const QByteArray keyText = makeKeyText(-1, name);
    attributes.insert(keyText, value);
}

/*
     Retrieve container-global attribute.
*/
QByteArray TokenAttributes::attribute(const QByteArray &name) const
{
    const QByteArray keyText = makeKeyText(-1, name);
    return attributes.value(keyText);
}

/*
    Add an attribute. Note: Don't use names starting with a number, that will
    break the indexing.
*/
void TokenAttributes::addAttribute(const int index, const QByteArray &name, const QByteArray &value)
{
    const QByteArray keyText = makeKeyText(index, name);
    attributes.insert(keyText, value);
}

QByteArray TokenAttributes::attribute(const int index, const QByteArray &name) const
{
    const QByteArray keyText = makeKeyText(index, name);
    return attributes.value(keyText);
}

QByteArray TokenAttributes::makeKeyText(const int index, const QByteArray &name) const
{
    QByteArray indexText;
    return indexText.setNum(index) + name;
}


/*
     Construnct an empty TokenContainer.
*/
TokenContainer::TokenContainer()
{
    d = new TokenContainerData();
    d->typeInfo = 0;
}

/*
    Construnct a TokenContainer from data
*/
TokenContainer::TokenContainer(QByteArray text, QVector<Token> tokens, TypeInfo *typeInfo)
{
    d = new TokenContainerData();
    d->text = text;
    d->tokens = tokens;
    if(typeInfo == 0)
        d->typeInfo = new TypeInfo();
    else
        d->typeInfo = typeInfo;
}

int TokenContainer::count() const
{
    return d->tokens.count();
}

QByteArray TokenContainer::text(const int index) const
{
    Token token = d->tokens.at(index);
    return d->text.mid(token.start, token.length);
}

QByteArray TokenContainer::tempText(const int index) const
{
    Token token = d->tokens.at(index);
    return QByteArray::fromRawData(d->text.constData() + token.start, token.length);
}

QByteArray TokenContainer::fullText() const
{
    return d->text;
}

TokenContainer TokenContainer::tokenContainer(const int index) const
{
    Q_UNUSED(index);
    return *this;
}

Token TokenContainer::token(const int index) const
{
    return d->tokens.at(index);
}

TypeInfo *TokenContainer::typeInfo()
{
    return d->typeInfo;
}

TokenAttributes *TokenContainer::tokenAttributes()
{
    return d->tokenAttributes;
}

const TokenAttributes *TokenContainer::tokenAttributes() const
{
    return d->tokenAttributes;
}

/*
    Returns the line number for the given index.
*/
int TokenContainer::line(int index) const
{
    //This algorithm is a bit more complicated than it should be,
    //since we have to search for newlines inside comments.
    //(Comments are tokenized as one token)
    QByteArray contents = fullText();
    int pos=0;
    int lines=0;
    for(int t=0; t < index; ++t) {
        int tokenLength = d->tokens.at(t).length;
        if((tokenLength == 1) && (text(t) == "\n")) {
            ++lines;
        } else if(tokenLength > 2) {// 3 is the minimum size for a comment (// + newline)
            int newpos = d->tokens.at(t).length;
            for(int p = pos; p < newpos; ++p) {
                if(contents[p] == '\n')
                    ++lines;
            }
        }
        pos += d->tokens.at(t).length;
    }
    return lines;
}

/*
    Returns the column number for the given index.
*/
int TokenContainer::column(int index) const
{
    //Scan backwards, find \n.
    int chars = 0;
    int t = index;
    while (t>0) {
        if((d->tokens.at(t).length == 1) && (text(t) == "\n"))
            break;
        chars += d->tokens.at(t).length;
        --t;
    }
    return chars;
}

TokenTempRef TokenContainer::tokenTempRef(const int index) const
{
    Q_ASSERT(index < d->tokens.count());
    const Token token = d->tokens.at(index);
    return TokenTempRef(d->text.constData() + token.start, token.length);
}

QByteArray TokenSection::fullText() const
{
    QByteArray text;
    for (int t = m_start; t < m_start + m_count; ++t) {
        text += m_tokenContainer.text(t);
    }
    return text;
}

QByteArray TokenList::fullText() const
{
    QByteArray text;
    for (int t = 0; t < m_tokenList.count(); ++t) {
        text += m_tokenContainer.text(m_tokenList.at(t));
    }
    return text;
}

TokenSectionSequence::TokenSectionSequence(QVector<TokenSection> tokenSections)
:m_tokenSections(tokenSections), m_count(0)
{
    for(int s = 0; s < m_tokenSections.count(); ++s) {
        m_startIndexes.append(m_count);
        m_count += m_tokenSections.at(s).count();
    }
}

QByteArray TokenSectionSequence::fullText() const
{
    QByteArray text;
    foreach(TokenSection section, m_tokenSections) {
        text += section.fullText();
    }
    return text;
}

int TokenSectionSequence::count() const
{
    return m_count;
}

QVector<TokenSection> TokenSectionSequence::tokenSections() const
{
    return m_tokenSections;
}

QByteArray TokenSectionSequence::text(const int index) const
{
    const int sectionIndex = findSection(index);
    const int sectionInternalIndex = calculateInternalIndex(index, sectionIndex);
    return m_tokenSections.at(sectionIndex).text(sectionInternalIndex);
}

QByteArray TokenSectionSequence::tempText(const int index) const
{
    const int sectionIndex = findSection(index);
    const int sectionInternalIndex = calculateInternalIndex(index, sectionIndex);
    return m_tokenSections.at(sectionIndex).tempText(sectionInternalIndex);
}


TokenContainer TokenSectionSequence::tokenContainer(const int index) const
{
    const int sectionIndex = findSection(index);
    const int sectionInternalIndex = calculateInternalIndex(index, sectionIndex);
    return m_tokenSections.at(sectionIndex).tokenContainer(sectionInternalIndex);
}

int TokenSectionSequence::containerIndex(const int index) const
{
    const int sectionIndex = findSection(index);
    const int sectionInternalIndex = calculateInternalIndex(index, sectionIndex);
    return m_tokenSections.at(sectionIndex).containerIndex(sectionInternalIndex);
}

int TokenSectionSequence::findSection(const int index) const
{
    int c = 0;
    bool found = false;
    //Here we do a linear search through all collections in the list,
    //which could turn out to be to slow.
    while(!found && c < m_tokenSections.count()) {
        const int sectionEnd = m_startIndexes.at(c)
                                + m_tokenSections.at(c).count();
        if (index < sectionEnd)
            found = true;
        else
            ++c;
    }
    if(!found) {
        //error
        Q_ASSERT(0);
        return -1;
    }
    Q_ASSERT(c < m_tokenSections.count());
    return c;
}

int TokenSectionSequence::calculateInternalIndex(const int index, const int sectionIndex) const
{
    const int sectionInternalIndex =
        index - m_startIndexes.at(sectionIndex);
    Q_ASSERT(sectionInternalIndex < m_tokenSections.at(sectionIndex).count());
    return sectionInternalIndex;
}


TokenSectionSequenceIterator::TokenSectionSequenceIterator(const TokenSectionSequence &tokenSectionSequence)
:m_currentSection(0)
,m_currentToken(-1)
,m_numSections(tokenSectionSequence.m_tokenSections.count())
,m_tokenSectionSequence(tokenSectionSequence)

{ }

/*
    Resets the token iterator.
*/
void TokenSectionSequenceIterator::reset()
{
    m_currentSection = 0;
    m_currentToken = -1;
}

/*
    Advances the token iterator.
*/
bool TokenSectionSequenceIterator::nextToken()
{
    if(m_tokenSectionSequence.m_tokenSections.isEmpty())
        return false;
    ++m_currentToken;
    //check of we are past the end of the current section
    if(m_currentToken < m_tokenSectionSequence.m_tokenSections.at(m_currentSection).count())
        return true;
    //Advance to the next non-empty section.
    m_currentToken = 0;
    do {
        ++m_currentSection;
        //return false if no more sections
        if(m_currentSection >= m_numSections)
            return false;
    //skip empty sections
    } while(m_tokenSectionSequence.m_tokenSections.at(m_currentSection).count() == 0);

    return true;
}

/*
    Returns the text for the current token;
*/
QByteArray TokenSectionSequenceIterator::text() const
{
    return m_tokenSectionSequence.m_tokenSections.at(m_currentSection).text(m_currentToken);
}

/*
    Returns the text for the current token as a temporary QByteArray;
*/
QByteArray TokenSectionSequenceIterator::tempText() const
{
    return m_tokenSectionSequence.m_tokenSections.at(m_currentSection).tempText(m_currentToken);
}

/*
    Returns the container for the current token;
*/
TokenContainer TokenSectionSequenceIterator::tokenContainer() const
{
    return m_tokenSectionSequence.m_tokenSections.at(m_currentSection).tokenContainer(m_currentToken);
}

/*
    Returns the containerIndex for the current token;
*/
int TokenSectionSequenceIterator::containerIndex() const
{
    return m_tokenSectionSequence.m_tokenSections.at(m_currentSection).containerIndex(m_currentToken);
}

/*
    Returns a temporary token reference for the current token for the current token;
*/
TokenTempRef TokenSectionSequenceIterator::tokenTempRef() const
{
    return m_tokenSectionSequence.m_tokenSections.at(m_currentSection).tokenTempRef(m_currentToken);
}


} //namespace TokenEngine

QT_END_NAMESPACE
