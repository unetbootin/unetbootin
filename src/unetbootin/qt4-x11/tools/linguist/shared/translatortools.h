/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#ifndef LUPDATE_H
#define LUPDATE_H

#include "qglobal.h"

#include <QList>

QT_BEGIN_NAMESPACE

class QString;
class Translator;
class TranslatorMessage;

enum UpdateOption {
    Verbose = 1,
    NoObsolete = 2,
    PluralOnly = 4,
    NoSort = 8,
    HeuristicSameText = 16,
    HeuristicSimilarText = 32,
    HeuristicNumber = 64,
    AbsoluteLocations = 256,
    RelativeLocations = 512,
    NoLocations = 1024,
    NoUiLines = 2048
};

Q_DECLARE_FLAGS(UpdateOptions, UpdateOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(UpdateOptions)

Translator merge(const Translator &tor, const Translator &virginTor,
    UpdateOptions options, QString &err);

QT_END_NAMESPACE

#endif
