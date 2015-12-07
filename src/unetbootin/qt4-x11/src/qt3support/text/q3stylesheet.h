/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
