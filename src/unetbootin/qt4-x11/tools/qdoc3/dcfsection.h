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

#ifndef DCFSECTION_H
#define DCFSECTION_H

#include <qlist.h>
#include <qpair.h>
#include <qstring.h>

QT_BEGIN_NAMESPACE

class QTextStream;

struct DcfSection
{
    QString title;
    QString ref;
    QList<QPair<QString, QString> > keywords;
    QList<DcfSection> subsections;
};

inline bool operator<( const DcfSection& s1, const DcfSection& s2 ) {
    QString title1 = s1.title;
    QString title2 = s2.title;

    // cheat with Q3 classes
    if (title1.startsWith("Q3"))
        title1.insert(1, '~');
    if (title2.startsWith("Q3"))
        title2.insert(1, '~');

    int delta = title1.toLower().compare( title2.toLower() );
    if ( delta == 0 ) {
	delta = title1.compare( title2 );
	if ( delta == 0 )
	    delta = s1.ref.localeAwareCompare( s2.ref );
    }
    return delta < 0;
}

inline bool operator>( const DcfSection& s1, const DcfSection& s2 ) { return s2 < s1; }
inline bool operator<=( const DcfSection& s1, const DcfSection& s2 ) { return !( s2 < s1 ); }
inline bool operator>=( const DcfSection& s1, const DcfSection& s2 ) { return !( s1 < s2 ); }
inline bool operator==( const DcfSection& s1, const DcfSection& s2 ) { return &s1 == &s2; }
inline bool operator!=( const DcfSection& s1, const DcfSection& s2 ) { return !( s1 == s2 ); }

void appendDcfSubSection(DcfSection *dcfSect, const DcfSection &sub);
void appendDcfSubSections(DcfSection *dcfSect, const QList<DcfSection> &subs);
void generateDcfSubSections(QString indent, QTextStream &out, const DcfSection &sect);
void generateDcfSections(const DcfSection &rootSect, const QString& fileName,
                         const QString& category );

QT_END_NAMESPACE

#endif
