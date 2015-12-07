/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "qtbrushmanager.h"
#include "brushmanagerproxy.h"
#include "qsimpleresource_p.h"
#include "qdesigner_utils_p.h"
#include "ui4_p.h"

#include <QtXml/QXmlStreamWriter>

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
    if (file.open(QIODevice::WriteOnly)) {
        QSimpleResource resource(m_Core);

        DomBrush *dom = resource.saveBrush(brush);

        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(1);
        writer.writeStartDocument();
        writer.writeStartElement(QLatin1String("description"));
        writer.writeAttribute(QLatin1String("name"), name);
        dom->write(writer);
        writer.writeEndElement();
        writer.writeEndDocument();

        delete dom;
        file.close();

        m_FileToBrush[filename] = name;
        m_BrushToFile[name] = filename;
    }
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

            QString filename = fi.absoluteFilePath();

            QFile file(filename);
            if (file.open(QIODevice::ReadOnly)) {
                QXmlStreamReader reader(&file);

                //<description name="black" >
                // <brush brushstyle="SolidPattern" >
                //   <color alpha="255" .../>
                // </brush>
                //</description>

                QString descname;
                while (!reader.atEnd()) {
                    if (reader.readNext() == QXmlStreamReader::StartElement) {
                        const QString tag = reader.name().toString().toLower();
                        if (tag == description) {
                            if (!reader.attributes().hasAttribute(nameAttribute))
                                reader.raiseError(tr("The element '%1' is missing the required attribute '%2'.")
                                                  .arg(tag, nameAttribute));
                            else
                                descname = reader.attributes().value(nameAttribute).toString();
                            continue;
                        }
                        if (tag == brush) {
                            DomBrush brush;
                            brush.read(reader);

                            if (descname.isEmpty()) {
                                reader.raiseError(tr("Empty brush name encountered."));
                            } else {
                                QSimpleResource resource(d_ptr->m_Core);
                                QBrush br = resource.setupBrush(&brush);
                                d_ptr->m_Manager->addBrush(descname, br);
                                d_ptr->m_FileToBrush[filename] = descname;
                                d_ptr->m_BrushToFile[descname] = filename;
                            }
                            continue;
                        }
                        reader.raiseError(tr("An unexpected element '%1' was encountered.").arg(tag));
                    }
                }

                file.close();

                if (reader.hasError()) {
                    qdesigner_internal::designerWarning(tr("An error occurred when reading the brush definition file '%1' at line line %2, column %3: %4")
                                            .arg(fi.fileName())
                                            .arg(reader.lineNumber())
                                            .arg(reader.columnNumber())
                                            .arg(reader.errorString()));
                    continue;
                }
            }
        }
    }

    connect(d_ptr->m_Manager, SIGNAL(brushAdded(QString,QBrush)),
            this, SLOT(brushAdded(QString, QBrush)));
    connect(d_ptr->m_Manager, SIGNAL(brushRemoved(QString)),
            this, SLOT(brushRemoved(QString)));

    if (!customBrushesExist) {
        // load brushes from resources
        QFile qrcFile(QLatin1String(":trolltech/brushes/defaultbrushes.xml"));
        if (!qrcFile.open(QIODevice::ReadOnly))
            Q_ASSERT(0);

        QXmlStreamReader reader(&qrcFile);

        while (!reader.atEnd()) {
            if (reader.readNext() == QXmlStreamReader::StartElement) {
                if (reader.name().toString().toLower() == QLatin1String("description")) {
                    const QString name = reader.attributes().value(nameAttribute).toString();
                    do { // forward to <brush> element, which DomBrush expects
                        reader.readNext();
                    } while (!reader.atEnd() && reader.tokenType() != QXmlStreamReader::StartElement);
                    DomBrush brushDom;
                    brushDom.read(reader);
                    if (!reader.hasError()) {
                        QSimpleResource resource(d_ptr->m_Core);
                        QBrush br = resource.setupBrush(&brushDom);
                        d_ptr->m_Manager->addBrush(name, br);
                    }
                }
            }
        }
        if (reader.hasError()) {
            // Should never happen
            qdesigner_internal::designerWarning(tr("An error occurred when reading the resource file '%1' at line %2, column %3: %4")
                                                .arg(qrcFile.fileName())
                                                .arg(reader.lineNumber())
                                                .arg(reader.columnNumber())
                                                .arg(reader.errorString()));
        }

        qrcFile.close();
    }
}

QT_END_NAMESPACE

#include "moc_brushmanagerproxy.cpp"
