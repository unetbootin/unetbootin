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

#include "score.h"
#include "colors.h"
#include "demoitem.h"

Score::Score()
{
}

Score::~Score()
{
    // NB! Deleting all movies.
    qDeleteAll(this->index);
}

void Score::prepare(Movie *movie, RUN_MODE runMode, LOCK_MODE lockMode)
{
    if (lockMode == LOCK_ITEMS){
        for (int i=0; i<movie->size(); ++i){
            if (runMode == ONLY_IF_VISIBLE && !movie->at(i)->demoItem()->isVisible())
                continue;
            movie->at(i)->lockItem(true);
            movie->at(i)->prepare();
        }
    }
    else if (lockMode == UNLOCK_ITEMS){
        for (int i=0; i<movie->size(); ++i){
            if (runMode == ONLY_IF_VISIBLE && !movie->at(i)->demoItem()->isVisible())
                continue;
            movie->at(i)->lockItem(false);
            movie->at(i)->prepare();
        }
    }
    else {
        for (int i=0; i<movie->size(); ++i){
            if (runMode == ONLY_IF_VISIBLE && !movie->at(i)->demoItem()->isVisible())
                continue;
            movie->at(i)->prepare();
        }
    }
}

void Score::play(Movie *movie, RUN_MODE runMode)
{
    if (runMode == NEW_ANIMATION_ONLY){
        for (int i=0; i<movie->size(); ++i)
            if (movie->at(i)->notOwnerOfItem())
                movie->at(i)->play(true);
    }
    else if (runMode == ONLY_IF_VISIBLE){
        for (int i=0; i<movie->size(); ++i)
            if (movie->at(i)->demoItem()->isVisible())
                movie->at(i)->play(runMode == FROM_START);
    }
    else {
        for (int i=0; i<movie->size(); ++i)
            movie->at(i)->play(runMode == FROM_START);
    }
}

void Score::playMovie(const QString &indexName, RUN_MODE runMode, LOCK_MODE lockMode)
{
    MovieIndex::iterator movieIterator = this->index.find(indexName);
    if (movieIterator == this->index.end())
        return;

    Movie *movie = *movieIterator;
    this->prepare(movie, runMode, lockMode);
    this->play(movie, runMode);
}

void Score::queueMovie(const QString &indexName, RUN_MODE runMode, LOCK_MODE lockMode)
{
    MovieIndex::iterator movieIterator = this->index.find(indexName);
    if (movieIterator == this->index.end()){
        if (Colors::verbose)
            qDebug() << "Queuing movie:" << indexName << "(does not exist)";
        return;
    }

    Movie *movie = *movieIterator;
    this->prepare(movie, runMode, lockMode);
    this->playList.append(PlayListMember(movie, int(runMode)));
    if (Colors::verbose)
        qDebug() << "Queuing movie:" << indexName;
}

void Score::playQue()
{
    int movieCount = this->playList.size();
    for (int i=0; i<movieCount; i++)
        this->play(this->playList.at(i).movie, RUN_MODE(this->playList.at(i).runMode));
    this->playList.clear();
    if (Colors::verbose)
        qDebug() << "********* Playing que *********";
}

void Score::insertMovie(const QString &indexName, Movie *movie)
{
    this->index.insert(indexName, movie);
}

Movie *Score::insertMovie(const QString &indexName)
{
    Movie *movie = new Movie();
    insertMovie(indexName, movie);
    return movie;
}

