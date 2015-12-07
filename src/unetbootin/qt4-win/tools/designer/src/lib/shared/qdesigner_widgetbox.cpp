/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

/*
TRANSLATOR qdesigner_internal::QDesignerWidgetBox
*/

#include "qdesigner_widgetbox_p.h"
#include "qdesigner_utils_p.h"

#include <ui4_p.h>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtXml/QDomDocument>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
QDesignerWidgetBox::QDesignerWidgetBox(QWidget *parent, Qt::WindowFlags flags)
    : QDesignerWidgetBoxInterface(parent, flags),
      m_loadMode(LoadMerge)
{

}

QDesignerWidgetBox::LoadMode QDesignerWidgetBox::loadMode() const
{
    return m_loadMode;
}

void QDesignerWidgetBox::setLoadMode(LoadMode lm)
{
     m_loadMode = lm;
}

// Convenience to find a widget by class name
bool QDesignerWidgetBox::findWidget(const QDesignerWidgetBoxInterface *wbox, const QString &className, Widget *widgetData)
{
    // Note that entry names do not necessarily match the class name
    // (at least, not for the standard widgets), so,
    // look in the XML for the class name of the first widget to appear
    const QString widgetTag = QLatin1String("<widget");
    QString pattern = QLatin1String("^<widget\\s+class\\s*=\\s*\"");
    pattern += className;
    pattern += QLatin1String("\".*$");
    const QRegExp regexp(pattern);
    Q_ASSERT(regexp.isValid());
    const int catCount = wbox->categoryCount();
    for (int c = 0; c < catCount; c++) {
        const Category cat = wbox->category(c);
        if (const int widgetCount =  cat.widgetCount())
            for (int w = 0; w < widgetCount; w++) {
                const Widget widget = cat.widget(w);
                QString xml = widget.domXml(); // Erase the <ui> tag that can be present starting from 4.4
                const int widgetTagIndex = xml.indexOf(widgetTag);
                if (widgetTagIndex != -1) {
                    xml.remove(0, widgetTagIndex);
                    if (regexp.exactMatch(xml)) {
                        *widgetData = widget;
                        return true;
                    }
                }
            }
    }
    return false;
}

// Convenience to create a Dom Widget from widget box xml code.
DomUI *QDesignerWidgetBox::xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel, QString *errorMessage)
{
    QDomDocument doc;
    int errorLine, errorColumn;
    if (!doc.setContent(xml, errorMessage, &errorLine, &errorColumn)) {
        *errorMessage = QObject::tr("A parse error occurred at line %1, column %2 of the XML code specified for the widget %3: %4\n%5").
                                 arg(errorLine).arg(errorColumn).arg(name).arg(*errorMessage).arg(xml);
        return 0;
    }

    if (!doc.hasChildNodes()) {
        *errorMessage = QObject::tr("The XML code specified for the widget %1 does not contain any widget elements.\n%2").arg(name).arg(xml);
        return 0;
    }

    QDomElement rootElement = doc.firstChildElement();
    const QString rootNode = rootElement.nodeName();

    const QString widgetTag = QLatin1String("widget");
    if (rootNode == widgetTag) { // 4.3 legacy ,wrap into DomUI
        DomUI *rc = new DomUI;
        DomWidget *widget = new DomWidget;
        widget->read(rootElement);
        if (insertFakeTopLevel)  {
            DomWidget *fakeTopLevel = new DomWidget;
            QList<DomWidget *> children;
            children.push_back(widget);
            fakeTopLevel->setElementWidget(children);
            rc->setElementWidget(fakeTopLevel);
        } else {
            rc->setElementWidget(widget);
        }
        return rc;
    }

    if (rootNode == QLatin1String("ui")) { // 4.4
        QDomElement widgetChild = rootElement.firstChildElement(widgetTag);
        if (widgetChild.isNull()) {
            *errorMessage = QObject::tr("The XML code specified for the widget %1 does not contain valid widget element\n%2").arg(name).arg(xml);
            return 0;
        }
        if (insertFakeTopLevel)  {
            QDomElement fakeTopLevel = doc.createElement(widgetTag);
            rootElement.replaceChild(fakeTopLevel, widgetChild);
            fakeTopLevel.appendChild(widgetChild);
        }
        DomUI *rc = new DomUI;
        rc->read(rootElement);
        return rc;
    }

    *errorMessage = QObject::tr("The XML code specified for the widget %1 contains an invalid root element %2.\n%3").arg(name).arg(rootNode).arg(xml);
    return 0;
}

// Convenience to create a Dom Widget from widget box xml code.
DomUI *QDesignerWidgetBox::xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel)
{
    QString errorMessage;
    DomUI *rc = xmlToUi(name, xml, insertFakeTopLevel, &errorMessage);
    if (!rc)
        qdesigner_internal::designerWarning(errorMessage);
    return rc;
}

}  // namespace qdesigner_internal

QT_END_NAMESPACE
