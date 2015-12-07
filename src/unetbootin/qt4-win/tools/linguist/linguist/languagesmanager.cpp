/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "languagesmanager.h"
#include "messagemodel.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE

LanguagesManager::LanguagesManager(MessageModel *mainLanguage, QObject *w) : m_w(w)
{
    m_messageModels.append(mainLanguage);
}

LanguagesManager::~LanguagesManager()
{
    foreach (MessageModel *mm, m_messageModels) {
        mm->clearContextList();
        delete mm;
    }
}

bool LanguagesManager::openAuxLanguageFile( const QString &name )
{
    if (name.isEmpty())
        return false;

    foreach (MessageModel *model, m_messageModels)
        if (model->srcFileName() == QFileInfo(name).absoluteFilePath())
            return true;

    MessageModel *auxModel = new MessageModel(m_w);
    if (!auxModel->load(name)) {
        return false;
    }
    m_messageModels.append(auxModel);
    emit listChanged();

    return true;
}

void LanguagesManager::removeAuxLanguage(MessageModel *model)
{
    m_messageModels.removeOne(model);
    delete model;
    emit listChanged();
}

void LanguagesManager::moveAuxLanguage(MessageModel *model, int newPos)
{
    m_messageModels.removeOne(model);
    m_messageModels.insert(newPos, model);
    emit listChanged();
}

QList<MessageModel*> LanguagesManager::auxModels()
{
    QList<MessageModel*> auxLanguages(m_messageModels);
    auxLanguages.removeFirst();
    return auxLanguages;
}

QT_END_NAMESPACE
