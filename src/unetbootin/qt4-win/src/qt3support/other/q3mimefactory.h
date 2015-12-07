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

#ifndef Q3MIMEFACTORY_H
#define Q3MIMEFACTORY_H

#include <QtGui/qwindowdefs.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>
#include <QtGui/qpixmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_MIMEFACTORY

class QStringList;
class QMimeSource;
class Q3MimeSourceFactoryData;

class Q_COMPAT_EXPORT Q3MimeSourceFactory {
public:
    Q3MimeSourceFactory();
    virtual ~Q3MimeSourceFactory();

    static Q3MimeSourceFactory* defaultFactory();
    static void setDefaultFactory(Q3MimeSourceFactory*);
    static Q3MimeSourceFactory* takeDefaultFactory();
    static void addFactory(Q3MimeSourceFactory *f);
    static void removeFactory(Q3MimeSourceFactory *f);

    virtual const QMimeSource* data(const QString& abs_name) const;
    virtual QString makeAbsolute(const QString& abs_or_rel_name, const QString& context) const;
    const QMimeSource* data(const QString& abs_or_rel_name, const QString& context) const;

    virtual void setText(const QString& abs_name, const QString& text);
    virtual void setImage(const QString& abs_name, const QImage& im);
    virtual void setPixmap(const QString& abs_name, const QPixmap& pm);
    virtual void setData(const QString& abs_name, QMimeSource* data);
    virtual void setFilePath(const QStringList&);
    inline  void setFilePath(const QString &path) { setFilePath(QStringList(path)); }
    virtual QStringList filePath() const;
    void addFilePath(const QString&);
    virtual void setExtensionType(const QString& ext, const char* mimetype);

private:
    QMimeSource *dataInternal(const QString& abs_name, const QMap<QString, QString> &extensions) const;
    Q3MimeSourceFactoryData* d;
};

Q_COMPAT_EXPORT QPixmap qPixmapFromMimeSource(const QString &abs_name);

Q_COMPAT_EXPORT QImage qImageFromMimeSource(const QString &abs_name);

#endif // QT_NO_MIMEFACTORY

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3MIMEFACTORY_H
