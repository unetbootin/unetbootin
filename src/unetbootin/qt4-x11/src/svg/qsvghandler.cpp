/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtSVG module of the Qt Toolkit.
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

#include "qsvghandler_p.h"

#ifndef QT_NO_SVG

#include "qsvgtinydocument_p.h"
#include "qsvgstructure_p.h"
#include "qsvggraphics_p.h"
#include "qsvgnode_p.h"
#include "qsvgfont_p.h"

#include "qapplication.h"
#include "qwidget.h"
#include "qpen.h"
#include "qpainterpath.h"
#include "qbrush.h"
#include "qcolor.h"
#include "qtextformat.h"
#include "qvector.h"
#include "qfileinfo.h"
#include "qfile.h"
#include "qdebug.h"
#include "qmath.h"
#include "qnumeric.h"
#include "private/qmath_p.h"

#include "float.h"

QT_BEGIN_NAMESPACE

double qstrtod(const char *s00, char const **se, bool *ok);

static bool parsePathDataFast(const QStringRef &data, QPainterPath &path);

struct QSvgAttributes
{
    QSvgAttributes(const QXmlStreamAttributes &xmlAttributes, QSvgHandler *handler);

    QStringRef value(const QLatin1String &name) const;
    QStringRef value(const QString &namespaceUri, const QLatin1String &name) const;

    QXmlStreamAttributes m_xmlAttributes;
    QVector<QSvgCssAttribute> m_cssAttributes;
};

QSvgAttributes::QSvgAttributes(const QXmlStreamAttributes &xmlAttributes, QSvgHandler *handler)
    : m_xmlAttributes(xmlAttributes)
{
    QStringRef style = xmlAttributes.value(QLatin1String("style"));
    if (!style.isEmpty())
        handler->parseCSStoXMLAttrs(style.toString(), &m_cssAttributes);
}

QStringRef QSvgAttributes::value(const QLatin1String &name) const
{
    QStringRef v = m_xmlAttributes.value(name);
    if (v.isEmpty()) {
        for (int i = 0; i < m_cssAttributes.count(); ++i) {
            if (m_cssAttributes.at(i).name == name) {
                v = m_cssAttributes.at(i).value;
                break;
            }
        }
    }
    return v;
}

QStringRef QSvgAttributes::value(const QString &namespaceUri, const QLatin1String &name) const
{
    QStringRef v = m_xmlAttributes.value(namespaceUri, name);
    if (v.isEmpty()) {
        for (int i = 0; i < m_cssAttributes.count(); ++i) {
            if (m_cssAttributes.at(i).name == name) {
                v = m_cssAttributes.at(i).value;
                break;
            }
        }
    }
    return v;
}

static inline QString someId(const QXmlStreamAttributes &attributes)
{
    QString id = attributes.value(QLatin1String("id")).toString();
    if (id.isEmpty())
        id = attributes.value(QLatin1String("xml:id")).toString();
    return id;
}
static inline QString someId(const QSvgAttributes &attributes)
{ return someId(attributes.m_xmlAttributes); }



static const char * QSvgStyleSelector_nodeString[] = {
    "svg",
    "g",
    "defs",
    "switch",
    "animation",
    "arc",
    "circle",
    "ellipse",
    "image",
    "line",
    "path",
    "polygon",
    "polyline",
    "rect",
    "text",
    "textarea",
    "use",
    "video"
};

class QSvgStyleSelector : public QCss::StyleSelector
{
public:
    QSvgStyleSelector()
    {
        nameCaseSensitivity = Qt::CaseInsensitive;
    }
    virtual ~QSvgStyleSelector()
    {
    }

    inline QString nodeToName(QSvgNode *node) const
    {
        return QLatin1String(QSvgStyleSelector_nodeString[node->type()]);
    }

    inline QSvgNode *svgNode(NodePtr node) const
    {
        return (QSvgNode*)node.ptr;
    }
    inline QSvgStructureNode *nodeToStructure(QSvgNode *n) const
    {
        if (n &&
            (n->type() == QSvgNode::DOC ||
             n->type() == QSvgNode::G ||
             n->type() == QSvgNode::DEFS ||
             n->type() == QSvgNode::SWITCH)) {
            return (QSvgStructureNode*)n;
        }
        return 0;
    }

    inline QSvgStructureNode *svgStructure(NodePtr node) const
    {
        QSvgNode *n = svgNode(node);
        QSvgStructureNode *st = nodeToStructure(n);
        return st;
    }

    virtual bool nodeNameEquals(NodePtr node, const QString& nodeName) const
    {
        QSvgNode *n = svgNode(node);
        if (!n)
            return false;
        QString name = nodeToName(n);
        return QString::compare(name, nodeName, Qt::CaseInsensitive) == 0;
    }
    virtual QString attribute(NodePtr node, const QString &name) const
    {
        QSvgNode *n = svgNode(node);
        if ((!n->nodeId().isEmpty() && (name == QLatin1String("id") ||
                                        name == QLatin1String("xml:id"))))
            return n->nodeId();
        if (!n->xmlClass().isEmpty() && name == QLatin1String("class"))
            return n->xmlClass();
        return QString();
    }
    virtual bool hasAttributes(NodePtr node) const
    {
        QSvgNode *n = svgNode(node);
        return (n &&
                (!n->nodeId().isEmpty() || !n->xmlClass().isEmpty()));
    }

    virtual QStringList nodeIds(NodePtr node) const
    {
        QSvgNode *n = svgNode(node);
        QString nid;
        if (n)
            nid = n->nodeId();
        QStringList lst; lst.append(nid);
        return lst;
    }

    virtual QStringList nodeNames(NodePtr node) const
    {
        QSvgNode *n = svgNode(node);
        if (n)
           return QStringList(nodeToName(n));
        return QStringList();
    }

    virtual bool isNullNode(NodePtr node) const
    {
        return !node.ptr;
    }

    virtual NodePtr parentNode(NodePtr node) const
    {
        QSvgNode *n = svgNode(node);
        NodePtr newNode;
        newNode.ptr = 0;
        newNode.id = 0;
        if (n) {
            QSvgNode *svgParent = n->parent();
            if (svgParent) {
                newNode.ptr = svgParent;
            }
        }
        return newNode;
    }
    virtual NodePtr previousSiblingNode(NodePtr node) const
    {
        NodePtr newNode;
        newNode.ptr = 0;
        newNode.id = 0;

        QSvgNode *n = svgNode(node);
        if (!n)
            return newNode;
        QSvgStructureNode *svgParent = nodeToStructure(n->parent());

        if (svgParent) {
            newNode.ptr = svgParent->previousSiblingNode(n);
        }
        return newNode;
    }
    virtual NodePtr duplicateNode(NodePtr node) const
    {
        NodePtr n;
        n.ptr = node.ptr;
        n.id  = node.id;
        return n;
    }
    virtual void freeNode(NodePtr node) const
    {
        Q_UNUSED(node);
    }
};

static qreal toDouble(const QChar *&str)
{
    const int maxLen = 255;//technically doubles can go til 308+ but whatever
    char temp[maxLen+1];
    int pos = 0;

    if (*str == QLatin1Char('-')) {
        temp[pos++] = '-';
        ++str;
    } else if (*str == QLatin1Char('+')) {
        ++str;
    }
    while (*str >= QLatin1Char('0') && *str <= QLatin1Char('9') && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    if (*str == QLatin1Char('.') && pos < maxLen) {
        temp[pos++] = '.';
        ++str;
    }
    while (*str >= QLatin1Char('0') && *str <= QLatin1Char('9') && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    bool exponent = false;
    if (*str == QLatin1Char('e') && pos < maxLen) {
        exponent = true;
        temp[pos++] = 'e';
        ++str;
        if ((*str == QLatin1Char('-') || *str == QLatin1Char('+')) && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
        while (*str >= QLatin1Char('0') && *str <= QLatin1Char('9') && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
    }
    temp[pos] = '\0';

    qreal val;
    if (!exponent && pos < 10) {
        int ival = 0;
        const char *t = temp;
        bool neg = false;
        if(*t == '-') {
            neg = true;
            ++t;
        }
        while(*t && *t != '.') {
            ival *= 10;
            ival += (*t) - '0';
            ++t;
        }
        if(*t == '.') {
            ++t;
            int div = 1;
            while(*t) {
                ival *= 10;
                ival += (*t) - '0';
                div *= 10;
                ++t;
            }
            val = ((qreal)ival)/((qreal)div);
        } else {
            val = ival;
        }
        if (neg)
            val = -val;
    } else {
#ifdef Q_WS_QWS
        if(sizeof(qreal) == sizeof(float))
            val = strtof(temp, 0);
        else
#endif
        {
            bool ok = false;
            val = qstrtod(temp, 0, &ok);
        }
    }
    return val;

}
static qreal toDouble(const QString &str)
{
    const QChar *c = str.constData();
    return toDouble(c);
}

static qreal toDouble(const QStringRef &str)
{
    const QChar *c = str.constData();
    return toDouble(c);
}

static QVector<qreal> parseNumbersList(const QChar *&str)
{
    QVector<qreal> points;
    if (!str)
        return points;
    points.reserve(32);

    while (*str == QLatin1Char(' '))
        ++str;
    while ((*str >= QLatin1Char('0') && *str <= QLatin1Char('9')) ||
           *str == QLatin1Char('-') || *str == QLatin1Char('+') ||
           *str == QLatin1Char('.')) {

        points.append(toDouble(str));

        while (*str == QLatin1Char(' '))
            ++str;
        if (*str == QLatin1Char(','))
            ++str;

        //eat the rest of space
        while (*str == QLatin1Char(' '))
            ++str;
    }

    return points;
}

static QVector<qreal> parsePercentageList(const QChar *&str)
{
    QVector<qreal> points;
    if (!str)
        return points;

    while (str->isSpace())
        ++str;
    while ((*str >= QLatin1Char('0') && *str <= QLatin1Char('9')) ||
           *str == QLatin1Char('-') || *str == QLatin1Char('+') ||
           *str == QLatin1Char('.')) {

        points.append(toDouble(str));

        while (*str == QLatin1Char(' '))
            ++str;
        if (*str == QLatin1Char('%'))
            ++str;
        while (*str == QLatin1Char(' '))
            ++str;
        if (*str == QLatin1Char(','))
            ++str;

        //eat the rest of space
        while (*str == QLatin1Char(' '))
            ++str;
    }

    return points;
}

static QString idFromUrl(const QString &url)
{
    QString::const_iterator itr = url.constBegin();
    while ((*itr).isSpace())
        ++itr;
    if ((*itr) == QLatin1Char('('))
        ++itr;
    while ((*itr).isSpace())
        ++itr;
    if ((*itr) == QLatin1Char('#'))
        ++itr;
    QString id;
    while ((*itr) != QLatin1Char(')')) {
        id += *itr;
        ++itr;
    }
    return id;
}

/**
 * returns true when successfuly set the color. false signifies
 * that the color should be inherited
 */
static bool resolveColor(const QString &colorStr, QColor &color, QSvgHandler *handler)
{
    QString colorStrTr = colorStr.trimmed();
    if (colorStr.startsWith(QLatin1String("rgb("))) {
        const QChar *s = colorStr.constData() + 4;
        QVector<qreal> compo = parseNumbersList(s);
        //1 means that it failed after reaching non-parsable
        //character which is going to be "%"
        if (compo.size() == 1) {
            const QChar *s = colorStr.constData() + 4;
            compo = parsePercentageList(s);
            compo[0] *= (qreal)2.55;
            compo[1] *= (qreal)2.55;
            compo[2] *= (qreal)2.55;
        }

        color = QColor(int(compo[0]),
                       int(compo[1]),
                       int(compo[2]));
        return true;
    } else if (colorStr == QLatin1String("inherited") ||
               colorStr == QLatin1String("inherit"))  {
        return false;
    } else if (colorStr == QLatin1String("currentColor")) {
        color = handler->currentColor();
        return true;
    }

    color = QColor(colorStrTr);
    return color.isValid();
}

static bool constructColor(const QString &colorStr, const QString &opacity,
                           QColor &color, QSvgHandler *handler)
{
    if (!resolveColor(colorStr, color, handler))
        return false;
    if (!opacity.isEmpty()) {
        qreal op = toDouble(opacity);
        if (op <= 1)
            op *= 255;
        color.setAlpha(int(op));
    }
    return true;
}

static qreal parseLength(const QString &str, QSvgHandler::LengthType &type,
                         QSvgHandler *handler)
{
    QString numStr = str.trimmed();

    if (numStr.endsWith(QLatin1Char('%'))) {
        numStr.chop(1);
        type = QSvgHandler::LT_PERCENT;
    } else if (numStr.endsWith(QLatin1String("px"))) {
        numStr.chop(2);
        type = QSvgHandler::LT_PX;
    } else if (numStr.endsWith(QLatin1String("pc"))) {
        numStr.chop(2);
        type = QSvgHandler::LT_PC;
    } else if (numStr.endsWith(QLatin1String("pt"))) {
        numStr.chop(2);
        type = QSvgHandler::LT_PT;
    } else if (numStr.endsWith(QLatin1String("mm"))) {
        numStr.chop(2);
        type = QSvgHandler::LT_MM;
    } else if (numStr.endsWith(QLatin1String("cm"))) {
        numStr.chop(2);
        type = QSvgHandler::LT_CM;
    } else if (numStr.endsWith(QLatin1String("in"))) {
        numStr.chop(2);
        type = QSvgHandler::LT_IN;
    } else {
        type = handler->defaultCoordinateSystem();
        //type = QSvgHandler::LT_OTHER;
    }
    qreal len = toDouble(numStr);
    //qDebug()<<"len is "<<len<<", from '"<<numStr << "'";
    return len;
}

static inline qreal convertToNumber(const QString &str, QSvgHandler *handler)
{
    QSvgHandler::LengthType type;
    qreal num = parseLength(str, type, handler);
    if (type == QSvgHandler::LT_PERCENT) {
        num = num/100.0;
    }
    return num;
}

static bool createSvgGlyph(QSvgFont *font, const QXmlStreamAttributes &attributes)
{
    QStringRef uncStr = attributes.value(QLatin1String("unicode"));
    QStringRef havStr = attributes.value(QLatin1String("horiz-adv-x"));
    QStringRef pathStr = attributes.value(QLatin1String("d"));

    QChar unicode = (uncStr.isEmpty()) ? 0 : uncStr.at(0);
    qreal havx = (havStr.isEmpty()) ? -1 : toDouble(havStr);
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    parsePathDataFast(pathStr, path);

    font->addGlyph(unicode, path, havx);

    return true;
}

// this should really be called convertToDefaultCoordinateSystem
// and convert when type != QSvgHandler::defaultCoordinateSystem
static qreal convertToPixels(qreal len, bool , QSvgHandler::LengthType type)
{

    switch (type) {
    case QSvgHandler::LT_PERCENT:
        break;
    case QSvgHandler::LT_PX:
        break;
    case QSvgHandler::LT_PC:
        break;
    case QSvgHandler::LT_PT:
        return len * 1.25;
        break;
    case QSvgHandler::LT_MM:
        return len * 3.543307;
        break;
    case QSvgHandler::LT_CM:
        return len * 35.43307;
        break;
    case QSvgHandler::LT_IN:
        return len * 90;
        break;
    case QSvgHandler::LT_OTHER:
        break;
    default:
        break;
    }
    return len;
}

static void parseColor(QSvgNode *,
                       const QSvgAttributes &attributes,
                       QSvgHandler *handler)
{
    QString colorStr = attributes.value(QLatin1String("color")).toString();
    QString opacity  = attributes.value(QLatin1String("color-opacity")).toString();
    QColor color;
    if (constructColor(colorStr, opacity, color, handler)) {
        handler->pushColor(color);
    }
}

static QSvgStyleProperty *styleFromUrl(QSvgNode *node, const QString &url)
{
    while (node && (node->type() != QSvgNode::DOC  &&
                    node->type() != QSvgNode::G    &&
                    node->type() != QSvgNode::DEFS &&
                    node->type() != QSvgNode::SWITCH)) {
        node = node->parent();
    }
    if (!node)
        return 0;
    return static_cast<QSvgStructureNode*>(node)->scopeStyle(idFromUrl(url));
}

static void parseBrush(QSvgNode *node,
                       const QSvgAttributes &attributes,
                       QSvgHandler *handler)
{
    QString value = attributes.value(QLatin1String("fill")).toString();
    QString fillRule = attributes.value(QLatin1String("fill-rule")).toString();
    QString opacity = attributes.value(QLatin1String("fill-opacity")).toString();
    QString myId = someId(attributes);

    value = value.trimmed();
    fillRule = fillRule.trimmed();
    if (!value.isEmpty() || !fillRule.isEmpty()) {
        Qt::FillRule f = Qt::WindingFill;
        if (fillRule == QLatin1String("evenodd"))
            f = Qt::OddEvenFill;
        if (value.startsWith(QLatin1String("url"))) {
            value = value.remove(0, 3);
            QSvgStyleProperty *style = styleFromUrl(node, value);
            if (style) {
                QSvgFillStyle *prop = new QSvgFillStyle(style);
                if (!opacity.isEmpty())
                    prop->setFillOpacity(toDouble(opacity));
                node->appendStyleProperty(prop, myId);
            } else {
                qWarning("Couldn't resolve property: %s", qPrintable(idFromUrl(value)));
            }
        } else if (value != QLatin1String("none")) {
            QColor color;
            if (constructColor(value, opacity, color, handler)) {
                QSvgFillStyle *prop = new QSvgFillStyle(QBrush(color));
                if (!fillRule.isEmpty())
                    prop->setFillRule(f);
                node->appendStyleProperty(prop, myId);
            }
        } else {
            QSvgFillStyle *prop = new QSvgFillStyle(QBrush(Qt::NoBrush));
            if (!fillRule.isEmpty())
                prop->setFillRule(f);
            node->appendStyleProperty(prop, myId);
        }
    }
}

static void parseQPen(QPen &pen, QSvgNode *node,
                      const QSvgAttributes &attributes,
                      QSvgHandler *handler)
{
    QString value      = attributes.value(QLatin1String("stroke")).toString();
    QString dashArray  = attributes.value(QLatin1String("stroke-dasharray")).toString();
    QString dashOffset = attributes.value(QLatin1String("stroke-dashoffset")).toString();
    QString linecap    = attributes.value(QLatin1String("stroke-linecap")).toString();
    QString linejoin   = attributes.value(QLatin1String("stroke-linejoin")).toString();
    QString miterlimit = attributes.value(QLatin1String("stroke-miterlimit")).toString();
    QString opacity    = attributes.value(QLatin1String("stroke-opacity")).toString();
    QString width      = attributes.value(QLatin1String("stroke-width")).toString();
    QString myId       = someId(attributes);

    if (!value.isEmpty() || !width.isEmpty()) {
        if (value != QLatin1String("none")) {
            if (!value.isEmpty()) {
                if (node && value.startsWith(QLatin1String("url"))) {
                    value = value.remove(0, 3);
                    QSvgStyleProperty *style = styleFromUrl(node, value);
                    if (style) {
                        if (style->type() == QSvgStyleProperty::GRADIENT) {
                            QBrush b(*((QSvgGradientStyle*)style)->qgradient());
                            pen.setBrush(b);
                        } else if (style->type() == QSvgStyleProperty::SOLID_COLOR) {
                            pen.setColor(
                                ((QSvgSolidColorStyle*)style)->qcolor());
                        }
                    } else {
                        qWarning()<<"QSvgHandler::parsePen could not resolve property" << idFromUrl(value);
                    }
                } else {
                    QColor color;
                    if (constructColor(value, opacity, color, handler))
                        pen.setColor(color);
                }
                //since we could inherit stroke="none"
                //we need to reset the style of our stroke to something
                pen.setStyle(Qt::SolidLine);
            }
            if (!width.isEmpty()) {
                QSvgHandler::LengthType lt;
                qreal widthF = parseLength(width, lt, handler);
                //### fixme
                if (!widthF) {
                    pen.setStyle(Qt::NoPen);
                    return;
                }
                pen.setWidthF(widthF);
            }
            qreal penw = pen.widthF();

            if (!linejoin.isEmpty()) {
                if (linejoin == QLatin1String("miter"))
                    pen.setJoinStyle(Qt::SvgMiterJoin);
                else if (linejoin == QLatin1String("round"))
                    pen.setJoinStyle(Qt::RoundJoin);
                else if (linejoin == QLatin1String("bevel"))
                    pen.setJoinStyle(Qt::BevelJoin);
            }
            if (!miterlimit.isEmpty())
                pen.setMiterLimit(toDouble(miterlimit));

            if (!linecap.isEmpty()) {
                if (linecap == QLatin1String("butt"))
                    pen.setCapStyle(Qt::FlatCap);
                else if (linecap == QLatin1String("round"))
                    pen.setCapStyle(Qt::RoundCap);
                else if (linecap == QLatin1String("square"))
                    pen.setCapStyle(Qt::SquareCap);
            }

            if (!dashArray.isEmpty()) {
                const QChar *s = dashArray.constData();
                QVector<qreal> dashes = parseNumbersList(s);
                qreal *d = dashes.data();
                if (penw != 0)
                    for (int i = 0; i < dashes.size(); ++i) {
                        *d /= penw;
                        ++d;
                }
                pen.setDashPattern(dashes);
            }
            if (!dashOffset.isEmpty()) {
                pen.setDashOffset(toDouble(dashOffset));
            }

        } else {
            pen.setStyle(Qt::NoPen);
        }
    }
}

static QMatrix parseTransformationMatrix(const QString &value)
{
    QMatrix matrix;
    const QChar *str = value.constData();

    while (*str != QLatin1Char(0)) {
        if (str->isSpace() || *str == QLatin1Char(',')) {
            ++str;
            continue;
        }
        enum State {
            Matrix,
            Translate,
            Rotate,
            Scale,
            SkewX,
            SkewY
        };
        State state = Matrix;
        if (*str == QLatin1Char('m')) {  //matrix
            const char *ident = "atrix";
            for (int i = 0; i < 5; ++i)
                if (*(++str) != QLatin1Char(ident[i]))
                    goto error;
            ++str;
            state = Matrix;
        } else if (*str == QLatin1Char('t')) { //translate
            const char *ident = "ranslate";
            for (int i = 0; i < 8; ++i)
                if (*(++str) != QLatin1Char(ident[i]))
                    goto error;
            ++str;
            state = Translate;
        } else if (*str == QLatin1Char('r')) { //rotate
            const char *ident = "otate";
            for (int i = 0; i < 5; ++i)
                if (*(++str) != QLatin1Char(ident[i]))
                    goto error;
            ++str;
            state = Rotate;
        } else if (*str == QLatin1Char('s')) { //scale, skewX, skewY
            ++str;
            if (*str == QLatin1Char('c')) {
                const char *ident = "ale";
                for (int i = 0; i < 3; ++i)
                    if (*(++str) != QLatin1Char(ident[i]))
                        goto error;
                ++str;
                state = Scale;
            } else if (*str == QLatin1Char('k')) {
                if (*(++str) != QLatin1Char('e'))
                    goto error;
                if (*(++str) != QLatin1Char('w'))
                    goto error;
                ++str;
                if (*str == QLatin1Char('X'))
                    state = SkewX;
                else if (*str == QLatin1Char('Y'))
                    state = SkewY;
                else
                    goto error;
                ++str;
            } else {
                goto error;
            }
        } else {
            goto error;
        }


        while (str->isSpace())
            ++str;
        if (*str != QLatin1Char('('))
            goto error;
        ++str;
        QVector<qreal> points = parseNumbersList(str);
        if (*str != QLatin1Char(')'))
            goto error;
        ++str;

        if(state == Matrix) {
            if(points.count() != 6)
                goto error;
            matrix = matrix * QMatrix(points[0], points[1],
                                      points[2], points[3],
                                      points[4], points[5]);
        } else if (state == Translate) {
            if (points.count() == 1)
                matrix.translate(points[0], 0);
            else if (points.count() == 2)
                matrix.translate(points[0], points[1]);
            else
                goto error;
        } else if (state == Rotate) {
            if(points.count() == 1) {
                matrix.rotate(points[0]);
            } else if (points.count() == 3) {
                matrix.translate(points[1], points[2]);
                matrix.rotate(points[0]);
                matrix.translate(-points[1], -points[2]);
            } else {
                goto error;
            }
        } else if (state == Scale) {
            if (points.count() < 1 || points.count() > 2)
                goto error;
            qreal sx = points[0];
            qreal sy = sx;
            if(points.count() == 2)
                sy = points[1];
            matrix.scale(sx, sy);
        } else if (state == SkewX) {
            if (points.count() != 1)
                goto error;
            const qreal deg2rad = qreal(0.017453292519943295769);
            matrix.shear(tan(points[0]*deg2rad), 0);
        } else if (state == SkewY) {
            if (points.count() != 1)
                goto error;
            const qreal deg2rad = qreal(0.017453292519943295769);
            matrix.shear(0, tan(points[0]*deg2rad));
        }
    }
  error:
    return matrix;
}

static void parsePen(QSvgNode *node,
                     const QSvgAttributes &attributes,
                     QSvgHandler *handler)
{
    QString value      = attributes.value(QLatin1String("stroke")).toString();
    QString dashArray  = attributes.value(QLatin1String("stroke-dasharray")).toString();
    QString dashOffset = attributes.value(QLatin1String("stroke-dashoffset")).toString();
    QString linecap    = attributes.value(QLatin1String("stroke-linecap")).toString();
    QString linejoin   = attributes.value(QLatin1String("stroke-linejoin")).toString();
    QString miterlimit = attributes.value(QLatin1String("stroke-miterlimit")).toString();
    QString opacity    = attributes.value(QLatin1String("stroke-opacity")).toString();
    QString width      = attributes.value(QLatin1String("stroke-width")).toString();
    QString myId       = someId(attributes);

    //qDebug()<<"Node "<<node->type()<<", attrs are "<<value<<width;

    if (!value.isEmpty() || !width.isEmpty() || !linecap.isEmpty() || !linejoin.isEmpty()) {
        if (value != QLatin1String("none")) {
            QSvgStrokeStyle *inherited =
                static_cast<QSvgStrokeStyle*>(node->styleProperty(
                                                  QSvgStyleProperty::STROKE));
            if (!inherited)
                inherited = static_cast<QSvgStrokeStyle*>(node->parent()->styleProperty(
                                                              QSvgStyleProperty::STROKE));
            QPen pen(handler->defaultPen());
            if (inherited)
                pen = inherited->qpen();

            if (!value.isEmpty()) {
                if (value.startsWith(QLatin1String("url"))) {
                    value = value.remove(0, 3);
                    QSvgStyleProperty *style = styleFromUrl(node, value);
                    if (style) {
                        if (style->type() == QSvgStyleProperty::GRADIENT) {
                            QBrush b(*((QSvgGradientStyle*)style)->qgradient());
                            pen.setBrush(b);
                        } else if (style->type() == QSvgStyleProperty::SOLID_COLOR) {
                            pen.setColor(
                                ((QSvgSolidColorStyle*)style)->qcolor());
                        }
                    } else {
                        qWarning() << "QSvgHandler::parsePen could not resolve property" << idFromUrl(value);
                    }
                } else {
                    QColor color;
                    if (constructColor(value, opacity, color, handler))
                        pen.setColor(color);
                }
                //since we could inherit stroke="none"
                //we need to reset the style of our stroke to something
                pen.setStyle(Qt::SolidLine);
            }
            if (!width.isEmpty()) {
                QSvgHandler::LengthType lt;
                qreal widthF = parseLength(width, lt, handler);
                //### fixme
                if (!widthF) {
                    pen.setStyle(Qt::NoPen);
                    return;
                }
                pen.setWidthF(widthF);
            }

            if (!linejoin.isEmpty()) {
                if (linejoin == QLatin1String("miter"))
                    pen.setJoinStyle(Qt::SvgMiterJoin);
                else if (linejoin == QLatin1String("round"))
                    pen.setJoinStyle(Qt::RoundJoin);
                else if (linejoin == QLatin1String("bevel"))
                    pen.setJoinStyle(Qt::BevelJoin);
            }

            if (!linecap.isEmpty()) {
                if (linecap == QLatin1String("butt"))
                    pen.setCapStyle(Qt::FlatCap);
                else if (linecap == QLatin1String("round"))
                    pen.setCapStyle(Qt::RoundCap);
                else if (linecap == QLatin1String("square"))
                    pen.setCapStyle(Qt::SquareCap);
            }

            qreal penw = pen.widthF();
            if (!dashArray.isEmpty()) {
                const QChar *s = dashArray.constData();
                QVector<qreal> dashes = parseNumbersList(s);
                qreal *d = dashes.data();
                if(penw != 0)
                    for (int i = 0; i < dashes.size(); ++i) {
                        *d /= penw;
                        ++d;
                    }

                // if the dash count is odd the dashes should be duplicated
                if (dashes.size() % 2 != 0)
                    dashes << QVector<qreal>(dashes);

                pen.setDashPattern(dashes);
            }
            if (!dashOffset.isEmpty()) {
                qreal doffset = toDouble(dashOffset);
                if (penw != 0)
                    doffset /= penw;
                pen.setDashOffset(doffset);
            }
            if (!miterlimit.isEmpty())
                pen.setMiterLimit(toDouble(miterlimit));

            node->appendStyleProperty(new QSvgStrokeStyle(pen), myId);
        } else {
            QPen pen(handler->defaultPen());
            pen.setStyle(Qt::NoPen);
            node->appendStyleProperty(new QSvgStrokeStyle(pen), myId);
        }
    }
}


static bool parseQBrush(const QSvgAttributes &attributes, QSvgNode *node,
                        QBrush &brush, QSvgHandler *handler)
{
    QString value   = attributes.value(QLatin1String("fill")).toString();
    QString opacity = attributes.value(QLatin1String("fill-opacity")).toString();

    QColor color;
    if (!value.isEmpty() || !opacity.isEmpty()) {
        if (value.startsWith(QLatin1String("url"))) {
            value = value.remove(0, 3);
            QSvgStyleProperty *style = styleFromUrl(node, value);
            if (style) {
                switch (style->type()) {
                case QSvgStyleProperty::FILL:
                {
                    brush = static_cast<QSvgFillStyle*>(style)->qbrush();
                    break;
                }
                case QSvgStyleProperty::SOLID_COLOR:
                {
                    brush = QBrush(static_cast<QSvgSolidColorStyle*>(style)->qcolor());
                    break;
                }
                case QSvgStyleProperty::GRADIENT:
                {
                    brush = QBrush(*static_cast<QSvgGradientStyle*>(style)->qgradient());
                    break;
                }
                default:
                    qWarning("Cannot use property \"%s\" as brush.", qPrintable(idFromUrl(value)));
                }
            } else {
                qWarning("Couldn't resolve property: %s", qPrintable(idFromUrl(value)));
            }
        } else if (value != QLatin1String("none")) {
            if (constructColor(value, opacity, color, handler)) {
                brush.setStyle(Qt::SolidPattern);
                brush.setColor(color);
            }
        } else {
            brush = QBrush(Qt::NoBrush);
        }
        return true;
    }
    return false;
}

static bool parseQFont(const QSvgAttributes &attributes,
                       QFont &font, qreal &fontSize, QSvgHandler *handler)
{
    QString family = attributes.value(QLatin1String("font-family")).toString();
    QString size = attributes.value(QLatin1String("font-size")).toString();
    QString style = attributes.value(QLatin1String("font-style")).toString();
    QString weight = attributes.value(QLatin1String("font-weight")).toString();

    if (!family.isEmpty() || !size.isEmpty() ||
        !style.isEmpty() || !weight.isEmpty()) {

        if (!family.isEmpty()) {
            font.setFamily(family.trimmed());
        }
        if (!size.isEmpty()) {
            QSvgHandler::LengthType dummy; // should always be pixel size
            fontSize = parseLength(size, dummy, handler);
            if (fontSize <= 0)
                fontSize = 1;
            font.setPixelSize(qMax(1, int(fontSize)));
        }
        if (!style.isEmpty()) {
            if (style == QLatin1String("normal")) {
                font.setStyle(QFont::StyleNormal);
            } else if (style == QLatin1String("italic")) {
                font.setStyle(QFont::StyleItalic);
            } else if (style == QLatin1String("oblique")) {
                font.setStyle(QFont::StyleOblique);
            } else if (style == QLatin1String("inherit")) {
                //inherited already
            }
        }
        if (!weight.isEmpty()) {
            bool ok = false;
            int weightNum = weight.toInt(&ok);
            if (ok) {
                switch (weightNum) {
                case 100:
                case 200:
                    font.setWeight(QFont::Light);
                    break;
                case 300:
                case 400:
                    font.setWeight(QFont::Normal);
                    break;
                case 500:
                case 600:
                    font.setWeight(QFont::DemiBold);
                    break;
                case 700:
                case 800:
                    font.setWeight(QFont::Bold);
                    break;
                case 900:
                    font.setWeight(QFont::Black);
                    break;
                default:
                    break;
                }
            } else {
                if (weight == QLatin1String("normal")) {
                    font.setWeight(QFont::Normal);
                } else if (weight == QLatin1String("bold")) {
                    font.setWeight(QFont::Bold);
                } else if (weight == QLatin1String("bolder")) {
                    font.setWeight(QFont::DemiBold);
                } else if (weight == QLatin1String("lighter")) {
                    font.setWeight(QFont::Light);
                }
            }
        }
        // QFontInfo fi(font);
        // font.setPointSize(fi.pointSize());
        return true;
    }

    return false;
}

static void parseFont(QSvgNode *node,
                      const QSvgAttributes &attributes,
                      QSvgHandler *handler)
{
    QFont font;
    font.setPixelSize(12);
    qreal fontSize = font.pixelSize();

    QSvgFontStyle *inherited =
        static_cast<QSvgFontStyle*>(node->styleProperty(
                                        QSvgStyleProperty::FONT));
    if (!inherited)
        inherited =
            static_cast<QSvgFontStyle*>(node->parent()->styleProperty(
                                            QSvgStyleProperty::FONT));
    if (inherited) {
        font = inherited->qfont();
        fontSize = inherited->pointSize();
    }
    if (parseQFont(attributes, font, fontSize, handler)) {
        QString myId = someId(attributes);
        QString anchor = attributes.value(QLatin1String("text-anchor")).toString();
        QSvgTinyDocument *doc = node->document();
        QSvgFontStyle *fontStyle = 0;
        QString family = (font.family().isEmpty())?myId:font.family();
        if (!family.isEmpty()) {
            QSvgFont *svgFont = doc->svgFont(family);
            if (svgFont) {
                fontStyle = new QSvgFontStyle(svgFont, doc);
                fontStyle->setPointSize(fontSize);
            }
        }
        if (!fontStyle) {
            fontStyle = new QSvgFontStyle(font, node->document());
            fontStyle->setPointSize(fontSize);
        }
        if (!anchor.isEmpty())
            fontStyle->setTextAnchor(anchor);

        node->appendStyleProperty(fontStyle, myId);
    }
}

static void parseTransform(QSvgNode *node,
                           const QSvgAttributes &attributes,
                           QSvgHandler *)
{
    QString value = attributes.value(QLatin1String("transform")).toString();
    QString myId = someId(attributes);
    value = value.trimmed();
    if (value.isEmpty())
        return;
    QMatrix matrix = parseTransformationMatrix(value);

    if (!matrix.isIdentity()) {
        node->appendStyleProperty(new QSvgTransformStyle(QTransform(matrix)), myId);
    }

}

static void parseVisibility(QSvgNode *node,
                            const QSvgAttributes &attributes,
                            QSvgHandler *)
{
    QString value = attributes.value(QLatin1String("visibility")).toString();
    QSvgNode *parent = node->parent();

    if (parent && (value.isEmpty() || value == QLatin1String("inherit")))
        node->setVisible(parent->isVisible());
    else if (value == QLatin1String("hidden") || value == QLatin1String("collapse")) {
        node->setVisible(false);
    } else
        node->setVisible(true);
}

static void pathArcSegment(QPainterPath &path,
                           qreal xc, qreal yc,
                           qreal th0, qreal th1,
                           qreal rx, qreal ry, qreal xAxisRotation)
{
    qreal sinTh, cosTh;
    qreal a00, a01, a10, a11;
    qreal x1, y1, x2, y2, x3, y3;
    qreal t;
    qreal thHalf;

    sinTh = qSin(xAxisRotation * (Q_PI / 180.0));
    cosTh = qCos(xAxisRotation * (Q_PI / 180.0));

    a00 =  cosTh * rx;
    a01 = -sinTh * ry;
    a10 =  sinTh * rx;
    a11 =  cosTh * ry;

    thHalf = 0.5 * (th1 - th0);
    t = (8.0 / 3.0) * qSin(thHalf * 0.5) * qSin(thHalf * 0.5) / qSin(thHalf);
    x1 = xc + qCos(th0) - t * qSin(th0);
    y1 = yc + qSin(th0) + t * qCos(th0);
    x3 = xc + qCos(th1);
    y3 = yc + qSin(th1);
    x2 = x3 + t * qSin(th1);
    y2 = y3 - t * qCos(th1);

    path.cubicTo(a00 * x1 + a01 * y1, a10 * x1 + a11 * y1,
                 a00 * x2 + a01 * y2, a10 * x2 + a11 * y2,
                 a00 * x3 + a01 * y3, a10 * x3 + a11 * y3);
}

// the arc handling code underneath is from XSVG (BSD license)
/*
 * Copyright  2002 USC/Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Information Sciences Institute not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Information Sciences Institute
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * INFORMATION SCIENCES INSTITUTE DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INFORMATION SCIENCES
 * INSTITUTE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
static void pathArc(QPainterPath &path,
                    qreal               rx,
                    qreal               ry,
                    qreal               x_axis_rotation,
                    int         large_arc_flag,
                    int         sweep_flag,
                    qreal               x,
                    qreal               y,
                    qreal curx, qreal cury)
{
    qreal sin_th, cos_th;
    qreal a00, a01, a10, a11;
    qreal x0, y0, x1, y1, xc, yc;
    qreal d, sfactor, sfactor_sq;
    qreal th0, th1, th_arc;
    int i, n_segs;
    qreal dx, dy, dx1, dy1, Pr1, Pr2, Px, Py, check;

    rx = qAbs(rx);
    ry = qAbs(ry);

    sin_th = qSin(x_axis_rotation * (Q_PI / 180.0));
    cos_th = qCos(x_axis_rotation * (Q_PI / 180.0));

    dx = (curx - x) / 2.0;
    dy = (cury - y) / 2.0;
    dx1 =  cos_th * dx + sin_th * dy;
    dy1 = -sin_th * dx + cos_th * dy;
    Pr1 = rx * rx;
    Pr2 = ry * ry;
    Px = dx1 * dx1;
    Py = dy1 * dy1;
    /* Spec : check if radii are large enough */
    check = Px / Pr1 + Py / Pr2;
    if (check > 1) {
        rx = rx * qSqrt(check);
        ry = ry * qSqrt(check);
    }

    a00 =  cos_th / rx;
    a01 =  sin_th / rx;
    a10 = -sin_th / ry;
    a11 =  cos_th / ry;
    x0 = a00 * curx + a01 * cury;
    y0 = a10 * curx + a11 * cury;
    x1 = a00 * x + a01 * y;
    y1 = a10 * x + a11 * y;
    /* (x0, y0) is current point in transformed coordinate space.
       (x1, y1) is new point in transformed coordinate space.

       The arc fits a unit-radius circle in this space.
    */
    d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
    sfactor_sq = 1.0 / d - 0.25;
    if (sfactor_sq < 0) sfactor_sq = 0;
    sfactor = qSqrt(sfactor_sq);
    if (sweep_flag == large_arc_flag) sfactor = -sfactor;
    xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
    yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
    /* (xc, yc) is center of the circle. */

    th0 = atan2(y0 - yc, x0 - xc);
    th1 = atan2(y1 - yc, x1 - xc);

    th_arc = th1 - th0;
    if (th_arc < 0 && sweep_flag)
        th_arc += 2 * Q_PI;
    else if (th_arc > 0 && !sweep_flag)
        th_arc -= 2 * Q_PI;

    n_segs = qCeil(qAbs(th_arc / (Q_PI * 0.5 + 0.001)));

    for (i = 0; i < n_segs; i++) {
        pathArcSegment(path, xc, yc,
                       th0 + i * th_arc / n_segs,
                       th0 + (i + 1) * th_arc / n_segs,
                       rx, ry, x_axis_rotation);
    }
}

static bool parsePathDataFast(const QStringRef &dataStr, QPainterPath &path)
{
    qreal x0 = 0, y0 = 0;              // starting point
    qreal x = 0, y = 0;                // current point
    char lastMode = 0;
    QPointF ctrlPt;
    const QChar *str = dataStr.constData();
    const QChar *end = str + dataStr.size();

    while (str != end) {
        while (*str == QLatin1Char(' '))
            ++str;
        QChar pathElem = *str;
        ++str;
        QChar endc = *end;
        *const_cast<QChar *>(end) = 0; // parseNumbersList requires 0-termination that QStringRef cannot guarantee
        QVector<qreal> arg = parseNumbersList(str);
        *const_cast<QChar *>(end) = endc;
        if (pathElem == QLatin1Char('z') || pathElem == QLatin1Char('Z'))
            arg.append(0);//dummy
        while (!arg.isEmpty()) {
            qreal offsetX = x;        // correction offsets
            qreal offsetY = y;        // for relative commands
            switch (pathElem.unicode()) {
            case 'm': {
                if (arg.count() < 2) {
                    arg.pop_front();
                    break;
                }
                x = x0 = arg[0] + offsetX;
                y = y0 = arg[1] + offsetY;
                path.moveTo(x0, y0);
                arg.pop_front(); arg.pop_front();
            }
                break;
            case 'M': {
                if (arg.count() < 2) {
                    arg.pop_front();
                    break;
                }
                x = x0 = arg[0];
                y = y0 = arg[1];

                path.moveTo(x0, y0);
                arg.pop_front(); arg.pop_front();
            }
                break;
            case 'z':
            case 'Z': {
                x = x0;
                y = y0;
                path.closeSubpath();
                arg.pop_front();//pop dummy
            }
                break;
            case 'l': {
                if (arg.count() < 2) {
                    arg.pop_front();
                    break;
                }
                x = arg.front() + offsetX;
                arg.pop_front();
                y = arg.front() + offsetY;
                arg.pop_front();
                path.lineTo(x, y);

            }
                break;
            case 'L': {
                if (arg.count() < 2) {
                    arg.pop_front();
                    break;
                }
                x = arg.front(); arg.pop_front();
                y = arg.front(); arg.pop_front();
                path.lineTo(x, y);
            }
                break;
            case 'h': {
                x = arg.front() + offsetX; arg.pop_front();
                path.lineTo(x, y);
            }
                break;
            case 'H': {
                x = arg[0];
                path.lineTo(x, y);
                arg.pop_front();
            }
                break;
            case 'v': {
                y = arg[0] + offsetY;
                path.lineTo(x, y);
                arg.pop_front();
            }
                break;
            case 'V': {
                y = arg[0];
                path.lineTo(x, y);
                arg.pop_front();
            }
                break;
            case 'c': {
                if (arg.count() < 6) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF c1(arg[0]+offsetX, arg[1]+offsetY);
                QPointF c2(arg[2]+offsetX, arg[3]+offsetY);
                QPointF e(arg[4]+offsetX, arg[5]+offsetY);
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 'C': {
                if (arg.count() < 6) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF c1(arg[0], arg[1]);
                QPointF c2(arg[2], arg[3]);
                QPointF e(arg[4], arg[5]);
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 's': {
                if (arg.count() < 4) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF c1;
                if (lastMode == 'c' || lastMode == 'C' ||
                    lastMode == 's' || lastMode == 'S')
                    c1 = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                else
                    c1 = QPointF(x, y);
                QPointF c2(arg[0]+offsetX, arg[1]+offsetY);
                QPointF e(arg[2]+offsetX, arg[3]+offsetY);
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 'S': {
                if (arg.count() < 4) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF c1;
                if (lastMode == 'c' || lastMode == 'C' ||
                    lastMode == 's' || lastMode == 'S')
                    c1 = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                else
                    c1 = QPointF(x, y);
                QPointF c2(arg[0], arg[1]);
                QPointF e(arg[2], arg[3]);
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 'q': {
                if (arg.count() < 4) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF c(arg[0]+offsetX, arg[1]+offsetY);
                QPointF e(arg[2]+offsetX, arg[3]+offsetY);
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 'Q': {
                if (arg.count() < 4) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF c(arg[0], arg[1]);
                QPointF e(arg[2], arg[3]);
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 't': {
                if (arg.count() < 2) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF e(arg[0]+offsetX, arg[1]+offsetY);
                QPointF c;
                if (lastMode == 'q' || lastMode == 'Q' ||
                    lastMode == 't' || lastMode == 'T')
                    c = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                else
                    c = QPointF(x, y);
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 'T': {
                if (arg.count() < 2) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                QPointF e(arg[0], arg[1]);
                QPointF c;
                if (lastMode == 'q' || lastMode == 'Q' ||
                    lastMode == 't' || lastMode == 'T')
                    c = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                else
                    c = QPointF(x, y);
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                arg.pop_front(); arg.pop_front();
                break;
            }
            case 'a': {
                if (arg.count() < 7) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                qreal rx = arg[0];
                qreal ry = arg[1];
                qreal xAxisRotation = arg[2];
                qreal largeArcFlag  = arg[3];
                qreal sweepFlag = arg[4];
                qreal ex = arg[5] + offsetX;
                qreal ey = arg[6] + offsetY;
                qreal curx = x;
                qreal cury = y;
                pathArc(path, rx, ry, xAxisRotation, int(largeArcFlag),
                        int(sweepFlag), ex, ey, curx, cury);

                x = ex;
                y = ey;

                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                arg.pop_front();
            }
                break;
            case 'A': {
                if (arg.count() < 7) {
                    while (arg.count())
                        arg.pop_front();
                    break;
                }
                qreal rx = arg[0];
                qreal ry = arg[1];
                qreal xAxisRotation = arg[2];
                qreal largeArcFlag  = arg[3];
                qreal sweepFlag = arg[4];
                qreal ex = arg[5];
                qreal ey = arg[6];
                qreal curx = x;
                qreal cury = y;
                pathArc(path, rx, ry, xAxisRotation, int(largeArcFlag),
                        int(sweepFlag), ex, ey, curx, cury);
                x = ex;
                y = ey;
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                arg.pop_front(); arg.pop_front();
                arg.pop_front();
            }
                break;
            default:
                return false;
            }
            lastMode = pathElem.toLatin1();
        }
    }
    return true;
}

static bool parseStyle(QSvgNode *node,
                       const QXmlStreamAttributes &attributes,
                       QSvgHandler *);

static bool parseStyle(QSvgNode *node,
                       const QSvgAttributes &attributes,
                       QSvgHandler *);

static void parseCSStoXMLAttrs(const QVector<QCss::Declaration> &declarations,
                               QXmlStreamAttributes &attributes)
{
    for (int i = 0; i < declarations.count(); ++i) {
        const QCss::Declaration &decl = declarations.at(i);
        if (decl.d->property.isEmpty())
            continue;
        if (decl.d->values.count() != 1)
            continue;
        QCss::Value val = decl.d->values.first();
        QString valueStr = val.toString();
        if (val.type == QCss::Value::Uri) {
            valueStr.prepend(QLatin1String("url("));
            valueStr.append(QLatin1Char(')'));
        } else if (val.type == QCss::Value::Function) {
            QStringList lst = val.variant.toStringList();
            valueStr.append(lst.at(0));
            valueStr.append(QLatin1Char('('));
            for (int i = 1; i < lst.count(); ++i) {
                valueStr.append(lst.at(i));
                if ((i +1) < lst.count())
                    valueStr.append(QLatin1Char(','));
            }
            valueStr.append(QLatin1Char(')'));
        } else if (val.type == QCss::Value::KnownIdentifier) {
            switch (val.variant.toInt()) {
            case QCss::Value_None:
                valueStr = QLatin1String("none");
                break;
            default:
                break;
            }
        }

        attributes.append(QString(), decl.d->property, valueStr);
    }
}

void QSvgHandler::parseCSStoXMLAttrs(QString css, QVector<QSvgCssAttribute> *attributes)
{
    // preprocess (for unicode escapes), tokenize and remove comments
    m_cssParser.init(css);
    QString key;

    attributes->reserve(10);

    while (m_cssParser.hasNext()) {
        m_cssParser.skipSpace();

        if (!m_cssParser.hasNext())
            break;
        m_cssParser.next();

        QStringRef name;
        if (m_cssParser.hasEscapeSequences) {
            key = m_cssParser.lexem();
            name = QStringRef(&key, 0, key.length());
        } else {
            const QCss::Symbol &sym = m_cssParser.symbol();
            name = QStringRef(&sym.text, sym.start, sym.len);
        }

        m_cssParser.skipSpace();
        if (!m_cssParser.test(QCss::COLON))
            break;

        m_cssParser.skipSpace();
        if (!m_cssParser.hasNext())
            break;

        QSvgCssAttribute attribute;
        attribute.name = QXmlStreamStringRef(name);

        const int firstSymbol = m_cssParser.index;
        int symbolCount = 0;
        do {
            m_cssParser.next();
            ++symbolCount;
        } while (m_cssParser.hasNext() && !m_cssParser.test(QCss::SEMICOLON));

        bool canExtractValueByRef = !m_cssParser.hasEscapeSequences;
        if (canExtractValueByRef) {
            int len = m_cssParser.symbols.at(firstSymbol).len;
            for (int i = firstSymbol + 1; i < firstSymbol + symbolCount; ++i) {
                len += m_cssParser.symbols.at(i).len;

                if (m_cssParser.symbols.at(i - 1).start + m_cssParser.symbols.at(i - 1).len
                        != m_cssParser.symbols.at(i).start) {
                    canExtractValueByRef = false;
                    break;
                }
            }
            if (canExtractValueByRef) {
                const QCss::Symbol &sym = m_cssParser.symbols.at(firstSymbol);
                attribute.value = QXmlStreamStringRef(QStringRef(&sym.text, sym.start, len));
            }
        }
        if (!canExtractValueByRef) {
            QString value;
            for (int i = firstSymbol; i < m_cssParser.index - 1; ++i)
                value += m_cssParser.symbols.at(i).lexem();
            attribute.value = QXmlStreamStringRef(QStringRef(&value, 0, value.length()));
        }

        attributes->append(attribute);

        m_cssParser.skipSpace();
    }
}

static void cssStyleLookup(QSvgNode *node,
                           QSvgHandler *handler,
                           QSvgStyleSelector *selector)
{
    QCss::StyleSelector::NodePtr cssNode;
    cssNode.ptr = node;
    QVector<QCss::Declaration> decls = selector->declarationsForNode(cssNode);

    QXmlStreamAttributes attributes;
    parseCSStoXMLAttrs(decls, attributes);
    parseStyle(node, attributes, handler);
}

static bool parseDefaultTextStyle(QSvgNode *node,
                                  const QXmlStreamAttributes &attributes,
                                  bool initial,
                                  QSvgHandler *handler)
{
    Q_ASSERT(node->type() == QSvgText::TEXT || node->type() == QSvgNode::TEXTAREA);
    QSvgText *textNode = static_cast<QSvgText*>(node);

    QSvgAttributes attrs(attributes, handler);

    QString fontFamily = attrs.value(QString(), QLatin1String("font-family")).toString();

    QString anchor = attrs.value(QString(), QLatin1String("text-anchor")).toString();

    QSvgFontStyle *fontStyle = static_cast<QSvgFontStyle*>(
        node->styleProperty(QSvgStyleProperty::FONT));
    if (fontStyle) {
        QSvgTinyDocument *doc = fontStyle->doc();
        if (doc && fontStyle->svgFont()) {
            cssStyleLookup(node, handler, handler->selector());
            parseStyle(node, attrs, handler);
            return true;
        }
    } else if (!fontFamily.isEmpty()) {
        QSvgTinyDocument *doc = node->document();
        QSvgFont *svgFont = doc->svgFont(fontFamily);
        if (svgFont) {
            cssStyleLookup(node, handler, handler->selector());
            parseStyle(node, attrs, handler);
            return true;
        }
    }

    QTextCharFormat format;
    QBrush brush(QColor(0, 0, 0));
    QFont font;
    font.setPixelSize(12);
    qreal fontSize = font.pixelSize();

    if (!initial) {
        font = textNode->topFormat().font();
        fontSize = font.pixelSize() / textNode->scale();
        brush = textNode->topFormat().foreground();
    } else {
        QSvgFontStyle *fontStyle = static_cast<QSvgFontStyle*>(
            node->styleProperty(QSvgStyleProperty::FONT));
        if (!fontStyle)
            fontStyle = static_cast<QSvgFontStyle*>(
                node->parent()->styleProperty(QSvgStyleProperty::FONT));
        if (fontStyle) {
            font = fontStyle->qfont();
            fontSize = fontStyle->pointSize();
            if (anchor.isEmpty())
                anchor = fontStyle->textAnchor();
        }

        Qt::Alignment align = Qt::AlignLeft;
        if (anchor == QLatin1String("middle"))
           align = Qt::AlignHCenter;
        else if (anchor == QLatin1String("end"))
           align = Qt::AlignRight;
        textNode->setTextAlignment(align);

        QSvgFillStyle *fillStyle = static_cast<QSvgFillStyle*>(
            node->styleProperty(QSvgStyleProperty::FILL));
        if (fillStyle)
            brush = fillStyle->qbrush();
    }

    if (parseQFont(attrs, font, fontSize, handler) || initial) {
        if (initial)
            textNode->setScale(100 / fontSize);
        font.setPixelSize(qMax(1, int(fontSize * textNode->scale())));
        format.setFont(font);
    }

    if (parseQBrush(attrs, node, brush, handler) || initial) {
        if (brush.style() != Qt::NoBrush || initial)
            format.setForeground(brush);
    }

    QPen pen(Qt::NoPen);
//     QSvgStrokeStyle *inherited =
//         static_cast<QSvgStrokeStyle*>(node->parent()->styleProperty(
//                                           QSvgStyleProperty::STROKE));
//     if (inherited)
//         pen = inherited->qpen();
    parseQPen(pen, node, attrs, handler);
    if (pen.style() != Qt::NoPen) {
        format.setTextOutline(pen);
    }

    parseTransform(node, attrs, handler);

    textNode->insertFormat(format);

    return true;
}

static inline QStringList stringToList(const QString &str)
{
    QStringList lst = str.split(QLatin1Char(','), QString::SkipEmptyParts);
    return lst;
}

static bool parseCoreNode(QSvgNode *node,
                          const QXmlStreamAttributes &attributes)
{
    QString featuresStr   = attributes.value(QLatin1String("requiredFeatures")).toString();
    QString extensionsStr = attributes.value(QLatin1String("requiredExtensions")).toString();
    QString languagesStr  = attributes.value(QLatin1String("systemLanguage")).toString();
    QString formatsStr    = attributes.value(QLatin1String("requiredFormats")).toString();
    QString fontsStr      = attributes.value(QLatin1String("requiredFonts")).toString();
    QString nodeIdStr     = someId(attributes);
    QString xmlClassStr   = attributes.value(QLatin1String("class")).toString();


    QStringList features = stringToList(featuresStr);
    QStringList extensions = stringToList(extensionsStr);
    QStringList languages = stringToList(languagesStr);
    QStringList formats = stringToList(formatsStr);
    QStringList fonts = stringToList(fontsStr);

    node->setRequiredFeatures(features);
    node->setRequiredExtensions(extensions);
    node->setRequiredLanguages(languages);
    node->setRequiredFormats(formats);
    node->setRequiredFonts(fonts);
    node->setNodeId(nodeIdStr);
    node->setXmlClass(xmlClassStr);

    return true;
}

static void parseOpacity(QSvgNode *node,
                         const QSvgAttributes &attributes,
                         QSvgHandler *)
{
    QString value = attributes.value(QLatin1String("opacity")).toString();
    value = value.trimmed();

    bool ok = false;
    qreal op = value.toDouble(&ok);

    if (ok) {
        QSvgOpacityStyle *opacity = new QSvgOpacityStyle(op);
        node->appendStyleProperty(opacity, someId(attributes));
    }
}

static QPainter::CompositionMode svgToQtCompositionMode(const QString &op)
{
#define NOOP qDebug()<<"Operation: "<<op<<" is not implemented"
    if (op == QLatin1String("clear")) {
        return QPainter::CompositionMode_Clear;
    } else if (op == QLatin1String("src")) {
        return QPainter::CompositionMode_Source;
    } else if (op == QLatin1String("dst")) {
        return QPainter::CompositionMode_Destination;
    } else if (op == QLatin1String("src-over")) {
        return QPainter::CompositionMode_SourceOver;
    } else if (op == QLatin1String("dst-over")) {
        return QPainter::CompositionMode_DestinationOver;
    } else if (op == QLatin1String("src-in")) {
        return QPainter::CompositionMode_SourceIn;
    } else if (op == QLatin1String("dst-in")) {
        return QPainter::CompositionMode_DestinationIn;
    } else if (op == QLatin1String("src-out")) {
        return QPainter::CompositionMode_SourceOut;
    } else if (op == QLatin1String("dst-out")) {
        return QPainter::CompositionMode_DestinationOut;
    } else if (op == QLatin1String("src-atop")) {
        return QPainter::CompositionMode_SourceAtop;
    } else if (op == QLatin1String("dst-atop")) {
        return QPainter::CompositionMode_DestinationAtop;
    } else if (op == QLatin1String("xor")) {
        return QPainter::CompositionMode_Xor;
    } else if (op == QLatin1String("plus")) {
        return QPainter::CompositionMode_Plus;
    } else if (op == QLatin1String("multiply")) {
        return QPainter::CompositionMode_Multiply;
    } else if (op == QLatin1String("screen")) {
        return QPainter::CompositionMode_Screen;
    } else if (op == QLatin1String("overlay")) {
        return QPainter::CompositionMode_Overlay;
    } else if (op == QLatin1String("darken")) {
        return QPainter::CompositionMode_Darken;
    } else if (op == QLatin1String("lighten")) {
        return QPainter::CompositionMode_Lighten;
    } else if (op == QLatin1String("color-dodge")) {
        return QPainter::CompositionMode_ColorDodge;
    } else if (op == QLatin1String("color-burn")) {
        return QPainter::CompositionMode_ColorBurn;
    } else if (op == QLatin1String("hard-light")) {
        return QPainter::CompositionMode_HardLight;
    } else if (op == QLatin1String("soft-light")) {
        return QPainter::CompositionMode_SoftLight;
    } else if (op == QLatin1String("difference")) {
        return QPainter::CompositionMode_Difference;
    } else if (op == QLatin1String("exclusion")) {
        return QPainter::CompositionMode_Exclusion;
    } else {
        NOOP;
    }

    return QPainter::CompositionMode_SourceOver;
}

static void parseCompOp(QSvgNode *node,
                        const QSvgAttributes &attributes,
                        QSvgHandler *)
{
    QString value = attributes.value(QLatin1String("comp-op")).toString();
    value = value.trimmed();

    if (!value.isEmpty()) {
        QSvgCompOpStyle *compop = new QSvgCompOpStyle(svgToQtCompositionMode(value));
        node->appendStyleProperty(compop, someId(attributes));
    }
}

static inline QSvgNode::DisplayMode displayStringToEnum(const QString &str)
{
    if (str == QLatin1String("inline")) {
        return QSvgNode::InlineMode;
    } else if (str == QLatin1String("block")) {
        return QSvgNode::BlockMode;
    } else if (str == QLatin1String("list-item")) {
        return QSvgNode::ListItemMode;
    } else if (str == QLatin1String("run-in")) {
        return QSvgNode::RunInMode;
    } else if (str == QLatin1String("compact")) {
        return QSvgNode::CompactMode;
    } else if (str == QLatin1String("marker")) {
        return QSvgNode::MarkerMode;
    } else if (str == QLatin1String("table")) {
        return QSvgNode::TableMode;
    } else if (str == QLatin1String("inline-table")) {
        return QSvgNode::InlineTableMode;
    } else if (str == QLatin1String("table-row")) {
        return QSvgNode::TableRowGroupMode;
    } else if (str == QLatin1String("table-header-group")) {
        return QSvgNode::TableHeaderGroupMode;
    } else if (str == QLatin1String("table-footer-group")) {
        return QSvgNode::TableFooterGroupMode;
    } else if (str == QLatin1String("table-row")) {
        return QSvgNode::TableRowMode;
    } else if (str == QLatin1String("table-column-group")) {
        return QSvgNode::TableColumnGroupMode;
    } else if (str == QLatin1String("table-column")) {
        return QSvgNode::TableColumnMode;
    } else if (str == QLatin1String("table-cell")) {
        return QSvgNode::TableCellMode;
    } else if (str == QLatin1String("table-caption")) {
        return QSvgNode::TableCaptionMode;
    } else if (str == QLatin1String("none")) {
        return QSvgNode::NoneMode;
    } else if (str == QLatin1String("inherit")) {
        return QSvgNode::InheritMode;
    }
    return QSvgNode::BlockMode;
}

static void parseOthers(QSvgNode *node,
                        const QSvgAttributes &attributes,
                        QSvgHandler *)
{
    QString displayStr = attributes.value(QLatin1String("display")).toString();
    displayStr = displayStr.trimmed();

    if (!displayStr.isEmpty()) {
        node->setDisplayMode(displayStringToEnum(displayStr));
    }
}

static bool parseStyle(QSvgNode *node,
                       const QSvgAttributes &attributes,
                       QSvgHandler *handler)
{
    parseColor(node, attributes, handler);
    parseBrush(node, attributes, handler);
    parsePen(node, attributes, handler);
    parseFont(node, attributes, handler);
    parseTransform(node, attributes, handler);
    parseVisibility(node, attributes, handler);
    parseOpacity(node, attributes, handler);
    parseCompOp(node, attributes, handler);
    parseOthers(node, attributes, handler);
#if 0
    value = attributes.value("audio-level");

    value = attributes.value("color-rendering");

    value = attributes.value("display-align");

    value = attributes.value("image-rendering");

    value = attributes.value("line-increment");

    value = attributes.value("pointer-events");

    value = attributes.value("shape-rendering");

    value = attributes.value("solid-color");

    value = attributes.value("solid-opacity");

    value = attributes.value("text-rendering");

    value = attributes.value("vector-effect");

    value = attributes.value("viewport-fill");

    value = attributes.value("viewport-fill-opacity");
#endif
    return true;
}

static bool parseStyle(QSvgNode *node,
                       const QXmlStreamAttributes &attrs,
                       QSvgHandler *handler)
{
    return parseStyle(node, QSvgAttributes(attrs, handler), handler);
}

static bool parseAnchorNode(QSvgNode *parent,
                            const QXmlStreamAttributes &attributes,
                            QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseAnimateNode(QSvgNode *parent,
                             const QXmlStreamAttributes &attributes,
                             QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseAnimateColorNode(QSvgNode *parent,
                                  const QXmlStreamAttributes &attributes,
                                  QSvgHandler *handler)
{
    QString typeStr    = attributes.value(QLatin1String("type")).toString();
    QString fromStr    = attributes.value(QLatin1String("from")).toString();
    QString toStr      = attributes.value(QLatin1String("to")).toString();
    QString valuesStr  = attributes.value(QLatin1String("values")).toString();
    QString beginStr   = attributes.value(QLatin1String("begin")).toString();
    QString durStr     = attributes.value(QLatin1String("dur")).toString();
    QString targetStr  = attributes.value(QLatin1String("attributeName")).toString();
    QString repeatStr  = attributes.value(QLatin1String("repeatCount")).toString();
    QString fillStr    = attributes.value(QLatin1String("fill")).toString();

    QList<QColor> colors;
    if (valuesStr.isEmpty()) {
        QColor startColor, endColor;
        constructColor(fromStr, QString(), startColor, handler);
        constructColor(toStr, QString(), endColor, handler);
        colors.append(startColor);
        colors.append(endColor);
    } else {
        QStringList str = valuesStr.split(QLatin1Char(';'));
        QStringList::const_iterator itr;
        for (itr = str.constBegin(); itr != str.constEnd(); ++itr) {
            QColor color;
            constructColor(*itr, QString(), color, handler);
            colors.append(color);
        }
    }

    int ms = 1000;
    beginStr = beginStr.trimmed();
    if (beginStr.endsWith(QLatin1String("ms"))) {
        beginStr.chop(2);
        ms = 1;
    } else if (beginStr.endsWith(QLatin1String("s"))) {
        beginStr.chop(1);
    }
    durStr = durStr.trimmed();
    if (durStr.endsWith(QLatin1String("ms"))) {
        durStr.chop(2);
        ms = 1;
    } else if (durStr.endsWith(QLatin1String("s"))) {
        durStr.chop(1);
    }
    int begin = static_cast<int>(toDouble(beginStr) * ms);
    int end   = static_cast<int>((toDouble(durStr) + begin) * ms);

    QSvgAnimateColor *anim = new QSvgAnimateColor(begin, end, 0);
    anim->setArgs((targetStr == QLatin1String("fill")), colors);
    anim->setFreeze(fillStr == QLatin1String("freeze"));
    anim->setRepeatCount(
        (repeatStr == QLatin1String("indefinite")) ? -1 :
            (repeatStr == QLatin1String("")) ? 1 : toDouble(repeatStr));

    parent->appendStyleProperty(anim, someId(attributes));
    parent->document()->setAnimated(true);
    handler->setAnimPeriod(begin, end);
    return true;
}

static bool parseAimateMotionNode(QSvgNode *parent,
                                  const QXmlStreamAttributes &attributes,
                                  QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseAnimateTransformNode(QSvgNode *parent,
                                      const QXmlStreamAttributes &attributes,
                                      QSvgHandler *handler)
{
    QString typeStr    = attributes.value(QLatin1String("type")).toString();
    QString values     = attributes.value(QLatin1String("values")).toString();
    QString beginStr   = attributes.value(QLatin1String("begin")).toString();
    QString durStr     = attributes.value(QLatin1String("dur")).toString();
    QString targetStr  = attributes.value(QLatin1String("attributeName")).toString();
    QString repeatStr  = attributes.value(QLatin1String("repeatCount")).toString();
    QString fillStr    = attributes.value(QLatin1String("fill")).toString();
    QString fromStr    = attributes.value(QLatin1String("from")).toString();
    QString toStr      = attributes.value(QLatin1String("to")).toString();

    QVector<qreal> vals;
    if (values.isEmpty()) {
        const QChar *s = fromStr.constData();
        QVector<qreal> lst = parseNumbersList(s);
        while (lst.count() < 3)
            lst.append(0.0);
        vals << lst;

        s = toStr.constData();
        lst = parseNumbersList(s);
        while (lst.count() < 3)
            lst.append(0.0);
        vals << lst;
    } else {
        const QChar *s = values.constData();
        while (s && *s != QLatin1Char(0)) {
            QVector<qreal> tmpVals = parseNumbersList(s);
            while (tmpVals.count() < 3)
                tmpVals.append(0.0);

            vals << tmpVals;
            if (*s == QLatin1Char(0))
                break;
            ++s;
        }
    }

    int ms = 1000;
    beginStr = beginStr.trimmed();
    if (beginStr.endsWith(QLatin1String("ms"))) {
        beginStr.chop(2);
        ms = 1;
    } else if (beginStr.endsWith(QLatin1String("s"))) {
        beginStr.chop(1);
    }
    int begin = static_cast<int>(toDouble(beginStr) * ms);
    durStr = durStr.trimmed();
    if (durStr.endsWith(QLatin1String("ms"))) {
        durStr.chop(2);
        ms = 1;
    } else if (durStr.endsWith(QLatin1String("s"))) {
        durStr.chop(1);
        ms = 1000;
    }
    int end = static_cast<int>(toDouble(durStr)*ms) + begin;

    QSvgAnimateTransform::TransformType type = QSvgAnimateTransform::Empty;
    if (typeStr == QLatin1String("translate")) {
        type = QSvgAnimateTransform::Translate;
    } else if (typeStr == QLatin1String("scale")) {
        type = QSvgAnimateTransform::Scale;
    } else if (typeStr == QLatin1String("rotate")) {
        type = QSvgAnimateTransform::Rotate;
    } else if (typeStr == QLatin1String("skewX")) {
        type = QSvgAnimateTransform::SkewX;
    } else if (typeStr == QLatin1String("skewY")) {
        type = QSvgAnimateTransform::SkewY;
    } else {
        return false;
    }

    QSvgAnimateTransform *anim = new QSvgAnimateTransform(begin, end, 0);
    anim->setArgs(type, vals);
    anim->setFreeze(fillStr == QLatin1String("freeze"));
    anim->setRepeatCount(
            (repeatStr == QLatin1String("indefinite"))? -1 :
            (repeatStr == QLatin1String(""))? 1 : toDouble(repeatStr));

    parent->appendStyleProperty(anim, someId(attributes));
    parent->document()->setAnimated(true);
    handler->setAnimPeriod(begin, end);
    return true;
}

static QSvgNode * createAnimationNode(QSvgNode *parent,
                                      const QXmlStreamAttributes &attributes,
                                      QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return 0;
}

static bool parseAudioNode(QSvgNode *parent,
                           const QXmlStreamAttributes &attributes,
                           QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static QSvgNode *createCircleNode(QSvgNode *parent,
                                  const QXmlStreamAttributes &attributes,
                                  QSvgHandler *)
{
    QString cx      = attributes.value(QLatin1String("cx")).toString();
    QString cy      = attributes.value(QLatin1String("cy")).toString();
    QString r       = attributes.value(QLatin1String("r")).toString();
    qreal ncx = toDouble(cx);
    qreal ncy = toDouble(cy);
    qreal nr  = toDouble(r);

    QRectF rect(ncx-nr, ncy-nr, nr*2, nr*2);
    QSvgNode *circle = new QSvgCircle(parent, rect);
    return circle;
}

static QSvgNode *createDefsNode(QSvgNode *parent,
                                const QXmlStreamAttributes &attributes,
                                QSvgHandler *)
{
    Q_UNUSED(attributes);
    QSvgDefs *defs = new QSvgDefs(parent);
    return defs;
}

static bool parseDescNode(QSvgNode *parent,
                          const QXmlStreamAttributes &attributes,
                          QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseDiscardNode(QSvgNode *parent,
                             const QXmlStreamAttributes &attributes,
                             QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static QSvgNode *createEllipseNode(QSvgNode *parent,
                                   const QXmlStreamAttributes &attributes,
                                   QSvgHandler *)
{
    QString cx      = attributes.value(QLatin1String("cx")).toString();
    QString cy      = attributes.value(QLatin1String("cy")).toString();
    QString rx      = attributes.value(QLatin1String("rx")).toString();
    QString ry      = attributes.value(QLatin1String("ry")).toString();
    qreal ncx = toDouble(cx);
    qreal ncy = toDouble(cy);
    qreal nrx = toDouble(rx);
    qreal nry = toDouble(ry);

    QRectF rect(ncx-nrx, ncy-nry, nrx*2, nry*2);
    QSvgNode *ellipse = new QSvgEllipse(parent, rect);
    return ellipse;
}

static QSvgStyleProperty *createFontNode(QSvgNode *parent,
                                         const QXmlStreamAttributes &attributes,
                                         QSvgHandler *)
{
    QString hax      = attributes.value(QLatin1String("horiz-adv-x")).toString();
    QString myId     = someId(attributes);

    qreal horizAdvX = toDouble(hax);

    while (parent && parent->type() != QSvgNode::DOC) {
        parent = parent->parent();
    }

    if (parent) {
        QSvgTinyDocument *doc = static_cast<QSvgTinyDocument*>(parent);
        QSvgFont *font = new QSvgFont(horizAdvX);
        font->setFamilyName(myId);
        if (!font->familyName().isEmpty()) {
            if (!doc->svgFont(font->familyName()))
                doc->addSvgFont(font);
        }
        return new QSvgFontStyle(font, doc);
    }
    return 0;
}

static bool parseFontFaceNode(QSvgStyleProperty *parent,
                              const QXmlStreamAttributes &attributes,
                              QSvgHandler *)
{
    if (parent->type() != QSvgStyleProperty::FONT) {
        return false;
    }

    QSvgFontStyle *style = static_cast<QSvgFontStyle*>(parent);
    QSvgFont *font = style->svgFont();
    QString name   = attributes.value(QLatin1String("font-family")).toString();
    QString unitsPerEmStr   = attributes.value(QLatin1String("units-per-em")).toString();

    qreal unitsPerEm = toDouble(unitsPerEmStr);
    if (!unitsPerEm)
        unitsPerEm = 1000;

    if (!name.isEmpty())
        font->setFamilyName(name);
    font->setUnitsPerEm(unitsPerEm);

    if (!font->familyName().isEmpty())
        if (!style->doc()->svgFont(font->familyName()))
            style->doc()->addSvgFont(font);

    return true;
}

static bool parseFontFaceNameNode(QSvgStyleProperty *parent,
                                  const QXmlStreamAttributes &attributes,
                                  QSvgHandler *)
{
    if (parent->type() != QSvgStyleProperty::FONT) {
        return false;
    }

    QSvgFontStyle *style = static_cast<QSvgFontStyle*>(parent);
    QSvgFont *font = style->svgFont();
    QString name   = attributes.value(QLatin1String("name")).toString();

    if (!name.isEmpty())
        font->setFamilyName(name);

    if (!font->familyName().isEmpty())
        if (!style->doc()->svgFont(font->familyName()))
            style->doc()->addSvgFont(font);

    return true;
}

static bool parseFontFaceSrcNode(QSvgStyleProperty *parent,
                                 const QXmlStreamAttributes &attributes,
                                 QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseFontFaceUriNode(QSvgStyleProperty *parent,
                                 const QXmlStreamAttributes &attributes,
                                 QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseForeignObjectNode(QSvgNode *parent,
                                   const QXmlStreamAttributes &attributes,
                                   QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static QSvgNode *createGNode(QSvgNode *parent,
                             const QXmlStreamAttributes &attributes,
                             QSvgHandler *)
{
    Q_UNUSED(attributes);
    QSvgG *node = new QSvgG(parent);
    return node;
}

static bool parseGlyphNode(QSvgStyleProperty *parent,
                           const QXmlStreamAttributes &attributes,
                           QSvgHandler *)
{
    if (parent->type() != QSvgStyleProperty::FONT) {
        return false;
    }

    QSvgFontStyle *style = static_cast<QSvgFontStyle*>(parent);
    QSvgFont *font = style->svgFont();
    createSvgGlyph(font, attributes);
    return true;
}

static bool parseHandlerNode(QSvgNode *parent,
                             const QXmlStreamAttributes &attributes,
                             QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseHkernNode(QSvgNode *parent,
                           const QXmlStreamAttributes &attributes,
                           QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static QSvgNode *createImageNode(QSvgNode *parent,
                                 const QXmlStreamAttributes &attributes,
                                 QSvgHandler *handler)
{
    QString x = attributes.value(QLatin1String("x")).toString();
    QString y = attributes.value(QLatin1String("y")).toString();
    QString width  = attributes.value(QLatin1String("width")).toString();
    QString height = attributes.value(QLatin1String("height")).toString();
    QString filename = attributes.value(QLatin1String("xlink:href")).toString();
    qreal nx = toDouble(x);
    qreal ny = toDouble(y);
    QSvgHandler::LengthType type;
    qreal nwidth = parseLength(width, type, handler);
    nwidth = convertToPixels(nwidth, true, type);

    qreal nheight = parseLength(height, type, handler);
    nheight = convertToPixels(nheight, false, type);


    filename = filename.trimmed();
    QImage image;
    if (filename.startsWith(QLatin1String("data"))) {
        int idx = filename.lastIndexOf(QLatin1String("base64,"));
        if (idx != -1) {
            idx += 7;
            QString dataStr = filename.mid(idx);
            QByteArray data = QByteArray::fromBase64(dataStr.toAscii());
            image = QImage::fromData(data);
        } else {
            qDebug()<<"QSvgHandler::createImageNode: Unrecognized inline image format!";
        }
    } else
        image = QImage(filename);

    if (image.isNull()) {
        qDebug()<<"couldn't create image from "<<filename;
        return 0;
    }

    QSvgNode *img = new QSvgImage(parent,
                                  image,
                                  QRect(int(nx),
                                        int(ny),
                                        int(nwidth),
                                        int(nheight)));
    return img;
}

static QSvgNode *createLineNode(QSvgNode *parent,
                                const QXmlStreamAttributes &attributes,
                                QSvgHandler *)
{
    QString x1 = attributes.value(QLatin1String("x1")).toString();
    QString y1 = attributes.value(QLatin1String("y1")).toString();
    QString x2 = attributes.value(QLatin1String("x2")).toString();
    QString y2 = attributes.value(QLatin1String("y2")).toString();
    qreal nx1 = toDouble(x1);
    qreal ny1 = toDouble(y1);
    qreal nx2 = toDouble(x2);
    qreal ny2 = toDouble(y2);

    QLineF lineBounds(nx1, ny1, nx2, ny2);
    QSvgNode *line = new QSvgLine(parent, lineBounds);
    return line;
}


static void parseBaseGradient(QSvgNode *node,
                              const QXmlStreamAttributes &attributes,
                              QSvgGradientStyle *gradProp,
                              QSvgHandler *handler)
{
    QString link   = attributes.value(QLatin1String("xlink:href")).toString();
    QString trans  = attributes.value(QLatin1String("gradientTransform")).toString();
    QString spread = attributes.value(QLatin1String("spreadMethod")).toString();
    QString units = attributes.value(QLatin1String("gradientUnits")).toString();

    QMatrix matrix;
    QGradient *grad = gradProp->qgradient();
    if (!link.isEmpty()) {
        QSvgStyleProperty *prop = node->styleProperty(link);
        //qDebug()<<"inherited "<<prop<<" ("<<link<<")";
        if (prop && prop->type() == QSvgStyleProperty::GRADIENT) {
            QSvgGradientStyle *inherited =
                static_cast<QSvgGradientStyle*>(prop);
            if (!inherited->stopLink().isEmpty())
                gradProp->setStopLink(inherited->stopLink(), handler->document());
            else
                grad->setStops(inherited->qgradient()->stops());

            matrix = inherited->qmatrix();
        } else {
            gradProp->setStopLink(link, handler->document());
        }
    }

    if (!trans.isEmpty()) {
        matrix = parseTransformationMatrix(trans);
        gradProp->setMatrix(matrix);
    } else if (!matrix.isIdentity()) {
        gradProp->setMatrix(matrix);
    }

    if (!spread.isEmpty()) {
        if (spread == QLatin1String("pad")) {
            grad->setSpread(QGradient::PadSpread);
        } else if (spread == QLatin1String("reflect")) {
            grad->setSpread(QGradient::ReflectSpread);
        } else if (spread == QLatin1String("repeat")) {
            grad->setSpread(QGradient::RepeatSpread);
        }
    }

    if (units.isEmpty() || units == QLatin1String("objectBoundingBox")) {
         grad->setCoordinateMode(QGradient::ObjectBoundingMode);
    }
}

static QSvgStyleProperty *createLinearGradientNode(QSvgNode *node,
                                                   const QXmlStreamAttributes &attributes,
                                                   QSvgHandler *handler)
{
    QString x1 = attributes.value(QLatin1String("x1")).toString();
    QString y1 = attributes.value(QLatin1String("y1")).toString();
    QString x2 = attributes.value(QLatin1String("x2")).toString();
    QString y2 = attributes.value(QLatin1String("y2")).toString();

    qreal nx1 = 0.0;
    qreal ny1 = 0.0;
    qreal nx2 = 1.0;
    qreal ny2 = 0.0;

    if (!x1.isEmpty())
        nx1 =  convertToNumber(x1, handler);
    if (!y1.isEmpty())
        ny1 =  convertToNumber(y1, handler);
    if (!x2.isEmpty())
        nx2 =  convertToNumber(x2, handler);
    if (!y2.isEmpty())
        ny2 =  convertToNumber(y2, handler);

    QSvgNode *itr = node;
    while (itr && itr->type() != QSvgNode::DOC) {
        itr = itr->parent();
    }

    QLinearGradient *grad = new QLinearGradient(nx1, ny1, nx2, ny2);
    grad->setColorAt(qQNaN(), QColor());
    grad->setInterpolationMode(QGradient::ComponentInterpolation);
    QSvgGradientStyle *prop = new QSvgGradientStyle(grad);
    parseBaseGradient(node, attributes, prop, handler);

    return prop;
}

static bool parseMetadataNode(QSvgNode *parent,
                              const QXmlStreamAttributes &attributes,
                              QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseMissingGlyphNode(QSvgStyleProperty *parent,
                                  const QXmlStreamAttributes &attributes,
                                  QSvgHandler *)
{
    if (parent->type() != QSvgStyleProperty::FONT) {
        return false;
    }

    QSvgFontStyle *style = static_cast<QSvgFontStyle*>(parent);
    QSvgFont *font = style->svgFont();
    createSvgGlyph(font, attributes);
    return true;
}

static bool parseMpathNode(QSvgNode *parent,
                           const QXmlStreamAttributes &attributes,
                           QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static QSvgNode *createPathNode(QSvgNode *parent,
                                const QXmlStreamAttributes &attributes,
                                QSvgHandler *)
{
    QStringRef data      = attributes.value(QLatin1String("d"));

    QPainterPath qpath;
    qpath.setFillRule(Qt::WindingFill);
    //XXX do error handling
    parsePathDataFast(data, qpath);

    QSvgNode *path = new QSvgPath(parent, qpath);
    return path;
}

static QSvgNode *createPolygonNode(QSvgNode *parent,
                                   const QXmlStreamAttributes &attributes,
                                   QSvgHandler *)
{
    QString pointsStr  = attributes.value(QLatin1String("points")).toString();

    //same QPolygon parsing is in createPolylineNode
    const QChar *s = pointsStr.constData();
    QVector<qreal> points = parseNumbersList(s);
    QPolygonF poly(points.count()/2);
    int i = 0;
    QVector<qreal>::const_iterator itr = points.constBegin();
    while (itr != points.constEnd()) {
        qreal one = *itr; ++itr;
        qreal two = *itr; ++itr;
        poly[i] = QPointF(one, two);
        ++i;
    }
    QSvgNode *polygon = new QSvgPolygon(parent, poly);
    return polygon;
}

static QSvgNode *createPolylineNode(QSvgNode *parent,
                                    const QXmlStreamAttributes &attributes,
                                    QSvgHandler *)
{
    QString pointsStr  = attributes.value(QLatin1String("points")).toString();

    //same QPolygon parsing is in createPolygonNode
    const QChar *s = pointsStr.constData();
    QVector<qreal> points = parseNumbersList(s);
    QPolygonF poly(points.count()/2);
    int i = 0;
    QVector<qreal>::const_iterator itr = points.constBegin();
    while (itr != points.constEnd()) {
        qreal one = *itr; ++itr;
        qreal two = *itr; ++itr;
        poly[i] = QPointF(one, two);
        ++i;
    }

    QSvgNode *line = new QSvgPolyline(parent, poly);
    return line;
}

static bool parsePrefetchNode(QSvgNode *parent,
                              const QXmlStreamAttributes &attributes,
                              QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static QSvgStyleProperty *createRadialGradientNode(QSvgNode *node,
                                                   const QXmlStreamAttributes &attributes,
                                                   QSvgHandler *handler)
{
    QString cx = attributes.value(QLatin1String("cx")).toString();
    QString cy = attributes.value(QLatin1String("cy")).toString();
    QString r  = attributes.value(QLatin1String("r")).toString();
    QString fx = attributes.value(QLatin1String("fx")).toString();
    QString fy = attributes.value(QLatin1String("fy")).toString();

    qreal ncx = 0.5;
    qreal ncy = 0.5;
    qreal nr  = 0.5;
    if (!cx.isEmpty())
        ncx = toDouble(cx);
    if (!cy.isEmpty())
        ncy = toDouble(cy);
    if (!r.isEmpty())
        nr = toDouble(r);

    qreal nfx = ncx;
    if (!fx.isEmpty())
        nfx = toDouble(fx);
    qreal nfy = ncy;
    if (!fy.isEmpty())
        nfy = toDouble(fy);

    QRadialGradient *grad = new QRadialGradient(ncx, ncy, nr, nfx, nfy);
    grad->setColorAt(qQNaN(), QColor());
    grad->setInterpolationMode(QGradient::ComponentInterpolation);

    QSvgGradientStyle *prop = new QSvgGradientStyle(grad);
    parseBaseGradient(node, attributes, prop, handler);

    return prop;
}

static QSvgNode *createRectNode(QSvgNode *parent,
                                const QXmlStreamAttributes &attributes,
                                QSvgHandler *handler)
{
    QString x      = attributes.value(QLatin1String("x")).toString();
    QString y      = attributes.value(QLatin1String("y")).toString();
    QString width  = attributes.value(QLatin1String("width")).toString();
    QString height = attributes.value(QLatin1String("height")).toString();
    QString rx      = attributes.value(QLatin1String("rx")).toString();
    QString ry      = attributes.value(QLatin1String("ry")).toString();

    QSvgHandler::LengthType type;
    qreal nwidth = parseLength(width, type, handler);
    nwidth = convertToPixels(nwidth, true, type);

    qreal nheight = parseLength(height, type, handler);
    nheight = convertToPixels(nheight, true, type);
    qreal nrx = toDouble(rx);
    qreal nry = toDouble(ry);

    QRectF bounds(toDouble(x), toDouble(y),
                  nwidth, nheight);

    //9.2 The 'rect'  element clearly specifies it
    // but the case might in fact be handled because
    // we draw rounded rectangles differently
    if (nrx > bounds.width()/2)
        nrx = bounds.width()/2;
    if (nry > bounds.height()/2)
        nry = bounds.height()/2;

    if (nrx && !nry)
        nry = nrx;
    else if (nry && !nrx)
        nrx = nry;

    //we draw rounded rect from 0...99
    //svg from 0...bounds.width()/2 so we're adjusting the
    //coordinates
    nrx *= (100/(bounds.width()/2));
    nry *= (100/(bounds.height()/2));

    QSvgNode *rect = new QSvgRect(parent, bounds,
                                  int(nrx),
                                  int(nry));
    return rect;
}

static bool parseScriptNode(QSvgNode *parent,
                            const QXmlStreamAttributes &attributes,
                            QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseSetNode(QSvgNode *parent,
                         const QXmlStreamAttributes &attributes,
                         QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static QSvgStyleProperty *createSolidColorNode(QSvgNode *parent,
                                               const QXmlStreamAttributes &attributes,
                                               QSvgHandler *handler)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    QString solidColorStr = attributes.value(QLatin1String("solid-color")).toString();
    QString solidOpacityStr = attributes.value(QLatin1String("solid-opacity")).toString();

    if (solidOpacityStr.isEmpty())
        solidOpacityStr = attributes.value(QLatin1String("opacity")).toString();

    QColor color;
    if (!constructColor(solidColorStr, solidOpacityStr, color, handler))
        return 0;
    QSvgSolidColorStyle *style = new QSvgSolidColorStyle(color);
    return style;
}

static bool parseStopNode(QSvgStyleProperty *parent,
                          const QXmlStreamAttributes &attributes,
                          QSvgHandler *handler)
{
    if (parent->type() != QSvgStyleProperty::GRADIENT)
        return false;
    QString nodeIdStr     = someId(attributes);
    QString xmlClassStr   = attributes.value(QLatin1String("class")).toString();

    //### nasty hack because stop gradients are not in the rendering tree
    //    we force a dummy node with the same id and class into a rendering
    //    tree to figure out whether the selector has a style for it
    //    QSvgStyleSelector should be coded in a way that could avoid it
    QSvgAnimation anim;
    anim.setNodeId(nodeIdStr);
    anim.setXmlClass(xmlClassStr);

    QCss::StyleSelector::NodePtr cssNode;
    cssNode.ptr = &anim;
    QVector<QCss::Declaration> decls = handler->selector()->declarationsForNode(cssNode);

    QSvgAttributes attrs(attributes, handler);

    for (int i = 0; i < decls.count(); ++i) {
        const QCss::Declaration &decl = decls.at(i);

        if (decl.d->property.isEmpty())
            continue;
        if (decl.d->values.count() != 1)
            continue;
        QCss::Value val = decl.d->values.first();
        QString valueStr = val.toString();
        if (val.type == QCss::Value::Uri) {
            valueStr.prepend(QLatin1String("url("));
            valueStr.append(QLatin1Char(')'));
        }
        attrs.m_xmlAttributes.append(QString(), decl.d->property, valueStr);
    }

    QSvgGradientStyle *style =
        static_cast<QSvgGradientStyle*>(parent);
    QString offsetStr   = attrs.value(QString(), QLatin1String("offset")).toString();
    QString colorStr    = attrs.value(QString(), QLatin1String("stop-color")).toString();
    QString opacityStr  = attrs.value(QString(), QLatin1String("stop-opacity")).toString();
    QColor color;
    qreal offset = convertToNumber(offsetStr, handler);
    if (colorStr.isEmpty()) {
        colorStr = QLatin1String("#000000");
    }

    bool colorOK = constructColor(colorStr, opacityStr, color, handler);

    QGradient *grad = style->qgradient();

    offset = qMin(qreal(1), qMax(qreal(0), offset)); // Clamp to range [0, 1]
    QGradientStops stops = grad->stops();
    // Check if the gradient is marked as empty (marked with one single stop at NaN).
    if ((stops.size() == 1) && qIsNaN(stops.at(0).first)) {
        stops.clear();
        grad->setStops(stops);
    } else {
        // If the stop offset equals the one previously added, add an epsilon to make it greater.
        if (offset <= stops.back().first)
            offset = stops.back().first + FLT_EPSILON;
    }

    // If offset is greater than one, it must be clamped to one.
    if (offset > 1.0) {
        if ((stops.size() == 1) || (stops.at(stops.size() - 2).first < 1.0 - FLT_EPSILON)) {
            stops.back().first = 1.0 - FLT_EPSILON;
            grad->setStops(stops);
        }
        offset = 1.0;
    }

    grad->setColorAt(offset, color);
    if (!colorOK)
        style->addResolve(offset);
    return true;
}

static bool parseStyleNode(QSvgNode *parent,
                           const QXmlStreamAttributes &attributes,
                           QSvgHandler *handler)
{
    Q_UNUSED(parent);
    QString type = attributes.value(QLatin1String("type")).toString();
    type = type.toLower();

    if (type == QLatin1String("text/css")) {
        handler->setInStyle(true);
    }

    return true;
}

static QSvgNode *createSvgNode(QSvgNode *parent,
                               const QXmlStreamAttributes &attributes,
                               QSvgHandler *handler)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);

    QString baseProfile = attributes.value(QLatin1String("baseProfile")).toString();
#if 0
    if (baseProfile.isEmpty() && baseProfile != QLatin1String("tiny")) {
        qWarning("Profile is %s while we only support tiny!",
                 qPrintable(baseProfile));
    }
#endif

    QSvgTinyDocument *node = new QSvgTinyDocument();
    QString widthStr  = attributes.value(QLatin1String("width")).toString();
    QString heightStr = attributes.value(QLatin1String("height")).toString();
    QString viewBoxStr = attributes.value(QLatin1String("viewBox")).toString();

    QSvgHandler::LengthType type = QSvgHandler::LT_PX; // FIXME: is the default correct?
    qreal width = 0;
    if (!widthStr.isEmpty()) {
        width = parseLength(widthStr, type, handler);
        if (type != QSvgHandler::LT_PT)
            width = convertToPixels(width, true, type);
        node->setWidth(int(width), type == QSvgHandler::LT_PERCENT);
    }
    qreal height = 0;
    if (!heightStr.isEmpty()) {
        height = parseLength(heightStr, type, handler);
        if (type != QSvgHandler::LT_PT)
            height = convertToPixels(height, false, type);
        node->setHeight(int(height), type == QSvgHandler::LT_PERCENT);
    }


    if (!viewBoxStr.isEmpty()) {
        QStringList lst = viewBoxStr.split(QLatin1Char(' '), QString::SkipEmptyParts);
        if (lst.count() != 4)
            lst = viewBoxStr.split(QLatin1Char(','), QString::SkipEmptyParts);
        QString xStr      = lst.at(0).trimmed();
        QString yStr      = lst.at(1).trimmed();
        QString widthStr  = lst.at(2).trimmed();
        QString heightStr = lst.at(3).trimmed();


        QSvgHandler::LengthType lt;
        qreal x = parseLength(xStr, lt, handler);
        qreal y = parseLength(yStr, lt, handler);
        qreal w = parseLength(widthStr, lt, handler);
        qreal h = parseLength(heightStr, lt, handler);

        node->setViewBox(QRectF(x, y, w, h));
    } else if (width && height){
        if (type == QSvgHandler::LT_PT) {
            width = convertToPixels(width, false, type);
            height = convertToPixels(height, false, type);
        }

        node->setViewBox(QRectF(0, 0, width, height));
    }

    handler->setDefaultCoordinateSystem(QSvgHandler::LT_PX);

    return node;
}

static QSvgNode *createSwitchNode(QSvgNode *parent,
                                  const QXmlStreamAttributes &attributes,
                                  QSvgHandler *)
{
    Q_UNUSED(attributes);
    QSvgSwitch *node = new QSvgSwitch(parent);
    return node;
}

static bool parseTbreakNode(QSvgNode *parent,
                            const QXmlStreamAttributes &,
                            QSvgHandler *)
{
    if (parent->type() != QSvgNode::TEXTAREA)
        return false;
    static_cast<QSvgText*>(parent)->insertLineBreak();
    return true;
}

static QSvgNode *createTextNode(QSvgNode *parent,
                                const QXmlStreamAttributes &attributes,
                                QSvgHandler *handler)
{
    QString x = attributes.value(QLatin1String("x")).toString();
    QString y = attributes.value(QLatin1String("y")).toString();
    //### editable and rotate not handled
    QSvgHandler::LengthType type;
    qreal nx = parseLength(x, type, handler);
    qreal ny = parseLength(y, type, handler);

    //### not to pixels but to the default coordinate system
    //    and text should be already in the correct coordinate
    //    system here
    //nx = convertToPixels(nx, true, type);
    //ny = convertToPixels(ny, true, type);

    QSvgNode *text = new QSvgText(parent, QPointF(nx, ny));
    return text;
}

static QSvgNode *createTextAreaNode(QSvgNode *parent,
                                    const QXmlStreamAttributes &attributes,
                                    QSvgHandler *handler)
{
    QSvgText *node = static_cast<QSvgText *>(createTextNode(parent, attributes, handler));
    if (node) {
        QSvgHandler::LengthType type;
        qreal width = parseLength(attributes.value(QLatin1String("width")).toString(), type, handler);
        qreal height = parseLength(attributes.value(QLatin1String("height")).toString(), type, handler);
        node->setTextArea(QSizeF(width, height));
    }
    return node;
}

static bool parseTitleNode(QSvgNode *parent,
                           const QXmlStreamAttributes &attributes,
                           QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return true;
}

static bool parseTspanNode(QSvgNode *parent,
                           const QXmlStreamAttributes &attributes,
                           QSvgHandler *handler)
{

    cssStyleLookup(parent, handler, handler->selector());
    return parseDefaultTextStyle(parent, attributes, false, handler);
}

static QSvgNode *createUseNode(QSvgNode *parent,
                               const QXmlStreamAttributes &attributes,
                               QSvgHandler *handler)
{
    QString linkId = attributes.value(QLatin1String("xlink:href")).toString().remove(0, 1);
    QString xStr = attributes.value(QLatin1String("x")).toString();
    QString yStr = attributes.value(QLatin1String("y")).toString();
    QSvgStructureNode *group = 0;

    if (linkId.isEmpty())
        linkId = attributes.value(QLatin1String("href")).toString().remove(0, 1);
    switch (parent->type()) {
    case QSvgNode::DOC:
    case QSvgNode::DEFS:
    case QSvgNode::G:
    case QSvgNode::SWITCH:
        group = static_cast<QSvgStructureNode*>(parent);
        break;
    default:
        break;
    }

    if (group) {
        QSvgNode *link = group->scopeNode(linkId);
        if (link) {
            QPointF pt;
            if (!xStr.isNull() || !yStr.isNull()) {
                QSvgHandler::LengthType type;
                qreal nx = parseLength(xStr, type, handler);
                nx = convertToPixels(nx, true, type);

                qreal ny = parseLength(yStr, type, handler);
                ny = convertToPixels(ny, true, type);
                pt = QPointF(nx, ny);
            }

            //delay link resolving till the first draw call on
            //use nodes, link 2might have not been created yet
            QSvgUse *node = new QSvgUse(pt, parent, link);
            return node;
        }
    }

    qWarning("link %s hasn't been detected!", qPrintable(linkId));
    return 0;
}

static QSvgNode *createVideoNode(QSvgNode *parent,
                                 const QXmlStreamAttributes &attributes,
                                 QSvgHandler *)
{
    Q_UNUSED(parent); Q_UNUSED(attributes);
    return 0;
}

typedef QSvgNode *(*FactoryMethod)(QSvgNode *, const QXmlStreamAttributes &, QSvgHandler *);

static FactoryMethod findGroupFactory(const QString &name)
{
    if (name.isEmpty())
        return 0;

    QStringRef ref(&name, 1, name.length() - 1);
    switch (name.at(0).unicode()) {
    case 'd':
        if (ref == QLatin1String("efs")) return createDefsNode;
        break;
    case 'g':
        if (ref.isEmpty()) return createGNode;
        break;
    case 's':
        if (ref == QLatin1String("vg")) return createSvgNode;
        if (ref == QLatin1String("witch")) return createSwitchNode;
        break;
    default:
        break;
    }
    return 0;
}

static FactoryMethod findGraphicsFactory(const QString &name)
{
    if (name.isEmpty())
        return 0;

    QStringRef ref(&name, 1, name.length() - 1);
    switch (name.at(0).unicode()) {
    case 'a':
        if (ref == QLatin1String("nimation")) return createAnimationNode;
        break;
    case 'c':
        if (ref == QLatin1String("ircle")) return createCircleNode;
        break;
    case 'e':
        if (ref == QLatin1String("llipse")) return createEllipseNode;
        break;
    case 'i':
        if (ref == QLatin1String("mage")) return createImageNode;
        break;
    case 'l':
        if (ref == QLatin1String("ine")) return createLineNode;
        break;
    case 'p':
        if (ref == QLatin1String("ath")) return createPathNode;
        if (ref == QLatin1String("olygon")) return createPolygonNode;
        if (ref == QLatin1String("olyline")) return createPolylineNode;
        break;
    case 'r':
        if (ref == QLatin1String("ect")) return createRectNode;
        break;
    case 't':
        if (ref == QLatin1String("ext")) return createTextNode;
        if (ref == QLatin1String("extArea")) return createTextAreaNode;
        break;
    case 'u':
        if (ref == QLatin1String("se")) return createUseNode;
        break;
    case 'v':
        if (ref == QLatin1String("ideo")) return createVideoNode;
        break;
    default:
        break;
    }
    return 0;
}

typedef bool (*ParseMethod)(QSvgNode *, const QXmlStreamAttributes &, QSvgHandler *);

static ParseMethod findUtilFactory(const QString &name)
{
    if (name.isEmpty())
        return 0;

    QStringRef ref(&name, 1, name.length() - 1);
    switch (name.at(0).unicode()) {
    case 'a':
        if (ref.isEmpty()) return parseAnchorNode;
        if (ref == QLatin1String("nimate")) return parseAnimateNode;
        if (ref == QLatin1String("nimateColor")) return parseAnimateColorNode;
        if (ref == QLatin1String("nimateMotion")) return parseAimateMotionNode;
        if (ref == QLatin1String("nimateTransform")) return parseAnimateTransformNode;
        if (ref == QLatin1String("udio")) return parseAudioNode;
        break;
    case 'd':
        if (ref == QLatin1String("esc")) return parseDescNode;
        if (ref == QLatin1String("iscard")) return parseDiscardNode;
        break;
    case 'f':
        if (ref == QLatin1String("oreignObject")) return parseForeignObjectNode;
        break;
    case 'h':
        if (ref == QLatin1String("andler")) return parseHandlerNode;
        if (ref == QLatin1String("kern")) return parseHkernNode;
        break;
    case 'm':
        if (ref == QLatin1String("etadata")) return parseMetadataNode;
        if (ref == QLatin1String("path")) return parseMpathNode;
        break;
    case 'p':
        if (ref == QLatin1String("refetch")) return parsePrefetchNode;
        break;
    case 's':
        if (ref == QLatin1String("cript")) return parseScriptNode;
        if (ref == QLatin1String("et")) return parseSetNode;
        if (ref == QLatin1String("tyle")) return parseStyleNode;
        break;
    case 't':
        if (ref == QLatin1String("break")) return parseTbreakNode;
        if (ref == QLatin1String("itle")) return parseTitleNode;
        if (ref == QLatin1String("span")) return parseTspanNode;
        break;
    default:
        break;
    }
    return 0;
}

typedef QSvgStyleProperty *(*StyleFactoryMethod)(QSvgNode *,
                                                 const QXmlStreamAttributes &,
                                                 QSvgHandler *);

static StyleFactoryMethod findStyleFactoryMethod(const QString &name)
{
    if (name.isEmpty())
        return 0;

    QStringRef ref(&name, 1, name.length() - 1);
    switch (name.at(0).unicode()) {
    case 'f':
        if (ref == QLatin1String("ont")) return createFontNode;
        break;
    case 'l':
        if (ref == QLatin1String("inearGradient")) return createLinearGradientNode;
        break;
    case 'r':
        if (ref == QLatin1String("adialGradient")) return createRadialGradientNode;
        break;
    case 's':
        if (ref == QLatin1String("olidColor")) return createSolidColorNode;
        break;
    default:
        break;
    }
    return 0;
}

typedef bool (*StyleParseMethod)(QSvgStyleProperty *,
                                 const QXmlStreamAttributes &,
                                 QSvgHandler *);

static StyleParseMethod findStyleUtilFactoryMethod(const QString &name)
{
    if (name.isEmpty())
        return 0;

    QStringRef ref(&name, 1, name.length() - 1);
    switch (name.at(0).unicode()) {
    case 'f':
        if (ref == QLatin1String("ont-face")) return parseFontFaceNode;
        if (ref == QLatin1String("ont-face-name")) return parseFontFaceNameNode;
        if (ref == QLatin1String("ont-face-src")) return parseFontFaceSrcNode;
        if (ref == QLatin1String("ont-face-uri")) return parseFontFaceUriNode;
        break;
    case 'g':
        if (ref == QLatin1String("lyph")) return parseGlyphNode;
        break;
    case 'm':
        if (ref == QLatin1String("issing-glyph")) return parseMissingGlyphNode;
        break;
    case 's':
        if (ref == QLatin1String("top")) return parseStopNode;
        break;
    default:
        break;
    }
    return 0;
}

QSvgHandler::QSvgHandler(QIODevice *device) : xml(new QXmlStreamReader(device))
                                             , m_ownsReader(true)
{
    init();
}

QSvgHandler::QSvgHandler(const QByteArray &data) : xml(new QXmlStreamReader(data))
                                                 , m_ownsReader(true)
{
    init();
}

QSvgHandler::QSvgHandler(QXmlStreamReader *const reader) : xml(reader)
                                                         , m_ownsReader(false)
{
    init();
}

void QSvgHandler::init()
{
    m_doc = 0;
    m_style = 0;
    m_defaultCoords = LT_PX;
    m_defaultPen = QPen(Qt::black, 1, Qt::NoPen, Qt::FlatCap, Qt::SvgMiterJoin);
    m_defaultPen.setMiterLimit(4);
    parse();
}

void QSvgHandler::parse()
{
    xml->setNamespaceProcessing(false);
    m_selector = new QSvgStyleSelector;
    m_inStyle = false;
    bool done = false;
    while (!xml->atEnd() && !done) {
        switch (xml->readNext()) {
        case QXmlStreamReader::StartElement:
            // he we could/should verify the namespaces, and simply
            // call m_skipNodes(Unknown) if we don't know the
            // namespace.  We do support http://www.w3.org/2000/svg
            // but also http://www.w3.org/2000/svg-20000303-stylable
            // And if the document uses an external dtd, the reported
            // namespaceUri is empty. The only possible strategy at
            // this point is to do what everyone else seems to do and
            // ignore the reported namespaceUri completely.
            startElement(xml->name().toString(), xml->attributes());
            break;
        case QXmlStreamReader::EndElement:
            endElement(xml->name());
            // if we are using somebody else's qxmlstreamreader
            // we should not read until the end of the stream
            done = !m_ownsReader && (xml->name() == QLatin1String("svg"));
            break;
        case QXmlStreamReader::Characters:
            characters(xml->text());
            break;
        case QXmlStreamReader::ProcessingInstruction:
            processingInstruction(xml->processingInstructionTarget().toString(), xml->processingInstructionData().toString());
            break;
        default:
            ;
        }
    }
}

bool QSvgHandler::startElement(const QString &localName,
                               const QXmlStreamAttributes &attributes)
{
    QSvgNode *node = 0;

    if (m_colorTagCount.count()) {
        int top = m_colorTagCount.pop();
        ++top;
        m_colorTagCount.push(top);
    }

    /* The xml:space attribute may appear on any element. We do
     * a lookup by the qualified name here, but this is namespace aware, since
     * the XML namespace can only be bound to prefix "xml." */
    const QStringRef xmlSpace(attributes.value(QLatin1String("xml:space")));
    if(xmlSpace.isNull())
    {
        // This element has no xml:space attribute.
        m_whitespaceMode.push(QSvgText::Default);
    }
    else if(xmlSpace == QLatin1String("preserve"))
        m_whitespaceMode.push(QSvgText::Preserve);
    else if(xmlSpace == QLatin1String("default"))
        m_whitespaceMode.push(QSvgText::Default);
    else
    {
        qWarning() << QString::fromLatin1("\"%1\" is an invalid value for attribute xml:space. "
                                          "Valid values are \"preserve\" and \"default\".").arg(xmlSpace.toString());

        m_whitespaceMode.push(QSvgText::Default);
    }

    if (FactoryMethod method = findGroupFactory(localName)) {
        //group
        node = method(m_doc ? m_nodes.top() : 0, attributes, this);
        Q_ASSERT(node);
        if (!m_doc) {
            Q_ASSERT(node->type() == QSvgNode::DOC);
            m_doc = static_cast<QSvgTinyDocument*>(node);
        } else {
            switch (m_nodes.top()->type()) {
            case QSvgNode::DOC:
            case QSvgNode::G:
            case QSvgNode::DEFS:
            case QSvgNode::SWITCH:
            {
                QSvgStructureNode *group =
                    static_cast<QSvgStructureNode*>(m_nodes.top());
                group->addChild(node, someId(attributes));
            }
                break;
            default:
                break;
            }
        }
        parseCoreNode(node, attributes);
        cssStyleLookup(node, this, m_selector);
        parseStyle(node, attributes, this);
    } else if (FactoryMethod method = findGraphicsFactory(localName)) {
        //rendering element
        Q_ASSERT(!m_nodes.isEmpty());
        node = method(m_nodes.top(), attributes, this);
        if (node) {
            switch (m_nodes.top()->type()) {
            case QSvgNode::DOC:
            case QSvgNode::G:
            case QSvgNode::DEFS:
            case QSvgNode::SWITCH:
            {
                QSvgStructureNode *group =
                    static_cast<QSvgStructureNode*>(m_nodes.top());
                group->addChild(node, someId(attributes));
            }
                break;
            default:
                Q_ASSERT(!"not a grouping element is the parent");
            }

            parseCoreNode(node, attributes);
            cssStyleLookup(node, this, m_selector);
            if (node->type() != QSvgNode::TEXT && node->type() != QSvgNode::TEXTAREA)
                parseStyle(node, attributes, this);
            else
                parseDefaultTextStyle(node, attributes, true, this);
        }
    } else if (ParseMethod method = findUtilFactory(localName)) {
        Q_ASSERT(!m_nodes.isEmpty());
        if (!method(m_nodes.top(), attributes, this)) {
            qWarning("Problem parsing %s", qPrintable(localName));
        }
    } else if (StyleFactoryMethod method = findStyleFactoryMethod(localName)) {
        QSvgStyleProperty *prop = method(m_nodes.top(), attributes, this);
        if (prop) {
            m_style = prop;
            m_nodes.top()->appendStyleProperty(prop, someId(attributes), true);
        } else {
            qWarning("Couldn't parse node: %s", qPrintable(localName));
        }
    } else if (StyleParseMethod method = findStyleUtilFactoryMethod(localName)) {
        if (m_style) {
            if (!method(m_style, attributes, this)) {
                qWarning("Problem parsing %s", qPrintable(localName));
            }
        }
    } else {
        //qWarning()<<"Skipping unknown element!"<<namespaceURI<<"::"<<localName;
        m_skipNodes.push(Unknown);
        return true;
    }

    if (node) {
        m_nodes.push(node);
        m_skipNodes.push(Graphics);
    } else {
        //qDebug()<<"Skipping "<<localName;
        m_skipNodes.push(Style);
    }
    return true;
}

bool QSvgHandler::endElement(const QStringRef &localName)
{
    CurrentNode node = m_skipNodes.top();
    m_skipNodes.pop();
    m_whitespaceMode.pop();

    if (m_colorTagCount.count()) {
        int top = m_colorTagCount.pop();
        --top;
        if (!top) {
            m_colorStack.pop();
        } else {
            m_colorTagCount.push(top);
        }
    }

    if (node == Unknown) {
        return true;
    }

    if (m_inStyle && localName == QLatin1String("style")) {
        m_inStyle = false;
    } else if (m_nodes.top()->type() == QSvgNode::TEXT || m_nodes.top()->type() == QSvgNode::TEXTAREA) {
        QSvgText *node = static_cast<QSvgText*>(m_nodes.top());
        if (localName == QLatin1String("tspan"))
            node->popFormat();
    }

    if (node == Graphics)
        m_nodes.pop();
    else if (m_style && !m_skipNodes.isEmpty() && m_skipNodes.top() != Style)
        m_style = 0;

    return true;
}

bool QSvgHandler::characters(const QStringRef &str)
{
    if (m_inStyle) {
        QString css = str.toString();
        QCss::StyleSheet sheet;
        QCss::Parser(css).parse(&sheet);
        m_selector->styleSheets.append(sheet);
        return true;
    } else if (m_skipNodes.isEmpty() || m_skipNodes.top() == Unknown)
        return true;

    if (m_nodes.top()->type() == QSvgNode::TEXT || m_nodes.top()->type() == QSvgNode::TEXTAREA) {
        QSvgText *node = static_cast<QSvgText*>(m_nodes.top());
        node->insertText(str.toString(), m_whitespaceMode.top());
    }

    return true;
}

QSvgTinyDocument * QSvgHandler::document() const
{
    return m_doc;
}

QSvgHandler::LengthType QSvgHandler::defaultCoordinateSystem() const
{
    return m_defaultCoords;
}

void QSvgHandler::setDefaultCoordinateSystem(LengthType type)
{
    m_defaultCoords = type;
}

void QSvgHandler::pushColor(const QColor &color)
{
    m_colorStack.push(color);
    m_colorTagCount.push(1);
}

QColor QSvgHandler::currentColor() const
{
    if (!m_colorStack.isEmpty())
        return m_colorStack.top();
    else
        return QColor(0, 0, 0);
}

void QSvgHandler::setInStyle(bool b)
{
    m_inStyle = b;
}

bool QSvgHandler::inStyle() const
{
    return m_inStyle;
}

QSvgStyleSelector * QSvgHandler::selector() const
{
    return m_selector;
}

bool QSvgHandler::processingInstruction(const QString &target, const QString &data)
{
    if (target == QLatin1String("xml-stylesheet")) {
        QRegExp rx(QLatin1String("type=\\\"(.+)\\\""));
        rx.setMinimal(true);
        bool isCss = false;
        int pos = 0;
        while ((pos = rx.indexIn(data, pos)) != -1) {
            QString type =  rx.cap(1);
            if (type.toLower() == QLatin1String("text/css")) {
                isCss = true;
            }
            pos += rx.matchedLength();
        }

        if (isCss) {
            QRegExp rx(QLatin1String("href=\\\"(.+)\\\""));
            rx.setMinimal(true);
            pos = 0;
            pos = rx.indexIn(data, pos);
            QString addr = rx.cap(1);
            QFileInfo fi(addr);
            //qDebug()<<"External CSS file "<<fi.absoluteFilePath()<<fi.exists();
            if (fi.exists()) {
                QFile file(fi.absoluteFilePath());
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    return true;
                }
                QByteArray cssData = file.readAll();
                QString css = QString::fromUtf8(cssData);

                QCss::StyleSheet sheet;
                QCss::Parser(css).parse(&sheet);
                m_selector->styleSheets.append(sheet);
            }

        }
    }

    return true;
}

void QSvgHandler::setAnimPeriod(int start, int end)
{
    Q_UNUSED(start);
    m_animEnd   = qMax(end, m_animEnd);
}

int QSvgHandler::animationDuration() const
{
    return m_animEnd;
}

QSvgHandler::~QSvgHandler()
{
    delete m_selector;
    m_selector = 0;

    if(m_ownsReader)
        delete xml;
}

QT_END_NAMESPACE

#endif // QT_NO_SVG
