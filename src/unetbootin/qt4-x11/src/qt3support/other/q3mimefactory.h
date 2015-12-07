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
