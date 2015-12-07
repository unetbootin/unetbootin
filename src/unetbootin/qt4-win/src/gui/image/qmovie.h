/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QMOVIE_H
#define QMOVIE_H

#include <QtCore/qobject.h>

#ifndef QT_NO_MOVIE

#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qobject.h>
#include <QtGui/qimagereader.h>

#ifdef QT3_SUPPORT
#include <QtGui/qimage.h>
#include <QtGui/qpixmap.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QByteArray;
class QColor;
class QIODevice;
class QImage;
class QPixmap;
class QRect;
class QSize;

class QMoviePrivate;
class Q_GUI_EXPORT QMovie : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMovie)
    Q_ENUMS(MovieState CacheMode)
    Q_PROPERTY(int speed READ speed WRITE setSpeed)
    Q_PROPERTY(CacheMode cacheMode READ cacheMode WRITE setCacheMode)
public:
    enum MovieState {
        NotRunning,
        Paused,
        Running
    };
    enum CacheMode {
        CacheNone,
        CacheAll
    };

    QMovie(QObject *parent = 0);
    explicit QMovie(QIODevice *device, const QByteArray &format = QByteArray(), QObject *parent = 0);
    explicit QMovie(const QString &fileName, const QByteArray &format = QByteArray(), QObject *parent = 0);
    ~QMovie();

    static QList<QByteArray> supportedFormats();

    void setDevice(QIODevice *device);
    QIODevice *device() const;

    void setFileName(const QString &fileName);
    QString fileName() const;

    void setFormat(const QByteArray &format);
    QByteArray format() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    MovieState state() const;

    QRect frameRect() const;
    QImage currentImage() const;
    QPixmap currentPixmap() const;

    bool isValid() const;

    bool jumpToFrame(int frameNumber);
    int loopCount() const;
    int frameCount() const;
    int nextFrameDelay() const;
    int currentFrameNumber() const;

    int speed() const;

    QSize scaledSize();
    void setScaledSize(const QSize &size);

    CacheMode cacheMode() const;
    void setCacheMode(CacheMode mode);

    CacheMode cacheMode(); // ### Qt 5: remove me

Q_SIGNALS:
    void started();
    void resized(const QSize &size);
    void updated(const QRect &rect);
    void stateChanged(QMovie::MovieState state);
    void error(QImageReader::ImageReaderError error);
    void finished();
    void frameChanged(int frameNumber);

public Q_SLOTS:
    void start();
    bool jumpToNextFrame();
    void setPaused(bool paused);
    void stop();
    void setSpeed(int percentSpeed);

private:
    Q_DISABLE_COPY(QMovie)
    Q_PRIVATE_SLOT(d_func(), void _q_loadNextFrame())

#ifdef QT3_SUPPORT
public:
    inline QT3_SUPPORT bool isNull() const { return isValid(); }
    inline QT3_SUPPORT int frameNumber() const { return currentFrameNumber(); }
    inline QT3_SUPPORT bool running() const { return state() == Running; }
    inline QT3_SUPPORT bool paused() const { return state() == Paused; }
    inline QT3_SUPPORT bool finished() const { return state() == NotRunning; }
    inline QT3_SUPPORT void restart() { stop(); start(); }
    inline QT3_SUPPORT QImage frameImage() const { return currentImage(); }
    inline QT3_SUPPORT QPixmap framePixmap() const { return currentPixmap(); }
    inline QT3_SUPPORT void step() { jumpToNextFrame(); }
    inline QT3_SUPPORT void pause() { setPaused(true); }
    inline QT3_SUPPORT void unpause() { setPaused(false); }
#endif
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_MOVIE

#endif // QMOVIE_H
