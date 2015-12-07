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