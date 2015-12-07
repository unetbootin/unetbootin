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

#include "qdesigner_membersheet_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <abstractintrospection_p.h>

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

static QList<QByteArray> stringListToByteArray(const QStringList &l)
{
    if (l.empty())
        return QList<QByteArray>();
    QList<QByteArray> rc;
    const QStringList::const_iterator cend = l.constEnd();
    for (QStringList::const_iterator it = l.constBegin(); it != cend; ++it)
        rc += it->toUtf8();
    return rc;
}

// Find the form editor in the hierarchy.
// We know that the parent of the sheet is the extension manager
// whose parent is the core.

static QDesignerFormEditorInterface *formEditorForObject(QObject *o) {
    do {
        if (QDesignerFormEditorInterface* core = qobject_cast<QDesignerFormEditorInterface*>(o))
            return core;
        o = o->parent();
    } while(o);
    Q_ASSERT(o);
    return 0;
}

// ------------ QDesignerMemberSheetPrivate
class QDesignerMemberSheetPrivate {
public:
    explicit QDesignerMemberSheetPrivate(QObject *object, QObject *sheetParent);

    QDesignerFormEditorInterface *m_core;
    const QDesignerMetaObjectInterface *m_meta;

    class Info {
    public:
        inline Info() : visible(true) {}

        QString group;
        bool visible;
    };

    typedef QHash<int, Info> InfoHash;

    Info &ensureInfo(int index);

    InfoHash m_info;
};

QDesignerMemberSheetPrivate::QDesignerMemberSheetPrivate(QObject *object, QObject *sheetParent) :
    m_core(formEditorForObject(sheetParent)),
    m_meta(m_core->introspection()->metaObject(object))
{
}

QDesignerMemberSheetPrivate::Info &QDesignerMemberSheetPrivate::ensureInfo(int index)
{
    InfoHash::iterator it = m_info.find(index);
    if (it == m_info.end()) {
        it = m_info.insert(index, Info());
    }
    return it.value();
}

// --------- QDesignerMemberSheet

QDesignerMemberSheet::QDesignerMemberSheet(QObject *object, QObject *parent) :
    QObject(parent),
    d(new QDesignerMemberSheetPrivate(object, parent))
{
}

QDesignerMemberSheet::~QDesignerMemberSheet()
{
    delete d;
}

int QDesignerMemberSheet::count() const
{
    return d->m_meta->methodCount();
}

int QDesignerMemberSheet::indexOf(const QString &name) const
{
    return d->m_meta->indexOfMethod(name.toUtf8());
}

QString QDesignerMemberSheet::memberName(int index) const
{
    return d->m_meta->method(index)->tag();
}

QString QDesignerMemberSheet::declaredInClass(int index) const
{
    const QString member = d->m_meta->method(index)->signature();

    // Find class whose superclass does not contain the method.
    const QDesignerMetaObjectInterface *meta_obj = d->m_meta;

    for (;;) {
        const QDesignerMetaObjectInterface *tmp = meta_obj->superClass();
        if (tmp == 0)
            break;
        if (tmp->indexOfMethod(member) == -1)
            break;
        meta_obj = tmp;
    }
    return meta_obj->className();
}

QString QDesignerMemberSheet::memberGroup(int index) const
{
    return d->m_info.value(index).group;
}

void QDesignerMemberSheet::setMemberGroup(int index, const QString &group)
{
    d->ensureInfo(index).group = group;
}

QString QDesignerMemberSheet::signature(int index) const
{
    return d->m_meta->method(index)->normalizedSignature();
}

bool QDesignerMemberSheet::isVisible(int index) const
{
    typedef QDesignerMemberSheetPrivate::InfoHash InfoHash;
    const InfoHash::const_iterator it = d->m_info.constFind(index);
    if (it != d->m_info.constEnd())
        return it.value().visible;

   return d->m_meta->method(index)->methodType() == QDesignerMetaMethodInterface::Signal
           || d->m_meta->method(index)->access() == QDesignerMetaMethodInterface::Public;
}

void QDesignerMemberSheet::setVisible(int index, bool visible)
{
    d->ensureInfo(index).visible = visible;
}

bool QDesignerMemberSheet::isSignal(int index) const
{
    return d->m_meta->method(index)->methodType() == QDesignerMetaMethodInterface::Signal;
}

bool QDesignerMemberSheet::isSlot(int index) const
{
    return d->m_meta->method(index)->methodType() == QDesignerMetaMethodInterface::Slot;
}

bool QDesignerMemberSheet::inheritedFromWidget(int index) const
{
    const QString name = d->m_meta->method(index)->signature();
    return declaredInClass(index) == QLatin1String("QWidget") || declaredInClass(index) == QLatin1String("QObject");
}


QList<QByteArray> QDesignerMemberSheet::parameterTypes(int index) const
{
    return stringListToByteArray(d->m_meta->method(index)->parameterTypes());
}

QList<QByteArray> QDesignerMemberSheet::parameterNames(int index) const
{
    return stringListToByteArray(d->m_meta->method(index)->parameterNames());
}

bool QDesignerMemberSheet::signalMatchesSlot(const QString &signal, const QString &slot)
{
    bool result = true;

    do {
        int signal_idx = signal.indexOf(QLatin1Char('('));
        int slot_idx = slot.indexOf(QLatin1Char('('));
        if (signal_idx == -1 || slot_idx == -1)
            break;

        ++signal_idx; ++slot_idx;

        if (slot.at(slot_idx) == QLatin1Char(')'))
            break;

        while (signal_idx < signal.size() && slot_idx < slot.size()) {
            const QChar signal_c = signal.at(signal_idx);
            const QChar slot_c = slot.at(slot_idx);

            if (signal_c == QLatin1Char(',') && slot_c == QLatin1Char(')'))
                break;

            if (signal_c == QLatin1Char(')') && slot_c == QLatin1Char(')'))
                break;

            if (signal_c != slot_c) {
                result = false;
                break;
            }

            ++signal_idx; ++slot_idx;
        }
    } while (false);

    return result;
}

// ------------ QDesignerMemberSheetFactory

QDesignerMemberSheetFactory::QDesignerMemberSheetFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}

QObject *QDesignerMemberSheetFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid == Q_TYPEID(QDesignerMemberSheetExtension)) {
        return new QDesignerMemberSheet(object, parent);
    }

    return 0;
}

QT_END_NAMESPACE
