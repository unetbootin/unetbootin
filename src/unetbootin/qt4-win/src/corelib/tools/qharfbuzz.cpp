/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qharfbuzz_p.h"

#include "qunicodetables_p.h"

QT_USE_NAMESPACE

HB_GraphemeClass HB_GetGraphemeClass(HB_UChar32 ch)
{
    const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ch);
    return (HB_GraphemeClass) prop->graphemeBreak;
}

HB_WordClass HB_GetWordClass(HB_UChar32 ch)
{
    const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ch);
    return (HB_WordClass) prop->wordBreak;
}

HB_SentenceClass HB_GetSentenceClass(HB_UChar32 ch)
{
    const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ch);
    return (HB_SentenceClass) prop->sentenceBreak;
}

HB_LineBreakClass HB_GetLineBreakClass(HB_UChar32 ch)
{
    return (HB_LineBreakClass)QUnicodeTables::lineBreakClass(ch);
}


void HB_GetGraphemeAndLineBreakClass(HB_UChar32 ch, HB_GraphemeClass *grapheme, HB_LineBreakClass *lineBreak)
{
    const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ch);
    *grapheme = (HB_GraphemeClass) prop->graphemeBreak;
    *lineBreak = (HB_LineBreakClass) prop->line_break_class;
}

void HB_GetUnicodeCharProperties(HB_UChar32 ch, HB_CharCategory *category, int *combiningClass)
{
    const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ch);
    *category = (HB_CharCategory)prop->category;
    *combiningClass = prop->combiningClass;
}

HB_CharCategory HB_GetUnicodeCharCategory(HB_UChar32 ch)
{
    return (HB_CharCategory)QChar::category(ch);
}

int HB_GetUnicodeCharCombiningClass(HB_UChar32 ch)
{
    return QChar::combiningClass(ch);
}

HB_UChar16 HB_GetMirroredChar(HB_UChar16 ch)
{
    return QChar::mirroredChar(ch);
}

QT_BEGIN_NAMESPACE

void qGetCharAttributes(const HB_UChar16 *string, hb_uint32 stringLength,
                        const HB_ScriptItem *items, hb_uint32 numItems,
                        HB_CharAttributes *attributes)
{
    HB_GetCharAttributes(string, stringLength, items, numItems, attributes);
}

HB_Bool qShapeItem(HB_ShaperItem *item)
{
    return HB_ShapeItem(item);
}

HB_Face qHBNewFace(void *font, HB_GetFontTableFunc tableFunc)
{
    return HB_NewFace(font, tableFunc);
}

void qHBFreeFace(HB_Face face)
{
    HB_FreeFace(face);
}

QT_END_NAMESPACE
