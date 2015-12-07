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

#include "qpixmapcache.h"
#include "qcache.h"
#include "qobject.h"
#include "qdebug.h"

#ifdef Q_OS_WIN
#include "qpaintengine.h"
#include <private/qimage_p.h>
#include <private/qpixmap_raster_p.h>
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QPixmapCache

    \brief The QPixmapCache class provides an application-wide cache for pixmaps.

    \ingroup environment
    \ingroup multimedia

    This class is a tool for optimized drawing with QPixmap. You can
    use it to store temporary pixmaps that are expensive to generate
    without using more storage space than cacheLimit(). Use insert()
    to insert pixmaps, find() to find them, and clear() to empty the
    cache.

    QPixmapCache contains no member data, only static functions to
    access the global pixmap cache. It creates an internal QCache
    object for caching the pixmaps.

    The cache associates a pixmap with a string (key). If two pixmaps
    are inserted into the cache using equal keys, then the last pixmap
    will hide the first pixmap. The QHash and QCache classes do
    exactly the same.

    The cache becomes full when the total size of all pixmaps in the
    cache exceeds cacheLimit(). The initial cache limit is 1024 KB (1
    MB); it is changed with setCacheLimit(). A pixmap takes roughly
    (\e{width} * \e{height} * \e{depth})/8 bytes of memory.

    The \e{Qt Quarterly} article
    \l{http://doc.trolltech.com/qq/qq12-qpixmapcache.html}{Optimizing
    with QPixmapCache} explains how to use QPixmapCache to speed up
    applications by caching the results of painting.

    \sa QCache, QPixmap
*/

#if defined(Q_WS_QWS) || defined(Q_OS_WINCE)
static int cache_limit = 2048; // 2048 KB cache limit for embedded
#else
static int cache_limit = 10240; // 10 MB cache limit for desktop
#endif

// XXX: hw: is this a general concept we need to abstract?
class QDetachedPixmap : public QPixmap
{
public:
    QDetachedPixmap(const QPixmap &pix) : QPixmap(pix)
    {
#ifdef Q_OS_WIN
        QRasterPixmapData *d = static_cast<QRasterPixmapData*>(data);
        if (!d->image.isNull() && d->image.d->paintEngine
            && !d->image.d->paintEngine->isActive())
        {
            delete d->image.d->paintEngine;
            d->image.d->paintEngine = 0;
        }
#endif
    }
};

class QPMCache : public QObject, public QCache<qint64, QDetachedPixmap>
{
    Q_OBJECT
public:
    QPMCache()
        : QObject(0),
          QCache<qint64, QDetachedPixmap>(cache_limit * 1024),
          id(0), ps(0), t(false) { }
    ~QPMCache() { }

    void timerEvent(QTimerEvent *);
    bool insert(const QString& key, const QPixmap &pixmap, int cost);
    bool remove(const QString &key);

    QPixmap *object(const QString &key) const;

private:
    QHash<QString, qint64> cacheKeys;
    int id;
    int ps;
    bool t;
};
QT_BEGIN_INCLUDE_NAMESPACE
#include "qpixmapcache.moc"
QT_END_INCLUDE_NAMESPACE

/*
  This is supposed to cut the cache size down by about 80-90% in a
  minute once the application becomes idle, to let any inserted pixmap
  remain in the cache for some time before it becomes a candidate for
  cleaning-up, and to not cut down the size of the cache while the
  cache is in active use.

  When the last pixmap has been deleted from the cache, kill the
  timer so Qt won't keep the CPU from going into sleep mode.
*/

void QPMCache::timerEvent(QTimerEvent *)
{
    int mc = maxCost();
    bool nt = totalCost() == ps;
    setMaxCost(nt ? totalCost() * 3 / 4 : totalCost() -1);
    setMaxCost(mc);
    ps = totalCost();

    QHash<QString, qint64>::iterator it = cacheKeys.begin();
    while (it != cacheKeys.end()) {
        if (!contains(it.value())) {
            it = cacheKeys.erase(it);
        } else {
            ++it;
        }
    }

    if (!size()) {
        killTimer(id);
        id = 0;
    } else if (nt != t) {
        killTimer(id);
        id = startTimer(nt ? 10000 : 30000);
        t = nt;
    }
}

QPixmap *QPMCache::object(const QString &key) const
{
    return QCache<qint64, QDetachedPixmap>::object(cacheKeys.value(key, -1));
}


bool QPMCache::insert(const QString& key, const QPixmap &pixmap, int cost)
{
    qint64 cacheKey = pixmap.cacheKey();
    if (QCache<qint64, QDetachedPixmap>::object(cacheKey)) {
        cacheKeys.insert(key, cacheKey);
        return true;
    }
    bool success = QCache<qint64, QDetachedPixmap>::insert(cacheKey, new QDetachedPixmap(pixmap), cost);
    if (success) {
        cacheKeys.insert(key, cacheKey);
        if (!id) {
            id = startTimer(30000);
            t = false;
        }
    }
    return success;
}

bool QPMCache::remove(const QString &key)
{
    qint64 cacheKey = cacheKeys.value(key, -1);
    cacheKeys.remove(key);
    return QCache<qint64, QDetachedPixmap>::remove(cacheKey);
}

Q_GLOBAL_STATIC(QPMCache, pm_cache)

/*!
  \obsolete
    \overload

    Returns the pixmap associated with the \a key in the cache, or
    null if there is no such pixmap.

    \warning If valid, you should copy the pixmap immediately (this is
    fast). Subsequent insertions into the cache could cause the
    pointer to become invalid. For this reason, we recommend you use
    find(const QString&, QPixmap&) instead.

    Example:
    \snippet doc/src/snippets/code/src.gui.image.qpixmapcache.cpp 0
*/

QPixmap *QPixmapCache::find(const QString &key)
{
    return pm_cache()->object(key);
}


/*!
    Looks for a cached pixmap associated with the \a key in the cache.
    If the pixmap is found, the function sets \a pm to that pixmap and
    returns true; otherwise it leaves \a pm alone and returns false.

    Example:
    \snippet doc/src/snippets/code/src.gui.image.qpixmapcache.cpp 1
*/

bool QPixmapCache::find(const QString &key, QPixmap& pm)
{
    QPixmap *ptr = pm_cache()->object(key);
    if (ptr)
        pm = *ptr;
    return ptr != 0;
}


/*!
    Inserts a copy of the pixmap \a pm associated with the \a key into
    the cache.

    All pixmaps inserted by the Qt library have a key starting with
    "$qt", so your own pixmap keys should never begin "$qt".

    When a pixmap is inserted and the cache is about to exceed its
    limit, it removes pixmaps until there is enough room for the
    pixmap to be inserted.

    The oldest pixmaps (least recently accessed in the cache) are
    deleted when more space is needed.

    The function returns true if the object was inserted into the
    cache; otherwise it returns false.

    \sa setCacheLimit()
*/

bool QPixmapCache::insert(const QString &key, const QPixmap &pm)
{
    return pm_cache()->insert(key, pm, pm.width() * pm.height() * pm.depth() / 8);
}

/*!
    Returns the cache limit (in kilobytes).

    The default setting is 1024 kilobytes.

    \sa setCacheLimit()
*/

int QPixmapCache::cacheLimit()
{
    return cache_limit;
}

/*!
    Sets the cache limit to \a n kilobytes.

    The default setting is 1024 kilobytes.

    \sa cacheLimit()
*/

void QPixmapCache::setCacheLimit(int n)
{
    cache_limit = n;
    pm_cache()->setMaxCost(1024 * cache_limit);
}

/*!
  Removes the pixmap associated with \a key from the cache.
*/
void QPixmapCache::remove(const QString &key)
{
    pm_cache()->remove(key);
}


/*!
    Removes all pixmaps from the cache.
*/

void QPixmapCache::clear()
{
    pm_cache()->clear();
}

QT_END_NAMESPACE
