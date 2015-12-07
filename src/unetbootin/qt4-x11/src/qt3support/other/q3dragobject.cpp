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

#include "qplatformdefs.h"

#ifndef QT_NO_MIME

#include "q3dragobject.h"
#include "qpixmap.h"
#include "qevent.h"
#include "qfile.h"
#include "qtextcodec.h"
#include "qapplication.h"
#include "qpoint.h"
#include "qwidget.h"
#include "qbuffer.h"
#include "qimagereader.h"
#include "qimagewriter.h"
#include "qimage.h"
#include "qregexp.h"
#include "qdir.h"
#include "qdrag.h"
#include "q3strlist.h"
#include "q3cstring.h"

#include <private/qobject_p.h>

#include <ctype.h>
#if defined(Q_OS_WINCE)
#include <winsock.h>
#include "qfunctions_wince.h"
#endif

QT_BEGIN_NAMESPACE

static QWidget *last_target = 0;

class QDragMime;

class Q3DragObjectPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(Q3DragObject)
public:
    Q3DragObjectPrivate(): hot(0,0),pm_cursor(0) {}
    QPixmap pixmap;
    QPoint hot;
    // store default cursors
    QPixmap *pm_cursor;
};

class Q3TextDragPrivate : public Q3DragObjectPrivate
{
    Q_DECLARE_PUBLIC(Q3TextDrag)
public:
    Q3TextDragPrivate() { setSubType(QLatin1String("plain")); }
    void setSubType(const QString & st) {
        subtype = st;
        fmt = QString(QLatin1String("text/")).toLatin1() + subtype.toLatin1();
    }

    QString txt;
    QString subtype;
    QByteArray fmt;
};

class Q3StoredDragPrivate : public Q3DragObjectPrivate
{
    Q_DECLARE_PUBLIC(Q3StoredDrag)
public:
    Q3StoredDragPrivate() {}
    const char* fmt;
    QByteArray enc;
};

class Q3ImageDragPrivate : public Q3DragObjectPrivate
{
    Q_DECLARE_PUBLIC(Q3ImageDrag)
public:
    QImage img;
    QList<QByteArray> ofmts;
};

class QDragMime : public QMimeData
{
public:
    QDragMime(Q3DragObject *parent) : QMimeData(), dragObject(parent) { }
    ~QDragMime();

    QByteArray data(const QString &mimetype) const;
    bool hasFormat(const QString &mimetype) const;
    QStringList formats() const;

    QPointer<Q3DragObject> dragObject;
};

QDragMime::~QDragMime()
{
    delete dragObject;
}
QByteArray QDragMime::data(const QString &mimetype) const
{
    return dragObject->encodedData(mimetype.latin1());
}

bool QDragMime::hasFormat(const QString &mimetype) const
{
    return dragObject->provides(mimetype.latin1());
}

QStringList QDragMime::formats() const
{
    int i = 0;
    const char *format;
    QStringList f;
    while ((format = dragObject->format(i))) {
        f.append(QLatin1String(format));
        ++i;
    }
    return f;
}

/*!
    Constructs a drag object called \a name with a parent \a
    dragSource.

    Note that the drag object will be deleted when the \a dragSource is
    deleted.
*/

Q3DragObject::Q3DragObject(QWidget * dragSource, const char * name)
    : QObject(*(new Q3DragObjectPrivate), dragSource)
{
    setObjectName(QLatin1String(name));
}

/*! \internal */
Q3DragObject::Q3DragObject(Q3DragObjectPrivate &dd, QWidget *dragSource)
    : QObject(dd, dragSource)
{
}

/*!
    Destroys the drag object, canceling any drag and drop operation in
    which it is involved.
*/

Q3DragObject::~Q3DragObject()
{
}

#ifndef QT_NO_DRAGANDDROP
/*!
    Set the pixmap, \a pm, to display while dragging the object. The
    platform-specific implementation will use this where it can - so
    provide a small masked pixmap, and do not assume that the user
    will actually see it. For example, cursors on Windows 95 are of
    limited size.

    The \a hotspot is the point on (or off) the pixmap that should be
    under the cursor as it is dragged. It is relative to the top-left
    pixel of the pixmap.

    \warning We have seen problems with drag cursors on different
    graphics hardware and driver software on Windows. Setting the
    graphics acceleration in the display settings down one tick solved
    the problems in all cases.
*/
void Q3DragObject::setPixmap(QPixmap pm, const QPoint& hotspot)
{
    Q_D(Q3DragObject);
    d->pixmap = pm;
    d->hot = hotspot;
#if 0
    QDragManager *manager = QDragManager::self();
    if (manager && manager->object == d->data)
        manager->updatePixmap();
#endif
}

/*!
    \overload

    Uses a hotspot that positions the pixmap below and to the right of
    the mouse pointer. This allows the user to clearly see the point
    on the window where they are dragging the data.
*/
void Q3DragObject::setPixmap(QPixmap pm)
{
    setPixmap(pm,QPoint(-10, -10));
}

/*!
    Returns the currently set pixmap, or a null pixmap if none is set.

    \sa QPixmap::isNull()
*/
QPixmap Q3DragObject::pixmap() const
{
    return d_func()->pixmap;
}

/*!
    Returns the currently set pixmap hotspot.

    \sa setPixmap()
*/
QPoint Q3DragObject::pixmapHotSpot() const
{
    return d_func()->hot;
}

/*!
    Starts a drag operation using the contents of this object, using
    DragDefault mode.

    The function returns true if the caller should delete the original
    copy of the dragged data (but see target()); otherwise returns
    false.

    If the drag contains \e references to information (e.g. file names
    in a Q3UriDrag are references) then the return value should always
    be ignored, as the target is expected to directly manipulate the
    content referred to by the drag object. On X11 the return value should
    always be correct anyway, but on Windows this is not necessarily
    the case; e.g. the file manager starts a background process to
    move files, so the source \e{must not} delete the files!

    Note that on Windows the drag operation will start a blocking modal
    event loop that will not dispatch any QTimers.
*/
bool Q3DragObject::drag()
{
    return drag(DragDefault);
}

/*!
    After the drag completes, this function will return the QWidget
   which received the drop, or 0 if the data was dropped on another
    application.

    This can be useful for detecting the case where drag and drop is
    to and from the same widget.
*/
QWidget *Q3DragObject::target()
{
    return last_target;
}

/*!
    Starts a drag operation using the contents of this object, using
    \c DragMove mode. Be sure to read the constraints described in
    drag().

    Returns true if the data was dragged as a \e move, indicating that
    the caller should remove the original source of the data (the drag
    object must continue to have a copy); otherwise returns false.

    \sa drag() dragCopy() dragLink()
*/
bool Q3DragObject::dragMove()
{
    return drag(DragMove);
}


/*!
    Starts a drag operation using the contents of this object, using
    \c DragCopy mode. Be sure to read the constraints described in
    drag().

    \sa drag() dragMove() dragLink()
*/
void Q3DragObject::dragCopy()
{
    (void)drag(DragCopy);
}

/*!
    Starts a drag operation using the contents of this object, using
    \c DragLink mode. Be sure to read the constraints described in
    drag().

    \sa drag() dragCopy() dragMove()
*/
void Q3DragObject::dragLink()
{
    (void)drag(DragLink);
}


/*!
    \enum Q3DragObject::DragMode

    This enum describes the possible drag modes.

    \value DragDefault     The mode is determined heuristically.
    \value DragCopy        The data is copied.
    \value DragMove        The data is moved.
    \value DragLink        The data is linked.
    \value DragCopyOrMove  The user chooses the mode by using the
                           \key{Shift} key to switch from the default
                           copy mode to move mode.
*/


/*!
    \overload
    Starts a drag operation using the contents of this object.

    At this point, the object becomes owned by Qt, not the
    application. You should not delete the drag object or anything it
    references. The actual transfer of data to the target application
    will be done during future event processing - after that time the
    drag object will be deleted.

    Returns true if the dragged data was dragged as a \e move,
    indicating that the caller should remove the original source of
    the data (the drag object must continue to have a copy); otherwise
    returns false.

    The \a mode specifies the drag mode (see
    \l{Q3DragObject::DragMode}.) Normally one of the simpler drag(),
    dragMove(), or dragCopy() functions would be used instead.
*/
bool Q3DragObject::drag(DragMode mode)
{
    Q_D(Q3DragObject);
    QDragMime *data = new QDragMime(this);
    int i = 0;
    const char *fmt;
    while ((fmt = format(i))) {
        data->setData(QLatin1String(fmt), encodedData(fmt));
        ++i;
    }

    QDrag *drag = new QDrag(qobject_cast<QWidget *>(parent()));
    drag->setMimeData(data);
    drag->setPixmap(d->pixmap);
    drag->setHotSpot(d->hot);

    Qt::DropActions allowedOps;
    Qt::DropAction defaultOp = Qt::IgnoreAction;
    switch(mode) {
    default:
    case DragDefault:
    case DragCopyOrMove:
        allowedOps = Qt::CopyAction|Qt::MoveAction;
        defaultOp = Qt::IgnoreAction;
        break;
    case DragCopy:
        allowedOps = Qt::CopyAction;
        defaultOp = Qt::CopyAction;
        break;
    case DragMove:
        allowedOps = Qt::MoveAction;
        defaultOp = Qt::MoveAction;
        break;
    case DragLink:
        allowedOps = Qt::LinkAction;
        defaultOp = Qt::LinkAction;
        break;
    }
    bool retval = (drag->exec(allowedOps, defaultOp) == Qt::MoveAction);
    last_target = drag->target();

    return retval;
}

#endif


/*!
    Returns a pointer to the widget where this object originated (the drag
    source).
*/

QWidget * Q3DragObject::source()
{
    if (parent() && parent()->isWidgetType())
        return (QWidget *)parent();
    else
        return 0;
}


/*!
    \class Q3DragObject qdragobject.h

    \brief The Q3DragObject class encapsulates MIME-based data
    transfer.

    \compat

    Q3DragObject is the base class for all data that needs to be
    transferred between and within applications, both for drag and
    drop and for the clipboard.

    See the \link dnd.html Drag and drop documentation\endlink for an
    overview of how to provide drag and drop in your application.

    See the QClipboard documentation for an overview of how to provide
    cut and paste in your application.

    The drag() function is used to start a drag operation. You can
    specify the \l DragMode in the call or use one of the convenience
    functions dragCopy(), dragMove(), or dragLink(). The drag source
    where the data originated is retrieved with source(). If the data
    was dropped on a widget within the application, target() will
    return a pointer to that widget. Specify the pixmap to display
    during the drag with setPixmap().
*/

static
void stripws(QByteArray& s)
{
    int f;
    while ((f = s.indexOf(' ')) >= 0)
        s.remove(f,1);
}

/*!
    \class Q3TextDrag qdragobject.h

    \brief The Q3TextDrag class is a drag and drop object for
    transferring plain and Unicode text.

    \compat

    Plain text is passed in a QString which may contain multiple lines
    (i.e. may contain newline characters). The drag target will receive
    the newlines according to the runtime environment, e.g. LF on Unix,
    and CRLF on Windows.

    Qt provides no built-in mechanism for delivering only a single-line.

    For more information about drag and drop, see the Q3DragObject class
    and the \link dnd.html drag and drop documentation\endlink.
*/


/*!
    Constructs a text drag object with the given \a name, and sets its data
    to \a text. The \a dragSource is the widget that the drag operation started
    from.
*/

Q3TextDrag::Q3TextDrag(const QString &text, QWidget * dragSource, const char * name)
    : Q3DragObject(*new Q3TextDragPrivate, dragSource)
{
    setObjectName(QLatin1String(name));
    setText(text);
}


/*!
    Constructs a default text drag object with the given \a name.
    The \a dragSource is the widget that the drag operation started from.
*/

Q3TextDrag::Q3TextDrag(QWidget * dragSource, const char * name)
    : Q3DragObject(*(new Q3TextDragPrivate), dragSource)
{
    setObjectName(QLatin1String(name));
}

/*! \internal */
Q3TextDrag::Q3TextDrag(Q3TextDragPrivate &dd, QWidget *dragSource)
    : Q3DragObject(dd, dragSource)
{

}

/*!
    Destroys the text drag object.
*/
Q3TextDrag::~Q3TextDrag()
{

}

/*!
    \fn void Q3TextDrag::setSubtype(const QString &subtype)

    Sets the MIME \a subtype of the text being dragged. The default subtype
    is "plain", so the default MIME type of the text is "text/plain".
    You might use this to declare that the text is "text/html" by calling
    setSubtype("html").
*/
void Q3TextDrag::setSubtype(const QString & st)
{
    d_func()->setSubType(st);
}

/*!
    Sets the \a text to be dragged. You will need to call this if you did
    not pass the text during construction.
*/
void Q3TextDrag::setText(const QString &text)
{
    d_func()->txt = text;
}


/*!
    \reimp
*/
const char * Q3TextDrag::format(int i) const
{
    if (i > 0)
        return 0;
    return d_func()->fmt.constData();
}

QTextCodec* qt_findcharset(const QByteArray& mimetype)
{
    int i=mimetype.indexOf("charset=");
    if (i >= 0) {
        QByteArray cs = mimetype.mid(i+8);
        stripws(cs);
        i = cs.indexOf(';');
        if (i >= 0)
            cs = cs.left(i);
        // win98 often has charset=utf16, and we need to get the correct codec for
        // it to be able to get Unicode text drops.
        if (cs == "utf16")
            cs = "ISO-10646-UCS-2";
        // May return 0 if unknown charset
        return QTextCodec::codecForName(cs);
    }
    // no charset=, use locale
    return QTextCodec::codecForName("utf-8");
}

static QTextCodec *codecForHTML(const QByteArray &ba)
{
    // determine charset
    int mib = 0;
    int pos;
    QTextCodec *c = 0;

    if (ba.size() > 1 && (((uchar)ba[0] == 0xfe && (uchar)ba[1] == 0xff)
                          || ((uchar)ba[0] == 0xff && (uchar)ba[1] == 0xfe))) {
        mib = 1015; // utf16
    } else if (ba.size() > 2
               && (uchar)ba[0] == 0xef
               && (uchar)ba[1] == 0xbb
               && (uchar)ba[2] == 0xbf) {
        mib = 106; // utf-8
    } else {
        pos = 0;
        while ((pos = ba.indexOf('<', pos)) != -1) {
            int end = ba.indexOf('>', pos+1);
            if (end == -1)
                break;
            const QString str(QString::fromLatin1(ba.mid(pos, end-pos)));
            if (str.contains(QLatin1String("meta http-equiv="), Qt::CaseInsensitive)) {
                pos = str.indexOf(QLatin1String("charset="), 0, Qt::CaseInsensitive) + int(strlen("charset="));
                if (pos != -1) {
                    int pos2 = ba.indexOf('\"', pos+1);
                    QByteArray cs = ba.mid(pos, pos2-pos);
                    c = QTextCodec::codecForName(cs);
                    if (c)
                        return c;
                }
            }
            pos = end;
        }
    }
    if (mib)
        c = QTextCodec::codecForMib(mib);

    return c;
}

static
QTextCodec* findcodec(const QMimeSource* e)
{
    QTextCodec* r = 0;
    const char* f;
    int i;
    for (i=0; (f=e->format(i)); i++) {
        bool html = !qstrnicmp(f, "text/html", 9);
        if (html)
            r = codecForHTML(e->encodedData(f));
        if (!r)
            r = qt_findcharset(QByteArray(f).toLower());
        if (r)
            return r;
    }
    return 0;
}



/*!
    \reimp
*/
QByteArray Q3TextDrag::encodedData(const char* mime) const
{
    Q_D(const Q3TextDrag);
    if (mime != d->fmt)
        return QByteArray();
    return d->txt.toUtf8();
}

/*!
    \fn bool Q3TextDrag::canDecode(const QMimeSource *source)

    Returns true if the information in the MIME \a source can be decoded
    into a QString; otherwise returns false.

    \sa decode()
*/
bool Q3TextDrag::canDecode(const QMimeSource* e)
{
    const char* f;
    for (int i=0; (f=e->format(i)); i++) {
        if (0==qstrnicmp(f,"text/",5)) {
            return findcodec(e) != 0;
        }
    }
    return false;
}

/*!
    \fn bool Q3TextDrag::decode(const QMimeSource *source, QString &string, QString &subtype)

    \overload

    Attempts to decode the dropped information in the MIME \a source into
    the \a string given.
    Returns true if successful; otherwise returns false. If \a subtype
    is null, any text subtype is accepted; otherwise only the
    specified \a subtype is accepted.

    \sa canDecode()
*/
bool Q3TextDrag::decode(const QMimeSource* e, QString& str, QString& subtype)
{
    if(!e)
        return false;

    const char* mime;
    for (int i=0; (mime = e->format(i)); i++) {
        if (0==qstrnicmp(mime,"text/",5)) {
            QByteArray m(mime);
            m = m.toLower();
            int semi = m.indexOf(';');
            if (semi < 0)
                semi = m.length();
            QString foundst(QString::fromLatin1(m.mid(5,semi-5)));
            if (subtype.isNull() || foundst == subtype) {
                bool html = !qstrnicmp(mime, "text/html", 9);
                QTextCodec* codec = 0;
                if (html)
                    // search for the charset tag in the HTML
                    codec = codecForHTML(e->encodedData(mime));
                if (!codec)
                    codec = qt_findcharset(m);
                if (codec) {
                    QByteArray payload;

                    payload = e->encodedData(mime);
                    if (payload.size()) {
                        int l;
                        if (codec->mibEnum() != 1015) {
                            // length is at NUL or payload.size()
                            l = 0;
                            while (l < (int)payload.size() && payload[l])
                                l++;
                        } else {
                            l = payload.size();
                        }

                        str = codec->toUnicode(payload,l);

                        if (subtype.isNull())
                            subtype = foundst;

                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/*!
    \fn bool Q3TextDrag::decode(const QMimeSource *source, QString &string)

    Attempts to decode the dropped information in the MIME \a source into
    the \a string given.
    Returns true if successful; otherwise returns false.

    \sa canDecode()
*/
bool Q3TextDrag::decode(const QMimeSource* e, QString& str)
{
    QString st;
    return decode(e, str, st);
}


/*
  Q3ImageDrag could use an internal MIME type for communicating QPixmaps
  and QImages rather than always converting to raw data. This is available
  for that purpose and others. It is not currently used.
*/

/*!
    \class Q3ImageDrag qdragobject.h

    \brief The Q3ImageDrag class provides a drag and drop object for
    transferring images.

    \compat

    Images are offered to the receiving application in multiple
    formats, determined by Qt's output formats.
*/

/*!
    Constructs an image drag object with the given \a name, and sets its
    data to \a image. The \a dragSource is the widget that the drag operation
    started from.
*/

Q3ImageDrag::Q3ImageDrag(QImage image,
                        QWidget * dragSource, const char * name)
    : Q3DragObject(*(new Q3ImageDragPrivate), dragSource)
{
    setObjectName(QLatin1String(name));
    setImage(image);
}

/*!
    Constructs a default image drag object with the given \a name.
    The \a dragSource is the widget that the drag operation started from.
*/

Q3ImageDrag::Q3ImageDrag(QWidget * dragSource, const char * name)
    : Q3DragObject(*(new Q3ImageDragPrivate), dragSource)
{
    setObjectName(QLatin1String(name));
}

/*! \internal */
Q3ImageDrag::Q3ImageDrag(Q3ImageDragPrivate &dd, QWidget *dragSource)
    : Q3DragObject(dd, dragSource)
{
}

/*!
    Destroys the image drag object.
*/

Q3ImageDrag::~Q3ImageDrag()
{
    // nothing
}


/*!
    Sets the \a image to be dragged. You will need to call this if you did
    not pass the image during construction.
*/
void Q3ImageDrag::setImage(QImage image)
{
    Q_D(Q3ImageDrag);
    d->img = image;
    QList<QByteArray> formats = QImageWriter::supportedImageFormats();
    formats.removeAll("PBM"); // remove non-raw PPM
    if (image.depth()!=32) {
        // BMP better than PPM for paletted images
        if (formats.removeAll("BMP")) // move to front
            formats.insert(0,"BMP");
    }
    // PNG is best of all
    if (formats.removeAll("PNG")) // move to front
        formats.insert(0,"PNG");

    for(int i = 0; i < formats.count(); i++) {
        QByteArray format("image/");
        format += formats.at(i);
        format = format.toLower();
        if (format == "image/pbmraw")
            format = "image/ppm";
        d->ofmts.append(format);
    }
    d->ofmts.append("application/x-qt-image");
}

/*!
    \reimp
*/
const char * Q3ImageDrag::format(int i) const
{
    Q_D(const Q3ImageDrag);
    return i < d->ofmts.count() ? d->ofmts.at(i).data() : 0;
}

/*!
    \reimp
*/
QByteArray Q3ImageDrag::encodedData(const char* fmt) const
{
    Q_D(const Q3ImageDrag);
    QString imgFormat(fmt);
    if (imgFormat == QLatin1String("application/x-qt-image"))
        imgFormat = QLatin1String("image/PNG");

    if (imgFormat.startsWith("image/")){
        QByteArray f(imgFormat.mid(6).toAscii());
        QByteArray dat;
        QBuffer w(&dat);
        w.open(QIODevice::WriteOnly);
        QImageWriter writer(&w, f.toUpper());
        if (!writer.write(d->img))
            return QByteArray();
        w.close();
        return dat;
    } else {
        return QByteArray();
    }
}

/*!
    \fn bool Q3ImageDrag::canDecode(const QMimeSource *source)

    Returns true if the information in the MIME \a source can be decoded
    into an image; otherwise returns false.

    \sa decode()
*/
bool Q3ImageDrag::canDecode(const QMimeSource* e)
{
    return e->provides("application/x-qt-image");
}

/*!
    \fn bool Q3ImageDrag::decode(const QMimeSource *source, QImage &image)

    Decode the dropped information in the MIME \a source into the \a image.
    Returns true if successful; otherwise returns false.

    \sa canDecode()
*/
bool Q3ImageDrag::decode(const QMimeSource* e, QImage& img)
{
    if (!e)
        return false;

    QByteArray payload = e->encodedData("application/x-qt-image");
    if (payload.isEmpty())
        return false;

    img.loadFromData(payload);
    if (img.isNull())
        return false;

    return true;
}

/*!
    \fn bool Q3ImageDrag::decode(const QMimeSource *source, QPixmap &pixmap)

    \overload

    Decodes the dropped information in the MIME \a source into the \a pixmap.
    Returns true if successful; otherwise returns false.

    This is a convenience function that converts the information to a QPixmap
    via a QImage.

    \sa canDecode()
*/
bool Q3ImageDrag::decode(const QMimeSource* e, QPixmap& pm)
{
    if (!e)
        return false;

    QImage img;
    // We avoid dither, since the image probably came from this display
    if (decode(e, img)) {
        pm = QPixmap::fromImage(img, Qt::AvoidDither);
        if (pm.isNull())
            return false;

        return true;
    }
    return false;
}




/*!
    \class Q3StoredDrag qdragobject.h
    \brief The Q3StoredDrag class provides a simple stored-value drag object for arbitrary MIME data.

    \compat

    When a block of data has only one representation, you can use a
    Q3StoredDrag to hold it.

    For more information about drag and drop, see the Q3DragObject
    class and the \link dnd.html drag and drop documentation\endlink.
*/

/*!
    Constructs a Q3StoredDrag. The \a dragSource and \a name are passed
    to the Q3DragObject constructor, and the format is set to \a
    mimeType.

    The data will be unset. Use setEncodedData() to set it.
*/
Q3StoredDrag::Q3StoredDrag(const char* mimeType, QWidget * dragSource, const char * name) :
    Q3DragObject(*new Q3StoredDragPrivate, dragSource)
{
    Q_D(Q3StoredDrag);
    setObjectName(QLatin1String(name));
    d->fmt = qstrdup(mimeType);
}

/*! \internal */
Q3StoredDrag::Q3StoredDrag(Q3StoredDragPrivate &dd, const char* mimeType, QWidget * dragSource)
    : Q3DragObject(dd, dragSource)
{
    d_func()->fmt = qstrdup(mimeType);
}

/*!
    Destroys the drag object.
*/
Q3StoredDrag::~Q3StoredDrag()
{
    delete [] (char*)d_func()->fmt;
}

/*!
    \reimp
*/
const char * Q3StoredDrag::format(int i) const
{
    if (i==0)
        return d_func()->fmt;
    else
        return 0;
}


/*!
    \fn void Q3StoredDrag::setEncodedData(const QByteArray &data)

    Sets the encoded \a data of this drag object. The encoded data is
    delivered to drop sites; it must be in a strictly defined and portable
    format.

    The drag object can't be dropped (by the user) until this function
    has been called.
*/

void Q3StoredDrag::setEncodedData(const QByteArray & encodedData)
{
    d_func()->enc = encodedData;
}

/*!
    \fn QByteArray Q3StoredDrag::encodedData(const char *format) const

    Returns the stored data in the \a format given.

    \sa setEncodedData()
*/
QByteArray Q3StoredDrag::encodedData(const char* m) const
{
    if (!qstricmp(m, d_func()->fmt))
        return d_func()->enc;
    else
        return QByteArray();
}


/*!
    \class Q3UriDrag qdragobject.h
    \brief The Q3UriDrag class provides a drag object for a list of URI references.

    \compat

    URIs are a useful way to refer to files that may be distributed
    across multiple machines. A URI will often refer to a file on a
    machine local to both the drag source and the drop target, so the
    URI can be equivalent to passing a file name but is more
    extensible.

    Use URIs in Unicode form so that the user can comfortably edit and
    view them. For use in HTTP or other protocols, use the correctly
    escaped ASCII form.

    You can convert a list of file names to file URIs using
    setFileNames(), or into human-readable form with setUnicodeUris().

    Static functions are provided to convert between filenames and
    URIs; e.g. uriToLocalFile() and localFileToUri(). Static functions
    are also provided to convert URIs to and from human-readable form;
    e.g. uriToUnicodeUri() and unicodeUriToUri().
    You can also decode URIs from a MIME source into a list with
    decodeLocalFiles() and decodeToUnicodeUris().
*/

/*!
    Constructs an object to drag the list of \a uris.
    The \a dragSource and \a name are passed to the Q3StoredDrag constructor.

    Note that URIs are always in escaped UTF8 encoding.
*/
Q3UriDrag::Q3UriDrag(const Q3StrList &uris, QWidget * dragSource, const char * name) :
    Q3StoredDrag("text/uri-list", dragSource)
{
    setObjectName(QLatin1String(name));
    setUris(uris);
}

/*!
    Constructs an object to drag with the given \a name.
    You must call setUris() before you start the drag().
    Both the \a dragSource and the \a name are passed to the Q3StoredDrag
    constructor.
*/
Q3UriDrag::Q3UriDrag(QWidget * dragSource, const char * name) :
    Q3StoredDrag("text/uri-list", dragSource)
{
    setObjectName(QLatin1String(name));
}
#endif

/*!
    Destroys the URI drag object.
*/
Q3UriDrag::~Q3UriDrag()
{
}

/*!
    \fn void Q3UriDrag::setUris(const QList<QByteArray> &list)

    Changes the \a list of URIs to be dragged.

    Note that URIs are always in escaped UTF8 encoding.
*/
void Q3UriDrag::setUris(const QList<QByteArray> &uris)
{
    QByteArray a;
    int c = 0;
    int i;
    int count = uris.count();
    for (i = 0; i < count; ++i)
        c += uris.at(i).size() + 2; //length + \r\n
    a.reserve(c+1);
    for (i = 0; i < count; ++i) {
        a.append(uris.at(i));
        a.append("\r\n");
    }
    a[c] = 0;
    setEncodedData(a);
}


/*!
    \fn bool Q3UriDrag::canDecode(const QMimeSource *source)

    Returns true if decode() can decode the MIME \a source; otherwise
    returns false.
*/
bool Q3UriDrag::canDecode(const QMimeSource* e)
{
    return e->provides("text/uri-list");
}

/*!
    \fn bool Q3UriDrag::decode(const QMimeSource* source, Q3StrList& list)

    Decodes URIs from the MIME \a source, placing the result in the \a list.
    The list is cleared before any items are inserted.

    Returns true if the MIME \a source contained a valid list of URIs;
    otherwise returns false.
*/
bool Q3UriDrag::decode(const QMimeSource* e, Q3StrList& l)
{
    QByteArray payload = e->encodedData("text/uri-list");
    if (payload.size()) {
        l.clear();
        l.setAutoDelete(true);
        uint c=0;
        const char* data = payload.data();
        while ((int)c < payload.size() && data[c]) {
            uint f = c;
            // Find line end
            while ((int)c < payload.size() && data[c] && data[c]!='\r'
                   && data[c] != '\n')
                c++;
            Q3CString s(data+f,c-f+1);
            if (s[0] != '#') // non-comment?
                l.append(s);
            // Skip junk
            while ((int)c < payload.size() && data[c] &&
                   (data[c]=='\n' || data[c]=='\r'))
                c++;
        }
        return true;
    }
    return false;
}

static uint htod(int h)
{
    if (isdigit(h))
        return h - '0';
    return tolower(h) - 'a' + 10;
}

/*!
  \fn Q3UriDrag::setFilenames(const QStringList &list)

  \obsolete

  Sets the filename's in the drag object to those in the given \a
  list.

  Use setFileNames() instead.
*/

/*!
    \fn void Q3UriDrag::setFileNames(const QStringList &filenames)

    Sets the URIs to be local file URIs equivalent to the \a filenames.

    \sa localFileToUri(), setUris()
*/
void Q3UriDrag::setFileNames(const QStringList & fnames)
{
    QList<QByteArray> uris;
    for (QStringList::ConstIterator i = fnames.begin();
    i != fnames.end(); ++i) {
        QByteArray fileUri = localFileToUri(*i);
        if (!fileUri.isEmpty())
            uris.append(fileUri);
    }

    setUris(uris);
}

/*!
    \fn void Q3UriDrag::setFileNames(const QString &name)
    \fn void Q3UriDrag::setFilenames(const QString &name)

    Same as setFileNames(QStringList(\a name)).
*/

/*!
    \fn void Q3UriDrag::setUnicodeUris(const QStringList &list)

    Sets the URIs in the \a list to be Unicode URIs (only useful for
    displaying to humans).

    \sa localFileToUri(), setUris()
*/
void Q3UriDrag::setUnicodeUris(const QStringList & uuris)
{
    QList<QByteArray> uris;
    for (int i = 0; i < uuris.count(); ++i)
        uris.append(unicodeUriToUri(uuris.at(i)));
    setUris(uris);
}

/*!
    \fn QByteArray Q3UriDrag::unicodeUriToUri(const QString &string)

    Returns the URI equivalent of the Unicode URI given in the \a string
    (only useful for displaying to humans).

    \sa uriToLocalFile()
*/
QByteArray Q3UriDrag::unicodeUriToUri(const QString& uuri)
{
    QByteArray utf8 = uuri.toUtf8();
    QByteArray escutf8;
    int n = utf8.length();
    bool isFile = uuri.startsWith(QLatin1String("file://"));
    for (int i=0; i<n; i++) {
        if ((utf8[i] >= 'a' && utf8[i] <= 'z')
          || utf8[i] == '/'
          || (utf8[i] >= '0' && utf8[i] <= '9')
          || (utf8[i] >= 'A' && utf8[i] <= 'Z')

          || utf8[i] == '-' || utf8[i] == '_'
          || utf8[i] == '.' || utf8[i] == '!'
          || utf8[i] == '~' || utf8[i] == '*'
          || utf8[i] == '(' || utf8[i] == ')'
          || utf8[i] == '\''

          // Allow this through, so that all URI-references work.
          || (!isFile && utf8[i] == '#')

          || utf8[i] == ';'
          || utf8[i] == '?' || utf8[i] == ':'
          || utf8[i] == '@' || utf8[i] == '&'
          || utf8[i] == '=' || utf8[i] == '+'
          || utf8[i] == '$' || utf8[i] == ',')
        {
            escutf8 += utf8[i];
        } else {
            // Everything else is escaped as %HH
            QString s;
            s.sprintf("%%%02x",(uchar)utf8[i]);
            escutf8 += s.latin1();
        }
    }
    return escutf8;
}

/*!
    Returns the URI equivalent to the absolute local \a filename.

    \sa uriToLocalFile()
*/
QByteArray Q3UriDrag::localFileToUri(const QString& filename)
{
    QString r = filename;

    //check that it is an absolute file
    if (QDir::isRelativePath(r))
        return QByteArray();
#ifdef Q_WS_WIN


    bool hasHost = false;
    // convert form network path
    if (r.left(2) == QLatin1String("\\\\") || r.left(2) == QLatin1String("//")) {
        r.remove(0, 2);
        hasHost = true;
    }

    // Slosh -> Slash
    int slosh;
    while ((slosh=r.indexOf(QLatin1Char('\\'))) >= 0) {
        r[slosh] = QLatin1Char('/');
    }

    // Drive
    if (r[0] != QLatin1Char('/') && !hasHost)
        r.insert(0,QLatin1Char('/'));

#endif
#if defined (Q_WS_X11) && 0
    // URL without the hostname is considered to be errorneous by XDnD.
    // See: http://www.newplanetsoftware.com/xdnd/dragging_files.html
    // This feature is not active because this would break dnd between old and new qt apps.
    char hostname[257];
    if (gethostname(hostname, 255) == 0) {
        hostname[256] = '\0';
        r.prepend(QString::fromLatin1(hostname));
    }
#endif
    return unicodeUriToUri(QString(QLatin1String("file://") + r));
}

/*!
    \fn QString Q3UriDrag::uriToUnicodeUri(const char *string)

    Returns the Unicode URI (only useful for displaying to humans)
    equivalent of the URI given in the \a string.

    Note that URIs are always in escaped UTF8 encoding.

    \sa localFileToUri()
*/
QString Q3UriDrag::uriToUnicodeUri(const char* uri)
{
    QByteArray utf8;

    while (*uri) {
        switch (*uri) {
          case '%': {
                uint ch = (uchar) uri[1];
                if (ch && uri[2]) {
                    ch = htod(ch) * 16 + htod((uchar) uri[2]);
                    utf8 += (char) ch;
                    uri += 2;
                }
            }
            break;
          default:
            utf8 += *uri;
        }
        ++uri;
    }

    return QString::fromUtf8(utf8);
}

/*!
    \fn QString Q3UriDrag::uriToLocalFile(const char *string)

    Returns the name of a local file equivalent to the URI given in the
    \a string, or an empty string if it does not refer to a local file.

    Note that URIs are always in escaped UTF8 encoding.

    \sa localFileToUri()
*/
QString Q3UriDrag::uriToLocalFile(const char* uri)
{
    QString file;

    if (!uri)
        return file;
    if (0==qstrnicmp(uri,"file:/",6)) // It is a local file uri
        uri += 6;
    else if (QString(QLatin1String(uri)).indexOf(QLatin1String(":/")) != -1) // It is a different scheme uri
        return file;

    bool local = uri[0] != '/' || (uri[0] != '\0' && uri[1] == '/');
#ifdef Q_WS_X11
    // do we have a hostname?
    if (!local && uri[0] == '/' && uri[2] != '/') {
        // then move the pointer to after the 'hostname/' part of the uri
        const char* hostname_end = strchr(uri+1, '/');
        if (hostname_end != NULL) {
            char hostname[257];
            if (gethostname(hostname, 255) == 0) {
                hostname[256] = '\0';
                if (qstrncmp(uri+1, hostname, hostname_end - (uri+1)) == 0) {
                    uri = hostname_end + 1; // point after the slash
                    local = true;
                }
            }
        }
    }
#endif
    if (local) {
        file = uriToUnicodeUri(uri);
        if (uri[1] == '/') {
            file.remove((uint)0,1);
        } else {
                file.insert(0, QLatin1Char('/'));
        }
#ifdef Q_WS_WIN
        if (file.length() > 2 && file[0] == QLatin1Char('/') && file[2] == QLatin1Char('|')) {
            file[2] = QLatin1Char(':');
            file.remove(0,1);
        } else if (file.length() > 2 && file[0] == QLatin1Char('/') && file[1].isLetter() && file[2] == QLatin1Char(':')) {
            file.remove(0, 1);
        }
        // Leave slash as slashes.
#endif
    }
#ifdef Q_WS_WIN
    else {
        file = uriToUnicodeUri(uri);
        // convert to network path
        file.insert(1, QLatin1Char('/')); // leave as forward slashes
    }
#endif

    return file;
}

/*!
    \fn bool Q3UriDrag::decodeLocalFiles(const QMimeSource *source, QStringList &list)

    Decodes URIs from the MIME \a source, converting them to local filenames
    where possible, and places them in the \a list (which is first cleared).

    Returns true if the MIME \a source contained a valid list of URIs;
    otherwise returns false. The list will be empty if no URIs referred to
    local files.
*/
bool Q3UriDrag::decodeLocalFiles(const QMimeSource* e, QStringList& l)
{
    Q3StrList u;
    if (!decode(e, u))
        return false;

    l.clear();
    for (uint i = 0; i < u.count(); ++i) {
        QString lf = uriToLocalFile(u.at(i));
        if (!lf.isEmpty())
            l.append(lf);
    }
    return true;
}

/*!
    \fn bool Q3UriDrag::decodeToUnicodeUris(const QMimeSource *source, QStringList &list)

    Decodes URIs from the MIME \a source, converting them to Unicode URIs
    (only useful for displaying to humans), and places them in the \a list
    (which is first cleared).

    Returns true if the MIME \a source contained a valid list of URIs;
    otherwise returns false.
*/
bool Q3UriDrag::decodeToUnicodeUris(const QMimeSource* e, QStringList& l)
{
    Q3StrList u;
    if (!decode(e, u))
        return false;

    l.clear();
    for (uint i = 0; i < u.count(); ++i)
        l.append(uriToUnicodeUri(u.at(i)));

    return true;
}

/*!
    \class Q3ColorDrag

    \brief The Q3ColorDrag class provides a drag and drop object for
    transferring colors between widgets.

    \compat

    This class provides a drag object which can be used to transfer data
    about colors for drag and drop and in the clipboard. For example, it
    is used in QColorDialog.

    The color is set in the constructor but can be changed with
    setColor().

    For more information about drag and drop, see the Q3DragObject class
    and the \link dnd.html drag and drop documentation\endlink.
*/

/*!
    Constructs a color drag object with the given \a col. Passes \a
    dragsource and \a name to the Q3StoredDrag constructor.
*/

Q3ColorDrag::Q3ColorDrag(const QColor &col, QWidget *dragsource, const char *name)
    : Q3StoredDrag("application/x-color", dragsource)
{
    setObjectName(QLatin1String(name));
    setColor(col);
}

/*!
    Constructs a color drag object with a white color. Passes \a
    dragsource and \a name to the Q3StoredDrag constructor.
*/

Q3ColorDrag::Q3ColorDrag(QWidget *dragsource, const char *name)
    : Q3StoredDrag("application/x-color", dragsource)
{
    setObjectName(QLatin1String(name));
    setColor(Qt::white);
}

/*!
    \fn void Q3ColorDrag::setColor(const QColor &color)

    Sets the \a color of the color drag.
*/

void Q3ColorDrag::setColor(const QColor &col)
{
    short r = (col.red()   << 8) | col.red();
    short g = (col.green() << 8) | col.green();
    short b = (col.blue()  << 8) | col.blue();

    // make sure we transmit data in network order
    r = htons(r);
    g = htons(g);
    b = htons(b);

    ushort rgba[4] = {
        r, g, b,
        0xffff // Alpha not supported yet.
    };
    QByteArray data;
    data.resize(sizeof(rgba));
    memcpy(data.data(), rgba, sizeof(rgba));
    setEncodedData(data);
}

/*!
    \fn bool Q3ColorDrag::canDecode(QMimeSource *source)

    Returns true if the color drag object can decode the MIME \a source;
    otherwise returns false.
*/

bool Q3ColorDrag::canDecode(QMimeSource *e)
{
    return e->provides("application/x-color");
}

/*!
    \fn bool Q3ColorDrag::decode(QMimeSource *source, QColor &color)

    Decodes the MIME \a source, and sets the decoded values to the
    given \a color. Returns true if the decoding is successful.
    Returns false if the size of the encoded data is not the
    expected size.
*/

bool Q3ColorDrag::decode(QMimeSource *e, QColor &col)
{
    QByteArray data = e->encodedData("application/x-color");
    ushort rgba[4];
    if (data.size() != sizeof(rgba))
        return false;

    memcpy(rgba, data.constData(), sizeof(rgba));

    short r = rgba[0];
    short g = rgba[1];
    short b = rgba[2];
    short a = rgba[3];

    // data is in network order
    r = ntohs(r);
    g = ntohs(g);
    b = ntohs(b);
    a = ntohs(a);

    r = (r >> 8) & 0xff;
    g = (g >> 8) & 0xff;
    b = (b >> 8) & 0xff;
    a = (a >> 8) & 0xff;

    col.setRgb(r, g, b, a);
    return true;
}

QT_END_NAMESPACE
