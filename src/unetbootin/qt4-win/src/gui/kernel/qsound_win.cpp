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

#include "qsound.h"

#ifndef QT_NO_SOUND

#include "qapplication.h"
#include "qapplication_p.h"
#include <qfile.h>
#include "qpointer.h"
#include "qsound_p.h"

#include <qt_windows.h>

QT_BEGIN_NAMESPACE

class QAuServerWindows : public QAuServer {
    Q_OBJECT

public:
    QAuServerWindows(QObject* parent);
    ~QAuServerWindows();

    void playHelper(const QString &filename, int loop, QSound *snd);
    void play(const QString& filename, int loop);
    void play(QSound*);

    void stop(QSound*);
    bool okay();

    int decLoop(QSound *snd) { return QAuServer::decLoop(snd); }

    HANDLE current;
    HANDLE mutex;
    HANDLE event;
};

QAuServerWindows::QAuServerWindows(QObject* parent) :
    QAuServer(parent), current(0)
{
    QT_WA({
        mutex = CreateMutexW(0, 0, 0);
        event = CreateEventW(0, FALSE, FALSE, 0);
    } , {
        mutex = CreateMutexA(0, 0, 0);
        event = CreateEventA(0, FALSE, FALSE, 0);
    });
}

QAuServerWindows::~QAuServerWindows()
{
    HANDLE mtx = mutex;
    WaitForSingleObject(mtx, INFINITE);
    mutex = 0;

    ReleaseMutex(mtx);
    CloseHandle(mtx);
    CloseHandle(event);
}

struct SoundInfo
{
    SoundInfo(const QString &fn, int lp, QSound *snd, QAuServerWindows *srv)
        : sound(snd), server(srv), filename(fn), loops(lp)
    {
    }

    QSound *sound;
    QAuServerWindows *server;
    QString filename;
    int loops;
};

DWORD WINAPI SoundPlayProc(LPVOID param)
{
    SoundInfo *info = (SoundInfo*)param;

    // copy data before waking up GUI thread
    QAuServerWindows *server = info->server;
    QSound *sound = info->sound;
    int loops = info->loops;
    QString filename = info->filename;
    HANDLE mutex = server->mutex;
    HANDLE event = server->event;
    info = 0;

    // server must not be destroyed until thread finishes
    // and all other sounds have to wait
    WaitForSingleObject(mutex, INFINITE);

    if (loops <= 1) {
        server->current = 0;
        int flags = SND_FILENAME|SND_ASYNC;
        if (loops == -1)
            flags |= SND_LOOP;

        QT_WA({
            PlaySoundW((TCHAR*)filename.utf16(), 0, flags);
        } , {
            PlaySoundA(QFile::encodeName(filename).data(), 0, flags);
        });
	if (sound && loops == 1)
	    server->decLoop(sound);

        // GUI thread continues, but we are done as well.
        SetEvent(event);
    } else {
        // signal GUI thread to continue - sound might be reset!
        QPointer<QSound> guarded_sound = sound;
        SetEvent(event);

	for (int l = 0; l < loops && server->current; ++l) {
	    QT_WA( {
		PlaySoundW( (TCHAR*)filename.utf16(), 0, SND_FILENAME|SND_SYNC );
	    } , {
		PlaySoundA( QFile::encodeName(filename).data(), 0,
		    SND_FILENAME|SND_SYNC );
	    } );

	    if (guarded_sound)
		server->decLoop(guarded_sound);
	}
	server->current = 0;
    }
    ReleaseMutex(mutex);

    return 0;
}

void QAuServerWindows::playHelper(const QString &filename, int loop, QSound *snd)
{
    if (loop == 0)
	return;
    // busy?
    if (WaitForSingleObject(mutex, 0) == WAIT_TIMEOUT)
        return;
    ReleaseMutex(mutex);

    DWORD threadid = 0;
    SoundInfo info(filename, loop, snd, this);
    current = CreateThread(0, 0, SoundPlayProc, &info, 0, &threadid);
    CloseHandle(current);

    WaitForSingleObject(event, INFINITE);
}

void QAuServerWindows::play(const QString& filename, int loop)
{
    playHelper(filename, loop, 0);
}

void QAuServerWindows::play(QSound* s)
{
    playHelper(s->fileName(), s->loops(), s);
}

void QAuServerWindows::stop(QSound*)
{
    // stop unlooped sound
    if (!current)
        PlaySound(0, 0, 0);
    // stop after loop is done
    current = 0;
}

bool QAuServerWindows::okay()
{
    return true;
}

QAuServer* qt_new_audio_server()
{
    return new QAuServerWindows(qApp);
}

QT_END_NAMESPACE

#include "qsound_win.moc"

#endif // QT_NO_SOUND
