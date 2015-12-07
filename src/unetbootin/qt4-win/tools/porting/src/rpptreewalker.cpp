/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "rpptreewalker.h"

QT_BEGIN_NAMESPACE

namespace Rpp {

void RppTreeWalker::evaluateItem(const Item *item)
{
    if(!item)
        return;
    if (Source *source = item->toSource())
        evaluateSource(source);
    else if (Directive *directive = item->toDirective())
        evaluateDirective(directive);
    else if (IfSection *ifSection = item->toIfSection())
        evaluateIfSection(ifSection);
    else if (Text *text = item->toText())
        evaluateText(text);
}

void RppTreeWalker::evaluateItemComposite(const ItemComposite *itemComposite)
{
    if (!itemComposite)
        return;
    for (int i = 0; i < itemComposite->count(); ++i) {
        evaluateItem(itemComposite->item(i));
    }
}

void RppTreeWalker::evaluateSource(const Source *source)
{
    evaluateItemComposite(source->toItemComposite());
}

void RppTreeWalker::evaluateDirective(const Directive *directive)
{
    if (!directive)
        return;
    if (EmptyDirective *dir = directive->toEmptyDirective())
        evaluateEmptyDirective(dir);
    else if (ErrorDirective *dir = directive->toErrorDirective())
        evaluateErrorDirective(dir);
    else if (PragmaDirective *dir = directive->toPragmaDirective())
        evaluatePragmaDirective(dir);
    else if (IncludeDirective *dir = directive->toIncludeDirective())
        evaluateIncludeDirective(dir);
    else if (DefineDirective *dir = directive->toDefineDirective())
        evaluateDefineDirective(dir);
    else if (UndefDirective *dir = directive->toUndefDirective())
        evaluateUndefDirective(dir);
    else if (LineDirective *dir = directive->toLineDirective())
        evaluateLineDirective(dir);
    else if (NonDirective *dir = directive->toNonDirective())
        evaluateNonDirective(dir);
    else if (NonDirective *dir = directive->toNonDirective())
        evaluateNonDirective(dir);
    else if (ConditionalDirective *dir = directive->toConditionalDirective())
        evaluateConditionalDirective(dir);
}

/*
    This function evaluates all the branches of an IfSection. You should 
    override it if you want to only evaluate the "correct" branch.
*/
void RppTreeWalker::evaluateIfSection(const IfSection *ifSection)
{
    if (!ifSection)
        return;
    evaluateItemComposite(ifSection->toItemComposite());
}

void RppTreeWalker::evaluateConditionalDirective(const ConditionalDirective *conditionalDirective)
{
    if (!conditionalDirective)
        return;
    if (IfdefDirective *dir = conditionalDirective->toIfdefDirective())
         evaluateIfdefDirective(dir);
    else if (IfndefDirective *dir = conditionalDirective->toIfndefDirective())
         evaluateIfndefDirective(dir);
    else if (IfDirective *dir = conditionalDirective->toIfDirective())
         evaluateIfDirective(dir);
    else if (ElifDirective *dir = conditionalDirective->toElifDirective())
         evaluateElifDirective(dir);
    else if (ElseDirective *dir = conditionalDirective->toElseDirective())
         evaluateElseDirective(dir);
}

void RppTreeWalker::evaluateIfdefDirective(const IfdefDirective *directive)
{
   if (!directive) 
       return;
   evaluateItemComposite(directive->toItemComposite());
}

void RppTreeWalker::evaluateIfndefDirective(const IfndefDirective *directive)
{
   if (!directive) 
       return;
   evaluateItemComposite(directive->toItemComposite());
}

void RppTreeWalker::evaluateIfDirective(const IfDirective *directive)
{
   if (!directive) 
       return;
   evaluateItemComposite(directive->toItemComposite());
}

void RppTreeWalker::evaluateElifDirective(const ElifDirective *directive)
{
   if (!directive) 
       return;
   evaluateItemComposite(directive->toItemComposite());
}

void RppTreeWalker::evaluateElseDirective(const ElseDirective *directive)
{
   if (!directive) 
       return;
   evaluateItemComposite(directive->toItemComposite());
}

}

QT_END_NAMESPACE
