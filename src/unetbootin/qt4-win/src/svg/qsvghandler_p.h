/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSVG module of the Qt Toolkit.
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

#ifndef QSVGHANDLER_P_H
#define QSVGHANDLER_P_H

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

#include "QtXml/qxmlstream.h"

#ifndef QT_NO_SVG

#include "QtCore/qhash.h"
#include "QtCore/qstack.h"
#include "qsvgstyle_p.h"
#include "private/qcssparser_p.h"

QT_BEGIN_NAMESPACE

class QSvgNode;
class QSvgTinyDocument;
class QSvgHandler;
class QColor;
class QSvgStyleSelector;

struct QSvgCssAttribute
{
    QXmlStreamStringRef name;
    QXmlStreamStringRef value;
};

#if defined(Q_OS_WINCE) && defined(IN)
#undef IN
#endif

class QSvgHandler
{
public:
    enum LengthType {
        PERCENT,
        PX,
        PC,
        PT,
        MM,
        CM,
        IN,
        OTHER
    };

public:
    QSvgHandler(QIODevice *device);
    QSvgHandler(const QByteArray &data);
    ~QSvgHandler();

    QSvgTinyDocument *document() const;

    inline bool ok() const {
        return document() != 0 && !xml.error();
    }

    inline QString errorString() const { return xml.errorString(); }
    inline int lineNumber() const { return xml.lineNumber(); }

    void setDefaultCoordinateSystem(LengthType type);
    LengthType defaultCoordinateSystem() const;

    void pushColor(const QColor &color);
    QColor currentColor() const;

    void setInStyle(bool b);
    bool inStyle() const;

    QSvgStyleSelector *selector() const;

    void setAnimPeriod(int start, int end);
    int animationDuration() const;

    void parseCSStoXMLAttrs(QString css, QVector<QSvgCssAttribute> *attributes);

    inline QPen defaultPen() const
    { return m_defaultPen; }

public:
    bool startElement(const QString &localName, const QXmlStreamAttributes &attributes);
    bool endElement(const QStringRef &localName);
    bool characters(const QStringRef &str);
    bool processingInstruction(const QString &target, const QString &data);

private:
    QString normalizeCharacters(const QString &input) const;

    QSvgTinyDocument *m_doc;
    QStack<QSvgNode*> m_nodes;

    QList<QSvgNode*>  m_resolveNodes;

    enum CurrentNode
    {
        Unknown,
        Graphics,
        Style
    };
    QStack<CurrentNode> m_skipNodes;

    enum WhitespaceMode
    {
        Preserve,
        Default
    };

    /*!
        Follows the depths of elements. The top is current xml:space
        value that applies for a given element.
     */
    QStack<WhitespaceMode> m_whitespaceMode;

    QSvgRefCounter<QSvgStyleProperty> m_style;

    LengthType m_defaultCoords;

    QStack<QColor> m_colorStack;
    QStack<int>    m_colorTagCount;

    bool m_inStyle;

    QSvgStyleSelector *m_selector;

    int m_animEnd;

    QXmlStreamReader xml;
    QCss::Parser m_cssParser;
    void parse();

    QPen m_defaultPen;
};

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGHANDLER_P_H
