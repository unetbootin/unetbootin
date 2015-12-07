/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
#ifndef Patternist_XQueryTokenizer_H
#define Patternist_XQueryTokenizer_H

#include <QHash>
#include <QSet>
#include <QStack>
#include <QString>
#include <QUrl>

#include "qtokenizer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    struct TokenMap;

    /**
     * @short A hand-written tokenizer which tokenizes XQuery 1.0 & XPath 2.0,
     * and delivers tokens to the Bison generated parser.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class XQueryTokenizer : public Tokenizer
    {
    public:
        /**
         * Tokenizer states. Organized alphabetically.
         */
        enum State
        {
            AfterAxisSeparator,
            AposAttributeContent,
            Axis,
            Default,
            ElementContent,
            EndTag,
            ItemType,
            KindTest,
            KindTestForPI,
            NamespaceDecl,
            NamespaceKeyword,
            OccurrenceIndicator,
            Operator,
            Pragma,
            PragmaContent,
            ProcessingInstructionContent,
            ProcessingInstructionName,
            QuotAttributeContent,
            StartTag,
            VarName,
            XMLComment,
            XMLSpaceDecl,
            XQueryVersion
        };

        XQueryTokenizer(const QString &query,
                        const QUrl &location,
                        const State startingState = Default);

        virtual Token nextToken(YYLTYPE *const sourceLocator);
        virtual int commenceScanOnly();
        virtual void resumeTokenizationFrom(const int position);

        /**
         * Does nothing.
         */
        virtual void setParserContext(const ParserContext::Ptr &parseInfo);

    private:

        /**
         * Returns the character corresponding to the builtin reference @p
         * reference. For instance, passing @c gt will give you '>' in return.
         *
         * If @p reference is an invalid character reference, a null QChar is
         * returned.
         *
         * @see QChar::isNull()
         */
        QChar charForReference(const QString &reference);

        inline Token tokenAndChangeState(const TokenType code,
                                         const State state,
                                         const int advance = 1);
        inline Token tokenAndChangeState(const TokenType code,
                                         const QString &value,
                                         const State state);
        inline Token tokenAndAdvance(const TokenType code,
                                     const int advance = 1);
        QString tokenizeCharacterReference();

        inline Token tokenizeStringLiteral();
        inline Token tokenizeNumberLiteral();
        inline char peekAhead(const int length = 1) const;
        inline bool aheadEquals(const char *const chs,
                                const int len,
                                const int offset = 1) const;
        inline Token tokenizeNCName();
        static inline bool isOperatorKeyword(const TokenType);

        static inline bool isDigit(const char ch);
        static inline Token error();
        inline TokenType consumeWhitespace();
        inline char peekCurrent() const;
        static inline bool isNCNameStart(const QChar ch);
        static inline bool isNCNameBody(const QChar ch);
        static inline const TokenMap *lookupKeyword(const QString &keyword);
        inline const QChar current() const;
        inline void popState();
        inline void pushState(const State state);
        inline State state() const;
        inline void setState(const State s);
        static bool isTypeToken(const TokenType t);

        inline Token tokenizeNCNameOrQName();
        /**
         * Advances m_pos until content is encountered.
         *
         * Returned is the length stretching from m_pos when starting, until
         * @p content is encountered. @p content is not included in the length.
         */
        int scanUntil(const char *const content);

        /**
         * Same as calling:
         * @code
         * pushState(currentState());
         * @endcode
         */
        inline void pushState();

        /**
         * Consumes only whitespace, in the traditional sense. The function exits
         * if non-whitespace is encountered, such as the start of a comment.
         *
         * @returns @c true if the end was reached, otherwise @c false
         */
        inline bool consumeRawWhitespace();

        /**
         * @short Parses comments: <tt>(: comment content :)</tt>. It recurses for
         * parsing nested comments.
         *
         * It is assumed that the start token for the comment, "(:", has
         * already been parsed.
         *
         * Typically, don't call this function, but ignoreWhitespace().
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#comments">XML Path Language (XPath)
         * 2.0, 2.6 Comments</a>
         * @returns
         * - SUCCESS if everything went ok
         * - ERROR if there was an error in parsing one or more comments
         * - END_OF_FILE if the end was reached
         */
        Tokenizer::TokenType consumeComment();

        /**
         * Determines whether @p code is a keyword
         * that is followed by a second keyword. For instance <tt>declare
         * function</tt>.
         */
        static inline bool isPhraseKeyword(const TokenType code);

        /**
         * A set of indexes into a QString, the one being passed to
         * normalizeEOL() whose characters shouldn't be normalized. */
        typedef QSet<int> CharacterSkips;

        /**
         * Returns @p input, normalized according to
         * <a href="http://www.w3.org/TR/xquery/#id-eol-handling">XQuery 1.0:
         * An XML Query Language, A.2.3 End-of-Line Handling</a>
         */
        static QString normalizeEOL(const QString &input,
                                    const CharacterSkips &characterSkips);

        inline bool atEnd() const
        {
            return m_pos == m_length;
        }

        Token nextToken();
        /**
         * Instead of recognizing and tokenizing embedded expressions in
         * direct attriute constructors, this function is essentially a mini
         * recursive-descent parser that has the necessary logic to recognize
         * embedded expressions and their potentially interfering string literals, in
         * order to scan to the very end of the attribute value, and return the
         * whole as a string.
         *
         * There is of course syntax errors this function will not detect, but
         * that is ok since the attributes will be parsed once more.
         *
         * An inelegant solution, but which gets the job done.
         *
         * @see commenceScanOnly(), resumeTokenizationFrom()
         */
        Token attributeAsRaw(const QChar separator,
                             int &stack,
                             const int startPos,
                             const bool inLiteral,
                             QString &result);

        const QString           m_data;
        const int               m_length;
        State                   m_state;
        QStack<State>           m_stateStack;
        int                     m_pos;

        /**
         * The current line number.
         *
         * The line number and column number both starts at 1.
         */
        int                     m_line;

        /**
         * The offset into m_length for where
         * the current column starts. So m_length - m_columnOffset
         * is the current column.
         *
         * The line number and column number both starts at 1.
         */
        int                     m_columnOffset;

        const NamePool::Ptr     m_namePool;
        QStack<Token>           m_tokenStack;
        QHash<QString, QChar>   m_charRefs;
        bool                    m_scanOnly;

        Q_DISABLE_COPY(XQueryTokenizer)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
