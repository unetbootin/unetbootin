/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include "proparser.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

#ifdef Q_OS_WIN32
#define QT_POPEN _popen
#else
#define QT_POPEN popen
#endif

QT_BEGIN_NAMESPACE

QString loadFile( const QString &fileName )
{
    QFile file( fileName );
    if ( !file.open(QIODevice::ReadOnly) ) {
        fprintf( stderr, "error: Cannot load '%s': %s\n",
                 file.fileName().toLocal8Bit().constData(),
                 file.errorString().toLatin1().constData() );
        return QString();
    }

    QTextStream in( &file );
    return in.readAll();
}

QMap<QString, QString> proFileTagMap( const QString& text, QString currentPath )
{
    QString t = text;
    if (currentPath.isEmpty())
        currentPath = QDir::currentPath();


    QMap<QString, QString> tagMap;
        /*
            Strip any commments before we try to include.  We
            still need to do it after we include to make sure the
            included file does not have comments
        */
        t.replace( QRegExp(QLatin1String("#[^\n]*\n")), QLatin1String(" ") );
      /*
            Strip comments, merge lines ending with backslash, add
            spaces around '=' and '+=', replace '\n' with ';', and
            simplify white spaces.
        */
        t.replace( QRegExp(QLatin1String("#[^\n]*\n")), QLatin1String(" ") );
        t.replace( QRegExp(QLatin1String("\\\\[^\n\\S]*\n")), QLatin1String(" ") );
        t.replace( QLatin1String("="), QLatin1String(" = ") );
        t.replace( QLatin1String("+ ="), QLatin1String(" += ") );
        t.replace( QLatin1String("\n"), QLatin1String(";") );
        t = t.simplified();

        /*
            Populate tagMap with 'key = value' entries.
        */
        QStringList lines = t.split(QLatin1Char(';'));
        QStringList::Iterator line;
        for ( line = lines.begin(); line != lines.end(); ++line ) {
            QStringList toks = (*line).split(QLatin1Char(' '), QString::SkipEmptyParts);

            if ( toks.count() >= 3 &&
                (toks[1] == QLatin1String("=") || toks[1] == QLatin1String("+=") ||
                toks[1] == QLatin1String("*=")) ) {
                QString tag = toks.first();
                int k = tag.lastIndexOf( QLatin1Char(':') ); // as in 'unix:'
                if ( k != -1 )
                    tag = tag.mid( k + 1 );
                toks.erase( toks.begin() );

                QString action = toks.first();
                toks.erase( toks.begin() );

                if ( tagMap.contains(tag) ) {
                    if ( action == QLatin1String("=") )
                        tagMap.insert( tag, toks.join(QLatin1String(" ")) );
                    else
                        tagMap[tag] += QLatin1Char( ' ' ) + toks.join( QLatin1String(" ") );
                } else {
                    tagMap[tag] = toks.join( QLatin1String(" ") );
                }
            }
        }
        /*
            Expand $$variables within the 'value' part of a 'key = value'
            pair.
        */
        QRegExp var( QLatin1String("\\$\\$[({]?([a-zA-Z0-9_]+)[)}]?") );
        QMap<QString, QString>::Iterator it;
        for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
            int i = 0;
            while ( (i = var.indexIn((*it), i)) != -1 ) {
                int len = var.matchedLength();
                QString invocation = var.cap(1);
                QString after;

                if ( invocation == QLatin1String("system") ) {
                    // skip system(); it will be handled in the next pass
                    ++i;
                } else {
                    if ( tagMap.contains(invocation) )
                        after = tagMap[invocation];
                    else if (invocation.toLower() == QLatin1String("pwd"))
                        after = currentPath;
                    (*it).replace( i, len, after );
                    i += after.length();
                }
            }
        }

        /*
          Execute system() calls.
        */
        QRegExp callToSystem( QLatin1String("\\$\\$system\\s*\\(([^()]*)\\)") );
        for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
            int i = 0;
            while ( (i = callToSystem.indexIn((*it), i)) != -1 ) {
                /*
                  This code is stolen from qmake's project.cpp file.
                  Ideally we would use the same parser, so we wouldn't
                  have this code duplication.
                */
                QString after;
                char buff[256];
                FILE *proc = QT_POPEN( callToSystem.cap(1).toLatin1().constData(), "r" );
                while ( proc && !feof(proc) ) {
                    int read_in = int(fread( buff, 1, 255, proc ));
                    if ( !read_in )
                        break;
                    for ( int i = 0; i < read_in; i++ ) {
                        if ( buff[i] == '\n' || buff[i] == '\t' )
                            buff[i] = ' ';
                    }
                    buff[read_in] = '\0';
                    after += QLatin1String(buff);
                }
                (*it).replace( i, callToSystem.matchedLength(), after );
                i += after.length();
            }
        }
    return tagMap;
}

QT_END_NAMESPACE
