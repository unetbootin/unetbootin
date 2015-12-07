/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
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

#include <QtXml>

#include "fluidlauncher.h"


#define DEFAULT_INPUT_TIMEOUT 10000

FluidLauncher::FluidLauncher(QStringList* args)
{
    pictureFlowWidget = new PictureFlow();
    slideShowWidget = new SlideShow();
    inputTimer = new QTimer();

    QRect screen_size = QApplication::desktop()->screenGeometry();

    QObject::connect(pictureFlowWidget, SIGNAL(itemActivated(int)), this, SLOT(launchApplication(int)));
    QObject::connect(pictureFlowWidget, SIGNAL(inputReceived()),    this, SLOT(resetInputTimeout()));
    QObject::connect(slideShowWidget,   SIGNAL(inputReceived()),    this, SLOT(switchToLauncher()));
    QObject::connect(inputTimer,        SIGNAL(timeout()),          this, SLOT(inputTimedout()));

    inputTimer->setSingleShot(true);
    inputTimer->setInterval(DEFAULT_INPUT_TIMEOUT);

    pictureFlowWidget->setSlideSize(QSize( (screen_size.width()*2)/5, (screen_size.height()*2)/5 ));

    bool success;
    int configIndex = args->indexOf("-config");
    if ( (configIndex != -1) && (configIndex != args->count()-1) )
        success = loadConfig(args->at(configIndex+1));
    else
        success = loadConfig("config.xml");

    if (success) {
      populatePictureFlow();
  
      pictureFlowWidget->showFullScreen();
      inputTimer->start();
    }
}


bool FluidLauncher::loadConfig(QString configPath)
{
    QFile xmlFile(configPath);
    
    if (!xmlFile.exists() || (xmlFile.error() != QFile::NoError)) {
        qDebug() << "ERROR: Unable to open config file " << configPath;
        return false;
    }

    slideShowWidget->clearImages();

    QDomDocument xmlDoc;
    xmlDoc.setContent(&xmlFile, true);
    
    QDomElement rootElement = xmlDoc.documentElement();

    // Process the demos node:
    QDomNodeList demoNodes = rootElement.firstChildElement("demos").elementsByTagName("example");
    for (int i=0; i<demoNodes.size(); i++) {
        QDomElement element = demoNodes.item(i).toElement();

        if (element.hasAttribute("filename")) {
            DemoApplication* newDemo = new DemoApplication( 
                                                     element.attribute("filename"), 
                                                     element.attribute("name", "Unamed Demo"),
                                                     element.attribute("image"),
                                                     element.attribute("args").split(" "));
            demoList.append(newDemo);
        }
    }


    // Process the slideshow node:
    QDomElement slideshowElement = rootElement.firstChildElement("slideshow");

    if (slideshowElement.hasAttribute("timeout")) {
          bool valid;
          int timeout = slideshowElement.attribute("timeout").toInt(&valid);
          if (valid)
            inputTimer->setInterval(timeout);
    }

    if (slideshowElement.hasAttribute("interval")) {
          bool valid;
          int interval = slideshowElement.attribute("interval").toInt(&valid);
          if (valid)
            slideShowWidget->setSlideInterval(interval);
    }

    for (QDomNode node=slideshowElement.firstChild(); !node.isNull(); node=node.nextSibling()) {
        QDomElement element = node.toElement();

        if (element.tagName() == "imagedir")
            slideShowWidget->addImageDir(element.attribute("dir"));
        else if (element.tagName() == "image")
            slideShowWidget->addImage(element.attribute("image"));
    }

    // Append an exit Item
    DemoApplication* exitItem = new DemoApplication(QString(), QLatin1String("Exit Embedded Demo"), QString(), QStringList());
    demoList.append(exitItem);

    return true;
}


void FluidLauncher::populatePictureFlow()
{
    pictureFlowWidget->setSlideCount(demoList.count());

    for (int i=demoList.count()-1; i>=0; --i) {
        pictureFlowWidget->setSlide(i, *(demoList[i]->getImage()));
        pictureFlowWidget->setSlideCaption(i, demoList[i]->getCaption());
    }

    pictureFlowWidget->setCurrentSlide(demoList.count()/2);
}


void FluidLauncher::launchApplication(int index)
{
    // NOTE: Clearing the caches will free up more memory for the demo but will cause
    // a delay upon returning, as items are reloaded.
    //pictureFlowWidget->clearCaches();

    if (index == demoList.size() -1) {
        qApp->quit();
        return;
    }

    inputTimer->stop();

    QObject::connect(demoList[index], SIGNAL(demoStarted()), this, SLOT(demoStarted()));
    QObject::connect(demoList[index], SIGNAL(demoFinished()), this, SLOT(demoFinished()));

    demoList[index]->launch();
}


void FluidLauncher::switchToLauncher()
{
    slideShowWidget->stopShow();
    inputTimer->start();
}


void FluidLauncher::resetInputTimeout()
{
    if (inputTimer->isActive())
        inputTimer->start();
}

void FluidLauncher::inputTimedout()
{
    switchToSlideshow();
}


void FluidLauncher::switchToSlideshow()
{
    inputTimer->stop();
    slideShowWidget->startShow();
}

void FluidLauncher::demoFinished()
{
    pictureFlowWidget->showFullScreen();
    inputTimer->start();
}


void FluidLauncher::demoStarted()
{
    pictureFlowWidget->hide();
}

