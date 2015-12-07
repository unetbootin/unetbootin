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

#ifndef DOMTOOL_H
#define DOMTOOL_H

#include <QVariant>

QT_BEGIN_NAMESPACE

class QDomElement;
class QDomDocument;
class QDomNode;
class QDomNodeList;

struct Common
{
    int kind;

    enum {
        Kind_Unknown = 0,
        Kind_Color,
        Kind_Point,
        Kind_Size,
        Kind_Rect,
        Kind_Font,
        Kind_SizePolicy,
        Kind_Cursor
    };

    inline void init()
    { kind = Kind_Unknown; }
};

struct Color
{
    Common common;
    int red, green, blue;

    inline void init(int r, int g, int b)
    {
        common.kind = Common::Kind_Color;
        red = r;
        green = g;
        blue = b;
    }

    inline bool operator == (const Color &other) const
    { return red == other.red && green == other.green && blue == other.blue; }
};

struct Point
{
    Common common;
    int x, y;

    inline void init(int x, int y)
    {
        common.kind = Common::Kind_Point;
        this->x = x;
        this->y = y;
    }
};

struct Size
{
    Common common;
    int width, height;

    inline bool isNull() const
    { return this->width == 0 && this->height == 0; }

    inline void init(int width, int height)
    {
        common.kind = Common::Kind_Size;
        this->width = width;
        this->height = height;
    }
};

struct Rect
{
    Common common;
    int x, y;
    int width, height;

    inline void init(int x, int y, int width, int height)
    {
        common.kind = Common::Kind_Rect;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
};

struct Font
{
    Common common;
    char *family;
    int pointsize;
    bool bold;
    bool italic;
    bool underline;
    bool strikeout;

    inline void init()
    {
        common.kind = Common::Kind_Font;
        family = 0;
        pointsize = 0;
        bold = false;
        italic = false;
        underline = false;
        strikeout = false;
    }
};

struct SizePolicy
{
    Common common;
    int hsizetype;
    int vsizetype;
    int horstretch;
    int verstretch;

    inline void init()
    {
        common.kind = Common::Kind_SizePolicy;
        hsizetype = 0;
        vsizetype = 0;
        horstretch = 0;
        verstretch = 0;
    }
};

struct Cursor
{
    Common common;
    int shape;

    inline void init(int shape)
    {
        common.kind = Common::Kind_Cursor;
        this->shape = shape;
    }
};

union Variant
{
    Common common;
    Color color;
    Size size;
    Point point;
    Rect rect;
    Font font;
    SizePolicy sizePolicy;
    Cursor cursor;

    inline Variant()
    { common.kind = Common::Kind_Unknown; }

    inline ~Variant()
    {
        if (common.kind == Common::Kind_Font) {
            delete[] font.family;
            font.family = 0;
        }
    }

    inline int kind() const
    { return common.kind; }

    inline Variant &createColor(int r, int g, int b)
    { color.init(r, g, b); return *this; }

    inline Variant &createPoint(int x, int y)
    { point.init(x, y); return *this; }

    inline Variant &createSize(int width, int height)
    { size.init(width, height); return *this; }

    inline Variant &createRect(int x, int y, int w, int h)
    { rect.init(x, y, w, h); return *this; }

    inline Variant &createFont()
    { font.init(); return *this; }

    inline Variant &createSizePolicy()
    { sizePolicy.init(); return *this; }

    inline Variant &createCursor(int shape)
    { cursor.init(shape); return *this; }
};

class DomTool
{
public:
    static QVariant readProperty(const QDomElement& e, const QString& name, const QVariant& defValue);
    static QVariant readProperty(const QDomElement& e, const QString& name, const QVariant& defValue, QString& comment);
    static bool hasProperty(const QDomElement& e, const QString& name);
    static QStringList propertiesOfType(const QDomElement& e, const QString& type);
    static QVariant elementToVariant(const QDomElement& e, const QVariant& defValue);
    static QVariant elementToVariant(const QDomElement& e, const QVariant& defValue, QString &comment);
    static QVariant readAttribute(const QDomElement& e, const QString& name, const QVariant& defValue);
    static QVariant readAttribute(const QDomElement& e, const QString& name, const QVariant& defValue, QString& comment);
    static bool hasAttribute(const QDomElement& e, const QString& name);
    static Color readColor(const QDomElement &e);
    static void fixDocument(QDomDocument&);
    static void fixAttributes(QDomNodeList&, double);
    static void fixAttribute(QDomNode&, double);
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Size)
Q_DECLARE_METATYPE(Rect)
Q_DECLARE_METATYPE(Font)
Q_DECLARE_METATYPE(SizePolicy)
Q_DECLARE_METATYPE(Cursor)
Q_DECLARE_METATYPE(Color)
Q_DECLARE_METATYPE(Point)
Q_DECLARE_METATYPE(Common)
Q_DECLARE_METATYPE(Variant)

QT_BEGIN_NAMESPACE

inline Variant asVariant(const QVariant &v)
{
    Variant var;
    var = qVariantValue<Variant>(v);
    return var;
}

QT_END_NAMESPACE

#endif // DOMTOOL_H
