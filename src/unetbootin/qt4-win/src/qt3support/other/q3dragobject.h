/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3DRAGOBJECT_H
#define Q3DRAGOBJECT_H

#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>
#include <QtGui/qmime.h>
#include <QtGui/qimage.h>
#include <Qt3Support/q3strlist.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class QWidget;
class Q3TextDragPrivate;
class Q3DragObjectPrivate;
class Q3StoredDragPrivate;
class Q3ImageDragPrivate;
class Q3ImageDrag;
class Q3TextDrag;
class Q3StrList;
class QImage;
class QPixmap;

class Q_COMPAT_EXPORT Q3DragObject : public QObject, public QMimeSource {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Q3DragObject)
public:
    Q3DragObject(QWidget * dragSource = 0, const char *name = 0);
    virtual ~Q3DragObject();

    bool drag();
    bool dragMove();
    void dragCopy();
    void dragLink();

    virtual void setPixmap(QPixmap);
    virtual void setPixmap(QPixmap, const QPoint& hotspot);
    QPixmap pixmap() const;
    QPoint pixmapHotSpot() const;

    QWidget * source();
    static QWidget * target();

    enum DragMode { DragDefault, DragCopy, DragMove, DragLink, DragCopyOrMove };

protected:
    Q3DragObject(Q3DragObjectPrivate &, QWidget *dragSource = 0);
    virtual bool drag(DragMode);

private:
    friend class QDragMime;
    Q_DISABLE_COPY(Q3DragObject)
};

class Q_COMPAT_EXPORT Q3StoredDrag: public Q3DragObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Q3StoredDrag)
public:
    Q3StoredDrag(const char *mimeType, QWidget *dragSource = 0, const char *name = 0);
    ~Q3StoredDrag();

    virtual void setEncodedData(const QByteArray &);

    const char * format(int i) const;
    virtual QByteArray encodedData(const char*) const;

protected:
    Q3StoredDrag(Q3StoredDragPrivate &, const char *mimeType, QWidget *dragSource = 0);

private:
    Q_DISABLE_COPY(Q3StoredDrag)
};

class Q_COMPAT_EXPORT Q3TextDrag: public Q3DragObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Q3TextDrag)
public:
    Q3TextDrag(const QString &, QWidget *dragSource = 0, const char *name = 0);
    Q3TextDrag(QWidget * dragSource = 0, const char * name = 0);
    ~Q3TextDrag();

    virtual void setText(const QString &);
    virtual void setSubtype(const QString &);

    const char * format(int i) const;
    virtual QByteArray encodedData(const char*) const;

    static bool canDecode(const QMimeSource* e);
    static bool decode(const QMimeSource* e, QString& s);
    static bool decode(const QMimeSource* e, QString& s, QString& subtype);

protected:
    Q3TextDrag(Q3TextDragPrivate &, QWidget * dragSource = 0);

private:
    Q_DISABLE_COPY(Q3TextDrag)
};

class Q_COMPAT_EXPORT Q3ImageDrag: public Q3DragObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Q3ImageDrag)
public:
    Q3ImageDrag(QImage image, QWidget * dragSource = 0, const char * name = 0);
    Q3ImageDrag(QWidget * dragSource = 0, const char * name = 0);
    ~Q3ImageDrag();

    virtual void setImage(QImage image);

    const char * format(int i) const;
    virtual QByteArray encodedData(const char*) const;

    static bool canDecode(const QMimeSource* e);
    static bool decode(const QMimeSource* e, QImage& i);
    static bool decode(const QMimeSource* e, QPixmap& i);

protected:
    Q3ImageDrag(Q3ImageDragPrivate &, QWidget * dragSource = 0);

private:
    Q_DISABLE_COPY(Q3ImageDrag)
};


class Q_COMPAT_EXPORT Q3UriDrag: public Q3StoredDrag {
    Q_OBJECT

public:
    Q3UriDrag(const Q3StrList &uris, QWidget * dragSource = 0, const char * name = 0);
    Q3UriDrag(QWidget * dragSource = 0, const char * name = 0);
    ~Q3UriDrag();

    void setFileNames(const QStringList & fnames);
    inline void setFileNames(const QString & fname) { setFileNames(QStringList(fname)); }
    void setFilenames(const QStringList & fnames) { setFileNames(fnames); }
    inline void setFilenames(const QString & fname) { setFileNames(QStringList(fname)); }
    void setUnicodeUris(const QStringList & uuris);
    virtual void setUris(const QList<QByteArray> &uris);

    static QString uriToLocalFile(const char*);
    static QByteArray localFileToUri(const QString&);
    static QString uriToUnicodeUri(const char*);
    static QByteArray unicodeUriToUri(const QString&);
    static bool canDecode(const QMimeSource* e);
    static bool decode(const QMimeSource* e, Q3StrList& i);
    static bool decodeToUnicodeUris(const QMimeSource* e, QStringList& i);
    static bool decodeLocalFiles(const QMimeSource* e, QStringList& i);

private:
    Q_DISABLE_COPY(Q3UriDrag)
};

class Q_COMPAT_EXPORT Q3ColorDrag : public Q3StoredDrag
{
    Q_OBJECT
    QColor color;

public:
    Q3ColorDrag(const QColor &col, QWidget *dragsource = 0, const char *name = 0);
    Q3ColorDrag(QWidget * dragSource = 0, const char * name = 0);
    void setColor(const QColor &col);

    static bool canDecode(QMimeSource *);
    static bool decode(QMimeSource *, QColor &col);

private:
    Q_DISABLE_COPY(Q3ColorDrag)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3DRAGOBJECT_H
