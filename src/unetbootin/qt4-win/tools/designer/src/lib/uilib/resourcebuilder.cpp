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

#include "resourcebuilder_p.h"
#include "ui4_p.h"
#include <QtCore/QVariant>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

QResourceBuilder::QResourceBuilder()
{

}

QResourceBuilder::~QResourceBuilder()
{

}

int QResourceBuilder::iconStateFlags(const DomResourceIcon *dpi)
{
    int rc = 0;
    if (dpi->hasElementNormalOff())
        rc |= NormalOff;
    if (dpi->hasElementNormalOn())
        rc |= NormalOn;
    if (dpi->hasElementDisabledOff())
        rc |= DisabledOff;
    if (dpi->hasElementDisabledOn())
        rc |= DisabledOn;
    if (dpi->hasElementActiveOff())
        rc |= ActiveOff;
    if (dpi->hasElementActiveOn())
        rc |= ActiveOn;
    if (dpi->hasElementSelectedOff())
        rc |= SelectedOff;
    if (dpi->hasElementSelectedOn())
        rc |= SelectedOn;
    return rc;
}

QVariant QResourceBuilder::loadResource(const QDir &workingDirectory, const DomProperty *property) const
{
    switch (property->kind()) {
        case DomProperty::Pixmap: {
            const DomResourcePixmap *dpx = property->elementPixmap();
            QPixmap pixmap(QFileInfo(workingDirectory, dpx->text()).absoluteFilePath());
            return qVariantFromValue(pixmap);
        }
        case DomProperty::IconSet: {
            const DomResourceIcon *dpi = property->elementIconSet();
            if (const int flags = iconStateFlags(dpi)) { // new, post 4.4 format
                QIcon icon;
                if (flags & NormalOff)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementNormalOff()->text()).absoluteFilePath(), QIcon::Normal, QIcon::Off);
                if (flags & NormalOn)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementNormalOn()->text()).absoluteFilePath(), QIcon::Normal, QIcon::On);
                if (flags & DisabledOff)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementDisabledOff()->text()).absoluteFilePath(), QIcon::Disabled, QIcon::Off);
                if (flags & DisabledOn)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementDisabledOn()->text()).absoluteFilePath(), QIcon::Disabled, QIcon::On);
                if (flags & ActiveOff)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementActiveOff()->text()).absoluteFilePath(), QIcon::Active, QIcon::Off);
                if (flags & ActiveOn)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementActiveOn()->text()).absoluteFilePath(), QIcon::Active, QIcon::On);
                if (flags & SelectedOff)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementSelectedOff()->text()).absoluteFilePath(), QIcon::Selected, QIcon::Off);
                if (flags & SelectedOn)
                    icon.addPixmap(QFileInfo(workingDirectory, dpi->elementSelectedOn()->text()).absoluteFilePath(), QIcon::Selected, QIcon::On);
                return qVariantFromValue(icon);
            } else { // 4.3 legacy
                const QIcon icon(QFileInfo(workingDirectory, dpi->text()).absoluteFilePath());
                return qVariantFromValue(icon);
            }
        }
            break;
        default:
            break;
    }
    return QVariant();
}

QVariant QResourceBuilder::toNativeValue(const QVariant &value) const
{
    return value;
}

DomProperty *QResourceBuilder::saveResource(const QDir &workingDirectory, const QVariant &value) const
{
    Q_UNUSED(workingDirectory)
    Q_UNUSED(value)
    return 0;
}

bool QResourceBuilder::isResourceProperty(const DomProperty *p) const
{
    switch (p->kind()) {
        case DomProperty::Pixmap:
        case DomProperty::IconSet:
            return true;
        default:
            break;
    }
    return false;
}

bool QResourceBuilder::isResourceType(const QVariant &value) const
{
    switch (value.type()) {
        case QVariant::Pixmap:
        case QVariant::Icon:
            return true;
        default:
            break;
    }
    return false;
}

#ifdef QFORMINTERNAL_NAMESPACE
} // namespace QFormInternal
#endif

QT_END_NAMESPACE
