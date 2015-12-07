/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QGRAPHICSITEM_P_H
#define QGRAPHICSITEM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "qgraphicsitem.h"

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

QT_BEGIN_NAMESPACE

class QGraphicsItemPrivate;

class QGraphicsItemCache
{
public:
    QGraphicsItemCache() : allExposed(false) { }

    // ItemCoordinateCache only
    QRect boundingRect;
    QSize fixedSize;
    QString key;

    // DeviceCoordinateCache only
    struct DeviceData {
        QTransform lastTransform;
        QPoint cacheIndent;
        QString key;
    };
    QMap<QPaintDevice *, DeviceData> deviceData;

    // List of logical exposed rects
    QVector<QRectF> exposed;
    bool allExposed;

    // Empty cache
    void purge();
};

class Q_AUTOTEST_EXPORT QGraphicsItemPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsItem)
public:
    enum Extra {
        ExtraTransform,
        ExtraToolTip,
        ExtraCursor,
        ExtraCacheData,
        ExtraMaxDeviceCoordCacheSize,
        ExtraBoundingRegionGranularity,
        ExtraOpacity,
        ExtraEffectiveOpacity
    };

    enum AncestorFlag {
        NoFlag = 0,
        AncestorHandlesChildEvents = 0x1,
        AncestorClipsChildren = 0x2,
        AncestorIgnoresTransformations = 0x4
    };

    inline QGraphicsItemPrivate()
        : z(0),
        scene(0),
        parent(0),
        index(-1),
        depth(0),
        acceptedMouseButtons(0x1f),
        visible(1),
        explicitlyHidden(0),
        enabled(1),
        explicitlyDisabled(0),
        selected(0),
        acceptsHover(0),
        acceptDrops(0),
        isMemberOfGroup(0),
        handlesChildEvents(0),
        itemDiscovered(0),
        hasTransform(0),
        hasCursor(0),
        ancestorFlags(0),
        cacheMode(0),
        hasBoundingRegionGranularity(0),
        flags(0),
        hasOpacity(0),
        isWidget(0),
        dirty(0),
        dirtyChildren(0),
        localCollisionHack(0),
        globalStackingOrder(-1),
        sceneTransformIndex(-1),
        q_ptr(0)
    {
    }

    inline virtual ~QGraphicsItemPrivate()
    { }

    void updateAncestorFlag(QGraphicsItem::GraphicsItemFlag childFlag,
                            AncestorFlag flag = NoFlag, bool enabled = false, bool root = true);
    void setIsMemberOfGroup(bool enabled);
    void remapItemPos(QEvent *event, QGraphicsItem *item);
    QPointF genericMapFromScene(const QPointF &pos, const QWidget *viewport) const;
    bool itemIsUntransformable() const;

    // ### Qt 5: Remove. Workaround for reimplementation added after Qt 4.4.
    virtual QVariant inputMethodQueryHelper(Qt::InputMethodQuery query) const;
    static bool movableAncestorIsSelected(const QGraphicsItem *item);

    void setPosHelper(const QPointF &pos, bool update);
    void setVisibleHelper(bool newVisible, bool explicitly, bool update = true);
    void setEnabledHelper(bool newEnabled, bool explicitly, bool update = true);
    void updateHelper(const QRectF &rect = QRectF(), bool force = false);
    void fullUpdateHelper(bool childrenOnly = false);
    void resolveEffectiveOpacity(qreal effectiveParentOpacity);
    void resolveDepth(int parentDepth);
    void invalidateSceneTransformCache();

    virtual void resolveFont(uint inheritedMask)
    {
        for (int i = 0; i < children.size(); ++i)
            children.at(i)->d_ptr->resolveFont(inheritedMask);
    }

    virtual void resolvePalette(uint inheritedMask)
    {
        for (int i = 0; i < children.size(); ++i)
            children.at(i)->d_ptr->resolveFont(inheritedMask);
    }

    virtual bool isProxyWidget() const;

    inline QVariant extra(Extra type) const
    {
        for (int i = 0; i < extras.size(); ++i) {
            const ExtraStruct &extra = extras.at(i);
            if (extra.type == type)
                return extra.value;
        }
        return QVariant();
    }

    inline void setExtra(Extra type, const QVariant &value)
    {
        int index = -1;
        for (int i = 0; i < extras.size(); ++i) {
            if (extras.at(i).type == type) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            extras << ExtraStruct(type, value);
        } else {
            extras[index].value = value;
        }
    }

    inline void unsetExtra(Extra type)
    {
        for (int i = 0; i < extras.size(); ++i) {
            if (extras.at(i).type == type) {
                extras.removeAt(i);
                return;
            }
        }
    }

    struct ExtraStruct {
        ExtraStruct(Extra type, QVariant value)
            : type(type), value(value)
        { }

        Extra type;
        QVariant value;

        bool operator<(Extra extra) const
        { return type < extra; }
    };
    QList<ExtraStruct> extras;

    QGraphicsItemCache *extraItemCache() const;
    void removeExtraItemCache();

    QPointF pos;
    qreal z;
    QGraphicsScene *scene;
    QGraphicsItem *parent;
    QList<QGraphicsItem *> children;
    int index;
    int depth;

    // Packed 32 bytes
    quint32 acceptedMouseButtons : 5;
    quint32 visible : 1;
    quint32 explicitlyHidden : 1;
    quint32 enabled : 1;
    quint32 explicitlyDisabled : 1;
    quint32 selected : 1;
    quint32 acceptsHover : 1;
    quint32 acceptDrops : 1;
    quint32 isMemberOfGroup : 1;
    quint32 handlesChildEvents : 1;
    quint32 itemDiscovered : 1;
    quint32 hasTransform : 1;
    quint32 hasCursor : 1;
    quint32 ancestorFlags : 3;
    quint32 cacheMode : 2;
    quint32 hasBoundingRegionGranularity : 1;
    quint32 flags : 9;

    // New 32 bytes
    quint32 hasOpacity : 1;
    quint32 isWidget : 1;
    quint32 dirty : 1;    
    quint32 dirtyChildren : 1;    
    quint32 localCollisionHack : 1;

    // Optional stacking order
    int globalStackingOrder;
    int sceneTransformIndex;

    QGraphicsItem *q_ptr;
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif
