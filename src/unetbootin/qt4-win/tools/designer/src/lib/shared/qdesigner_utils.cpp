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

#include "qdesigner_utils_p.h"
#include "qdesigner_propertycommand_p.h"
#include "abstractformbuilder.h"
#include "formwindowbase_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtDesigner/QDesignerResourceBrowserInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QDesignerTaskMenuExtension>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtCore/QDir>

#include <QtGui/QApplication>
#include <QtCore/QProcess>
#include <QtCore/QLibraryInfo>
#include <QtCore/QDebug>
#include <QtCore/QQueue>
#include <QtGui/QListWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QComboBox>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal
{
    QDESIGNER_SHARED_EXPORT void designerWarning(const QString &message)
    {
        qWarning("Designer: %s", qPrintable(message));
    }

    void reloadTreeItem(DesignerIconCache *iconCache, QTreeWidgetItem *item)
    {
        if (!item)
            return;

        for (int c = 0; c < item->columnCount(); c++) {
            const QVariant v = item->data(c, QAbstractFormBuilder::resourceRole());
            if (qVariantCanConvert<PropertySheetIconValue>(v))
                item->setIcon(c, iconCache->icon(qVariantValue<PropertySheetIconValue>(v)));
        }
    }

    void reloadListItem(DesignerIconCache *iconCache, QListWidgetItem *item)
    {
        if (!item)
            return;

        const QVariant v = item->data(QAbstractFormBuilder::resourceRole());
        if (qVariantCanConvert<PropertySheetIconValue>(v))
            item->setIcon(iconCache->icon(qVariantValue<PropertySheetIconValue>(v)));
    }

    void reloadTableItem(DesignerIconCache *iconCache, QTableWidgetItem *item)
    {
        if (!item)
            return;

        const QVariant v = item->data(QAbstractFormBuilder::resourceRole());
        if (qVariantCanConvert<PropertySheetIconValue>(v))
            item->setIcon(iconCache->icon(qVariantValue<PropertySheetIconValue>(v)));
    }

    void reloadIconResources(DesignerIconCache *iconCache, QObject *object)
    {
        if (QListWidget *listWidget = qobject_cast<QListWidget *>(object)) {
            for (int i = 0; i < listWidget->count(); i++)
                reloadListItem(iconCache, listWidget->item(i));
        } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(object)) {
            for (int i = 0; i < comboBox->count(); i++) {
                const QVariant v = comboBox->itemData(i, QAbstractFormBuilder::resourceRole());
                if (qVariantCanConvert<PropertySheetIconValue>(v)) {
                    QIcon icon = iconCache->icon(qVariantValue<PropertySheetIconValue>(v));
                    comboBox->setItemIcon(i, icon);
                    comboBox->setItemData(i, icon);
                }
            }
        } else if (QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>(object)) {
            reloadTreeItem(iconCache, treeWidget->headerItem());
            QQueue<QTreeWidgetItem *> itemsQueue;
            for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
                itemsQueue.enqueue(treeWidget->topLevelItem(i));
            while (!itemsQueue.isEmpty()) {
                QTreeWidgetItem *item = itemsQueue.dequeue();
                for (int i = 0; i < item->childCount(); i++)
                    itemsQueue.enqueue(item->child(i));
                reloadTreeItem(iconCache, item);
            }
        } else if (QTableWidget *tableWidget = qobject_cast<QTableWidget *>(object)) {
            const int columnCount = tableWidget->columnCount();
            const int rowCount = tableWidget->rowCount();
            for (int c = 0; c < columnCount; c++)
                reloadTableItem(iconCache, tableWidget->horizontalHeaderItem(c));
            for (int r = 0; r < rowCount; r++)
                reloadTableItem(iconCache, tableWidget->verticalHeaderItem(r));
            for (int c = 0; c < columnCount; c++)
                for (int r = 0; r < rowCount; r++)
                    reloadTableItem(iconCache, tableWidget->item(r, c));
        }
    }

    // ------------- DesignerMetaEnum
    DesignerMetaEnum::DesignerMetaEnum(const QString &name, const QString &scope, const QString &separator) :
        MetaEnum<int>(name, scope, separator)
    {
    }


    QString DesignerMetaEnum::toString(int value, SerializationMode sm, bool *ok) const
    {
        // find value
        bool valueOk;
        const QString item = valueToKey(value, &valueOk);
        if (ok)
            *ok = valueOk;

        if (!valueOk || sm == NameOnly)
            return item;

        QString qualifiedItem;
        appendQualifiedName(item,  qualifiedItem);
        return qualifiedItem;
    }

    QString DesignerMetaEnum::messageToStringFailed(int value) const
    {
        return QObject::tr("%1 is not a valid enumeration value of '%2'.").arg(value).arg(name());
    }

    QString DesignerMetaEnum::messageParseFailed(const QString &s) const
    {
        return QObject::tr("'%1' could not be converted to an enumeration value of type '%2'.").arg(s).arg(name());
    }
    // -------------- DesignerMetaFlags
    DesignerMetaFlags::DesignerMetaFlags(const QString &name, const QString &scope, const QString &separator) :
       MetaEnum<uint>(name, scope, separator)
    {
    }

    QStringList DesignerMetaFlags::flags(int ivalue) const
    {
        typedef MetaEnum<uint>::KeyToValueMap::const_iterator KeyToValueMapIterator;
        QStringList rc;
        const uint v = static_cast<uint>(ivalue);
        const KeyToValueMapIterator cend = keyToValueMap().constEnd();
        for (KeyToValueMapIterator it = keyToValueMap().constBegin();it != cend; ++it )  {
            const uint itemValue = it.value();
            // Check for equality first as flag values can be 0 or -1, too. Takes preference over a bitwise flag
            if (v == itemValue) {
                rc.clear();
                rc.push_back(it.key());
                return rc;
            }
            // Do not add 0-flags (None-flags)
            if (itemValue)
                if ((v & itemValue) == itemValue)
                    rc.push_back(it.key());
        }
        return rc;
    }


    QString DesignerMetaFlags::toString(int value, SerializationMode sm) const
    {
        const QStringList flagIds = flags(value);
        if (flagIds.empty())
            return QString();

        const QChar delimiter = QLatin1Char('|');
        QString rc;
        const QStringList::const_iterator cend = flagIds.constEnd();
        for (QStringList::const_iterator it = flagIds.constBegin(); it != cend; ++it) {
            if (!rc.isEmpty())
                rc += delimiter ;
            if (sm == FullyQualified)
                appendQualifiedName(*it, rc);
            else
                rc += *it;
        }
        return rc;
    }


    int DesignerMetaFlags::parseFlags(const QString &s, bool *ok) const
    {
        if (s.isEmpty()) {
            if (ok)
                *ok = true;
            return 0;
        }
        uint flags = 0;
        bool valueOk = true;
        QStringList keys = s.split(QString(QLatin1Char('|')));
        const QStringList::iterator cend = keys.end();
        for (QStringList::iterator it = keys.begin(); it != cend; ++it) {
            const uint flagValue = keyToValue(*it, &valueOk);
            if (!valueOk) {
                flags = 0;
                break;
            }
            flags |= flagValue;
        }
        if (ok)
            *ok = valueOk;
        return static_cast<int>(flags);
    }

    QString DesignerMetaFlags::messageParseFailed(const QString &s) const
    {
        return QObject::tr("'%1' could not be converted to a flag value of type '%2'.").arg(s).arg(name());
    }

    // ---------- PropertySheetEnumValue

    PropertySheetEnumValue::PropertySheetEnumValue(int v, const DesignerMetaEnum &me) :
       value(v),
       metaEnum(me)
    {
    }
    PropertySheetEnumValue::PropertySheetEnumValue() :
       value(0)
    {
    }

    // ---------------- PropertySheetFlagValue
    PropertySheetFlagValue::PropertySheetFlagValue(int v, const DesignerMetaFlags &mf) :
        value(v),
        metaFlags(mf)
    {
    }

    PropertySheetFlagValue::PropertySheetFlagValue() :
        value(0)
    {
    }

    // ---------------- PropertySheetPixmapValue
    PropertySheetPixmapValue::PropertySheetPixmapValue(const QString &path) : m_path(path)
    {
    }

    PropertySheetPixmapValue::PropertySheetPixmapValue()
    {
    }

    PropertySheetPixmapValue::PixmapSource PropertySheetPixmapValue::getPixmapSource(QDesignerFormEditorInterface *core, const QString & path)
    {
        if (const QDesignerLanguageExtension *lang = qt_extension<QDesignerLanguageExtension *>(core->extensionManager(), core))
            return lang->isLanguageResource(path) ?  LanguageResourcePixmap : FilePixmap;
        return path.startsWith(QLatin1Char(':')) ? ResourcePixmap : FilePixmap;
    }

    int PropertySheetPixmapValue::compare(const PropertySheetPixmapValue &other) const
    {
        return m_path.compare(other.m_path);
    }

    QString PropertySheetPixmapValue::path() const
    {
        return m_path;
    }

    void PropertySheetPixmapValue::setPath(const QString &path)
    {
        if (m_path == path)
            return;
        m_path = path;
    }

    // ---------- PropertySheetIconValue
    PropertySheetIconValue::PropertySheetIconValue(const PropertySheetPixmapValue &pixmap)
    {
        setPixmap(QIcon::Normal, QIcon::Off, pixmap);
    }

    PropertySheetIconValue::PropertySheetIconValue()
    {
    }

    bool PropertySheetIconValue::equals(const PropertySheetIconValue &rhs) const
    {
        return m_paths == rhs.m_paths;
    }

    bool PropertySheetIconValue::operator<(const PropertySheetIconValue &other) const
    {
        QMapIterator<ModeStateKey, PropertySheetPixmapValue> itThis(m_paths);
        QMapIterator<ModeStateKey, PropertySheetPixmapValue> itOther(other.m_paths);
        while (itThis.hasNext() && itOther.hasNext()) {
            const ModeStateKey thisPair = itThis.next().key();
            const ModeStateKey otherPair = itOther.next().key();
            if (thisPair < otherPair)
                return true;
            else if (otherPair < thisPair)
                return false;
            const int crc = itThis.value().compare(itOther.value());
            if (crc < 0)
                return true;
            if (crc > 0)
                return false;
        }
        if (itOther.hasNext())
            return true;
        return false;
    }

    PropertySheetPixmapValue PropertySheetIconValue::pixmap(QIcon::Mode mode, QIcon::State state) const
    {
        const ModeStateKey pair = qMakePair(mode, state);
        return m_paths.value(pair);
    }

    void PropertySheetIconValue::setPixmap(QIcon::Mode mode, QIcon::State state, const PropertySheetPixmapValue &pixmap)
    {
        const ModeStateKey pair = qMakePair(mode, state);
        if (pixmap.path().isEmpty())
            m_paths.remove(pair);
        else
            m_paths.insert(pair, pixmap);
    }

    class StateMap
    {
    public:
        StateMap()
        {
            m_stateToFlag.insert(qMakePair(QIcon::Normal, QIcon::Off), 0x01);
            m_stateToFlag.insert(qMakePair(QIcon::Normal, QIcon::On), 0x02);
            m_stateToFlag.insert(qMakePair(QIcon::Disabled, QIcon::Off), 0x04);
            m_stateToFlag.insert(qMakePair(QIcon::Disabled, QIcon::On), 0x08);
            m_stateToFlag.insert(qMakePair(QIcon::Active, QIcon::Off), 0x10);
            m_stateToFlag.insert(qMakePair(QIcon::Active, QIcon::On), 0x20);
            m_stateToFlag.insert(qMakePair(QIcon::Selected, QIcon::Off), 0x40);
            m_stateToFlag.insert(qMakePair(QIcon::Selected, QIcon::On), 0x80);

            m_flagToState.insert(0x01, qMakePair(QIcon::Normal, QIcon::Off));
            m_flagToState.insert(0x02, qMakePair(QIcon::Normal, QIcon::On));
            m_flagToState.insert(0x04, qMakePair(QIcon::Disabled, QIcon::Off));
            m_flagToState.insert(0x08, qMakePair(QIcon::Disabled, QIcon::On));
            m_flagToState.insert(0x10, qMakePair(QIcon::Active, QIcon::Off));
            m_flagToState.insert(0x20, qMakePair(QIcon::Active, QIcon::On));
            m_flagToState.insert(0x40, qMakePair(QIcon::Selected, QIcon::Off));
            m_flagToState.insert(0x80, qMakePair(QIcon::Selected, QIcon::On));
        }
        uint flag(const QPair<QIcon::Mode, QIcon::State> &pair) const
        {
            return m_stateToFlag.value(pair);
        }
        QPair<QIcon::Mode, QIcon::State> state(uint flag) const
        {
            return m_flagToState.value(flag);
        }
    private:
        QMap<QPair<QIcon::Mode, QIcon::State>, uint > m_stateToFlag;
        QMap<uint, QPair<QIcon::Mode, QIcon::State> > m_flagToState;
    };

    Q_GLOBAL_STATIC(StateMap, stateMap)

    uint PropertySheetIconValue::mask() const
    {
        uint flags = 0;
        QMapIterator<ModeStateKey, PropertySheetPixmapValue> itPath(m_paths);
        while (itPath.hasNext())
            flags |= stateMap()->flag(itPath.next().key());
        return flags;
    }

    uint PropertySheetIconValue::compare(const PropertySheetIconValue &other) const
    {
        uint diffMask = mask() | other.mask();
        for (int i = 0; i < 8; i++) {
            uint flag = 1 << i;
            if (diffMask & flag) { // if state is set in both icons, compare the values
                const ModeStateKey state = stateMap()->state(flag);
                if (pixmap(state.first, state.second) == other.pixmap(state.first, state.second))
                    diffMask &= ~flag;
            }
        }
        return diffMask;
    }

    void PropertySheetIconValue::assign(const PropertySheetIconValue &other, uint mask)
    {
        for (int i = 0; i < 8; i++) {
            uint flag = 1 << i;
            if (mask & flag) {
                const ModeStateKey state = stateMap()->state(flag);
                setPixmap(state.first, state.second, other.pixmap(state.first, state.second));
            }
        }
    }

    PropertySheetIconValue::ModeStateToPixmapMap PropertySheetIconValue::paths() const
    {
        return m_paths;
    }

    QDESIGNER_SHARED_EXPORT QDesignerFormWindowCommand *createTextPropertyCommand(const QString &propertyName, const QString &text, QObject *object, QDesignerFormWindowInterface *fw)
    {
        if (text.isEmpty()) {
            ResetPropertyCommand *cmd = new ResetPropertyCommand(fw);
            cmd->init(object, propertyName);
            return cmd;
        }
        SetPropertyCommand *cmd = new SetPropertyCommand(fw);
        cmd->init(object, propertyName, text);
        return cmd;
    }

    QDESIGNER_SHARED_EXPORT QAction *preferredEditAction(QDesignerFormEditorInterface *core, QWidget *managedWidget)
    {
        QAction *action = 0;
        if (const QDesignerTaskMenuExtension *taskMenu = qt_extension<QDesignerTaskMenuExtension*>(core->extensionManager(), managedWidget)) {
            action = taskMenu->preferredEditAction();
            if (!action) {
                const QList<QAction *> actions = taskMenu->taskActions();
                if (!actions.isEmpty())
                    action = actions.first();
            }
        }
        if (!action) {
            if (const QDesignerTaskMenuExtension *taskMenu = qobject_cast<QDesignerTaskMenuExtension *>(
                        core->extensionManager()->extension(managedWidget, QLatin1String("QDesignerInternalTaskMenuExtension")))) {
                action = taskMenu->preferredEditAction();
                if (!action) {
                    const QList<QAction *> actions = taskMenu->taskActions();
                    if (!actions.isEmpty())
                        action = actions.first();
                }
            }
        }
        return action;
    }

    QDESIGNER_SHARED_EXPORT bool runUIC(const QString &fileName, UIC_Mode mode, QByteArray& ba, QString &errorMessage)
    {
        QStringList argv;
        QString binary = QLibraryInfo::location(QLibraryInfo::BinariesPath);
        binary += QDir::separator();
        switch (mode) {
        case UIC_GenerateCode:
            binary += QLatin1String("uic");
            break;
        case UIC_ConvertV3:
            binary += QLatin1String("uic3");
            argv += QLatin1String("-convert");
            break;
        }
        argv += fileName;
        QProcess uic;
        uic.start(binary, argv);
        if (!uic.waitForStarted()) {
            errorMessage = QApplication::translate("Designer", "Unable to launch %1.").arg(binary);
            return false;
        }
        if (!uic.waitForFinished()) {
            errorMessage = QApplication::translate("Designer", "%1 timed out.").arg(binary);
            return false;
        }
        if (uic.exitCode()) {
            errorMessage =  uic.readAllStandardError();
            return false;
        }
        ba = uic.readAllStandardOutput();
        return true;
    }

    QDESIGNER_SHARED_EXPORT QString qtify(const QString &name)
    {
        QString qname = name;

        Q_ASSERT(qname.isEmpty() == false);


        if (qname.count() > 1 && qname.at(1).isUpper()) {
            const QChar first = qname.at(0);
            if (first == QLatin1Char('Q') || first == QLatin1Char('K'))
                qname.remove(0, 1);
        }

        const int len = qname.count();
        for (int i = 0; i < len && qname.at(i).isUpper(); i++)
            qname[i] = qname.at(i).toLower();

        return qname;
    }
} // namespace qdesigner_internal

QT_END_NAMESPACE
