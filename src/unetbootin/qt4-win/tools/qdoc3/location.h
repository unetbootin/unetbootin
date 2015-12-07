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

/*
  location.h
*/

#ifndef LOCATION_H
#define LOCATION_H

#include <qstack.h>

#include "tr.h"

QT_BEGIN_NAMESPACE

class Config;
class QRegExp;

class Location
{
public:
    Location();
    Location( const QString& filePath );
    Location( const Location& other );
    ~Location() { delete stk; }

    Location& operator=( const Location& other );

    void start();
    void advance( QChar ch );
    void advanceLines(int n) { stkTop->lineNo += n; stkTop->columnNo = 1; }

    void push( const QString& filePath );
    void pop();
    void setEtc( bool etc ) { etcetera = etc; }
    void setLineNo(int no) { stkTop->lineNo = no; }
    void setColumnNo(int no) { stkTop->columnNo = no; }

    bool isEmpty() const { return stkDepth == 0; }
    int depth() const { return stkDepth; }
    const QString& filePath() const { return stkTop->filePath; }
    QString fileName() const;
    int lineNo() const { return stkTop->lineNo; }
    int columnNo() const { return stkTop->columnNo; }
    bool etc() const { return etcetera; }
    void warning( const QString& message, const QString& details = QString() ) const;
    void error( const QString& message, const QString& details = QString() ) const;
    void fatal( const QString& message, const QString& details = QString() ) const;

    QT_STATIC_CONST Location null;

    static void initialize( const Config& config );
    static void terminate();
    static void information( const QString& message );
    static void internalError( const QString& hint );

private:
    enum MessageType { Warning, Error };

    struct StackEntry
    {
	QString filePath;
	int lineNo;
	int columnNo;
    };

    void emitMessage( MessageType type, const QString& message,
		      const QString& details ) const;
    QString toString() const;
    QString top() const;

    StackEntry stkBottom;
    QStack<StackEntry> *stk;
    StackEntry *stkTop;
    int stkDepth;
    bool etcetera;

    static int tabSize;
    static QString programName;
    static QRegExp *spuriousRegExp;
};

QT_END_NAMESPACE

#endif
