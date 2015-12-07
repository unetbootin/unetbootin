/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QTEXTFORMAT_P_H
#define QTEXTFORMAT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qtextformat.h"
#include "QtCore/qvector.h"
#include "QtCore/qset.h"

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QTextFormatCollection
{
public:
    QTextFormatCollection() {}
    ~QTextFormatCollection();

    QTextFormatCollection(const QTextFormatCollection &rhs);
    QTextFormatCollection &operator=(const QTextFormatCollection &rhs);

    QTextFormat objectFormat(int objectIndex) const;
    void setObjectFormat(int objectIndex, const QTextFormat &format);

    int objectFormatIndex(int objectIndex) const;
    void setObjectFormatIndex(int objectIndex, int formatIndex);

    int createObjectIndex(const QTextFormat &f);

    int indexForFormat(const QTextFormat &f);
    bool hasFormatCached(const QTextFormat &format) const;

    QTextFormat format(int idx) const;
    inline QTextBlockFormat blockFormat(int index) const
    { return format(index).toBlockFormat(); }
    inline QTextCharFormat charFormat(int index) const
    { return format(index).toCharFormat(); }
    inline QTextListFormat listFormat(int index) const
    { return format(index).toListFormat(); }
    inline QTextTableFormat tableFormat(int index) const
    { return format(index).toTableFormat(); }
    inline QTextImageFormat imageFormat(int index) const
    { return format(index).toImageFormat(); }

    inline int numFormats() const { return formats.count(); }

    typedef QVector<QTextFormat> FormatVector;

    FormatVector formats;
    QVector<qint32> objFormats;
    QSet<uint> hashes;

    inline QFont defaultFont() const { return defaultFnt; }
    void setDefaultFont(const QFont &f);

private:
    QFont defaultFnt;
};

QT_END_NAMESPACE

#endif // QTEXTFORMAT_P_H
