/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "qwizard_container.h"

#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtGui/QWizard>
#include <QtGui/QWizardPage>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

QWizardContainer::QWizardContainer(QWizard *widget, QObject *parent) :
    QObject(parent),
    m_wizard(widget)
{
}

QWizardContainer::Pages QWizardContainer::pages() const
{
    return qFindChildren<QWizardPage *>(m_wizard);
}

int QWizardContainer::count() const
{
    return pages().size();
}

QWidget *QWizardContainer::widget(int index) const
{
    if (index < 0)
        return 0;
    QWidget *rc = pages().at(index);
    // Hack: In some modi, pages have height 0.
    if (rc->isVisible() && rc->height() == 0)
        rc->resize(m_wizard->size());
    return rc;
}

int QWizardContainer::currentIndex() const
{
    QWizardPage *currentPage = m_wizard->currentPage();
    if (!currentPage)
        return  -1;
     return pages().indexOf(currentPage);
}

void QWizardContainer::setCurrentIndex(int /* index*/)
{
    qDebug() << "** WARNING QWizardContainer::setCurrentIndex is not implemented";
}

void QWizardContainer::addWidget(QWidget *widget)
{

    QWizardPage *page = qobject_cast<QWizardPage *>(widget);
    if (!page) {
        qDebug() << "** WARNING Attempt to add oject that is not of class WizardPage to a QWizard";
        return;
    }
    m_wizard->addPage(page);
}

void QWizardContainer::insertWidget(int /* index*/, QWidget *widget)
{
    qDebug() << "** WARNING QWizardContainer::insertWidget is not implemented, defaulting to add";
    addWidget(widget);
}

void QWizardContainer::remove(int /* index */)
{
    qDebug() << "** WARNING QWizardContainer::remove is not implemented";
}
}

QT_END_NAMESPACE
