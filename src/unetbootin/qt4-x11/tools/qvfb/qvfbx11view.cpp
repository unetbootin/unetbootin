/****************************************************************************
**
** Copyright (C) 1992-2006 TROLLTECH ASA. All rights reserved.
**
** This file is part of the Phone Edition of the Qt Toolkit.
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

#include "qvfbx11view.h"
#include "x11keyfaker.h"
#include <qevent.h>
#include <QX11Info>
#include <QTimer>
#include <QProcess>
#include <QDebug>
#include <QUuid>
#include <QDataStream>
#include <QTemporaryFile>
#include <X11/Xlib.h>

QT_BEGIN_NAMESPACE

QVFbX11View::QVFbX11View
	(int id, int w, int h, int d, Rotation r, QWidget *parent)
    : QVFbAbstractView(parent)
{
    this->id = id;
    this->w = w;
    this->h = h;
    this->d = d;
    this->rotation = r;
    this->gr = 1.0;
    this->gg = 1.0;
    this->gb = 1.0;
    this->touchscreen = false;
    this->lcd = false;
    this->keyFaker = 0;
    this->xnest = 0;
    this->serverAuthFile = 0;
    this->shutdown = false;

    // Try to find Xephyr, as it is better than Xnest in many ways.
    if (QFile::exists("/usr/bin/Xephyr"))
        xserver = "/usr/bin/Xephyr";
    else if (QFile::exists("/usr/local/bin/Xephyr"))
        xserver = "/usr/local/bin/Xephyr";
    else if (QFile::exists("/usr/X11R6/bin/Xephyr"))
        xserver = "/usr/X11R6/bin/Xephyr";
    else
        xserver = "Xnest";
}

QVFbX11View::~QVFbX11View()
{
    shutdown = true;
    if (xnest) {
	xnest->terminate();
	xnestStopped();
    }
}

int QVFbX11View::displayId() const
{
    return id;
}

int QVFbX11View::displayWidth() const
{
    return ( (int)rotation & 0x01 ) ? h : w;
}

int QVFbX11View::displayHeight() const
{
    return ( (int)rotation & 0x01 ) ? w : h;
}

int QVFbX11View::displayDepth() const
{
    return d;
}

QVFbX11View::Rotation QVFbX11View::displayRotation() const
{
    return rotation;
}

void QVFbX11View::skinKeyPressEvent(int code, const QString&, bool)
{
    if (keyFaker)
	keyFaker->sendKeyEvent(code, true);
}

void QVFbX11View::skinKeyReleaseEvent(int code, const QString&, bool)
{
    if (keyFaker)
	keyFaker->sendKeyEvent(code, false);
}

void QVFbX11View::setGamma(double gr, double gg, double gb)
{
    // We remember the values, but don't do anything with them.
    this->gr = gr;
    this->gg = gg;
    this->gb = gb;
}

double QVFbX11View::gammaRed() const
{
    return gr;
}

double QVFbX11View::gammaGreen() const
{
    return gg;
}

double QVFbX11View::gammaBlue() const
{
    return gb;
}

void QVFbX11View::getGamma(int, QRgb& rgb)
{
    rgb = qRgb(255, 255, 255);
}

bool QVFbX11View::touchScreenEmulation() const
{
    return touchscreen;
}

bool QVFbX11View::lcdScreenEmulation() const
{
    return lcd;
}

int QVFbX11View::rate()
{
    // We don't support refresh rates, so return a default value.
    return 30;
}

bool QVFbX11View::animating() const
{
    // We don't support animation.
    return false;
}

QImage QVFbX11View::image() const
{
    // We don't support image capture.
    return QImage();
}

void QVFbX11View::setRate(int)
{
    // We don't support rate adjustments.
}


double QVFbX11View::zoomH() const
{
    // Zoom is not possible with Xnest.
    return 1.0;
}

double QVFbX11View::zoomV() const
{
    // Zoom is not possible with Xnest.
    return 1.0;
}

QSize QVFbX11View::sizeHint() const
{
    return QSize(w, h);
}

void QVFbX11View::setTouchscreenEmulation( bool flag )
{
    touchscreen = flag;
}

void QVFbX11View::setLcdScreenEmulation( bool flag )
{
    lcd = flag;
}

void QVFbX11View::setZoom( double, double )
{
    // Zoom is not possible with Xnest.
}

void QVFbX11View::setRotation( Rotation )
{
    // Rotation is not possible with Xnest.
}

void QVFbX11View::startAnimation( const QString& )
{
    // Animation is not supported.
}

void QVFbX11View::stopAnimation()
{
    // Animation is not supported.
}

// Generate a 16-byte magic cookie string.
static QString generateMagicCookie()
{
    static const char hexchars[] = "0123456789abcdef";
    QUuid uuid = QUuid::createUuid();
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream << uuid;
    QString value;
    foreach ( char ch, ba ) {
	value += QChar( hexchars[(ch >> 4) & 0x0F] );
	value += QChar( hexchars[ch & 0x0F] );
    }
    return value;
}

void QVFbX11View::showEvent(QShowEvent *e)
{
    if (!xnest)
	startXnest();
    QWidget::showEvent(e);
}

void QVFbX11View::keyPressEvent(QKeyEvent *e)
{
    if (keyFaker)
	keyFaker->sendKeyEvent(e->key(), true);
    QWidget::keyPressEvent(e);
}

void QVFbX11View::keyReleaseEvent(QKeyEvent *e)
{
    if (keyFaker)
	keyFaker->sendKeyEvent(e->key(), false);
    QWidget::keyReleaseEvent(e);
}

void QVFbX11View::startXnest()
{
    // Add authentication credentials to the XAUTHORITY file.
    QString cookie = generateMagicCookie();
    QStringList xauthargs;
    xauthargs += "add";
    xauthargs += ":" + QString::number(displayId());
    xauthargs += "MIT-MAGIC-COOKIE-1";
    xauthargs += cookie;
    if (QProcess::execute("xauth", xauthargs) != 0)
	qWarning() << "xauth: failed to add Xnest client authentication credentials";

    // Write the credentials to another authentication file for the server.
    serverAuthFile = new QTemporaryFile(this);
    QString authFilename;
    if (serverAuthFile->open()) {
	authFilename = serverAuthFile->fileName();
	serverAuthFile->close();
	xauthargs.clear();
	xauthargs += "-f";
	xauthargs += authFilename;
	xauthargs += "add";
	xauthargs += ":" + QString::number(displayId());
	xauthargs += "MIT-MAGIC-COOKIE-1";
	xauthargs += cookie;
	if (QProcess::execute("xauth", xauthargs) != 0)
	    qWarning() << "xauth: failed to add Xnest server authentication credentials";
    }

    // Create a raw X11 window to act as the Xnest's root window.
    // We cannot use winId() directly because qvfb is already
    // selecting for events that Xnest wants to select for.
    WId root = XCreateSimpleWindow
	(QX11Info::display(), winId(), 0, 0, w, h, 0,
	 BlackPixel(QX11Info::display(), QX11Info::appScreen()),
	 BlackPixel(QX11Info::display(), QX11Info::appScreen()));
    XMapWindow(QX11Info::display(), root);

    // Warn the user if the visual number looks wrong.  Xnest expects
    // its root window to be on the default visual.
    if (QX11Info::appVisual() != DefaultVisual(QX11Info::display(), QX11Info::appScreen())) {
	qWarning() << "*** Qt is not using the default visual.  Xnest may fail "
		      "with a BadMatch error.";
	qWarning() << "*** If it fails, then restart qvfb with \" -visual"
		   << DefaultVisual(QX11Info::display(), QX11Info::appScreen())
			    ->visualid << "\"";
    }

    // Make sure the root window is in the X server before Xnest starts.
    XSync(QX11Info::display(), False);

    // Start the Xnest process.
    xnest = new QProcess(this);
    connect(xnest, SIGNAL(error(QProcess::ProcessError)),
	    this, SLOT(xnestStopped()));
    connect(xnest, SIGNAL(finished(int,QProcess::ExitStatus)),
	    this, SLOT(xnestStopped()));
    QStringList args;
    args += "-auth";
    args += authFilename;
    if (!xserver.contains("Xephyr")) {
        args += "-geometry";
        args += QString::number(w) + "x" + QString::number(h) + "+0+0";
        args += "-depth";
        args += QString::number(d);
    }
    args += "-br";	    // Start Xnest with a black background.
    args += "-parent";
    args += "0x" + QString::number(root, 16);
    args += ":" + QString::number(displayId());
    xnest->setProcessChannelMode(QProcess::ForwardedChannels);
    xnest->start(xserver, args);
    //qDebug() << args;

    QTimer::singleShot(200, this, SLOT(startKeyFaker()));
}

void QVFbX11View::xnestStopped()
{
    if (!shutdown) {
	if (xnest && xnest->error() == QProcess::FailedToStart)
	    qWarning() << xserver << "could not be started";
	else
	    qWarning() << xserver << "stopped unexpectedly";
    }
    if (keyFaker) {
	delete keyFaker;
	keyFaker = 0;
    }
    if (xnest) {
	xnest->deleteLater();
	xnest = 0;

	QStringList xauthargs;
	xauthargs += "remove";
	xauthargs += ":" + QString::number(displayId());
	if (QProcess::execute("xauth", xauthargs) != 0)
	    qWarning() << "xauth: failed to remove Xnest authentication credentials";
    }
    if (serverAuthFile) {
	delete serverAuthFile;
	serverAuthFile = 0;
    }
}

void QVFbX11View::startKeyFaker()
{
    if (!keyFaker && xnest)
	keyFaker = new X11KeyFaker(":" + QString::number(displayId()), this);
}

QT_END_NAMESPACE
