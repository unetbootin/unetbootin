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

#ifndef QSVGNODE_P_H
#define QSVGNODE_P_H

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

#include "qsvgstyle_p.h"

#ifndef QT_NO_SVG

#include "QtCore/qstring.h"
#include "QtCore/qhash.h"

QT_BEGIN_NAMESPACE

class QPainter;
class QSvgTinyDocument;

class QSvgNode
{
public:
    enum Type
    {
        DOC,
        G,
        DEFS,
        SWITCH,
        ANIMATION,
        ARC,
        CIRCLE,
        ELLIPSE,
        IMAGE,
        LINE,
        PATH,
        POLYGON,
        POLYLINE,
        RECT,
        TEXT,
        TEXTAREA,
        USE,
        VIDEO
    };
    enum DisplayMode {
        InlineMode,
        BlockMode,
        ListItemMode,
        RunInMode,
        CompactMode,
        MarkerMode,
        TableMode,
        InlineTableMode,
        TableRowGroupMode,
        TableHeaderGroupMode,
        TableFooterGroupMode,
        TableRowMode,
        TableColumnGroupMode,
        TableColumnMode,
        TableCellMode,
        TableCaptionMode,
        NoneMode,
        InheritMode
    };
public:
    QSvgNode(QSvgNode *parent=0);
    virtual ~QSvgNode();
    virtual void draw(QPainter *p) =0;

    QSvgNode *parent() const;

    void appendStyleProperty(QSvgStyleProperty *prop, const QString &id,
                             bool justLink=false);
    void applyStyle(QPainter *p);
    void revertStyle(QPainter *p);
    QSvgStyleProperty *styleProperty(QSvgStyleProperty::Type type) const;
    QSvgStyleProperty *styleProperty(const QString &id) const;

    QSvgTinyDocument *document() const;

    virtual Type type() const =0;
    virtual QRectF bounds() const;
    virtual QRectF transformedBounds(const QMatrix &mat) const;

    void setRequiredFeatures(const QStringList &lst);
    const QStringList & requiredFeatures() const;

    void setRequiredExtensions(const QStringList &lst);
    const QStringList & requiredExtensions() const;

    void setRequiredLanguages(const QStringList &lst);
    const QStringList & requiredLanguages() const;

    void setRequiredFormats(const QStringList &lst);
    const QStringList & requiredFormats() const;

    void setRequiredFonts(const QStringList &lst);
    const QStringList & requiredFonts() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void setDisplayMode(DisplayMode display);
    DisplayMode displayMode() const;

    QString nodeId() const;
    void setNodeId(const QString &i);

    QString xmlClass() const;
    void setXmlClass(const QString &str);
protected:
    QSvgStyle   m_style;

    qreal strokeWidth() const;
private:
    QSvgNode   *m_parent;
    QHash<QString, QSvgRefCounter<QSvgStyleProperty> > m_styles;

    QStringList m_requiredFeatures;
    QStringList m_requiredExtensions;
    QStringList m_requiredLanguages;
    QStringList m_requiredFormats;
    QStringList m_requiredFonts;

    bool        m_visible;

    QString m_id;
    QString m_class;

    DisplayMode m_displayMode;
};

inline QSvgNode *QSvgNode::parent() const
{
    return m_parent;
}

inline bool QSvgNode::isVisible() const
{
    return m_visible;
}

inline QString QSvgNode::nodeId() const
{
    return m_id;
}

inline QString QSvgNode::xmlClass() const
{
    return m_class;
}

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGNODE_P_H
