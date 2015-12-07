/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "qlock_p.h"

#include "qvfbshmem.h"
#include "qvfbhdr.h"

#define QTE_PIPE "QtEmbedded-%1"

#include <QFile>
#include <QTimer>

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

QT_BEGIN_NAMESPACE

#ifdef Q_WS_QWS
#error qvfb must be compiled with  the Qt for X11 package
#endif

// Get the name of the directory where Qt for Embedded Linux temporary data should
// live.
static QString qws_dataDir(int qws_display_id)
{
    QByteArray dataDir = QString("/tmp/qtembedded-%1").arg(qws_display_id).toLocal8Bit();
    if (mkdir(dataDir, 0700)) {
        if (errno != EEXIST) {
            qFatal("Cannot create Qt for Embedded Linux data directory: %s", dataDir.constData());
        }
    }

    struct stat buf;
    if (lstat(dataDir, &buf))
        qFatal("stat failed for Qt for Embedded Linux data directory: %s", dataDir.constData());

    if (!S_ISDIR(buf.st_mode))
        qFatal("%s is not a directory", dataDir.constData());
    if (buf.st_uid != getuid())
        qFatal("Qt for Embedded Linux data directory is not owned by user %uh", getuid());

    if ((buf.st_mode & 0677) != 0600)
        qFatal("Qt for Embedded Linux data directory has incorrect permissions: %s", dataDir.constData());
    dataDir += "/";

    return QString(dataDir);
}


static QString displayPipe;
static QString displayPiped;
class DisplayLock
{
public:
    DisplayLock() : qlock(0) {
        if (QFile::exists(displayPiped)) {
            qlock = new QLock(displayPipe, 'd', false);
            qlock->lock(QLock::Read);
        }
    }
    ~DisplayLock() {
        if (qlock) {
            qlock->unlock();
            delete qlock;
            qlock = 0;
        }
    }
private:
    QLock *qlock;
};

QShMemViewProtocol::QShMemViewProtocol(int displayid, const QSize &s,
                                       int d, QObject *parent)
    : QVFbViewProtocol(displayid, parent), hdr(0), dataCache(0), lockId(-1),
      windowId(0)
{
    int w = s.width();
    int h = s.height();

    QString username = "unknown";
    const char *logname = getenv("LOGNAME");
    if ( logname )
        username = logname;

    QString oldPipe = "/tmp/qtembedded-" + username + "/" + QString(QTE_PIPE).arg(displayid);
    int oldPipeSemkey = ftok(oldPipe.toLatin1().constData(), 'd');
    if (oldPipeSemkey != -1) {
        int oldPipeLockId = semget(oldPipeSemkey, 0, 0);
        if (oldPipeLockId >= 0){
            sembuf sops;
            sops.sem_num = 0;
            sops.sem_op = 1;
            sops.sem_flg = SEM_UNDO;
            int rv;
            do {
                rv = semop(lockId,&sops,1);
            } while (rv == -1 && errno == EINTR);

            perror("QShMemViewProtocol::QShMemViewProtocol");
            qFatal("Cannot create lock file as an old version of QVFb has "
                   "opened %s. Close other QVFb and try again",
                   oldPipe.toLatin1().constData());
        }
    }

    kh = new QVFbKeyPipeProtocol(displayid);
    /* should really depend on receiving qt version, but how can
       one tell? */
    mh = new QVFbMousePipe(displayid);

    QString mousePipe = mh->pipeName();

    key_t key = ftok(mousePipe.toLatin1().constData(), 'b');

    int bpl;
    if (d < 8)
	bpl = (w * d + 7) / 8;
    else
        bpl = w * ((d + 7) / 8);

    displaySize = bpl * h;

    unsigned char *data;
    uint data_offset_value = sizeof(QVFbHeader);

    int dataSize = bpl * h + data_offset_value;
    shmId = shmget(key, dataSize, IPC_CREAT | 0666);
    if (shmId != -1)
	data = (unsigned char *)shmat(shmId, 0, 0);
    else {
	struct shmid_ds shm;
	shmctl(shmId, IPC_RMID, &shm);
	shmId = shmget(key, dataSize, IPC_CREAT | 0666);
	if (shmId == -1) {
            perror("QShMemViewProtocol::QShMemViewProtocol");
            qFatal("Cannot get shared memory 0x%08x", key);
        }
	data = (unsigned char *)shmat(shmId, 0, 0);
    }

    if ((long)data == -1) {
        delete kh;
        delete mh;
        perror("QShMemViewProtocol::QShMemViewProtocol");
        qFatal("Cannot attach to shared memory %d",shmId);
    }
    dataCache = (unsigned char *)malloc(displaySize);
    memset(dataCache, 0, displaySize);
    memset(data+sizeof(QVFbHeader), 0, displaySize);

    hdr = (QVFbHeader *)data;
    hdr->width = w;
    hdr->height = h;
    hdr->depth = d;
    hdr->linestep = bpl;
    hdr->dataoffset = data_offset_value;
    hdr->update = QRect();
    hdr->dirty = 0;
    hdr->numcols = 0;
    hdr->viewerVersion = QT_VERSION;
    hdr->brightness = 255;
    hdr->windowId = 0;

    displayPipe = qws_dataDir(displayid) + QString(QTE_PIPE).arg(displayid);

    displayPiped = displayPipe + 'd';


    mRefreshTimer = new QTimer(this);
    connect(mRefreshTimer, SIGNAL(timeout()), this, SLOT(flushChanges()));
}

QShMemViewProtocol::~QShMemViewProtocol()
{
    struct shmid_ds shm;
    shmdt( (char*)hdr );
    shmctl( shmId, IPC_RMID, &shm );
    free(dataCache);
    delete kh;
    delete mh;
}

int QShMemViewProtocol::width() const
{
    return hdr->width;
}

int QShMemViewProtocol::height() const
{
    return hdr->height;
}

int QShMemViewProtocol::depth() const
{
    return hdr->depth;
}

int QShMemViewProtocol::linestep() const
{
    return hdr->linestep;
}

int  QShMemViewProtocol::numcols() const
{
    return hdr->numcols;
}

QVector<QRgb> QShMemViewProtocol::clut() const
{
    QVector<QRgb> vector(hdr->numcols);
    for (int i=0; i < hdr->numcols; ++i)
        vector[i]=hdr->clut[i];

    return vector;
}

unsigned char *QShMemViewProtocol::data() const
{
    return dataCache;
    //return ((unsigned char *)hdr)+hdr->dataoffset;
}

int QShMemViewProtocol::brightness() const
{
    return hdr->brightness;
}

void QShMemViewProtocol::flushChanges()
{
    QRect r;
    if (hdr->dirty) {
        DisplayLock();

        hdr->dirty = false;
        r = hdr->update;
        hdr->update = QRect();

        if (hdr->windowId != windowId) {
            windowId = hdr->windowId;
            emit displayEmbedRequested(hdr->windowId);
        } else if (!hdr->windowId) {
            // copy the memory area, for now, be inefficient.
            memcpy(dataCache, ((char *)hdr) + hdr->dataoffset, displaySize);
        }
    }
    emit displayDataChanged(r);
}

void QShMemViewProtocol::setRate(int interval)
{
    if (interval > 0)
        return mRefreshTimer->start(1000/interval);
    else
        mRefreshTimer->stop();
}

int QShMemViewProtocol::rate() const
{
    int i = mRefreshTimer->interval();
    if (i > 0)
        return 1000/i;
    else
        return 0;
}

QT_END_NAMESPACE
