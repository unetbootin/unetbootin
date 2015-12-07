/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef DEMO_ITEM_H
#define DEMO_ITEM_H

#include <QtGui>

class DemoItemAnimation;
class Guide;

class SharedImage
{
public:
    SharedImage() : refCount(0), image(0), pixmap(0){};
    ~SharedImage()
    {
        delete image;
        delete pixmap;
    }

    int refCount;
    QImage *image;
    QPixmap *pixmap;
    QMatrix matrix;
    QRectF unscaledBoundingRect;
};

class DemoItem : public QGraphicsItem
{

public:
    DemoItem(QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);
    virtual ~DemoItem();

    bool inTransition();
    virtual void animationStarted(int id = 0){ Q_UNUSED(id); };
    virtual void animationStopped(int id = 0){ Q_UNUSED(id); };
    virtual void prepare(){};
    void setRecursiveVisible(bool visible);
    void useSharedImage(const QString &hashKey);
    void setNeverVisible(bool never = true);
    static void setMatrix(const QMatrix &matrix);
    virtual QRectF boundingRect() const; // overridden
    void setPosUsingSheepDog(const QPointF &dest, const QRectF &sceneFence);

    qreal opacity;
    bool locked;
    DemoItemAnimation *currentAnimation;
    bool noSubPixeling;

    // Used if controlled by a guide:
    void useGuide(Guide *guide, float startFrame = 0);
    void guideAdvance(float distance);
    void guideMove(float moveSpeed);
    void setGuidedPos(const QPointF &position);
    QPointF getGuidedPos();
    float startFrame;
    float guideFrame;
    Guide *currGuide;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0); // overridden
    virtual QImage *createImage(const QMatrix &) const { return 0; };
    virtual bool collidesWithItem(const QGraphicsItem *, Qt::ItemSelectionMode) const { return false; };
    bool prepared;

private:
    SharedImage *sharedImage;
    QString hashKey;
    bool neverVisible;
    bool validateImage();

    // Used if controlled by a guide:
    void switchGuide(Guide *guide);
    friend class Guide;
    QPointF guidedPos;

    // The next static hash is shared amongst all demo items, and
    // has the purpose of reusing images to save memory and time
    static QHash<QString, SharedImage *> sharedImageHash;
    static QMatrix matrix;
};

#endif // DEMO_ITEM_H

