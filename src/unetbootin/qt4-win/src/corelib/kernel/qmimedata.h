/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QMIMEDATA_H
#define QMIMEDATA_H

#include <QtCore/qvariant.h>
#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QUrl;
class QMimeDataPrivate;

class Q_CORE_EXPORT QMimeData : public QObject
{
    Q_OBJECT
public:
    QMimeData();
    ~QMimeData();

    QList<QUrl> urls() const;
    void setUrls(const QList<QUrl> &urls);
    bool hasUrls() const;

    QString text() const;
    void setText(const QString &text);
    bool hasText() const;

    QString html() const;
    void setHtml(const QString &html);
    bool hasHtml() const;

    QVariant imageData() const;
    void setImageData(const QVariant &image);
    bool hasImage() const;

    QVariant colorData() const;
    void setColorData(const QVariant &color);
    bool hasColor() const;

    QByteArray data(const QString &mimetype) const;
    void setData(const QString &mimetype, const QByteArray &data);
    void removeFormat(const QString &mimetype);

    virtual bool hasFormat(const QString &mimetype) const;
    virtual QStringList formats() const;

    void clear();
protected:
    virtual QVariant retrieveData(const QString &mimetype,
                                      QVariant::Type preferredType) const;
private:
    Q_DISABLE_COPY(QMimeData)
    Q_DECLARE_PRIVATE(QMimeData)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMIMEDATA_H
