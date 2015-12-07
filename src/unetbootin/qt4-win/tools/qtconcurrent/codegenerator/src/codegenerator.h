/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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
#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QStack>

namespace CodeGenerator
{
    enum GeneratorType {NoopType, CompoundType, TextType, RepeaterType, CounterType, GroupType};
    class BaseGenerator;
    typedef QStack<BaseGenerator *> GeneratorStack;
    
    template <typename ValueType>
    class Stacker {
    public:
        Stacker(QStack<ValueType> *stack, ValueType value) : stack(stack) { stack->push(value); }
        ~Stacker() { stack->pop();}
    private:
        QStack<ValueType> *stack;
    };
    typedef Stacker<BaseGenerator *> GeneratorStacker;
    
    class BaseGenerator
    {
    public:
        BaseGenerator(GeneratorType type = NoopType) : type(type) {}
        virtual ~BaseGenerator() {};
        virtual QByteArray generate(GeneratorStack *stack) { Q_UNUSED(stack); return QByteArray(); };
        int currentCount(GeneratorStack *stack) const;
        int repeatCount(GeneratorStack *stack) const;
        GeneratorType type;   
    };

    class Item
    {
    public:
        Item(BaseGenerator * const base) : generator(base) {}
        Item(const char * const text);
        QByteArray generate() const
        {  GeneratorStack stack; return generator->generate(&stack);  }
        // ### TODO: Fix memory leak!
        // QExplicitlySharedDataPointer<BaseGenerator> generator;
        BaseGenerator * const generator;
    };
    
    class CompoundGenerator : public BaseGenerator
    {
    public:
        CompoundGenerator(BaseGenerator * const a, BaseGenerator * const b) 
          : BaseGenerator(CompoundType), a(a), b(b) {}
        virtual QByteArray generate(GeneratorStack *stack)
        { return a->generate(stack) + b->generate(stack); };
    protected:    
        BaseGenerator * const a;
        BaseGenerator * const b;
    };
    
    class Compound : public Item
    {
    public:
        Compound(const Item &a, const Item &b) : Item(new CompoundGenerator(a.generator, b.generator)) {}
    };
    
    class TextGenerator : public BaseGenerator
    {
    public:
        TextGenerator(const QByteArray &text) : BaseGenerator(TextType), text(text) {}
        virtual QByteArray generate(GeneratorStack *) { return text; };
    protected:    
        QByteArray text;
    };
        
    class Text : public Item {
    public:
        Text(const QByteArray &text) : Item(new TextGenerator(text)) {}
        Text(const char * const text) : Item(new TextGenerator(QByteArray(text))) {}
    };

    class RepeaterGenerator : public BaseGenerator
    {
    public:
        RepeaterGenerator(BaseGenerator * const childGenerator) 
          : BaseGenerator(RepeaterType), repeatCount(1), repeatOffset(0), childGenerator(childGenerator) {}
        virtual QByteArray generate(GeneratorStack *stack);

        int repeatCount;
        int repeatOffset;
        int currentRepeat;
        BaseGenerator * const childGenerator;
    };
    
    class Repeater : public Item {
    public:
        Repeater(const Item &item) : Item(new RepeaterGenerator(item.generator)) {}
        void setRepeatCount(int count) 
        { static_cast<RepeaterGenerator * const>(generator)->repeatCount = count;  }
        void setRepeatOffset(int offset)
        { static_cast<RepeaterGenerator * const>(generator)->repeatOffset = offset;  }
    };
    
    class CounterGenerator : public BaseGenerator
    {
    public:
        CounterGenerator() : BaseGenerator(CounterType), offset(0), increment(1), reverse(false) {}
        QByteArray generate(GeneratorStack *stack)
        { 
            if (reverse)
                return QByteArray::number(repeatCount(stack) - (currentCount(stack) * increment) + offset + 1);
            else
                return QByteArray::number((currentCount(stack) * increment) + offset);
        }
        int offset;
        int increment;
        bool reverse;
    };
    
    class Counter : public Item {
    public:
        Counter() : Item(new CounterGenerator()) {}
        Counter(int offset) : Item(new CounterGenerator()) { setOffset(offset); }
        void setOffset(int offset)
        { static_cast<CounterGenerator *>(generator)->offset = offset; }
        void setIncrement(int increment)
        { static_cast<CounterGenerator *>(generator)->increment = increment; }
        void setReverse(bool reverse)
        { static_cast<CounterGenerator *>(generator)->reverse = reverse; }

    };
    
    class GroupGenerator : public BaseGenerator
    {
    public:
        GroupGenerator(BaseGenerator * const childGenerator)
          : BaseGenerator(GroupType), currentRepeat(0), childGenerator(childGenerator), 
            separator(new BaseGenerator()), prefix(new BaseGenerator()), postfix(new BaseGenerator()) { }
        virtual QByteArray generate(GeneratorStack *stack);
        int currentRepeat;
        BaseGenerator * const childGenerator;
        BaseGenerator *separator;
        BaseGenerator *prefix;
        BaseGenerator *postfix;
    };
    
    class Group : public Item
    {
    public:
        Group(const Item &item) : Item(new GroupGenerator(item.generator)) { setSeparator(", "); }
        void setSeparator(const Item &separator)        
        { static_cast<GroupGenerator *>(generator)->separator = separator.generator; }
        void setPrefix(const Item &prefix)        
        { static_cast<GroupGenerator *>(generator)->prefix = prefix.generator; }
        void setPostfix(const Item &postfix)        
        { static_cast<GroupGenerator *>(generator)->postfix = postfix.generator; }
    };

    const Compound operator+(const Item &a, const Item &b); 
    const Compound operator+(const Item &a, const char * const text);
    const Compound operator+(const char * const text, const Item &b);

} //namespace CodeGenerator

#endif
