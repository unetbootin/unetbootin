/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qhelpengine.h"
#include "qhelpengine_p.h"
#include "qhelpdbreader_p.h"
#include "qhelpcontentwidget.h"
#include "qhelpindexwidget.h"
#include "qhelpsearchengine.h"
#include "qhelpcollectionhandler_p.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtGui/QApplication>
#include <QtSql/QSqlQuery>

QT_BEGIN_NAMESPACE

QHelpEnginePrivate::QHelpEnginePrivate()
    : QHelpEngineCorePrivate()
    , contentModel(0)
    , contentWidget(0)
    , indexModel(0)
    , indexWidget(0)
    , searchEngine(0)
{    
}

QHelpEnginePrivate::~QHelpEnginePrivate()
{
}

void QHelpEnginePrivate::init(const QString &collectionFile,
                              QHelpEngineCore *helpEngineCore)
{
    QHelpEngineCorePrivate::init(collectionFile, helpEngineCore);    

    contentModel = new QHelpContentModel(this);
    indexModel = new QHelpIndexModel(this);

    connect(helpEngineCore, SIGNAL(setupFinished()),
        this, SLOT(applyCurrentFilter()));
    connect(helpEngineCore, SIGNAL(currentFilterChanged(const QString&)),
        this, SLOT(applyCurrentFilter()));

}

void QHelpEnginePrivate::applyCurrentFilter()
{
    if (!error.isEmpty())
        return;
    contentModel->createContents(currentFilter);
    indexModel->createIndex(currentFilter);
}

void QHelpEnginePrivate::setContentsWidgetBusy()
{
    contentWidget->setCursor(Qt::WaitCursor);
}

void QHelpEnginePrivate::unsetContentsWidgetBusy()
{
    contentWidget->unsetCursor();
}

void QHelpEnginePrivate::setIndexWidgetBusy()
{
    indexWidget->setCursor(Qt::WaitCursor);
}

void QHelpEnginePrivate::unsetIndexWidgetBusy()
{
    indexWidget->unsetCursor();
}

void QHelpEnginePrivate::stopDataCollection()
{
    contentModel->invalidateContents(true);
    indexModel->invalidateIndex(true);
}



/*!
    \class QHelpEngine
    \inmodule QtHelp
    \brief The QHelpEngine class provides access to contents and
    indices of the help engine.


*/

/*!
    Constructs a new help engine with the given \a parent. The help
    engine uses the information stored in the \a collectionFile for
    providing help. If the collection file does not already exist,
    it will be created.
*/
QHelpEngine::QHelpEngine(const QString &collectionFile, QObject *parent)
    : QHelpEngineCore(d = new QHelpEnginePrivate(), parent)
{
    d->init(collectionFile, this);    
}

/*!
    Destroys the help engine object.
*/
QHelpEngine::~QHelpEngine()
{
    d->stopDataCollection();
}

/*!
    Returns the content model.
*/
QHelpContentModel *QHelpEngine::contentModel() const
{
    return d->contentModel;
}

/*!
    Returns the index model.
*/
QHelpIndexModel *QHelpEngine::indexModel() const
{
    return d->indexModel;
}

/*!
    Returns the content widget.
*/
QHelpContentWidget *QHelpEngine::contentWidget()
{
    if (!d->contentWidget) {
        d->contentWidget = new QHelpContentWidget();
        d->contentWidget->setModel(d->contentModel);
        connect(d->contentModel, SIGNAL(contentsCreationStarted()),
            d, SLOT(setContentsWidgetBusy()));
        connect(d->contentModel, SIGNAL(contentsCreated()),
            d, SLOT(unsetContentsWidgetBusy()));
    }
    return d->contentWidget;
}

/*!
    Returns the index widget.
*/
QHelpIndexWidget *QHelpEngine::indexWidget()
{
    if (!d->indexWidget) {
        d->indexWidget = new QHelpIndexWidget();
        d->indexWidget->setModel(d->indexModel);
        connect(d->indexModel, SIGNAL(indexCreationStarted()),
            d, SLOT(setIndexWidgetBusy()));
        connect(d->indexModel, SIGNAL(indexCreated()),
            d, SLOT(unsetIndexWidgetBusy()));
    }
    return d->indexWidget;
}

/*!
    Returns the default search engine.
*/
QHelpSearchEngine* QHelpEngine::searchEngine()
{
    if (!d->searchEngine)
        d->searchEngine = new QHelpSearchEngine(this, this);
    return d->searchEngine;
}

QT_END_NAMESPACE
