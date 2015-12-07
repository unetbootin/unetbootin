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

#include "q3wizard_container.h"
#include <Qt3Support/Q3Wizard>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/private/ui4_p.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

static const char *currentPageText = "currentPageText";

Q3WizardHelper::Q3WizardHelper(Q3Wizard *wizard)
    : QObject(wizard),
    m_wizard(wizard)
{
    connect(m_wizard, SIGNAL(selected(const QString &)), this, SLOT(slotCurrentChanged()));
}

void Q3WizardHelper::slotCurrentChanged()
{
    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_wizard)) {
        fw->clearSelection();
        fw->selectWidget(m_wizard, true);
    }
}

Q3WizardExtraInfo::Q3WizardExtraInfo(Q3Wizard *wizard, QDesignerFormEditorInterface *core, QObject *parent)
    : QObject(parent), m_wizard(wizard), m_core(core)
{}

QWidget *Q3WizardExtraInfo::widget() const
{ return m_wizard; }

Q3Wizard *Q3WizardExtraInfo::wizard() const
{ return m_wizard; }

QDesignerFormEditorInterface *Q3WizardExtraInfo::core() const
{ return m_core; }

bool Q3WizardExtraInfo::saveUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }

bool Q3WizardExtraInfo::loadUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }

bool Q3WizardExtraInfo::saveWidgetExtraInfo(DomWidget *ui_widget)
{
    int i = 0;
    foreach (DomWidget *ui_child, ui_widget->elementWidget()) {
        DomProperty *p = new DomProperty();
        p->setAttributeName(QLatin1String("title"));
        DomString *str = new DomString();
        str->setText(wizard()->title(wizard()->page(i)));
        p->setElementString(str);

        QList<DomProperty *> attributes = ui_child->elementAttribute();
        attributes.append(p);
        ui_child->setElementAttribute(attributes);

        i++;
    }
    return true;
}

bool Q3WizardExtraInfo::loadWidgetExtraInfo(DomWidget *ui_widget)
{
    int i = 0;
    foreach (const DomWidget *ui_child, ui_widget->elementWidget()) {
        foreach (const DomProperty *ui_prop, ui_child->elementAttribute()) {
            if (ui_prop->attributeName() == QLatin1String("title")) {
                const DomString *ui_string = ui_prop->elementString();
                if (ui_string)
                    wizard()->setTitle(wizard()->page(i), ui_string->text());
            }
        }
        i++;
    }
    return true;
}

Q3WizardExtraInfoFactory::Q3WizardExtraInfoFactory(QDesignerFormEditorInterface *core, QExtensionManager *parent)
    : QExtensionFactory(parent), m_core(core)
{}

QObject *Q3WizardExtraInfoFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerExtraInfoExtension))
        return 0;

    if (Q3Wizard *w = qobject_cast<Q3Wizard *>(object))
        return new Q3WizardExtraInfo(w, m_core, parent);

    return 0;
}

Q3WizardContainer::Q3WizardContainer(Q3Wizard *wizard, QObject *parent)
    : QObject(parent),
      m_wizard(wizard)
{}

int Q3WizardContainer::count() const
{
    return m_wizard->pageCount();
}

QWidget *Q3WizardContainer::widget(int index) const
{
    Q_ASSERT(index != -1);
    return m_wizard->page(index);
}

int Q3WizardContainer::currentIndex() const
{
    if (m_wizard->currentPage() == 0 && m_wizard->pageCount())
        m_wizard->showPage(widget(0));

    return m_wizard->indexOf(m_wizard->currentPage());
}

void Q3WizardContainer::setCurrentIndex(int index)
{
    const bool blocked = m_wizard->signalsBlocked();
    m_wizard->blockSignals(true);
    m_wizard->showPage(widget(index));
    m_wizard->blockSignals(blocked);
}

void Q3WizardContainer::addWidget(QWidget *widget)
{
    m_wizard->addPage(widget, tr("Page"));
}

void Q3WizardContainer::insertWidget(int index, QWidget *widget)
{
    m_wizard->insertPage(widget, tr("Page"), index);
}

void Q3WizardContainer::remove(int index)
{
    m_wizard->removePage(widget(index));
}

Q3WizardContainerFactory::Q3WizardContainerFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}

QObject *Q3WizardContainerFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension))
        return 0;

    if (Q3Wizard *w = qobject_cast<Q3Wizard*>(object))
        return new Q3WizardContainer(w, parent);

    return 0;
}

Q3WizardPropertySheet::Q3WizardPropertySheet(Q3Wizard *object, QObject *parent)
    : QDesignerPropertySheet(object, parent), m_wizard(object)
{
    createFakeProperty(QLatin1String(currentPageText), QString());
}

void Q3WizardPropertySheet::setProperty(int index, const QVariant &value)
{
    const QString prop = propertyName(index);
    if (prop == QLatin1String(currentPageText)) {
        m_wizard->setTitle(m_wizard->currentPage(), value.toString());
        return;
    }
    QDesignerPropertySheet::setProperty(index, value);
}

QVariant Q3WizardPropertySheet::property(int index) const
{
    const QString prop = propertyName(index);
    if (prop == QLatin1String(currentPageText))
        return m_wizard->title(m_wizard->currentPage());
    return QDesignerPropertySheet::property(index);
}

bool Q3WizardPropertySheet::reset(int index)
{
    const QString prop = propertyName(index);
    if (prop == QLatin1String(currentPageText)) {
        m_wizard->setTitle(m_wizard->currentPage(), QString());
        return true;
    }
    return QDesignerPropertySheet::reset(index);
}

QT_END_NAMESPACE
