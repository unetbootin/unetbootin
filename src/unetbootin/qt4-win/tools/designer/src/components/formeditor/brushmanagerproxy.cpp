/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "qtbrushmanager.h"
#include "brushmanagerproxy.h"
#include "qsimpleresource_p.h"
#include "ui4_p.h"

#include <QtXml/QDomDocument>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class BrushManagerProxyPrivate
{
    BrushManagerProxy *q_ptr;
    Q_DECLARE_PUBLIC(BrushManagerProxy)

public:
    BrushManagerProxyPrivate(BrushManagerProxy *bp, QDesignerFormEditorInterface *core);
    void brushAdded(const QString &name, const QBrush &brush);
    void brushRemoved(const QString &name);
    QString uniqueBrushFileName(const QString &brushName) const;

    QtBrushManager *m_Manager;
    QString m_designerFolder;
    const QString  m_BrushFolder;
    QString m_BrushPath;
    QDesignerFormEditorInterface *m_Core;
    QMap<QString, QString> m_FileToBrush;
    QMap<QString, QString> m_BrushToFile;
};

BrushManagerProxyPrivate::BrushManagerProxyPrivate(BrushManagerProxy *bp, QDesignerFormEditorInterface *core) :
    q_ptr(bp),
    m_Manager(0),
    m_BrushFolder(QLatin1String("brushes")),
    m_Core(core)
{
    m_designerFolder = QDir::homePath();
    m_designerFolder += QDir::separator();
    m_designerFolder += QLatin1String(".designer");
    m_BrushPath = m_designerFolder;
    m_BrushPath += QDir::separator();
    m_BrushPath += m_BrushFolder;
}
}  // namespace qdesigner_internal

using namespace qdesigner_internal;

void BrushManagerProxyPrivate::brushAdded(const QString &name, const QBrush &brush)
{
    const QString filename = uniqueBrushFileName(name);

    QDir designerDir(m_designerFolder);
    if (!designerDir.exists(m_BrushFolder))
        designerDir.mkdir(m_BrushFolder);

    QFile file(m_BrushPath + QDir::separator() +filename);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QSimpleResource resource(m_Core);

    DomBrush *dom = resource.saveBrush(brush);
    QDomDocument doc;
    QDomElement elem = doc.createElement(QLatin1String("description"));
    elem.setAttribute(QLatin1String("name"), name);
    elem.appendChild(dom->write(doc));
    doc.appendChild(elem);
    file.write(doc.toString().toUtf8());

    file.close();

    m_FileToBrush[filename] = name;
    m_BrushToFile[name] = filename;

    delete dom;
}

void BrushManagerProxyPrivate::brushRemoved(const QString &name)
{
    QDir brushDir(m_BrushPath);

    QString filename = m_BrushToFile[name];
    brushDir.remove(filename);
    m_BrushToFile.remove(name);
    m_FileToBrush.remove(filename);
}

QString BrushManagerProxyPrivate::uniqueBrushFileName(const QString &brushName) const
{
    const  QString extension = QLatin1String(".br");
    QString filename = brushName.toLower();
    filename += extension;
    int i = 0;
    while (m_FileToBrush.contains(filename)) {
        filename = brushName.toLower();
        filename += QString::number(++i);
        filename += extension;
    }
    return filename;
}


BrushManagerProxy::BrushManagerProxy(QDesignerFormEditorInterface *core, QObject *parent)
    : QObject(parent)
{
    d_ptr = new BrushManagerProxyPrivate(this, core);
}

BrushManagerProxy::~BrushManagerProxy()
{
    delete d_ptr;
}

void BrushManagerProxy::setBrushManager(QtBrushManager *manager)
{
    if (d_ptr->m_Manager == manager)
        return;

    if (d_ptr->m_Manager) {
        disconnect(d_ptr->m_Manager, SIGNAL(brushAdded(const QString &, const QBrush &)),
                    this, SLOT(brushAdded(const QString &, const QBrush &)));
        disconnect(d_ptr->m_Manager, SIGNAL(brushRemoved(const QString &)),
                    this, SLOT(brushRemoved(const QString &)));
    }

    d_ptr->m_Manager = manager;

    if (!d_ptr->m_Manager)
        return;

    // clear the manager
    QMap<QString, QBrush> brushes = d_ptr->m_Manager->brushes();
    QMap<QString, QBrush>::ConstIterator it = brushes.constBegin();
    while (it != brushes.constEnd()) {
        QString name = it.key();
        d_ptr->m_Manager->removeBrush(name);

        it++;
    }

    // fill up the manager from compiled resources or from brush folder here
    const QString nameAttribute = QLatin1String("name");
    const QString brush = QLatin1String("brush");
    const QString description = QLatin1String("description");

    QDir brushDir(d_ptr->m_BrushPath);
    bool customBrushesExist = brushDir.exists();
    if (customBrushesExist) {
        // load brushes from brush folder
        QStringList nameFilters;
        nameFilters.append(QLatin1String("*.br"));

        QFileInfoList infos = brushDir.entryInfoList(nameFilters);
        QListIterator<QFileInfo> it(infos);
        while (it.hasNext()) {
            const QFileInfo fi = it.next();

            QFile file(fi.absoluteFilePath());
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray contents = file.readAll();
                file.close();
                QDomDocument doc;
                if (doc.setContent(contents)) {
                    QDomElement domElement = doc.documentElement();

                    QString name = domElement.attribute(nameAttribute);
                    QString filename = fi.fileName();

                    QSimpleResource resource(d_ptr->m_Core);

                    QDomElement brushElement = domElement.firstChildElement(brush);
                    DomBrush dom;
                    dom.read(brushElement);
                    QBrush br = resource.setupBrush(&dom);

                    d_ptr->m_Manager->addBrush(name, br);
                    d_ptr->m_FileToBrush[filename] = name;
                    d_ptr->m_BrushToFile[name] = filename;
                }
            }
        }
    }

    connect(d_ptr->m_Manager, SIGNAL(brushAdded(const QString &, const QBrush &)),
            this, SLOT(brushAdded(const QString &, const QBrush &)));
    connect(d_ptr->m_Manager, SIGNAL(brushRemoved(const QString &)),
            this, SLOT(brushRemoved(const QString &)));

    if (!customBrushesExist) {
        // load brushes from resources
        QFile qrcFile(QLatin1String(":trolltech/brushes/defaultbrushes.xml"));
        if (qrcFile.open(QIODevice::ReadOnly)) {
            QByteArray contents = qrcFile.readAll();
            qrcFile.close();
            QDomDocument doc;
            if (doc.setContent(contents)) {
                QDomElement domElement = doc.documentElement();

                QDomElement descElement = domElement.firstChildElement(description);
                while (!descElement.isNull()) {
                    QString name = descElement.attribute(nameAttribute);

                    QSimpleResource resource(d_ptr->m_Core);

                    QDomElement brushElement = descElement.firstChildElement(brush);
                    DomBrush dom;
                    dom.read(brushElement);
                    QBrush br = resource.setupBrush(&dom);

                    d_ptr->m_Manager->addBrush(name, br);

                    descElement = descElement.nextSiblingElement(description);
                }
            }
        }
    }
}

QT_END_NAMESPACE

#include "moc_brushmanagerproxy.cpp"
