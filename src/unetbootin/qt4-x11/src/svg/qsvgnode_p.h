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
    virtual void draw(QPainter *p, QSvgExtraStates &states) =0;

    QSvgNode *parent() const;

    void appendStyleProperty(QSvgStyleProperty *prop, const QString &id,
                             bool justLink=false);
    void applyStyle(QPainter *p, QSvgExtraStates &states);
    void revertStyle(QPainter *p, QSvgExtraStates &states);
    QSvgStyleProperty *styleProperty(QSvgStyleProperty::Type type) const;
    QSvgStyleProperty *styleProperty(const QString &id) const;

    QSvgTinyDocument *document() const;

    virtual Type type() const =0;
    virtual QRectF bounds() const;
    virtual QRectF transformedBounds(const QTransform &transform) const;

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

    friend class QSvgTinyDocument;
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
