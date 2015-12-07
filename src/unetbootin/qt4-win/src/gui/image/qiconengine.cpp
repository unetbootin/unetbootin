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

#include "qiconengine.h"
#include "qpainter.h"

QT_BEGIN_NAMESPACE

/*!
  \class QIconEngine

  \brief The QIconEngine class provides an abstract base class for QIcon renderers.

  \ingroup multimedia

  \bold {Use QIconEngineV2 instead.}

  An icon engine provides the rendering functions for a QIcon. Each icon has a
  corresponding icon engine that is responsible for drawing the icon with a
  requested size, mode and state.

  The icon is rendered by the paint() function, and the icon can additionally be
  obtained as a pixmap with the pixmap() function (the default implementation
  simply uses paint() to achieve this). The addPixmap() function can be used to
  add new pixmaps to the icon engine, and is used by QIcon to add specialized
  custom pixmaps.

  The paint(), pixmap(), and addPixmap() functions are all virtual, and can
  therefore be reimplemented in subclasses of QIconEngine.

  \sa QIconEngineV2, QIconEnginePlugin

*/

/*!
  \fn virtual void QIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) = 0;

  Uses the given \a painter to paint the icon with the required \a mode and
  \a state into the rectangle \a rect.
*/

/*!  Returns the actual size of the icon the engine provides for the
  requested \a size, \a mode and \a state. The default implementation
  returns the given \a size.
 */
QSize QIconEngine::actualSize(const QSize &size, QIcon::Mode /*mode*/, QIcon::State /*state*/)
{
    return size;
}


/*!
  Destroys the icon engine.
 */
QIconEngine::~QIconEngine()
{
}


/*!
  Returns the icon as a pixmap with the required \a size, \a mode,
  and \a state. The default implementation creates a new pixmap and
  calls paint() to fill it.
*/
QPixmap QIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QPixmap pm(size);
    {
        QPainter p(&pm);
        paint(&p, QRect(QPoint(0,0),size), mode, state);
    }
    return pm;
}

/*!
  Called by QIcon::addPixmap(). Adds a specialized \a pixmap for the given
  \a mode and \a state. The default pixmap-based engine stores any supplied
  pixmaps, and it uses them instead of scaled pixmaps if the size of a pixmap
  matches the size of icon requested. Custom icon engines that implement
  scalable vector formats are free to ignores any extra pixmaps.
 */
void QIconEngine::addPixmap(const QPixmap &/*pixmap*/, QIcon::Mode /*mode*/, QIcon::State /*state*/)
{
}


/*!  Called by QIcon::addFile(). Adds a specialized pixmap from the
  file with the given \a fileName, \a size, \a mode and \a state. The
  default pixmap-based engine stores any supplied file names, and it
  loads the pixmaps on demand instead of using scaled pixmaps if the
  size of a pixmap matches the size of icon requested. Custom icon
  engines that implement scalable vector formats are free to ignores
  any extra files.
 */
void QIconEngine::addFile(const QString &/*fileName*/, const QSize &/*size*/, QIcon::Mode /*mode*/, QIcon::State /*state*/)
{
}



// version 2 functions


/*!
    \class QIconEngineV2

    \brief The QIconEngineV2 class provides an abstract base class for QIcon renderers.

    \ingroup multimedia
    \since 4.3

    An icon engine renders \l{QIcon}s. With icon engines, you can
    customize icons. Qt provides a default engine that makes icons
    adhere to the current style by scaling the icons and providing a
    disabled appearance.

    An engine is installed on an icon either through a QIcon
    constructor or through a QIconEnginePluginV2. The plugins are used
    by Qt if a specific engine is not given when the icon is created.
    See the QIconEngineV2 class description to learn how to create
    icon engine plugins.

    An icon engine provides the rendering functions for a QIcon. Each
    icon has a corresponding icon engine that is responsible for drawing
    the icon with a requested size, mode and state.

    QIconEngineV2 extends the API of QIconEngine to allow streaming of
    the icon engine contents, and should be used instead of QIconEngine
    for implementing new icon engines.

    \sa QIconEnginePluginV2

*/

/*!
    Returns a key that identifies this icon engine.
 */
QString QIconEngineV2::key() const
{
    return QString();
}

/*!
    Returns a clone of this icon engine.
 */
QIconEngineV2 *QIconEngineV2::clone() const
{
    return 0;
}

/*!
    Reads icon engine contents from the QDataStream \a in. Returns
    true if the contents were read; otherwise returns false.

    QIconEngineV2's default implementation always return false.
 */
bool QIconEngineV2::read(QDataStream &)
{
    return false;
}

/*!
    Writes the contents of this engine to the QDataStream \a out.
    Returns true if the contents were written; otherwise returns false.

    QIconEngineV2's default implementation always return false.
 */
bool QIconEngineV2::write(QDataStream &) const
{
    return false;
}

/*! \internal
*/
void QIconEngineV2::virtual_hook(int, void *)
{
}

QT_END_NAMESPACE
