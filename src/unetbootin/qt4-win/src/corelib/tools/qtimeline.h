/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QTIMELINE_H
#define QTIMELINE_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QTimeLinePrivate;
class Q_CORE_EXPORT QTimeLine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)
    Q_PROPERTY(int currentTime READ currentTime WRITE setCurrentTime)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection)
    Q_PROPERTY(int loopCount READ loopCount WRITE setLoopCount)
    Q_PROPERTY(CurveShape curveShape READ curveShape WRITE setCurveShape)
public:
    enum State {
        NotRunning,
        Paused,
        Running
    };
    enum Direction {
        Forward,
        Backward
    };
    enum CurveShape {
        EaseInCurve,
        EaseOutCurve,
        EaseInOutCurve,
        LinearCurve,
        SineCurve,
        CosineCurve
    };

    explicit QTimeLine(int duration = 1000, QObject *parent = 0);
    virtual ~QTimeLine();

    State state() const;

    int loopCount() const;
    void setLoopCount(int count);

    Direction direction() const;
    void setDirection(Direction direction);

    int duration() const;
    void setDuration(int duration);

    int startFrame() const;
    void setStartFrame(int frame);
    int endFrame() const;
    void setEndFrame(int frame);
    void setFrameRange(int startFrame, int endFrame);

    int updateInterval() const;
    void setUpdateInterval(int interval);

    CurveShape curveShape() const;
    void setCurveShape(CurveShape shape);

    int currentTime() const;
    int currentFrame() const;
    qreal currentValue() const;

    int frameForTime(int msec) const;
    virtual qreal valueForTime(int msec) const;

public Q_SLOTS:
    void start();
    void resume();
    void stop();
    void setPaused(bool paused);
    void setCurrentTime(int msec);
    void toggleDirection();

Q_SIGNALS:
    void valueChanged(qreal x);
    void frameChanged(int);
    void stateChanged(QTimeLine::State newState);
    void finished();

protected:
    void timerEvent(QTimerEvent *event);

private:
    Q_DISABLE_COPY(QTimeLine)
    Q_DECLARE_PRIVATE(QTimeLine)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif

