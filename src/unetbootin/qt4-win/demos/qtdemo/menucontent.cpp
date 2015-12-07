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

#include "menucontent.h"
#include "colors.h"
#include "menumanager.h"
#include "demotextitem.h"
#include "headingitem.h"

MenuContentItem::MenuContentItem(const QDomElement &el, QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    this->name = el.attribute("name");
    this->heading = 0;
    this->description1 = 0;
    this->description2 = 0;

    if (el.tagName() == "demos")
        this->readmePath = QLibraryInfo::location(QLibraryInfo::DemosPath) + "/README";
    else
        this->readmePath = QLibraryInfo::location(QLibraryInfo::ExamplesPath) + "/" + el.attribute("dirname") + "/README";

}

void MenuContentItem::prepare()
{
    if (!this->prepared){
        this->prepared= true;
        this->createContent();
    }
}

void MenuContentItem::animationStopped(int id)
{
    if (this->name == Colors::rootMenuName)
        return; // Optimization hack.

    if (id == DemoItemAnimation::ANIM_OUT){
        // Free up some memory:
        delete this->heading;
        delete this->description1;
        delete this->description2;
        this->heading = 0;
        this->description1 = 0;
        this->description2 = 0;
        this->prepared = false;
    }
}

QString MenuContentItem::loadDescription(int startPara, int nrPara)
{
    QString result;
    QFile readme(this->readmePath);
    if (!readme.open(QFile::ReadOnly)){
        if (Colors::verbose)
            qDebug() << "- MenuContentItem::loadDescription: Could not load:" << this->readmePath;
        return "";
    }

    QTextStream in(&readme);
    // Skip a certain number of paragraphs:
    while (startPara)
        if (in.readLine().isEmpty()) --startPara;

    // Read in the number of wanted paragraphs:
    QString line = in.readLine();
    do {
        result += line + " ";
        line = in.readLine();
        if (line.isEmpty()){
            --nrPara;
            line = "<br><br>" + in.readLine();
        }
    } while (nrPara && !in.atEnd());

    return Colors::contentColor + result;
}

void MenuContentItem::createContent()
{
    // Create the items:
    this->heading = new HeadingItem(this->name, this->scene(), this);
    QString para1 = this->loadDescription(0, 1);
    if (para1.isEmpty())
        para1 = Colors::contentColor + QLatin1String("Could not load description. Ensure that the documentation for Qt is built.");
    QColor bgcolor = Colors::sceneBg1.darker(200);
    bgcolor.setAlpha(100);
    this->description1 = new DemoTextItem(para1, Colors::contentFont(), Colors::heading, 500, this->scene(), this, DemoTextItem::STATIC_TEXT);
    this->description2 = new DemoTextItem(this->loadDescription(1, 2), Colors::contentFont(), Colors::heading, 250, this->scene(), this, DemoTextItem::STATIC_TEXT);

    // Place the items on screen:
    this->heading->setPos(0, 3);
    this->description1->setPos(0, this->heading->pos().y() + this->heading->boundingRect().height() + 10);
    this->description2->setPos(0, this->description1->pos().y() + this->description1->boundingRect().height() + 15);
}

QRectF MenuContentItem::boundingRect() const
{
    return QRectF(0, 0, 500, 350);
}


