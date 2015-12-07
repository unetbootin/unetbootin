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

#include "qdesigner_propertycommentcommand_p.h"
#include "metadatabase_p.h"
#include "qdesigner_propertyeditor_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QApplication>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

SetPropertyCommentCommand::Entry::Entry(QObject* object, const QString &oldCommentValue) :
    m_object(object),
    m_oldCommentValue(oldCommentValue)
{
}

SetPropertyCommentCommand::SetPropertyCommentCommand(QDesignerFormWindowInterface *formWindow) :
     QDesignerFormWindowCommand(QString(), formWindow),
     m_propertyType(QVariant::Invalid)
{
}

bool SetPropertyCommentCommand::init(QObject *object, const QString &propertyName, const QString &newCommentValue)
{
    m_propertyName = propertyName;
    m_newCommentValue = newCommentValue;

    m_Entries.clear();
    if (!add(object))
        return false;

    setDescription();
    return true;
}

void SetPropertyCommentCommand::setDescription()
{
    if (m_Entries.size() == 1) {
        setText(QApplication::translate("Command", "changed comment of '%1' of '%2'").arg(m_propertyName).arg(m_Entries[0].m_object->objectName()));
    } else {
        int count = m_Entries.size();
        setText(QApplication::translate("Command", "changed comment of '%1' of %2 objects", "", QCoreApplication::UnicodeUTF8, count).arg(m_propertyName).arg(count));
    }
}

bool SetPropertyCommentCommand::init(const ObjectList &list, const QString &propertyName, const QString &newCommentValue)
{
    m_propertyName = propertyName;
    m_newCommentValue = newCommentValue;

    m_Entries.clear();
    foreach (QObject *o, list) {
        add(o);
    }

    if (m_Entries.empty())
        return false;

    setDescription();
    return true;
}


bool SetPropertyCommentCommand::add(QObject *object)
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), object);
    if (!sheet)
        return false;

    const int index = sheet->indexOf(m_propertyName);
    if (index == -1 || !sheet->isVisible(index))
        return false;

    // Set or check type
    const QVariant::Type propertyType = sheet->property(index).type();
    if (m_Entries.empty()) {
        m_propertyType = propertyType;
    } else {
        if ( propertyType != m_propertyType)
            return false;
    }

    const QString oldCommentValue = propertyComment(core, object, m_propertyName);

    m_Entries.push_back(Entry(object, oldCommentValue));
    return true;
}


int SetPropertyCommentCommand::id() const
{
    return 1968;
}

bool SetPropertyCommentCommand::mergeWith(const QUndoCommand *other)
{
    if (id() != other->id())
        return false;

    // check property name and list of objects
    const SetPropertyCommentCommand *cmd = static_cast<const SetPropertyCommentCommand*>(other);

    if (cmd->m_propertyName != m_propertyName)
         return false;

    const int numEntries = m_Entries.size();
    if (numEntries != cmd->m_Entries.size()) {
        return false;
    }

    for (int i = 0; i < numEntries; i++) {
        if (m_Entries[i].m_object != cmd->m_Entries[i].m_object)
            return false;
    }

    m_newCommentValue = cmd->m_newCommentValue;
    return true;

}

void SetPropertyCommentCommand::redo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();

    QDesignerPropertyEditor *designerPropertyEditor = qobject_cast<QDesignerPropertyEditor *>(core->propertyEditor());
    Q_ASSERT(designerPropertyEditor);
    QObject* propertyEditorObject = designerPropertyEditor->object();
    // Set m_newCommentValue and update property editor
    const EntryList::const_iterator cend = m_Entries.end();
    for (EntryList::const_iterator it = m_Entries.begin(); it != cend; ++it) {
        if (QObject *object = it->m_object) { // might have been deleted
            setPropertyComment(core, object, m_propertyName, m_newCommentValue);
            if (object == propertyEditorObject)
                designerPropertyEditor->setPropertyComment(m_propertyName, m_newCommentValue);
        }
    }
}

void SetPropertyCommentCommand::undo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();

    QDesignerPropertyEditor *designerPropertyEditor = qobject_cast<QDesignerPropertyEditor *>(core->propertyEditor());
    Q_ASSERT(designerPropertyEditor);
    QObject* propertyEditorObject = designerPropertyEditor->object();

    // Set stored old value and update property editor
    const EntryList::const_iterator cend = m_Entries.end();
    for (EntryList::const_iterator it = m_Entries.begin(); it != cend; ++it) {
        if (QObject *object = it->m_object) {
            setPropertyComment(core, object, m_propertyName, it->m_oldCommentValue);
            if (object == propertyEditorObject)
                designerPropertyEditor->setPropertyComment(m_propertyName, it->m_oldCommentValue);
        }
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
