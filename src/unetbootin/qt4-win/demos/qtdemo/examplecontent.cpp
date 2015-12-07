/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "examplecontent.h"
#include "colors.h"
#include "menumanager.h"
#include "imageitem.h"
#include "headingitem.h"

ExampleContent::ExampleContent(const QString &name, QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    this->name = name;
    this->heading = 0;
    this->description = 0;
    this->screenshot = 0;
}

void ExampleContent::prepare()
{
    if (!this->prepared){
        this->prepared = true;
        this->createContent();
    }
}

void ExampleContent::animationStopped(int id)
{
    if (id == DemoItemAnimation::ANIM_OUT){
        // Free up some memory:
        delete this->heading;
        delete this->description;
        delete this->screenshot;
        this->heading = 0;
        this->description = 0;
        this->screenshot = 0;
        this->prepared = false;
    }
}

QString ExampleContent::loadDescription()
{
    QByteArray ba = MenuManager::instance()->getHtml(this->name);

    QDomDocument exampleDoc;
    exampleDoc.setContent(ba, false);

    QDomNodeList paragraphs = exampleDoc.elementsByTagName("p");
    if (paragraphs.length() < 1 && Colors::verbose)
        qDebug() << "- ExampleContent::loadDescription(): Could not load description:" << MenuManager::instance()->info[this->name]["docfile"];
    QString description = Colors::contentColor + QLatin1String("Could not load description. Ensure that the documentation for Qt is built.");
    for (int p = 0; p < int(paragraphs.length()); ++p) {
        description = this->extractTextFromParagraph(paragraphs.item(p));
        if (this->isSummary(description)) {
            break;
        }
    }
    return Colors::contentColor + description;
}

bool ExampleContent::isSummary(const QString &text)
{
    return (!text.contains("[") &&
        text.indexOf(QRegExp(QString("(In )?((The|This) )?(%1 )?.*(tutorial|example|demo|application)").arg(this->name), Qt::CaseInsensitive)) != -1);
}

QString ExampleContent::extractTextFromParagraph(const QDomNode &parentNode)
{
    QString description;
    QDomNode node = parentNode.firstChild();

    while (!node.isNull()) {
        QString beginTag;
        QString endTag;
        if (node.isText())
            description += Colors::contentColor + node.nodeValue();
        else if (node.hasChildNodes()) {
            if (node.nodeName() == "b") {
                beginTag = "<b>";
                endTag = "</b>";
            } else if (node.nodeName() == "a") {
                beginTag = Colors::contentColor;
                endTag = "</font>";
            } else if (node.nodeName() == "i") {
                beginTag = "<i>";
                endTag = "</i>";
            } else if (node.nodeName() == "tt") {
                beginTag = "<tt>";
                endTag = "</tt>";
            }
            description += beginTag + this->extractTextFromParagraph(node) + endTag;
        }
        node = node.nextSibling();
    }

    return description;
}

void ExampleContent::createContent()
{
    // Create the items:
    this->heading = new HeadingItem(this->name, this->scene(), this);
    this->description = new DemoTextItem(this->loadDescription(), Colors::contentFont(),
        Colors::heading, 500, this->scene(), this);
    int imgHeight = 340 - int(this->description->boundingRect().height()) + 50;
    this->screenshot = new ImageItem(QImage::fromData(MenuManager::instance()->getImage(this->name)),
          550, imgHeight, this->scene(), this);

    // Place the items on screen:
    this->heading->setPos(0, 3);
    this->description->setPos(0, this->heading->pos().y() + this->heading->boundingRect().height() + 10);
    this->screenshot->setPos(0, this->description->pos().y() + this->description->boundingRect().height() + 10);
}

QRectF ExampleContent::boundingRect() const
{
    return QRectF(0, 0, 500, 100);
}


