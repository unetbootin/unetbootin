/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include "codegenerator.h"
#include <qdebug.h>
namespace CodeGenerator
{

//Convenience constructor so you can say Item("foo")
Item::Item(const char * const text) : generator(Text(QByteArray(text)).generator) {}

int BaseGenerator::currentCount(GeneratorStack * const stack) const
{
    const int stackSize = stack->count();
    for (int i = stackSize - 1; i >= 0; --i) {
        BaseGenerator const * const generator = stack->at(i);
        switch (generator->type) {
            case RepeaterType: {
                RepeaterGenerator const * const repeater = static_cast<RepeaterGenerator const * const>(generator);
                return repeater->currentRepeat;
            } break;
            case GroupType: {
                GroupGenerator const * const group = static_cast<GroupGenerator const * const>(generator);
                return group->currentRepeat;
            } break;
            default:
            break;
        }
    }
    return -1;
}

int BaseGenerator::repeatCount(GeneratorStack * const stack) const
{
    const int stackSize = stack->count();
    for (int i = stackSize - 1; i >= 0; --i) {
        BaseGenerator const * const generator = stack->at(i);
        switch (generator->type) {
            case RepeaterType: {
                RepeaterGenerator const * const repeater = static_cast<RepeaterGenerator const * const>(generator);
                return repeater->currentRepeat;
            } break;
/*            case GroupType: {
                GroupGenerator const * const group = static_cast<GroupGenerator const * const>(generator);
                return group->currentRepeat;
            } break;
*/  
            default:
            break;
        }
    }
    return -1;
}

QByteArray RepeaterGenerator::generate(GeneratorStack * const stack)
{ 
    GeneratorStacker stacker(stack, this);
    QByteArray generated;
    for (int i = repeatOffset; i < repeatCount + repeatOffset; ++i) {
        currentRepeat = i;
        generated += childGenerator->generate(stack);
    }
    return generated;
};

QByteArray GroupGenerator::generate(GeneratorStack * const stack)
{ 
    const int repeatCount = currentCount(stack);
    GeneratorStacker stacker(stack, this);
    QByteArray generated;

    if (repeatCount > 0)
        generated += prefix->generate(stack);

    for (int i = 1; i <= repeatCount; ++i) {
        currentRepeat = i;
        generated += childGenerator->generate(stack);
        if (i != repeatCount)
            generated += separator->generate(stack);
    }

    if (repeatCount > 0)
        generated += postfix->generate(stack);

    return generated;
};

const Compound operator+(const Item &a, const Item &b)
{
    return Compound(a, b);
}

const Compound operator+(const Item &a, const char * const text)
{
    return Compound(a, Text(text));
}

const Compound operator+(const char * const text, const Item &b)
{
    return Compound(Text(text), b);    
}

}