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
  plaincodemarker.h
*/

#ifndef PLAINCODEMARKER_H
#define PLAINCODEMARKER_H

#include "codemarker.h"

QT_BEGIN_NAMESPACE

class PlainCodeMarker : public CodeMarker
{
public:
    PlainCodeMarker();
    ~PlainCodeMarker();

    bool recognizeCode( const QString& code );
    bool recognizeExtension( const QString& ext );
    bool recognizeLanguage( const QString& lang );
    QString plainName( const Node *node );
    QString plainFullName( const Node *node, const Node *relative );
    QString markedUpCode( const QString& code, const Node *relative, const QString& dirPath );
    QString markedUpSynopsis( const Node *node, const Node *relative,
        		      SynopsisStyle style );
    QString markedUpName( const Node *node );
    QString markedUpFullName( const Node *node, const Node *relative );
    QString markedUpEnumValue(const QString &enumValue, const Node *relative);
    QString markedUpIncludes( const QStringList& includes );
    QString functionBeginRegExp( const QString& funcName );
    QString functionEndRegExp( const QString& funcName );
    QList<Section> sections(const InnerNode *innerNode, SynopsisStyle style, Status status);
    const Node *resolveTarget(const QString &target, const Tree *tree, const Node *relative);
};

QT_END_NAMESPACE
  
#endif
