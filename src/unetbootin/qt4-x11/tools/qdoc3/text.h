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

/*
  text.h
*/

#ifndef TEXT_H
#define TEXT_H

#include "atom.h"

QT_BEGIN_NAMESPACE

class Text
{
public:
    Text();
    explicit Text(const QString &str);
    Text( const Text& text );
    ~Text();

    Text& operator=( const Text& text );

    Atom *firstAtom() { return first; }
    Atom *lastAtom() { return last; }
    Text& operator<<( Atom::Type atomType );
    Text& operator<<( const QString& string );
    Text& operator<<( const Atom& atom );
    Text& operator<<( const Text& text );
    void stripFirstAtom();
    void stripLastAtom();

    bool isEmpty() const { return first == 0; }
    QString toString() const;
    const Atom *firstAtom() const { return first; }
    const Atom *lastAtom() const { return last; }
    Text subText( Atom::Type left, Atom::Type right, const Atom *from = 0) const;
    void dump() const;

    static Text subText( const Atom *begin, const Atom *end = 0 );
    static Text sectionHeading(const Atom *sectionBegin);
    static const Atom *sectionHeadingAtom(const Atom *sectionLeft);
    static int compare(const Text &text1, const Text &text2);

private:
    void clear();

    Atom *first;
    Atom *last;
};

inline bool operator==(const Text &text1, const Text &text2)
{ return Text::compare(text1, text2) == 0; }
inline bool operator!=(const Text &text1, const Text &text2)
{ return Text::compare(text1, text2) != 0; }
inline bool operator<(const Text &text1, const Text &text2)
{ return Text::compare(text1, text2) < 0; }
inline bool operator<=(const Text &text1, const Text &text2)
{ return Text::compare(text1, text2) <= 0; }
inline bool operator>(const Text &text1, const Text &text2)
{ return Text::compare(text1, text2) > 0; }
inline bool operator>=(const Text &text1, const Text &text2)
{ return Text::compare(text1, text2) >= 0; }

QT_END_NAMESPACE

#endif
