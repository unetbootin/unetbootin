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

#include "textreplacement.h"

QT_BEGIN_NAMESPACE

bool TextReplacements::insert(QByteArray newText, int insertPosition, int currentLenght)
{
    //bubble sort the new replacement into the list
    int i;
    for(i=0; i<textReplacementList.size(); ++i) {
        if (insertPosition == textReplacementList.at(i).insertPosition)
            return false; // multiple replacements on the same insertPosition is not allowed.
        if(insertPosition < textReplacementList.at(i).insertPosition)
            break;  //we found the right position
    }
    //++i;
  //  cout << "inserting new text " << newText.constData() << endl;
    // %s at %d overwriting %d bytes at list pos %d\n", newText.constData(), insertPosition, currentLenght, i);
    TextReplacement rep;
    rep.newText=newText;
    rep.insertPosition=insertPosition;
    rep.currentLenght=currentLenght;

    textReplacementList.insert(i, rep);
    return true;
}

void TextReplacements::clear()
{
    textReplacementList.clear();
}

QByteArray TextReplacements::apply(QByteArray current)
{
    QByteArray newData=current;
    int i;
    int replacementOffset=0;

    for(i=0; i<textReplacementList.size(); ++i) {
        TextReplacement rep=textReplacementList.at(i);
        //printf("applying new text %s insert at %d overwriting %d bytes \n", rep.newText.constData(), rep.insertPosition, rep.currentLenght);
        newData.remove(rep.insertPosition+replacementOffset, rep.currentLenght);
        newData.insert(rep.insertPosition+replacementOffset, rep.newText);

        //modify all remaining replacements if we change the document length
        replacementOffset+=(rep.newText.size() - rep.currentLenght);
    }

    return newData;
}

TextReplacements &TextReplacements::operator+=(const TextReplacements &other)
{
    foreach(TextReplacement rep, other.replacements()) {
        insert(rep.newText, rep.insertPosition, rep.currentLenght);
    }
    return *this;
}

QT_END_NAMESPACE
