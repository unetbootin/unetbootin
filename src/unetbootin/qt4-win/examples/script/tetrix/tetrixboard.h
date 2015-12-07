/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include <QTimer>
#include <QFrame>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class TetrixBoard : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QObject* timer READ getTimer)
    Q_PROPERTY(QWidget* nextPieceLabel WRITE setNextPieceLabel)

public:
    TetrixBoard(QWidget *parent = 0);

    void setNextPieceLabel(QWidget *label);
    void setBoardWidth(int width);
    void setBoardHeight(int height);
    QSize minimumSizeHint() const;

    QObject *getTimer();

signals:
    void scoreChanged(int score);
    void levelChanged(int level);
    void linesRemovedChanged(int numLines);

#if !defined(Q_MOC_RUN)
private: // can only be emitted by TetrixBoard
#endif
    void keyPressed(int key);
    void paintRequested(QPainter *painter);
    void paintNextPieceRequested(QPainter *painter);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

protected slots:
    void showNextPiece(int width, int height);
    void drawPauseScreen(QPainter *painter);
    void drawSquare(QPainter *painter, int x, int y, int shape);

private:
    enum { BoardWidth = 10, BoardHeight = 22 };

    int squareWidth() { return contentsRect().width() / BoardWidth; }
    int squareHeight() { return contentsRect().height() / BoardHeight; }

    QTimer *timer;
    QPointer<QLabel> nextPieceLabel;
};

#endif
