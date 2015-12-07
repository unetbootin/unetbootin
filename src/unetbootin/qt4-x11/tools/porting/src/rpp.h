/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
** Copyright (C) 2001-2004 Roberto Raggi
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

#ifndef RPP_H
#define RPP_H

#include "tokenengine.h"
#include "rpplexer.h"
#include "tokens.h"
#include "smallobject.h"
#include <QHash>
#include <QStringList>
#include <QFile>
#include <QByteArray>
#include <QDir>
#include <QMultiMap>
#include <ctype.h>

QT_BEGIN_NAMESPACE

namespace Rpp
{

struct Item;
struct ItemComposite;

struct Source;

struct Directive;
struct EmptyDirective;
struct ErrorDirective;
struct PragmaDirective;
struct IncludeDirective;
struct ConditionalDirective;
struct DefineDirective;
struct UndefDirective;
struct LineDirective;
struct NonDirective;

struct IfSection;
struct IfLikeDirective;
struct IfDirective;
struct ElifDirective;
struct IfdefLikeDirective;
struct IfdefDirective;
struct IfndefDirective;
struct ElseDirective;
struct EndifDirective;

struct Text;
struct Token;
struct TokenComposite;
struct IdToken;
struct NonIdToken;
struct PastingToken;
struct LineComment;
struct MultiLineComment;
struct WhiteSpace;

struct MacroDefinition;
struct MacroFunctionDefinition;
struct MacroParameters;
struct MacroParameter;

struct Expression;
struct UnaryExpression;
struct BinaryExpression;
struct ConditionalExpression;

struct StringLiteral;
struct IntLiteral;
struct MacroReference;
struct MacroFunctionReference;
struct MacroArguments;
struct MacroArgument;

struct Item
{
    virtual ~Item() {}

    virtual Item *parent() const = 0;

    virtual ItemComposite *toItemComposite() const
    { return 0; }

    virtual Item *toItem() const
    { return const_cast<Item *>(this); }

    virtual Directive *toDirective() const
    { return 0; }

    virtual Text *toText() const
    { return 0; }

    virtual Token *toToken() const
    { return 0; }

    virtual Source *toSource() const
    { return 0; }

    virtual Expression *toExpression() const
    { return 0; }

    virtual IfSection *toIfSection() const
    { return 0; }

    // Text returns the original text for an item, e.g.
    // the way it is found in the source
    virtual TokenEngine::TokenSection text() const
     { return TokenEngine::TokenSection(); }

protected:
    //using the default constructor for an item is
    //only allowded for subclasses.
    Item() {};
};

struct ItemComposite
{
    virtual ~ItemComposite() {}
    virtual int count() const = 0;
    virtual Item *item(int index) const = 0;
    virtual void add(Item *item) = 0;
/*
    Classes that inherit ItemComposite must implement this
    function themselves
    virtual ItemComposite *toItemComposite() const
    { return const_cast<ItemComposite *>(this); }
*/
};

struct Directive: public Item
{
    inline Directive(Item *parent = 0)
        : m_parent(parent), m_numLines(0) {}

    virtual Item *parent() const
    { return m_parent; }

    inline void setParent(Item *parent)
    { m_parent = parent;}

    void setNumLines(const int numLines)
    {m_numLines = numLines;}

    virtual Directive *toDirective() const
    { return const_cast<Directive *>(this); }

    virtual EmptyDirective *toEmptyDirective() const
    { return 0; }

    virtual ErrorDirective *toErrorDirective() const
    { return 0; }

    virtual PragmaDirective *toPragmaDirective() const
    { return 0; }

    virtual IncludeDirective *toIncludeDirective() const
    { return 0; }

    virtual ConditionalDirective *toConditionalDirective() const
    { return 0; }

    virtual DefineDirective *toDefineDirective() const
    { return 0; }

    virtual UndefDirective *toUndefDirective() const
    { return 0; }

    virtual LineDirective *toLineDirective() const
    { return 0; }

    virtual NonDirective *toNonDirective() const
    { return 0; }

    void setTokenSection(TokenEngine::TokenSection section)
    { m_tokenSection = section; }

    TokenEngine::TokenSection text() const
    { return m_tokenSection; }

protected:
    Item *m_parent;
    int m_numLines;
    TokenEngine::TokenSection m_tokenSection;
};


struct Token: public Item
{
    inline Token(Item *parent = 0)
        : m_parent(parent) {}

    virtual Item *parent() const
    { return m_parent; }

    virtual MacroArguments *toMacroArguments() const
    { return 0; }

    virtual IdToken *toIdToken() const
    { return 0; }

    virtual NonIdToken *toNonIdToken() const
    { return 0; }

    virtual LineComment *toLineComment() const
    { return 0; }

    virtual MultiLineComment *toMultiLineComment() const
    { return 0; }

    virtual WhiteSpace *toWhiteSpace() const
    { return 0; }

    virtual Token *toToken() const
    { return const_cast<Token *>(this); }

    void setToken(int tokenIndex)
    { m_tokenIndex = tokenIndex;}

    int index() const
    { return m_tokenIndex; }

protected:
    int m_tokenIndex;
    Item *m_parent;
};

struct Text: public Item
{
    inline Text(Item *parent = 0)
        : m_parent(parent) {}

    virtual Text *toText() const
    { return const_cast<Text *>(this); }

    virtual Item *parent() const
    { return m_parent; }

    void setTokenSection(TokenEngine::TokenSection tokenSection)
    {m_tokenSection = tokenSection; }

    TokenEngine::TokenSection text() const
    { return m_tokenSection; }

    QVector<TokenEngine::TokenSection> cleanedText() const
    { return m_cleanedSection; }

    void setTokens( const QVector<Token *> &tokens )
    { m_tokens = tokens; }

    void addToken(Token *token)
    {m_tokens.append(token);}

    Token *token(int index) const
    {return m_tokens.at(index);}

    inline int count() const
    {return m_tokens.count();}

    QVector<Token *> tokenList() const
    { return m_tokens; }

protected:
    Item *m_parent;
    TokenEngine::TokenSection m_tokenSection;    // all tokens
    QVector<TokenEngine::TokenSection> m_cleanedSection; //comments removed
    QVector<Token *> m_tokens;
};

struct IdToken: public Token
{
    inline IdToken(Item *parent = 0)
        : Token(parent) {}

    virtual IdToken *toIdToken() const
    { return const_cast<IdToken *>(this); }
};

struct NonIdToken: public Token
{
    inline  NonIdToken(Item *parent = 0)
        : Token(parent) {}

    virtual NonIdToken *toNonIdToken() const
    { return const_cast< NonIdToken *>(this); }
};

struct LineComment : public NonIdToken
{
    inline LineComment(Item *parent = 0)
       : NonIdToken(parent) {}

    virtual LineComment *toLineComment() const
    { return const_cast< LineComment *>(this); }
};

struct MultiLineComment: public NonIdToken
{
    inline MultiLineComment(Item *parent = 0)
        : NonIdToken(parent) {}

    virtual MultiLineComment *toMultiLineComment() const
    { return const_cast< MultiLineComment *>(this); }
protected:
};

struct WhiteSpace: public NonIdToken
{
    inline WhiteSpace(Item *parent = 0)
        : NonIdToken(parent) {}

    virtual WhiteSpace *toWhiteSpace() const
    { return const_cast<WhiteSpace *>(this); }
};

struct Source: public Item, public ItemComposite
{
    Source(Item *parent = 0)
        :m_parent(parent) {}

    virtual Source *toSource() const
    { return const_cast<Source *>(this); }

    ItemComposite *toItemComposite() const
    { return const_cast<Source *>(this); }

    virtual int count() const
    { return m_items.count(); }

    virtual Item *item(int index) const
    {  return m_items.at(index); }

    inline QString fileName() const
    { return m_fileName; }

    void setFileName(const QString &fileName);

    virtual Item *parent() const
    { return m_parent; }

    inline void add(Item *item)
    {  m_items.append(item); }

private:
    Item *m_parent;
    QVector<Item *> m_items;
    QString m_fileName;
};

struct EmptyDirective: public Directive
{
    EmptyDirective(Item *item)
    : Directive(item) {}

    virtual EmptyDirective *toEmptyDirective() const
    { return const_cast<EmptyDirective *>(this); }
};

struct ErrorDirective: public Directive
{
    ErrorDirective(Item *item)
    : Directive(item) {}

    virtual ErrorDirective *toErrorDirective() const
    { return const_cast<ErrorDirective *>(this); }
};

struct PragmaDirective: public Directive
{
    PragmaDirective(Item *item)
    : Directive(item) {}

    virtual PragmaDirective *toPragmaDirective() const
    { return const_cast<PragmaDirective *>(this); }
};

struct IncludeDirective: public Directive
{
    IncludeDirective(Item *item)
    : Directive(item) {}

    IncludeDirective() : Directive() {}

    virtual IncludeDirective *toIncludeDirective() const
    { return const_cast<IncludeDirective *>(this); }

    enum IncludeType {QuoteInclude, AngleBracketInclude};

    void setFilenameTokens(const TokenEngine::TokenList &filenameTokens)
    { m_filenameTokens = filenameTokens; }

    TokenEngine::TokenList filenameTokens() const
    { return m_filenameTokens; }

    void setFilename(const QByteArray &filename)
    { m_filename = filename; }

    QByteArray filename() const
    { return m_filename;}

    void setIncludeType(IncludeType includeType)
    { m_includeType = includeType; }

    IncludeType includeType() const
    { return m_includeType; }
private:
    TokenEngine::TokenList m_filenameTokens;
    QByteArray m_filename;
    IncludeType m_includeType;
};

struct ConditionalDirective: public Directive, public ItemComposite
{
    inline ConditionalDirective(Item *parent = 0)
    :Directive(parent) {}

    virtual ConditionalDirective *toConditionalDirective() const
    { return const_cast<ConditionalDirective *>(this); }

    ItemComposite *toItemComposite() const
    { return const_cast<ConditionalDirective *>(this); }

    virtual IfDirective *toIfDirective() const
    { return 0; }

    virtual IfdefDirective *toIfdefDirective() const
    { return 0; }

    virtual IfndefDirective *toIfndefDirective() const
    { return 0; }

    virtual ElifDirective *toElifDirective() const
    { return 0; }

    virtual ElseDirective *toElseDirective() const
    { return 0; }

    int count() const
    { return m_items.count(); }

    Item *item(int index) const
    { return m_items.at(index); }

    void add(Item *item)
    { m_items.append(item); }
protected:
    QVector<Item *> m_items;
};

struct IfSection: public Item, public ItemComposite
{
    IfSection(Item *parent)
    :m_parent(parent), m_ifGroup(0), m_elseGroup(0), m_endifLine(0) {}

    IfSection *toIfSection() const
    { return const_cast<IfSection *>(this); }

    ItemComposite *toItemComposite() const
    { return const_cast<IfSection *>(this); }

    void setParent(Item *parent)
    { m_parent = parent; }

    Item *parent() const
    { return m_parent; }

    void setIfGroup(ConditionalDirective *ifGroup)
    { m_ifGroup = ifGroup; m_items.append(ifGroup); }

    ConditionalDirective *ifGroup() const
    { return m_ifGroup; }

    void addElifGroup(ConditionalDirective *elifGroup)
    { m_elifGroups.append(elifGroup); m_items.append(elifGroup); }

    QVector<ConditionalDirective *> elifGroups() const
    { return m_elifGroups; }

    void setElseGroup(ConditionalDirective *elseGroup)
    { m_elseGroup = elseGroup; m_items.append(elseGroup); }

    ConditionalDirective *elseGroup() const
    { return m_elseGroup; }

    void setEndifLine(Directive *endifLine)
    { m_endifLine = endifLine; m_items.append(endifLine); }

    Directive *endifLine() const
    { return m_endifLine; }

    int count() const
    { return m_items.count(); }

    Item *item(int index) const
    { return m_items.at(index);}

 private:
    void add(Item *item)
    { Q_UNUSED(item); }

    Item *m_parent;
    QVector<Item *> m_items;
    ConditionalDirective *m_ifGroup;
    QVector<ConditionalDirective *> m_elifGroups;
    ConditionalDirective *m_elseGroup;
    Directive *m_endifLine;
};

struct Expression: public Item
{
    enum Operator
    {
        LtEqOp = 300,
        GtEqOp,
        LtOp,
        GtOp,
        EqOp,
        NotEqOp,
        OrOp,
        AndOp,
        LShiftOp,
        RShiftOp
    };

    inline Expression(Item *parent = 0)
        : m_parent(parent) {}

    inline Expression *parentExpression() const
    { return m_parent ? m_parent->toExpression() : 0; }

    virtual Item *parent() const
    { return m_parent; }

    virtual Expression *toExpression() const
    { return const_cast<Expression *>(this); }

    virtual UnaryExpression *toUnaryExpression() const
    { return 0; }

    virtual BinaryExpression *toBinaryExpression() const
    { return 0; }

    virtual StringLiteral *toStringLiteral() const
    { return 0; }

    virtual IntLiteral *toIntLiteral() const
    { return 0; }

    virtual MacroReference *toMacroReference() const
    { return 0; }

    virtual MacroFunctionReference *toMacroFunctionReference() const
    { return 0; }

    virtual ConditionalExpression *toConditionalExpression() const
    { return 0; }

    int evaluate(bool *ok = 0);

private:
    Item *m_parent;
};

struct StringLiteral: public Expression
{
    inline StringLiteral(const QByteArray &value, Item *parent)
        : Expression(parent), m_value(value) {}

    QByteArray value() const
    { return m_value; }

    virtual StringLiteral *toStringLiteral() const
    { return const_cast<StringLiteral *>(this); }

private:
    QByteArray m_value;
};

struct IntLiteral: public Expression
{
    inline IntLiteral(int value, Item *parent = 0)
        : Expression(parent), m_value(value) {}

    inline int value() const
    { return m_value; }

    virtual IntLiteral *toIntLiteral() const
    { return const_cast<IntLiteral *>(this); }

private:
    int m_value;
};

struct MacroReference: public Expression
{
    enum Type {
        DefinedRef,    //#if defined(foo)
        ValueRef
    };

    inline MacroReference(const TokenEngine::TokenList &name, Type type, Item *parent = 0)
        : Expression(parent), m_type(type), m_name(name) {}

    virtual MacroReference *toMacroReference() const
    { return const_cast<MacroReference *>(this); }

    inline TokenEngine::TokenList name() const
    { return m_name; }

    inline void setName(const TokenEngine::TokenList &name)
    { m_name = name; }

    inline int type() const
    { return m_type; }

private:
    int m_type;
    TokenEngine::TokenList m_name;
};

struct MacroFunctionReference: public Expression
{
    MacroFunctionReference(const QByteArray &name, Item *parent);

    inline QByteArray name() const
    { return m_name; }

    inline void setName(const QByteArray &name)
    { m_name = name; }

    inline MacroArguments *arguments() const
    { return m_arguments; }

    virtual MacroFunctionReference *toMacroFunctionReference() const
    { return const_cast<MacroFunctionReference *>(this); }

private:
    QByteArray m_name;
    MacroArguments *m_arguments;
};

struct UnaryExpression: public Expression
{
     inline UnaryExpression(int op, Expression *e, Expression *parent = 0)
        : Expression(parent), m_op(op), m_expression(e) {}

    inline int op() const
    { return m_op; }

    inline Expression *expression() const
    { return m_expression; }

    virtual UnaryExpression *toUnaryExpression() const
    { return const_cast<UnaryExpression *>(this); }

private:
    int m_op;
    Expression *m_expression;
};

struct BinaryExpression: public Expression
{
   inline BinaryExpression(int op, Expression *left, Expression *right, Expression *parent = 0)
        : Expression(parent),
          m_op(op),
          m_leftExpression(left),
          m_rightExpression(right) {}

    inline int op() const
    { return m_op; }

    inline Expression *leftExpression() const
    { return m_leftExpression; }

    inline Expression *rightExpression() const
    { return m_rightExpression; }

    virtual BinaryExpression *toBinaryExpression() const
    { return const_cast<BinaryExpression *>(this); }

private:
    int m_op;
    Expression *m_leftExpression;
    Expression *m_rightExpression;
};

struct ConditionalExpression: public Expression
{
    inline ConditionalExpression(Expression *condition, Expression *left, Expression *right, Expression *parent = 0)
        : Expression(parent),
          m_condition(condition),
          m_leftExpression(left),
          m_rightExpression(right) {}

    inline Expression *condition() const
    { return m_condition; }

    inline Expression *leftExpression() const
    { return m_leftExpression; }

    inline Expression *rightExpression() const
    { return m_rightExpression; }

    virtual ConditionalExpression *toConditionalExpression() const
    { return const_cast<ConditionalExpression *>(this); }

private:
    Expression *m_condition;
    Expression *m_leftExpression;
    Expression *m_rightExpression;
};


struct IfLikeDirective: public ConditionalDirective
{
    inline IfLikeDirective(Item *parent = 0)
        :ConditionalDirective(parent), m_expression(0) {}

    void setExpression(Expression *expression)
    { m_expression  = expression; }

    Expression *expression() const
    { return m_expression; }

protected:
    Expression *m_expression;
};

struct IfDirective: public IfLikeDirective
{
    inline IfDirective(Item *parent = 0)
        :IfLikeDirective(parent) {}

    virtual IfDirective *toIfDirective() const
    { return const_cast<IfDirective *>(this); }
};


struct ElifDirective: public IfLikeDirective
{
    inline ElifDirective(Item *parent = 0)
    :IfLikeDirective(parent) {}

    virtual ElifDirective *toElifDirective() const
    { return const_cast<ElifDirective *>(this); }
};

struct IfdefLikeDirective: public ConditionalDirective
{
    inline IfdefLikeDirective(Item *parent = 0)
    :ConditionalDirective(parent) {}

    inline TokenEngine::TokenList identifier() const
    { return m_identifier; }

    inline void setIdentifier(const TokenEngine::TokenList &identifier)
    { m_identifier = identifier; }
protected:
    TokenEngine::TokenList m_identifier;
};

struct IfdefDirective: public IfdefLikeDirective
{
    IfdefDirective(Item *parent)
    :IfdefLikeDirective(parent) {}

    virtual IfdefDirective *toIfdefDirective() const
    { return const_cast<IfdefDirective *>(this); }
};

struct IfndefDirective: public IfdefLikeDirective
{
    inline IfndefDirective(Item *parent)
    :IfdefLikeDirective(parent) {}

    virtual IfndefDirective *toIfndefDirective() const
    { return const_cast<IfndefDirective *>(this); }
};

struct ElseDirective: public ConditionalDirective
{
    ElseDirective(Item *parent)
    :ConditionalDirective(parent) {}

    virtual ElseDirective *toElseDirective() const
    { return const_cast<ElseDirective *>(this); }
};

struct EndifDirective : public Directive
{
    EndifDirective(Item *parent)
    :Directive(parent) {}

    EndifDirective *toEndifDirective() const
    { return const_cast<EndifDirective *>(this); }
};

struct DefineDirective: public Directive
{
    DefineDirective(Item *parent)
    : Directive(parent) {};

    inline TokenEngine::TokenList identifier() const
    { return m_identifier; }

    inline void setIdentifier(TokenEngine::TokenList identifier)
    { m_identifier = identifier; }

    inline void setReplacementList(TokenEngine::TokenList replacementList)
    { m_replacementList = replacementList; }

    inline TokenEngine::TokenList replacementList() const
    { return m_replacementList; }

    virtual DefineDirective *toDefineDirective() const
    { return const_cast<DefineDirective *>(this); }

    virtual MacroDefinition *toMacroDefinition() const
    { return 0; }

    virtual MacroFunctionDefinition *toMacroFunctionDefinition() const
    { return 0; }
private:
    TokenEngine::TokenList m_identifier;
    TokenEngine::TokenList m_replacementList;
};

struct MacroDefinition: public DefineDirective
{
    MacroDefinition(Item *parent)
    : DefineDirective(parent) {};

    virtual MacroDefinition *toMacroDefinition() const
    { return const_cast<MacroDefinition *>(this); }
};

struct MacroFunctionDefinition: public DefineDirective
{
    MacroFunctionDefinition(Item *parent)
    : DefineDirective(parent) {}

    virtual MacroFunctionDefinition *toMacroFunctionDefinition() const
    { return const_cast<MacroFunctionDefinition *>(this); }

    void setParameters(TokenEngine::TokenList macroParameters)
    { m_parameters = macroParameters;}

    inline TokenEngine::TokenList parameters() const
    { return m_parameters; }

private:
    TokenEngine::TokenList m_parameters;
};

struct MacroParameter: public Item
{
    inline MacroParameter(Item *parent)
        : m_parent(parent) {}

    inline QByteArray name() const
    { return m_name; }

    inline void setName(const QByteArray &name)
    { m_name = name; }

    virtual Item *parent() const
    { return m_parent; }

private:
    Item *m_parent;
    QByteArray m_name;
};

struct MacroParameters: public Item, public ItemComposite
{
    MacroParameters(MacroFunctionDefinition *parent)
        : m_parent(parent) {}

    ItemComposite *toItemComposite() const
    { return const_cast<MacroParameters *>(this); }

    virtual Item *parent() const
    { return m_parent; }

    virtual int count() const
    { return m_items.count(); }

    virtual Item *item(int index) const
    { return m_items.at(index); }

    void addParameter(MacroParameter *param)
    { Q_ASSERT(param->parent() == this); m_items.append(param); }

    int indexOf(const QByteArray &param) const
    {
        for (int i=0; i<m_items.count(); ++i) {
         //   cout <<"checking |" << param.constData() << "| against |" << m_items.at(i)->name().constData() <<"|" << endl;
            if (m_items.at(i)->name() == param)
                return i;
        }
        return -1;
    }

    inline bool contains(const QByteArray &param) const
    { return indexOf(param) != -1; }
/*
    void add(const QByteArray &param)
    {
        MacroParameter *p = createNode<MacroParameter>(this);
        p->setName(param);
        addParameter(p);
    }
*/
private:
    MacroFunctionDefinition *m_parent;
    QVector<MacroParameter*> m_items;
};

struct UndefDirective: public Directive
{
    UndefDirective(Item *parent)
    :Directive(parent) {}

    inline TokenEngine::TokenList identifier() const
    { return m_identifier; }

    inline void setIdentifier(const TokenEngine::TokenList &identifier)
    { m_identifier = identifier; }

    virtual UndefDirective *toUndefDirective() const
    { return const_cast<UndefDirective *>(this); }
private:
    TokenEngine::TokenList m_identifier;
};

struct LineDirective: public Directive
{
    LineDirective(Item *parent)
    :Directive(parent) {}

    virtual LineDirective *toLineDirective() const
    { return const_cast<LineDirective *>(this); }
};

struct NonDirective: public Directive
{
    NonDirective(Item *parent)
    :Directive(parent) {}

    virtual NonDirective *toNonDirective() const
    { return const_cast<NonDirective *>(this); }
};

class Preprocessor : public QObject
{
Q_OBJECT
public:
    Preprocessor();
    Source *parse(const TokenEngine::TokenContainer &tokenContainer,
                  const QVector<Type> &tokenTypeList, TypedPool<Item> *memoryPool);
signals:
    void error(const QString type, const QString message);
private:
    bool parseGroup(Item *node);
    bool parseGroupPart(Item *node);

    bool parseIfSection(Item *node);
    bool parseNonDirective(Item *node);
    bool parseTextLine(Item *node);

    bool parseIfGroup(IfSection *node);
    bool parseElifGroups(IfSection *node);
    bool parseElifGroup(IfSection *node);
    bool parseElseGroup(IfSection *node);
    bool parseEndifLine(IfSection *node);

    bool parseIfdefLikeDirective(IfdefLikeDirective *node);
    bool parseIfLikeDirective(IfLikeDirective *node);

    bool parseDefineDirective(Item *node);
    bool parseUndefDirective(Item *node);
    bool parseIncludeDirective(Item *node);
    bool parseErrorDirective(Item *node);
    bool parsePragmaDirective(Item*node);

    TokenEngine::TokenSection readLine();
    inline bool isValidIndex(const int index) const;
    inline bool isWhiteSpace(const int index) const;
    Type lookAhead() const;
    Type lookAheadSkipHash() const;
    inline int skipWhiteSpaceAndComments() const;
    inline int skipWhiteSpaceCommentsHash() const;
    QVector<int> cleanEscapedNewLines(const TokenEngine::TokenSection &tokenSection) const;
    QVector<int> cleanTokenRange(const TokenEngine::TokenSection &tokenSection) const;

    Source *m_source;
    TokenEngine::TokenContainer m_tokenContainer;
    QVector<Type> m_tokenTypeList;
    TypedPool<Item> *m_memoryPool;
    int lexerTokenIndex;
    int numTokens;
};

/*
    T must be a subclass of Item, parent must implment
    the ItemComposite interface
*/
template <typename T>
T *createNode(TypedPool<Item> *memPool, Item *parent)
{
    Q_ASSERT(parent);
    T* node = new (memPool->allocate(sizeof(T))) T(parent);
    Q_ASSERT(node);
    return node;
}

template <typename T>
T *createNode(TypedPool<Item> *memPool)
{
    T* node = new (memPool->allocate(sizeof(T))) T(0);
    Q_ASSERT(node);
    return node;
}


QByteArray visitGetText(Item *item);

} // namespace Rpp

QT_END_NAMESPACE

#endif
