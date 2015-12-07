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

#include "menumanager.h"
#include "colors.h"
#include "menucontent.h"
#include "examplecontent.h"

MenuManager *MenuManager::pInstance = 0;

MenuManager * MenuManager::instance()
{
    if (!MenuManager::pInstance)
        MenuManager::pInstance = new MenuManager();
    return MenuManager::pInstance;
}

MenuManager::MenuManager()
{
    this->ticker = 0;
    this->tickerInAnim = 0;
    this->upButton = 0;
    this->downButton = 0;
    this->helpEngine = 0;
    this->score = new Score();
    this->currentMenu = QLatin1String("[no menu visible]");
    this->currentCategory = QLatin1String("[no category visible]");
    this->currentMenuButtons = QLatin1String("[no menu buttons visible]");
    this->currentInfo = QLatin1String("[no info visible]");
    this->currentMenuCode = -1;
    this->readXmlDocument();
    this->initHelpEngine();
}

MenuManager::~MenuManager()
{
    delete this->score;
    delete this->contentsDoc;
    delete this->helpEngine;
}

QByteArray MenuManager::getResource(const QString &name)
{
    QByteArray ba = this->helpEngine->fileData(name);
    if (Colors::verbose && ba.isEmpty())
        qDebug() << " - WARNING: Could not get " << name;
    return ba;
}

void MenuManager::readXmlDocument()
{
    this->contentsDoc = new QDomDocument();
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile file(":/xml/examples.xml");
    bool statusOK = this->contentsDoc->setContent(&file, true, &errorStr, &errorLine, &errorColumn);
    if (!statusOK){
        QMessageBox::critical(0,
                              QObject::tr("DOM Parser"),
                              QObject::tr("Could not read or find the contents document. Error at line %1, column %2:\n%3")
                              .arg(errorLine).arg(errorColumn).arg(errorStr)
                              );
        exit(-1);
    }
}

void MenuManager::initHelpEngine()
{
    this->helpRootUrl = QString("qthelp://com.trolltech.qt.%1%2%3/qdoc/")
        .arg(QT_VERSION >> 16).arg((QT_VERSION >> 8) & 0xFF)
        .arg(QT_VERSION & 0xFF);

    // Store help collection file in cache dir of assistant
    QString cacheDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation)
                     + QLatin1String("/Trolltech/Assistant/");
    QString helpDataFile = QString(QLatin1String("qtdemo_%1.qhc")).arg(QLatin1String(QT_VERSION_STR));

    QDir dir;
    if (!dir.exists(cacheDir))
        dir.mkpath(cacheDir);

    // Create help engine (and new
    // helpDataFile if it does not exist):
    this->helpEngine = new QHelpEngineCore(cacheDir + helpDataFile);
    this->helpEngine->setupData();

    QString qtDocRoot = QLibraryInfo::location(QLibraryInfo::DocumentationPath) + QLatin1String("/qch");
    qtDocRoot = QDir(qtDocRoot).absolutePath();

    QStringList qchFiles;
    qchFiles << QLatin1String("/qt.qch")
             << QLatin1String("/designer.qch")
             << QLatin1String("/linguist.qch");

    QString oldDir = helpEngine->customValue(QLatin1String("docDir"), QString()).toString();
    if (oldDir != qtDocRoot) {
        foreach (const QString &qchFile, qchFiles)
            helpEngine->unregisterDocumentation(QHelpEngineCore::namespaceName(qtDocRoot + qchFile));
    }

    // If the data that the engine will work
    // on is not yet registered, do it now:
    foreach (const QString &qchFile, qchFiles)
        helpEngine->registerDocumentation(qtDocRoot + qchFile);

    helpEngine->setCustomValue(QLatin1String("docDir"), qtDocRoot);
}

void MenuManager::itemSelected(int userCode, const QString &menuName)
{
    switch (userCode){
    case LAUNCH:
        this->launchExample(this->currentInfo);
        break;
    case DOCUMENTATION:
        this->showDocInAssistant(this->currentInfo);
        break;
    case QUIT:
        this->window->loop = false;
        QCoreApplication::quit();
        break;
    case FULLSCREEN:
        this->window->toggleFullscreen();
        break;
    case ROOT:
        // out:
        this->score->queueMovie(this->currentMenu + " -out", Score::FROM_START, Score::LOCK_ITEMS);
        this->score->queueMovie(this->currentMenuButtons + " -out", Score::FROM_START, Score::LOCK_ITEMS);
        this->score->queueMovie(this->currentInfo + " -out");
        this->score->queueMovie(this->currentInfo + " -buttons -out", Score::NEW_ANIMATION_ONLY);
        this->score->queueMovie("back -out", Score::ONLY_IF_VISIBLE);
        // book-keeping:
        this->currentMenuCode = ROOT;
        this->currentMenu = menuName + " -menu1";
        this->currentMenuButtons = menuName + " -buttons";
        this->currentInfo = menuName + " -info";
        // in:
        this->score->queueMovie("upndown -shake");
        this->score->queueMovie(this->currentMenu, Score::FROM_START, Score::UNLOCK_ITEMS);
        this->score->queueMovie(this->currentMenuButtons, Score::FROM_START, Score::UNLOCK_ITEMS);
        this->score->queueMovie(this->currentInfo);
        if (!Colors::noTicker){
            this->ticker->doIntroTransitions = true;
            this->tickerInAnim->startDelay = 2000;
            this->ticker->useGuideQt();
            this->score->queueMovie("ticker", Score::NEW_ANIMATION_ONLY);
            this->window->switchTimerOnOff(true);
        }
        break;
    case MENU1:
        // out:
        this->score->queueMovie(this->currentMenu + " -out", Score::FROM_START, Score::LOCK_ITEMS);
        this->score->queueMovie(this->currentMenuButtons + " -out", Score::FROM_START, Score::LOCK_ITEMS);
        this->score->queueMovie(this->currentInfo + " -out");
        // book-keeping:
        this->currentMenuCode = MENU1;
        this->currentCategory = menuName;
        this->currentMenu = menuName + " -menu1";
        this->currentInfo = menuName + " -info";
        // in:
        this->score->queueMovie("upndown -shake");
        this->score->queueMovie("back -in");
        this->score->queueMovie(this->currentMenu, Score::FROM_START, Score::UNLOCK_ITEMS);
        this->score->queueMovie(this->currentInfo);
        if (!Colors::noTicker)
            this->ticker->useGuideTt();
        break;
    case MENU2:
        // out:
        this->score->queueMovie(this->currentInfo + " -out", Score::NEW_ANIMATION_ONLY);
        this->score->queueMovie(this->currentInfo + " -buttons -out", Score::NEW_ANIMATION_ONLY);
        // book-keeping:
        this->currentMenuCode = MENU2;
        this->currentInfo = menuName;
        // in / shake:
        this->score->queueMovie("upndown -shake");
        this->score->queueMovie("back -shake");
        this->score->queueMovie(this->currentMenu + " -shake");
        this->score->queueMovie(this->currentInfo, Score::NEW_ANIMATION_ONLY);
        this->score->queueMovie(this->currentInfo + " -buttons", Score::NEW_ANIMATION_ONLY);
        if (!Colors::noTicker){
            this->score->queueMovie("ticker -out", Score::NEW_ANIMATION_ONLY);
            this->window->switchTimerOnOff(false);
        }
        break;
    case UP:{
        QString backMenu = this->info[this->currentMenu]["back"];
        if (!backMenu.isNull()){
            this->score->queueMovie(this->currentMenu + " -top_out", Score::FROM_START, Score::LOCK_ITEMS);
            this->score->queueMovie(backMenu + " -bottom_in", Score::FROM_START, Score::UNLOCK_ITEMS);
            this->currentMenu = backMenu;
        }
        break; }
    case DOWN:{
        QString moreMenu = this->info[this->currentMenu]["more"];
        if (!moreMenu.isNull()){
            this->score->queueMovie(this->currentMenu + " -bottom_out", Score::FROM_START, Score::LOCK_ITEMS);
            this->score->queueMovie(moreMenu + " -top_in", Score::FROM_START, Score::UNLOCK_ITEMS);
            this->currentMenu = moreMenu;
        }
        break; }
    case BACK:{
        if (this->currentMenuCode == MENU2){
            // out:
            this->score->queueMovie(this->currentInfo + " -out", Score::NEW_ANIMATION_ONLY);
            this->score->queueMovie(this->currentInfo + " -buttons -out", Score::NEW_ANIMATION_ONLY);
            // book-keeping:
            this->currentMenuCode = MENU1;
            this->currentMenuButtons = this->currentCategory + " -buttons";
            this->currentInfo = this->currentCategory + " -info";
            // in / shake:
            this->score->queueMovie("upndown -shake");
            this->score->queueMovie(this->currentMenu + " -shake");
            this->score->queueMovie(this->currentInfo, Score::NEW_ANIMATION_ONLY);
            this->score->queueMovie(this->currentInfo + " -buttons", Score::NEW_ANIMATION_ONLY);
            if (!Colors::noTicker){
                this->ticker->doIntroTransitions = false;
                this->tickerInAnim->startDelay = 500;
                this->score->queueMovie("ticker", Score::NEW_ANIMATION_ONLY);
                this->window->switchTimerOnOff(true);
            }
        } else if (this->currentMenuCode != ROOT)
            itemSelected(ROOT, Colors::rootMenuName);
        break; }
    }

    // update back- and more buttons
    bool noBackMenu = this->info[this->currentMenu]["back"].isNull();
    bool noMoreMenu = this->info[this->currentMenu]["more"].isNull();
    this->upButton->setState(noBackMenu ? TextButton::DISABLED : TextButton::OFF);
    this->downButton->setState(noMoreMenu ? TextButton::DISABLED : TextButton::OFF);

    if (this->score->hasQueuedMovies()){
        this->score->playQue();
        // Playing new movies might include
        // loading etc. So ignore the FPS
        // at this point
        this->window->fpsHistory.clear();
    }
}

void MenuManager::showDocInAssistant(const QString &name)
{
    QString url = this->resolveDocUrl(name);
    if (Colors::verbose)
        qDebug() << "Sending URL to Assistant:" << url;

    // Start assistant if it's not already running:
    if (this->assistantProcess.state() != QProcess::Running){
        QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
        app += QLatin1String("assistant");
#else
        app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif
        QStringList args;
        args << QLatin1String("-enableRemoteControl");
        this->assistantProcess.start(app, args);
        if (!this->assistantProcess.waitForStarted()) {
            QMessageBox::critical(0, tr("Qt Demo"), tr("Could not start Qt Assistant.").arg(app));
            return;
        }
    }

    // Send command through remote control even if the process
    // was started to activate assistant and bring it to front:
    QTextStream str(&this->assistantProcess);
    str << "SetSource " << url << QLatin1Char('\0') << endl;
}

void MenuManager::launchExample(const QString &name)
{
    QString executable = this->resolveExeFile(name);
#ifdef Q_OS_MAC
    if (Colors::verbose)
        qDebug() << "Launching:" << executable;
    bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(executable));
    if (!success){
        QMessageBox::critical(0, tr("Failed to launch the example"),
                          tr("Could not launch the example. Ensure that it has been built."),
                          QMessageBox::Cancel);
    }
#else // Not mac. To not break anything regarding dll's etc, keep it the way it was before:
    QProcess *process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), this, SLOT(exampleFinished()));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(exampleError(QProcess::ProcessError)));

#ifdef Q_OS_WIN
    //make sure it finds the dlls on windows
    QString curpath = QString::fromLocal8Bit(qgetenv("PATH").constData());
    QString newpath = QString("PATH=%1;%2").arg(QLibraryInfo::location(QLibraryInfo::BinariesPath), curpath);
    process->setEnvironment(QStringList(newpath));
#endif

    if (info[name]["changedirectory"] != "false"){
        QString workingDirectory = resolveDataDir(name);
        process->setWorkingDirectory(workingDirectory);
        if (Colors::verbose)
            qDebug() << "Setting working directory:" << workingDirectory;
    }

    if (Colors::verbose)
        qDebug() << "Launching:" << executable;
    process->start(executable);
#endif
}

void MenuManager::exampleFinished()
{
}

void MenuManager::exampleError(QProcess::ProcessError error)
{
    if (error != QProcess::Crashed)
        QMessageBox::critical(0, tr("Failed to launch the example"),
                          tr("Could not launch the example. Ensure that it has been built."),
                          QMessageBox::Cancel);
}

void MenuManager::init(MainWindow *window)
{
    this->window = window;

    // Create div:
    this->createTicker();
    this->createUpnDownButtons();
    this->createBackButton();

    // Create first level menu:
    QDomElement rootElement = this->contentsDoc->documentElement();
    this->createRootMenu(rootElement);

    // Create second level menus:
    QDomNode level2MenuNode = rootElement.firstChild();
    while (!level2MenuNode.isNull()){
        QDomElement level2MenuElement = level2MenuNode.toElement();
        this->createSubMenu(level2MenuElement);

        // create leaf menu and example info:
        QDomNode exampleNode = level2MenuElement.firstChild();
        while (!exampleNode.isNull()){
            QDomElement exampleElement = exampleNode.toElement();
            this->readInfoAboutExample(exampleElement);
            this->createLeafMenu(exampleElement);
            exampleNode = exampleNode.nextSibling();
        }

        level2MenuNode = level2MenuNode.nextSibling();
    }
}

void MenuManager::readInfoAboutExample(const QDomElement &example)
{
    QString name = example.attribute("name");
    if (this->info.contains(name))
        qWarning() << "__WARNING: MenuManager::readInfoAboutExample: Demo/example with name"
                    << name << "appears twize in the xml-file!__";

    this->info[name]["filename"] = example.attribute("filename");
    this->info[name]["category"] = example.parentNode().toElement().tagName();
    this->info[name]["dirname"] = example.parentNode().toElement().attribute("dirname");
    this->info[name]["changedirectory"] = example.attribute("changedirectory");
    this->info[name]["image"] = example.attribute("image");
}

QString MenuManager::resolveDataDir(const QString &name)
{
    QString dirName = this->info[name]["dirname"];
    QString category = this->info[name]["category"];
    QString fileName = this->info[name]["filename"];

    QDir dir;
    if (category == "demos")
        dir = QDir(QLibraryInfo::location(QLibraryInfo::DemosPath));
    else
        dir = QDir(QLibraryInfo::location(QLibraryInfo::ExamplesPath));

    dir.cd(dirName);
    dir.cd(fileName);
    return dir.absolutePath();
}

QString MenuManager::resolveExeFile(const QString &name)
{
    QString dirName = this->info[name]["dirname"];
    QString category = this->info[name]["category"];
    QString fileName = this->info[name]["filename"];

    QDir dir;
    if (category == "demos")
        dir = QDir(QLibraryInfo::location(QLibraryInfo::DemosPath));
    else
        dir = QDir(QLibraryInfo::location(QLibraryInfo::ExamplesPath));

    dir.cd(dirName);
    dir.cd(fileName);

    QFile unixFile(dir.path() + "/" + fileName);
    if (unixFile.exists()) return unixFile.fileName();
    QFile winR(dir.path() + "\\release\\" + fileName + ".exe");
    if (winR.exists()) return winR.fileName();
    QFile winD(dir.path() + "\\debug\\" + fileName + ".exe");
    if (winD.exists()) return winD.fileName();
    QFile mac(dir.path() + "/" + fileName + ".app");
    if (mac.exists()) return mac.fileName();

    if (Colors::verbose)
        qDebug() << "- WARNING: Could not resolve executable:" << dir.path() << fileName;
    return "__executable not found__";
}

QString MenuManager::resolveDocUrl(const QString &name)
{
    QString dirName = this->info[name]["dirname"];
    QString category = this->info[name]["category"];
    QString fileName = this->info[name]["filename"];

    if (category == "demos")
        return this->helpRootUrl + "demos-" + fileName + ".html";
    else
        return this->helpRootUrl + dirName.replace("/", "-") + "-" + fileName + ".html";
}

QString MenuManager::resolveImageUrl(const QString &name)
{
    return this->helpRootUrl + "images/" + name;
}

QByteArray MenuManager::getHtml(const QString &name)
{
    return getResource(this->resolveDocUrl(name));
}

QByteArray MenuManager::getImage(const QString &name)
{
    QString imageName = this->info[name]["image"];
    QString category = this->info[name]["category"];
    QString fileName = this->info[name]["filename"];

    if (imageName.isEmpty()){
        if (category == "demos")
            imageName = fileName + "-demo.png";
        else
            imageName = fileName + "-example.png";
        if ((getResource(resolveImageUrl(imageName))).isEmpty())
            imageName = fileName + ".png";
        if ((getResource(resolveImageUrl(imageName))).isEmpty())
            imageName = fileName + "example.png";
    }
    return getResource(resolveImageUrl(imageName));
}


void MenuManager::createRootMenu(const QDomElement &el)
{
    QString name = el.attribute("name");
    createMenu(el, MENU1);
    createInfo(new MenuContentItem(el, this->window->scene, 0), name + " -info");

    Movie *menuButtonsIn = this->score->insertMovie(name + " -buttons");
    Movie *menuButtonsOut = this->score->insertMovie(name + " -buttons -out");
    createLowLeftButton(QLatin1String("Quit"), QUIT, menuButtonsIn, menuButtonsOut, 0);
    createLowRightButton("Toggle fullscreen", FULLSCREEN, menuButtonsIn, menuButtonsOut, 0);
}

void MenuManager::createSubMenu(const QDomElement &el)
{
    QString name = el.attribute("name");
    createMenu(el, MENU2);
    createInfo(new MenuContentItem(el, this->window->scene, 0), name + " -info");
}

void MenuManager::createLeafMenu(const QDomElement &el)
{
    QString name = el.attribute("name");
    createInfo(new ExampleContent(name, this->window->scene, 0), name);

    Movie *infoButtonsIn = this->score->insertMovie(name + " -buttons");
    Movie *infoButtonsOut = this->score->insertMovie(name + " -buttons -out");
    createLowRightLeafButton("Documentation", 600, DOCUMENTATION, infoButtonsIn, infoButtonsOut, 0);
    if (el.attribute("executable") != "false")
        createLowRightLeafButton("Launch", 405, LAUNCH, infoButtonsIn, infoButtonsOut, 0);
}

void MenuManager::createMenu(const QDomElement &category, BUTTON_TYPE type)
{
    qreal sw = this->window->scene->sceneRect().width();
    int xOffset = 15;
    int yOffset = 10;
    int maxExamples = Colors::menuCount;
    int menuIndex = 1;
    QString name = category.attribute("name");
    QDomNode currentNode = category.firstChild();
    QString currentMenu = name + QLatin1String(" -menu") + QString::number(menuIndex);

    while (!currentNode.isNull()){
        Movie *movieIn = this->score->insertMovie(currentMenu);
        Movie *movieOut = this->score->insertMovie(currentMenu + " -out");
        Movie *movieNextTopOut = this->score->insertMovie(currentMenu + " -top_out");
        Movie *movieNextBottomOut = this->score->insertMovie(currentMenu + " -bottom_out");
        Movie *movieNextTopIn = this->score->insertMovie(currentMenu + " -top_in");
        Movie *movieNextBottomIn = this->score->insertMovie(currentMenu + " -bottom_in");
        Movie *movieShake = this->score->insertMovie(currentMenu + " -shake");

        int i = 0;
        while (!currentNode.isNull() && i < maxExamples){
            TextButton *item;

            // create normal menu button
            QString label = currentNode.toElement().attribute("name");
            item = new TextButton(label, TextButton::LEFT, type, this->window->scene, 0);
            currentNode = currentNode.nextSibling();

#ifndef QT_OPENGL_SUPPORT
            if (currentNode.toElement().attribute("dirname") == "opengl")
                currentNode = currentNode.nextSibling();
#endif

            item->setRecursiveVisible(false);
            item->setZValue(10);
            qreal ih = item->sceneBoundingRect().height();
            qreal iw = item->sceneBoundingRect().width();
            qreal ihp = ih + 3;

            // create in-animation:
            DemoItemAnimation *anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_IN);
            anim->setDuration(float(1000 + (i * 20)) * Colors::animSpeedButtons);
            anim->setStartPos(QPointF(xOffset, -ih));
            anim->setPosAt(0.20, QPointF(xOffset, -ih));
            anim->setPosAt(0.50, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY + (10 * float(i / 4.0f))));
            anim->setPosAt(0.60, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(0.70, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY + (5 * float(i / 4.0f))));
            anim->setPosAt(0.80, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(0.90, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY + (2 * float(i / 4.0f))));
            anim->setPosAt(1.00, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            movieIn->append(anim);

            // create out-animation:
            anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_OUT);
            anim->hideOnFinished = true;
            anim->setDuration((700 + (30 * i)) * Colors::animSpeedButtons);
            anim->setStartPos(QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(0.60, QPointF(xOffset, 600 - ih - ih));
            anim->setPosAt(0.65, QPointF(xOffset + 20, 600 - ih));
            anim->setPosAt(1.00, QPointF(sw + iw, 600 - ih));
            movieOut->append(anim);

            // create shake-animation:
            anim = new DemoItemAnimation(item);
            anim->setDuration(700 * Colors::animSpeedButtons);
            anim->setStartPos(QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(0.55, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY - i*2.0));
            anim->setPosAt(0.70, QPointF(xOffset - 10, (i * ihp) + yOffset + Colors::contentStartY - i*1.5));
            anim->setPosAt(0.80, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY - i*1.0));
            anim->setPosAt(0.90, QPointF(xOffset - 2, (i * ihp) + yOffset + Colors::contentStartY - i*0.5));
            anim->setPosAt(1.00, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            movieShake->append(anim);

            // create next-menu top-out-animation:
            anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_OUT);
            anim->hideOnFinished = true;
            anim->setDuration((200 + (30 * i)) * Colors::animSpeedButtons);
            anim->setStartPos(QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(0.70, QPointF(xOffset, yOffset + Colors::contentStartY));
            anim->setPosAt(1.00, QPointF(-iw, yOffset + Colors::contentStartY));
            movieNextTopOut->append(anim);

            // create next-menu bottom-out-animation:
            anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_OUT);
            anim->hideOnFinished = true;
            anim->setDuration((200 + (30 * i)) * Colors::animSpeedButtons);
            anim->setStartPos(QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(0.70, QPointF(xOffset, (maxExamples * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(1.00, QPointF(-iw, (maxExamples * ihp) + yOffset + Colors::contentStartY));
            movieNextBottomOut->append(anim);

            // create next-menu top-in-animation:
            anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_IN);
            anim->setDuration((700 - (30 * i)) * Colors::animSpeedButtons);
            anim->setStartPos(QPointF(-iw, yOffset + Colors::contentStartY));
            anim->setPosAt(0.30, QPointF(xOffset, yOffset + Colors::contentStartY));
            anim->setPosAt(1.00, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            movieNextTopIn->append(anim);

            // create next-menu bottom-in-animation:
            int reverse = maxExamples - i;
            anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_IN);
            anim->setDuration((1000 - (30 * reverse)) * Colors::animSpeedButtons);
            anim->setStartPos(QPointF(-iw, (maxExamples * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(0.30, QPointF(xOffset, (maxExamples * ihp) + yOffset + Colors::contentStartY));
            anim->setPosAt(1.00, QPointF(xOffset, (i * ihp) + yOffset + Colors::contentStartY));
            movieNextBottomIn->append(anim);

            i++;
        }

        if (!currentNode.isNull() && i == maxExamples){
            // We need another menu, so register for 'more' and 'back' buttons
            ++menuIndex;
            this->info[currentMenu]["more"] = name + QLatin1String(" -menu") + QString::number(menuIndex);
            currentMenu = name + QLatin1String(" -menu") + QString::number(menuIndex);
            this->info[currentMenu]["back"] = name + QLatin1String(" -menu") + QString::number(menuIndex - 1);
        }
    }
}


void MenuManager::createLowLeftButton(const QString &label, BUTTON_TYPE type,
    Movie *movieIn, Movie *movieOut, Movie *movieShake, const QString &menuString)
{
    TextButton *button = new TextButton(label, TextButton::RIGHT, type, this->window->scene, 0, TextButton::PANEL);
    if (!menuString.isNull())
        button->setMenuString(menuString);
    button->setRecursiveVisible(false);
    button->setZValue(10);

    qreal iw = button->sceneBoundingRect().width();
    int xOffset = 15;

    // create in-animation:
    DemoItemAnimation *buttonIn = new DemoItemAnimation(button, DemoItemAnimation::ANIM_IN);
    buttonIn->setDuration(1800 * Colors::animSpeedButtons);
    buttonIn->setStartPos(QPointF(-iw, Colors::contentStartY + Colors::contentHeight - 35));
    buttonIn->setPosAt(0.5, QPointF(-iw, Colors::contentStartY + Colors::contentHeight - 35));
    buttonIn->setPosAt(0.7, QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 35));
    buttonIn->setPosAt(1.0, QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 26));
    movieIn->append(buttonIn);

    // create out-animation:
    DemoItemAnimation *buttonOut = new DemoItemAnimation(button, DemoItemAnimation::ANIM_OUT);
    buttonOut->hideOnFinished = true;
    buttonOut->setDuration(400 * Colors::animSpeedButtons);
    buttonOut->setStartPos(QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 26));
    buttonOut->setPosAt(1.0, QPointF(-iw, Colors::contentStartY + Colors::contentHeight - 26));
    movieOut->append(buttonOut);

    if (movieShake){
        DemoItemAnimation *shakeAnim = new DemoItemAnimation(button, DemoItemAnimation::ANIM_UNSPECIFIED);
        shakeAnim->timeline->setCurveShape(QTimeLine::LinearCurve);
        shakeAnim->setDuration(650);
        shakeAnim->setStartPos(buttonIn->posAt(1.0f));
        shakeAnim->setPosAt(0.60, buttonIn->posAt(1.0f));
        shakeAnim->setPosAt(0.70, buttonIn->posAt(1.0f) + QPointF(-3, 0));
        shakeAnim->setPosAt(0.80, buttonIn->posAt(1.0f) + QPointF(2, 0));
        shakeAnim->setPosAt(0.90, buttonIn->posAt(1.0f) + QPointF(-1, 0));
        shakeAnim->setPosAt(1.00, buttonIn->posAt(1.0f));
        movieShake->append(shakeAnim);
    }
}

void MenuManager::createLowRightButton(const QString &label, BUTTON_TYPE type, Movie *movieIn, Movie *movieOut, Movie * /*movieShake*/)
{
    TextButton *item = new TextButton(label, TextButton::RIGHT, type, this->window->scene, 0, TextButton::PANEL);
    item->setRecursiveVisible(false);
    item->setZValue(10);

    qreal sw = this->window->scene->sceneRect().width();
    int xOffset = 70;

    // create in-animation:
    DemoItemAnimation *anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_IN);
    anim->setDuration(1800 * Colors::animSpeedButtons);
    anim->setStartPos(QPointF(sw, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.5, QPointF(sw, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.7, QPointF(xOffset + 535, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(1.0, QPointF(xOffset + 535, Colors::contentStartY + Colors::contentHeight - 26));
    movieIn->append(anim);

    // create out-animation:
    anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_OUT);
    anim->hideOnFinished = true;
    anim->setDuration(400 * Colors::animSpeedButtons);
    anim->setStartPos(QPointF(xOffset + 535, Colors::contentStartY + Colors::contentHeight - 26));
    anim->setPosAt(1.0, QPointF(sw, Colors::contentStartY + Colors::contentHeight - 26));
    movieOut->append(anim);
}

void MenuManager::createLowRightLeafButton(const QString &label, int xOffset, BUTTON_TYPE type, Movie *movieIn, Movie *movieOut, Movie * /*movieShake*/)
{
    TextButton *item = new TextButton(label, TextButton::RIGHT, type, this->window->scene, 0, TextButton::PANEL);
    item->setRecursiveVisible(false);
    item->setZValue(10);

    qreal sw = this->window->scene->sceneRect().width();
    qreal sh = this->window->scene->sceneRect().height();

    // create in-animation:
    DemoItemAnimation *anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_IN);
    anim->setDuration(1050 * Colors::animSpeedButtons);
    anim->setStartPos(QPointF(sw, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.10, QPointF(sw, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.30, QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.35, QPointF(xOffset + 30, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.40, QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.45, QPointF(xOffset + 5, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(0.50, QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 35));
    anim->setPosAt(1.00, QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 26));
    movieIn->append(anim);

    // create out-animation:
    anim = new DemoItemAnimation(item, DemoItemAnimation::ANIM_OUT);
    anim->hideOnFinished = true;
    anim->setDuration(300 * Colors::animSpeedButtons);
    anim->setStartPos(QPointF(xOffset, Colors::contentStartY + Colors::contentHeight - 26));
    anim->setPosAt(1.0, QPointF(xOffset, sh));
    movieOut->append(anim);
}

void MenuManager::createInfo(DemoItem *item, const QString &name)
{
    Movie *movie_in = this->score->insertMovie(name);
    Movie *movie_out = this->score->insertMovie(name + " -out");
    item->setZValue(8);
    item->setRecursiveVisible(false);

    float xOffset = 230.0f;
    DemoItemAnimation *infoIn = new DemoItemAnimation(item, DemoItemAnimation::ANIM_IN);
    infoIn->timeline->setCurveShape(QTimeLine::LinearCurve);
    infoIn->setDuration(650);
    infoIn->setStartPos(QPointF(this->window->scene->sceneRect().width(), Colors::contentStartY));
    infoIn->setPosAt(0.60, QPointF(xOffset, Colors::contentStartY));
    infoIn->setPosAt(0.70, QPointF(xOffset + 20, Colors::contentStartY));
    infoIn->setPosAt(0.80, QPointF(xOffset, Colors::contentStartY));
    infoIn->setPosAt(0.90, QPointF(xOffset + 7, Colors::contentStartY));
    infoIn->setPosAt(1.00, QPointF(xOffset, Colors::contentStartY));
    movie_in->append(infoIn);

    DemoItemAnimation *infoOut = new DemoItemAnimation(item, DemoItemAnimation::ANIM_OUT);
    infoOut->timeline->setCurveShape(QTimeLine::EaseInCurve);
    infoOut->setDuration(300);
    infoOut->hideOnFinished = true;
    infoOut->setStartPos(QPointF(xOffset, Colors::contentStartY));
    infoOut->setPosAt(1.0, QPointF(-600, Colors::contentStartY));
    movie_out->append(infoOut);
}

void MenuManager::createTicker()
{
    if (!Colors::noTicker){
        Movie *movie_in = this->score->insertMovie("ticker");
        Movie *movie_out = this->score->insertMovie("ticker -out");
        Movie *movie_activate = this->score->insertMovie("ticker -activate");
        Movie *movie_deactivate = this->score->insertMovie("ticker -deactivate");

        this->ticker = new ItemCircleAnimation(this->window->scene, 0);
        this->ticker->setZValue(50);
        this->ticker->hide();

        // Move ticker in:
        int qtendpos = 485;
        int qtPosY = 120;
        this->tickerInAnim = new DemoItemAnimation(this->ticker, DemoItemAnimation::ANIM_IN);
        this->tickerInAnim->setDuration(500);
        this->tickerInAnim->setStartPos(QPointF(this->window->scene->sceneRect().width(), Colors::contentStartY + qtPosY));
        this->tickerInAnim->setPosAt(0.60, QPointF(qtendpos, Colors::contentStartY + qtPosY));
        this->tickerInAnim->setPosAt(0.70, QPointF(qtendpos + 30, Colors::contentStartY + qtPosY));
        this->tickerInAnim->setPosAt(0.80, QPointF(qtendpos, Colors::contentStartY + qtPosY));
        this->tickerInAnim->setPosAt(0.90, QPointF(qtendpos + 5, Colors::contentStartY + qtPosY));
        this->tickerInAnim->setPosAt(1.00, QPointF(qtendpos, Colors::contentStartY + qtPosY));
        movie_in->append(this->tickerInAnim);

        // Move ticker out:
        DemoItemAnimation *qtOut = new DemoItemAnimation(this->ticker, DemoItemAnimation::ANIM_OUT);
        qtOut->hideOnFinished = true;
        qtOut->setDuration(500);
        qtOut->setStartPos(QPointF(qtendpos, Colors::contentStartY + qtPosY));
        qtOut->setPosAt(1.00, QPointF(this->window->scene->sceneRect().width() + 700, Colors::contentStartY + qtPosY));
        movie_out->append(qtOut);

        // Move ticker in on activate:
        DemoItemAnimation *qtActivate = new DemoItemAnimation(this->ticker);
        qtActivate->setDuration(400);
        qtActivate->setStartPos(QPointF(this->window->scene->sceneRect().width(), Colors::contentStartY + qtPosY));
        qtActivate->setPosAt(0.60, QPointF(qtendpos, Colors::contentStartY + qtPosY));
        qtActivate->setPosAt(0.70, QPointF(qtendpos + 30, Colors::contentStartY + qtPosY));
        qtActivate->setPosAt(0.80, QPointF(qtendpos, Colors::contentStartY + qtPosY));
        qtActivate->setPosAt(0.90, QPointF(qtendpos + 5, Colors::contentStartY + qtPosY));
        qtActivate->setPosAt(1.00, QPointF(qtendpos, Colors::contentStartY + qtPosY));
        movie_activate->append(qtActivate);

        // Move ticker out on deactivate:
        DemoItemAnimation *qtDeactivate = new DemoItemAnimation(this->ticker);
        qtDeactivate->hideOnFinished = true;
        qtDeactivate->setDuration(400);
        qtDeactivate->setStartPos(QPointF(qtendpos, Colors::contentStartY + qtPosY));
        qtDeactivate->setPosAt(1.00, QPointF(qtendpos, 800));
        movie_deactivate->append(qtDeactivate);
    }
}

void MenuManager::createUpnDownButtons()
{
    float xOffset = 15.0f;
    float yOffset = 450.0f;

    this->upButton = new TextButton("", TextButton::LEFT, MenuManager::UP, this->window->scene, 0, TextButton::UP);
    this->upButton->prepare();
    this->upButton->setPos(xOffset, yOffset);
    this->upButton->setState(TextButton::DISABLED);

    this->downButton = new TextButton("", TextButton::LEFT, MenuManager::DOWN, this->window->scene, 0, TextButton::DOWN);
    this->downButton->prepare();
    this->downButton->setPos(xOffset + 10 + this->downButton->sceneBoundingRect().width(), yOffset);

    Movie *movieShake = this->score->insertMovie("upndown -shake");

    DemoItemAnimation *shakeAnim = new DemoItemAnimation(this->upButton, DemoItemAnimation::ANIM_UNSPECIFIED);
    shakeAnim->timeline->setCurveShape(QTimeLine::LinearCurve);
    shakeAnim->setDuration(650);
    shakeAnim->setStartPos(this->upButton->pos());
    shakeAnim->setPosAt(0.60, this->upButton->pos());
    shakeAnim->setPosAt(0.70, this->upButton->pos() + QPointF(-2, 0));
    shakeAnim->setPosAt(0.80, this->upButton->pos() + QPointF(1, 0));
    shakeAnim->setPosAt(0.90, this->upButton->pos() + QPointF(-1, 0));
    shakeAnim->setPosAt(1.00, this->upButton->pos());
    movieShake->append(shakeAnim);

    shakeAnim = new DemoItemAnimation(this->downButton, DemoItemAnimation::ANIM_UNSPECIFIED);
    shakeAnim->timeline->setCurveShape(QTimeLine::LinearCurve);
    shakeAnim->setDuration(650);
    shakeAnim->setStartPos(this->downButton->pos());
    shakeAnim->setPosAt(0.60, this->downButton->pos());
    shakeAnim->setPosAt(0.70, this->downButton->pos() + QPointF(-5, 0));
    shakeAnim->setPosAt(0.80, this->downButton->pos() + QPointF(-3, 0));
    shakeAnim->setPosAt(0.90, this->downButton->pos() + QPointF(-1, 0));
    shakeAnim->setPosAt(1.00, this->downButton->pos());
    movieShake->append(shakeAnim);
}

void MenuManager::createBackButton()
{
    Movie *backIn = this->score->insertMovie("back -in");
    Movie *backOut = this->score->insertMovie("back -out");
    Movie *backShake = this->score->insertMovie("back -shake");
    createLowLeftButton(QLatin1String("Back"), ROOT, backIn, backOut, backShake, Colors::rootMenuName);
}
