/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3STYLESHEET_H
#define Q3STYLESHEET_H

#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qhash.h>
#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>
#include <QtGui/qfont.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_RICHTEXT

class Q3StyleSheet;
class Q3TextDocument;
template<class Key, class T> class QMap;
class Q3StyleSheetItemData;

class Q_COMPAT_EXPORT Q3StyleSheetItem
{
public:
    Q3StyleSheetItem(Q3StyleSheet* parent, const QString& name);
    Q3StyleSheetItem(const Q3StyleSheetItem &);
    ~Q3StyleSheetItem();

    Q3StyleSheetItem& operator=(const Q3StyleSheetItem& other);

    QString name() const;

    Q3StyleSheet* styleSheet();
    const Q3StyleSheet* styleSheet() const;

    enum AdditionalStyleValues { Undefined = -1 };

    enum DisplayMode {
        DisplayBlock,
        DisplayInline,
        DisplayListItem,
        DisplayNone,
	DisplayModeUndefined = -1
    };

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode m);

    int alignment() const;
    void setAlignment(int f);

    enum VerticalAlignment {
        VAlignBaseline,
        VAlignSub,
        VAlignSuper
    };

    VerticalAlignment verticalAlignment() const;
    void setVerticalAlignment(VerticalAlignment valign);

    int fontWeight() const;
    void setFontWeight(int w);

    int logicalFontSize() const;
    void setLogicalFontSize(int s);

    int logicalFontSizeStep() const;
    void setLogicalFontSizeStep(int s);

    int fontSize() const;
    void setFontSize(int s);

    QString fontFamily() const;
    void setFontFamily(const QString&);

    int numberOfColumns() const;
    void setNumberOfColumns(int ncols);

    QColor color() const;
    void setColor(const QColor &);

    bool fontItalic() const;
    void setFontItalic(bool);
    bool definesFontItalic() const;

    bool fontUnderline() const;
    void setFontUnderline(bool);
    bool definesFontUnderline() const;

    bool fontStrikeOut() const;
    void setFontStrikeOut(bool);
    bool definesFontStrikeOut() const;

    bool isAnchor() const;
    void setAnchor(bool anc);

    enum WhiteSpaceMode {
        WhiteSpaceNormal,
        WhiteSpacePre,
        WhiteSpaceNoWrap,
        WhiteSpaceModeUndefined = -1
    };
    WhiteSpaceMode whiteSpaceMode() const;
    void setWhiteSpaceMode(WhiteSpaceMode m);

    enum Margin {
        MarginLeft,
        MarginRight,
        MarginTop,
        MarginBottom,
        MarginFirstLine,
        MarginAll,
        MarginVertical,
        MarginHorizontal,
	MarginUndefined = -1
    };

    int margin(Margin m) const;
    void setMargin(Margin, int);

    enum ListStyle {
        ListDisc,
        ListCircle,
        ListSquare,
        ListDecimal,
        ListLowerAlpha,
        ListUpperAlpha,
	ListStyleUndefined = -1
    };

    ListStyle listStyle() const;
    void setListStyle(ListStyle);

    QString contexts() const;
    void setContexts(const QString&);
    bool allowedInContext(const Q3StyleSheetItem*) const;

    bool selfNesting() const;
    void setSelfNesting(bool);

    void setLineSpacing(int ls);
    int lineSpacing() const;

private:
    void init();
    Q3StyleSheetItemData* d;
};

#ifndef QT_NO_TEXTCUSTOMITEM
class Q3TextCustomItem;
#endif

class Q_COMPAT_EXPORT Q3StyleSheet : public QObject
{
    Q_OBJECT
public:
    Q3StyleSheet(QObject *parent=0, const char *name=0);
    virtual ~Q3StyleSheet();

    static Q3StyleSheet* defaultSheet();
    static void setDefaultSheet(Q3StyleSheet*);


    Q3StyleSheetItem* item(const QString& name);
    const Q3StyleSheetItem* item(const QString& name) const;

    void insert(Q3StyleSheetItem* item);

    static QString escape(const QString&);
    static QString convertFromPlainText(const QString&,
                                        Q3StyleSheetItem::WhiteSpaceMode mode = Q3StyleSheetItem::WhiteSpacePre);
    static bool mightBeRichText(const QString&);

    virtual void scaleFont(QFont& font, int logicalSize) const;

    virtual void error(const QString&) const;

private:
    Q_DISABLE_COPY(Q3StyleSheet)

    void init();
    QHash<QString, Q3StyleSheetItem *> styles;
    Q3StyleSheetItem* nullstyle;
};

#endif // QT_NO_RICHTEXT

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3STYLESHEET_H
