/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3mimefactory.h"

#ifndef QT_NO_MIMEFACTORY

#include "qmap.h"
#include "qmime.h"
#include "qstringlist.h"
#include "qfileinfo.h"
#include "qdir.h"
#include "q3dragobject.h"
#include "qpixmap.h"
#include "qimagereader.h"
#include "q3cleanuphandler.h"
#include "private/qtextimagehandler_p.h"

QT_BEGIN_NAMESPACE

static Q3MimeSourceFactory* defaultfactory = 0;
static Q3SingleCleanupHandler<Q3MimeSourceFactory> qmime_cleanup_factory;

class Q3MimeSourceFactoryData {
public:
    Q3MimeSourceFactoryData() :
        last(0)
    {
    }

    ~Q3MimeSourceFactoryData()
    {
        QMap<QString, QMimeSource*>::Iterator it = stored.begin();
        while (it != stored.end()) {
            delete *it;
            ++it;
        }
        delete last;
    }

    QMap<QString, QMimeSource*> stored;
    QMap<QString, QString> extensions;
    QStringList path;
    QMimeSource* last;
    QList<Q3MimeSourceFactory*> factories;
};

static QImage richTextImageLoader(const QString &name, const QString &context)
{
    QImage img;

    const QMimeSource *src = Q3MimeSourceFactory::defaultFactory()->data(name, context);
    if (src && Q3ImageDrag::decode(src, img))
        return img;

    return QImage();
}

/*!
    \class Q3MimeSourceFactory
    \brief The Q3MimeSourceFactory class is an extensible provider of mime-typed data.

    \compat

    A Q3MimeSourceFactory provides an abstract interface to a
    collection of information. Each piece of information is
    represented by a QMimeSource object which can be examined and
    converted to concrete data types by functions such as
    Q3ImageDrag::canDecode() and Q3ImageDrag::decode().

    The base Q3MimeSourceFactory can be used in two ways: as an
    abstraction of a collection of files or as specifically stored
    data. For it to access files, call setFilePath() before accessing
    data. For stored data, call setData() for each item (there are
    also convenience functions, e.g. setText(), setImage() and
    setPixmap(), that simply call setData() with appropriate
    parameters).

    The rich text widgets, QTextEdit and QTextBrowser, use
    Q3MimeSourceFactory to resolve references such as images or links
    within rich text documents. They either access the default factory
    (see \l{defaultFactory()}) or their own. Other classes that are
    capable of displaying rich text (such as QLabel, QWhatsThis or
    QMessageBox) always use the default factory.

    A factory can also be used as a container to store data associated
    with a name. This technique is useful whenever rich text contains
    images that are stored in the program itself, not loaded from the
    hard disk. Your program may, for example, define some image data
    as:
    \snippet doc/src/snippets/code/src_qt3support_other_q3mimefactory.cpp 0

    To be able to use this image within some rich text, for example
    inside a QLabel, you must create a QImage from the raw data and
    insert it into the factory with a unique name:
    \snippet doc/src/snippets/code/src_qt3support_other_q3mimefactory.cpp 1

    Now you can create a rich text QLabel with

    \snippet doc/src/snippets/code/src_qt3support_other_q3mimefactory.cpp 2

    When no longer needed, you can clear the data from the factory:

    \snippet doc/src/snippets/code/src_qt3support_other_q3mimefactory.cpp 3
*/


/*!
    Constructs a Q3MimeSourceFactory that has no file path and no
    stored content.
*/
Q3MimeSourceFactory::Q3MimeSourceFactory() :
    d(new Q3MimeSourceFactoryData)
{
    addFilePath(QLatin1String(":/qt/q3mimesourcefactory/")); //to get from the resources
    // add some reasonable defaults
    setExtensionType(QLatin1String("htm"), "text/html;charset=iso8859-1");
    setExtensionType(QLatin1String("html"), "text/html;charset=iso8859-1");
    setExtensionType(QLatin1String("txt"), "text/plain");
    setExtensionType(QLatin1String("xml"), "text/xml;charset=UTF-8");
    setExtensionType(QLatin1String("jpg"), "image/jpeg"); // support misspelled jpeg files
}

/*!
    Destroys the Q3MimeSourceFactory, deleting all stored content.
*/
Q3MimeSourceFactory::~Q3MimeSourceFactory()
{
    if (defaultFactory() == this)
        defaultfactory = 0;
    delete d;
}

QMimeSource* Q3MimeSourceFactory::dataInternal(const QString& abs_name, const QMap<QString, QString> &extensions) const
{
    QMimeSource* r = 0;
    QStringList attempted_names(abs_name);
    QFileInfo fi(abs_name);
    if (fi.isReadable()) {
        // get the right mimetype
        QString e = fi.extension(false);
        QByteArray mimetype("application/octet-stream");
        if (extensions.contains(e))
            mimetype = extensions[e].latin1();
        if (!QImageReader::imageFormat(abs_name).isEmpty())
            mimetype = "application/x-qt-image";

        QFile f(abs_name);
        if (f.open(QIODevice::ReadOnly) && f.size()) {
            QByteArray ba;
            ba.resize(f.size());
            f.readBlock(ba.data(), ba.size());
            Q3StoredDrag* sr = new Q3StoredDrag(mimetype);
            sr->setEncodedData(ba);
            delete d->last;
            d->last = r = sr;
        }
    }

    // we didn't find the mime-source, so ask the default factory for
    // the mime-source (this one will iterate over all installed ones)
    //
    // this looks dangerous, as this dataInternal() function will be
    // called again when the default factory loops over all installed
    // factories (including this), but the static bool looping in
    // data() avoids endless recursions
    if (!r && this != defaultFactory())
        r = (QMimeSource*)defaultFactory()->data(abs_name);

    return r;
}


/*!
    Returns a reference to the data associated with \a abs_name. The
    return value remains valid only until the next data() or setData()
    call, so you should immediately decode the result.

    If there is no data associated with \a abs_name in the factory's
    store, the factory tries to access the local filesystem. If \a
    abs_name isn't an absolute file name, the factory will search for
    it in all defined paths (see \l{setFilePath()}).

    The factory understands all the image formats supported by
    QImageReader. Any other mime types are determined by the file name
    extension. The default settings are
    \snippet doc/src/snippets/code/src_qt3support_other_q3mimefactory.cpp 4
    The effect of these is that file names ending in "txt" will be
    treated as text encoded in the local encoding; those ending in
    "xml" will be treated as text encoded in Unicode UTF-8 encoding.
    The text/html type is treated specially, since the encoding can be
    specified in the html file itself. "html" or "htm" will be treated
    as text encoded in the encoding specified by the html meta tag, if
    none could be found, the charset of the mime type will be used.
    The text subtype ("html", "plain", or "xml") does not affect the
    factory, but users of the factory may behave differently. We
    recommend creating "xml" files where practical. These files can be
    viewed regardless of the runtime encoding and can encode any
    Unicode characters without resorting to encoding definitions
    inside the file.

    Any file data that is not recognized will be retrieved as a
    QMimeSource providing the "application/octet-stream" mime type,
    meaning uninterpreted binary data.

    You can add further extensions or change existing ones with
    subsequent calls to setExtensionType(). If the extension mechanism
    is not sufficient for your problem domain, you can inherit
    Q3MimeSourceFactory and reimplement this function to perform some
    more specialized mime-type detection. The same applies if you want
    to use the mime source factory to access URL referenced data over
    a network.
*/
const QMimeSource *Q3MimeSourceFactory::data(const QString& abs_name) const
{
    if (d->stored.contains(abs_name))
        return d->stored[abs_name];

    const QMimeSource *r = 0;
    if (abs_name.isEmpty())
        return r;
    QStringList::Iterator it;
    if (abs_name[0] == QLatin1Char('/')
#ifdef Q_WS_WIN
            || (abs_name[0].isLetter() && abs_name[1] == QLatin1Char(':')) || abs_name.startsWith(QLatin1String("\\\\"))
#endif
   )
    {
        // handle absolute file names directly
        r = dataInternal(abs_name, d->extensions);
    }
    else { // check list of paths
        for (it = d->path.begin(); !r && it != d->path.end(); ++it) {
            QString filename = *it;
            if (filename[(int)filename.length()-1] != QLatin1Char('/'))
                filename += QLatin1Char('/');
            filename += abs_name;
            r = dataInternal(filename, d->extensions);
        }
    }

    static bool looping = false;
    if (!r && this == defaultFactory()) {
        // we found no mime-source and we are the default factory, so
        // we know all the other installed mime-source factories, so
        // ask them
        if (!looping) {
            // to avoid endless recustions, don't enter the loop below
            // if data() got called from within the loop below
            looping = true;
            for (int i = 0; i < d->factories.size(); ++i) {
                const Q3MimeSourceFactory *f = d->factories.at(i);
                if (f == this)
                    continue;
                r = static_cast<const QMimeSource *>(f->data(abs_name));
                if (r) {
                    looping = false;
                    return r;
                }
            }
            looping = false;
        }
    } else if (!r) {
        // we are not the default mime-source factory, so ask the
        // default one for the mime-source, as this one will loop over
        // all installed mime-source factories and ask these
        r = static_cast<const QMimeSource *>(defaultFactory()->data(abs_name));
    }
    return r;
}

/*!
    \fn void Q3MimeSourceFactory::setFilePath(const QStringList &path)
    \fn void Q3MimeSourceFactory::setFilePath(const QString &path)

    Sets the list of directories that will be searched when named data
    is requested to those given in the string list \a path.

    \sa filePath()
*/
void Q3MimeSourceFactory::setFilePath(const QStringList& path)
{
    d->path = path;
}

/*!
    Returns the currently set search paths.
*/
QStringList Q3MimeSourceFactory::filePath() const
{
    return d->path;
}

/*!
    Adds another search path, \a p to the existing search paths.

    \sa setFilePath()
*/
void Q3MimeSourceFactory::addFilePath(const QString& p)
{
    d->path += p;
}

/*!
    Sets the mime-type to be associated with the file name extension,
    \a ext to \a mimetype. This determines the mime-type for files
    found via the paths set by setFilePath().
*/
void Q3MimeSourceFactory::setExtensionType(const QString& ext, const char* mimetype)
{
    d->extensions.insert(ext, QLatin1String(mimetype));
}

/*!
    Converts the absolute or relative data item name \a
    abs_or_rel_name to an absolute name, interpreted within the
    context (path) of the data item named \a context (this must be an
    absolute name).
*/
QString Q3MimeSourceFactory::makeAbsolute(const QString& abs_or_rel_name, const QString& context) const
{
    if (context.isNull() ||
         !(context[0] == QLatin1Char('/')
#ifdef Q_WS_WIN
         || (context[0].isLetter() && context[1] == QLatin1Char(':'))
#endif
          ))
        return abs_or_rel_name;
    if (abs_or_rel_name.isEmpty())
        return context;
    QFileInfo c(context);
    if (!c.isDir()) {
        QFileInfo r(c.dir(true), abs_or_rel_name);
        return r.absFilePath();
    } else {
        QDir d(context);
        QFileInfo r(d, abs_or_rel_name);
        return r.absFilePath();
    }
}

/*!
    \overload
    A convenience function. See data(const QString& abs_name). The
    file name is given in \a abs_or_rel_name and the path is in \a
    context.
*/
const QMimeSource* Q3MimeSourceFactory::data(const QString& abs_or_rel_name, const QString& context) const
{
    const QMimeSource* r = data(makeAbsolute(abs_or_rel_name,context));
    if (!r && !d->path.isEmpty())
        r = data(abs_or_rel_name);
    return r;
}


/*!
    Sets \a text to be the data item associated with the absolute name
    \a abs_name.

    Equivalent to setData(abs_name, new Q3TextDrag(text)).
*/
void Q3MimeSourceFactory::setText(const QString& abs_name, const QString& text)
{
    setData(abs_name, new Q3TextDrag(text));
}

/*!
    Sets \a image to be the data item associated with the absolute
    name \a abs_name.

    Equivalent to setData(abs_name, new Q3ImageDrag(image)).
*/
void Q3MimeSourceFactory::setImage(const QString& abs_name, const QImage& image)
{
    setData(abs_name, new Q3ImageDrag(image));
}

/*!
    Sets \a pixmap to be the data item associated with the absolute
    name \a abs_name.
*/
void Q3MimeSourceFactory::setPixmap(const QString& abs_name, const QPixmap& pixmap)
{
    setData(abs_name, new Q3ImageDrag(pixmap.convertToImage()));
}

/*!
  Sets \a data to be the data item associated with
  the absolute name \a abs_name. Note that the ownership of \a data is
  transferred to the factory: do not delete or access the pointer after
  passing it to this function.

  Passing 0 for data removes previously stored data.
*/
void Q3MimeSourceFactory::setData(const QString& abs_name, QMimeSource* data)
{
    if (d->stored.contains(abs_name))
        delete d->stored[abs_name];
    d->stored.insert(abs_name,data);
}


/*!
    Returns the application-wide default mime source factory. This
    factory is used by rich text rendering classes such as
    QSimpleRichText, QWhatsThis and QMessageBox to resolve named
    references within rich text documents. It serves also as the
    initial factory for the more complex render widgets, QTextEdit and
    QTextBrowser.

    \sa setDefaultFactory()
*/
Q3MimeSourceFactory* Q3MimeSourceFactory::defaultFactory()
{
    if (!defaultfactory)
    {
        defaultfactory = new Q3MimeSourceFactory();
        qmime_cleanup_factory.set(&defaultfactory);
        QTextImageHandler::externalLoader = richTextImageLoader;
    }
    return defaultfactory;
}

/*!
    Sets the default \a factory, destroying any previously set mime
    source provider. The ownership of the factory is transferred to
    Qt.

    \sa defaultFactory()
*/
void Q3MimeSourceFactory::setDefaultFactory(Q3MimeSourceFactory* factory)
{
    if (!defaultfactory)
        qmime_cleanup_factory.set(&defaultfactory);
    else if (defaultfactory != factory)
        delete defaultfactory;
    defaultfactory = factory;
}

/*!
    Sets the defaultFactory() to 0 and returns the previous one.
*/

Q3MimeSourceFactory* Q3MimeSourceFactory::takeDefaultFactory()
{
    Q3MimeSourceFactory *f = defaultfactory;
    defaultfactory = 0;
    return f;
}

/*!
    Adds the Q3MimeSourceFactory \a f to the list of available
    mimesource factories. If the defaultFactory() can't resolve a
    data() it iterates over the list of installed mimesource factories
    until the data can be resolved.

    \sa removeFactory()
*/

void Q3MimeSourceFactory::addFactory(Q3MimeSourceFactory *f)
{
    Q3MimeSourceFactory::defaultFactory()->d->factories.append(f);
}

/*!
    Removes the mimesource factory \a f from the list of available
    mimesource factories.

    \sa addFactory()
*/

void Q3MimeSourceFactory::removeFactory(Q3MimeSourceFactory *f)
{
    Q3MimeSourceFactory::defaultFactory()->d->factories.removeAll(f);
}

QPixmap qPixmapFromMimeSource(const QString &abs_name)
{
    const QMimeSource *m = Q3MimeSourceFactory::defaultFactory()->data(abs_name);
    if (!m) {
        if (QFile::exists(abs_name))
            return QPixmap(abs_name);
        if (!abs_name.isEmpty())
            qWarning("QPixmap::fromMimeSource: Cannot find pixmap \"%s\" in the mime source factory",
                      abs_name.latin1());
        return QPixmap();
    }
    QPixmap pix;
    Q3ImageDrag::decode(m, pix);
    return pix;
}

QImage qImageFromMimeSource(const QString &abs_name)
{
    const QMimeSource *m = Q3MimeSourceFactory::defaultFactory()->data(abs_name);
    if (!m) {
        qWarning("QImage::fromMimeSource: Cannot find image \"%s\" in the mime source factory", abs_name.latin1());
        return QImage();
    }
    QImage img;
    Q3ImageDrag::decode(m, img);
    return img;
}

QT_END_NAMESPACE

#endif // QT_NO_MIMEFACTORY
