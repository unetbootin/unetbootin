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

/* It is possible to link the skins as resources into Designer by specifying:
 * QVFB_ROOT=$$QT_SOURCE_TREE/tools/qvfb
 * RESOURCES += $$QVFB_ROOT/ClamshellPhone.qrc  $$QVFB_ROOT/PDAPhone.qrc ...
 * in lib/shared/shared.pri. However, this exceeds a limit of Visual Studio 6. */

#include "previewconfigurationwidget_p.h"
#include "ui_previewconfigurationwidget.h"
#include "previewmanager_p.h"

#include <iconloader_p.h>
#include <stylesheeteditor_p.h>

#include <deviceskin.h>

#include <QtGui/QFileDialog>
#include <QtGui/QStyleFactory>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QPair>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QSharedData>

// #define DEFAULT_SKINS_FROM_RESOURCE
#ifdef DEFAULT_SKINS_FROM_RESOURCE
QT_BEGIN_NAMESPACE
static const char *skinResourcePathC = ":/skins/";
QT_END_NAMESPACE
#else
#  include <QtCore/QLibraryInfo>
#endif

QT_BEGIN_NAMESPACE

static const char *skinExtensionC = ".skin";
static const char *enabledKey = "Enabled";
static const char *userDeviceSkinsKey= "UserDeviceSkins";

namespace {
    // Pair of skin name, path
    typedef QPair<QString, QString> SkinNamePath;
    typedef QList<SkinNamePath> Skins;
    enum { SkinComboNoneIndex = 0 };
}

// find default skins (resources)
static const Skins &defaultSkins() {
    static Skins rc;
    if (rc.empty()) {
#ifdef DEFAULT_SKINS_FROM_RESOURCE
        const QString skinPath = QLatin1String(skinResourcePathC);
#else
        QString skinPath = QLibraryInfo::location(QLibraryInfo::PrefixPath);
        skinPath += QDir::separator();
        skinPath += QLatin1String("tools");
        skinPath += QDir::separator();
        skinPath += QLatin1String("qvfb");
#endif
        QString pattern = QLatin1String(skinExtensionC);
        pattern.insert(0, QLatin1Char('*'));
        const QDir dir(skinPath, pattern);
        const QFileInfoList list = dir.entryInfoList();
        if (list.empty())
            return rc;
        const QFileInfoList::const_iterator lcend = list.constEnd();
        for (QFileInfoList::const_iterator it = list.constBegin(); it != lcend; ++it)
            rc.push_back(SkinNamePath(it->baseName(), it->filePath()));
    }
    return rc;
}

namespace qdesigner_internal {
// ------------ PreviewConfigurationWidgetStateData

class PreviewConfigurationWidgetStateData : public QSharedData {
public:
    PreviewConfigurationWidgetStateData() : m_enabled(false) {}
    PreviewConfigurationWidgetStateData(const QStringList &userDeviceSkins, bool enabled);

    bool m_enabled;
    QStringList m_userDeviceSkins;
};

PreviewConfigurationWidgetStateData::PreviewConfigurationWidgetStateData(const QStringList &userDeviceSkins, bool enabled) :
    m_enabled(enabled),
    m_userDeviceSkins(userDeviceSkins)
{
}

// ------------ PreviewConfigurationWidgetState
PreviewConfigurationWidgetState::PreviewConfigurationWidgetState() :
    m_d(new PreviewConfigurationWidgetStateData)
{
}

PreviewConfigurationWidgetState::PreviewConfigurationWidgetState(const QStringList &skins, bool e) :
    m_d(new PreviewConfigurationWidgetStateData(skins, e))
{
}

void PreviewConfigurationWidgetState::clear()
{
    PreviewConfigurationWidgetStateData &d = *m_d;
    d.m_enabled = false;
    d.m_userDeviceSkins.clear();
}

PreviewConfigurationWidgetState::PreviewConfigurationWidgetState(const PreviewConfigurationWidgetState &o) :
    m_d(o.m_d)
{
}

PreviewConfigurationWidgetState& PreviewConfigurationWidgetState::operator=(const PreviewConfigurationWidgetState &o)
{
    m_d.operator=(o.m_d);
    return *this;
}

PreviewConfigurationWidgetState::~PreviewConfigurationWidgetState()
{
}

bool PreviewConfigurationWidgetState::isEnabled() const
{
    return m_d->m_enabled;
}

void PreviewConfigurationWidgetState::setEnabled(bool e)
{
    m_d->m_enabled = e;
}

QStringList PreviewConfigurationWidgetState::userDeviceSkins() const
{
    return m_d->m_userDeviceSkins;
}

void PreviewConfigurationWidgetState::setUserDeviceSkins(const QStringList &u)
{
     m_d->m_userDeviceSkins = u;
}

void PreviewConfigurationWidgetState::toSettings(const QString &prefix, QSettings &settings) const
{
    const PreviewConfigurationWidgetStateData &d = *m_d;
    settings.beginGroup(prefix);
    settings.setValue(QLatin1String(enabledKey), d.m_enabled);
    settings.setValue(QLatin1String(userDeviceSkinsKey), d.m_userDeviceSkins);
    settings.endGroup();
}

void PreviewConfigurationWidgetState::fromSettings(const QString &prefix, const QSettings &settings)
{
    clear();

    QString key = prefix;
    key += QLatin1Char('/');
    const int prefixSize = key.size();

    key += QLatin1String(enabledKey);

    PreviewConfigurationWidgetStateData &d = *m_d;
    d.m_enabled = settings.value(key, false).toBool();

    key.replace(prefixSize, key.size() - prefixSize, QLatin1String(userDeviceSkinsKey));
    d.m_userDeviceSkins = settings.value(key, QStringList()).toStringList();
}

PreviewConfiguration PreviewConfigurationWidgetState::previewConfiguration(const PreviewConfiguration &serializedConfiguration) const
{
    return isEnabled() ? serializedConfiguration : PreviewConfiguration();
}

// ------------- PreviewConfigurationWidgetPrivate
class PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate {
public:
    PreviewConfigurationWidgetPrivate(QGroupBox *g);

    PreviewConfigurationWidgetState  previewConfigurationWidgetState() const;
    void setPreviewConfigurationWidgetState(const PreviewConfigurationWidgetState &pc);

    PreviewConfiguration previewConfiguration() const;
    void setPreviewConfiguration(const PreviewConfiguration &pc);

    void slotEditAppStyleSheet();
    void slotDeleteSkinEntry();
    void slotSkinChanged(int index);

    QAbstractButton *appStyleSheetChangeButton() const { return m_ui.m_appStyleSheetChangeButton; }
    QAbstractButton *skinRemoveButton() const { return m_ui.m_skinRemoveButton; }
    QComboBox *skinCombo() const { return m_ui.m_skinCombo; }

    QDesignerFormEditorInterface *m_core;

private:
    QStringList userSkins() const;
    void addUserSkins(const QStringList &files);
    bool canRemoveSkin(int index) const;
    int browseSkin();

    const QString m_defaultStyle;
    QGroupBox *m_parent;
    Ui::PreviewConfigurationWidget m_ui;

    int m_firstUserSkinIndex;
    int m_browseSkinIndex;
    int m_lastSkinIndex; // required in case browse fails
};

PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::PreviewConfigurationWidgetPrivate(QGroupBox *g) :
    m_core(0),
    m_defaultStyle(PreviewConfigurationWidget::tr("Default")),
    m_parent(g),
    m_firstUserSkinIndex(0),
    m_browseSkinIndex(0),
    m_lastSkinIndex(0)
{
    m_ui.setupUi(g);
    // styles
    m_ui.m_styleCombo->setEditable(false);
    QStringList styleItems(m_defaultStyle);
    styleItems += QStyleFactory::keys();
    m_ui.m_styleCombo->addItems(styleItems);
    // sheet
    m_ui.m_appStyleSheetLineEdit->setTextPropertyValidationMode(qdesigner_internal::ValidationStyleSheet);
    m_ui.m_appStyleSheetClearButton->setIcon(qdesigner_internal::createIconSet(QString::fromUtf8("resetproperty.png")));
    QObject::connect(m_ui.m_appStyleSheetClearButton, SIGNAL(clicked()), m_ui.m_appStyleSheetLineEdit, SLOT(clear()));

    m_ui.m_skinRemoveButton->setIcon(qdesigner_internal::createIconSet(QString::fromUtf8("editdelete.png")));
    // skins: find default skins (resources)
    m_ui.m_skinRemoveButton->setEnabled(false);
    Skins skins = defaultSkins();
    skins.push_front(SkinNamePath(PreviewConfigurationWidget::tr("None"), QString()));

    const Skins::const_iterator scend = skins.constEnd();
    for (Skins::const_iterator it = skins.constBegin(); it != scend; ++it)
        m_ui.m_skinCombo->addItem (it->first, QVariant(it->second));
    m_browseSkinIndex = m_firstUserSkinIndex = skins.size();
    m_ui.m_skinCombo->addItem (PreviewConfigurationWidget::tr("Browse..."), QString());

    m_ui.m_skinCombo->setMaxVisibleItems (qMax(15, 2 * m_browseSkinIndex));
    m_ui.m_skinCombo->setEditable(false);
}

bool PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::canRemoveSkin(int index) const
{
    return index >= m_firstUserSkinIndex && index != m_browseSkinIndex;
}

QStringList PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::userSkins() const
{
    QStringList rc;
    for (int i = m_firstUserSkinIndex; i < m_browseSkinIndex; i++)
        rc.push_back(m_ui.m_skinCombo->itemData(i).toString());
    return rc;
}

void PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::addUserSkins(const QStringList &files)
{
    if (files.empty())
        return;
    const QStringList ::const_iterator fcend = files.constEnd();
    for (QStringList::const_iterator it = files.constBegin(); it != fcend; ++it) {
        const QFileInfo fi(*it);
        if (fi.isDir() && fi.isReadable()) {
            m_ui.m_skinCombo->insertItem(m_browseSkinIndex++, fi.baseName(), QVariant(*it));
        } else {
            qWarning() << "Unable to access the skin directory '" << *it << "'.";
        }
    }
}

PreviewConfigurationWidgetState PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::previewConfigurationWidgetState() const
{
    return PreviewConfigurationWidgetState(userSkins(), m_parent->isChecked());
}

void PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::setPreviewConfigurationWidgetState(const PreviewConfigurationWidgetState &ps)
{
    m_parent->setChecked(ps.isEnabled());
    addUserSkins(ps.userDeviceSkins());
}

PreviewConfiguration PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::previewConfiguration() const
{
    PreviewConfiguration  rc;
    QString style = m_ui.m_styleCombo->currentText();
    if (style == m_defaultStyle)
        style.clear();
    const QString applicationStyleSheet = m_ui.m_appStyleSheetLineEdit->text();
    // Figure out skin. 0 is None by definition..
    const int skinIndex = m_ui.m_skinCombo->currentIndex();
    QString deviceSkin;
    if (skinIndex != SkinComboNoneIndex &&  skinIndex != m_browseSkinIndex)
        deviceSkin = m_ui.m_skinCombo->itemData(skinIndex).toString();

    return PreviewConfiguration(style, applicationStyleSheet, deviceSkin);
}

void PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::setPreviewConfiguration(const PreviewConfiguration &pc)
{
    int styleIndex = m_ui.m_styleCombo->findText(pc.style());
    if (styleIndex == -1)
        styleIndex = m_ui.m_styleCombo->findText(m_defaultStyle);
    m_ui.m_styleCombo->setCurrentIndex(styleIndex);
    m_ui.m_appStyleSheetLineEdit->setText(pc.applicationStyleSheet());
    // find skin by file name. 0 is "none"
    const QString deviceSkin = pc.deviceSkin();
    int skinIndex = deviceSkin.isEmpty() ? 0 : m_ui.m_skinCombo->findData(QVariant(deviceSkin));
    if (skinIndex == -1) {
        qWarning() << "Unable to find skin '" << deviceSkin << "'.";
        skinIndex = 0;
    }
    m_ui.m_skinCombo->setCurrentIndex(skinIndex);
}

void  PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::slotEditAppStyleSheet()
{
    StyleSheetEditorDialog dlg(m_core, m_parent, StyleSheetEditorDialog::ModeGlobal);
    dlg.setText(m_ui.m_appStyleSheetLineEdit->text());
    if (dlg.exec() == QDialog::Accepted)
        m_ui.m_appStyleSheetLineEdit->setText(dlg.text());
}

void  PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::slotDeleteSkinEntry()
{
    const int index = m_ui.m_skinCombo->currentIndex();
    if (canRemoveSkin(index)) {
        m_ui.m_skinCombo->setCurrentIndex(SkinComboNoneIndex);
        m_ui.m_skinCombo->removeItem(index);
        m_browseSkinIndex--;
    }
}

void  PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::slotSkinChanged(int index)
{
    if (index == m_browseSkinIndex) {
        m_ui.m_skinCombo->setCurrentIndex(browseSkin());
    } else {
        m_lastSkinIndex = index;
        m_ui.m_skinRemoveButton->setEnabled(canRemoveSkin(index));
        m_ui.m_skinCombo->setToolTip(index != SkinComboNoneIndex ? m_ui.m_skinCombo->itemData(index).toString() : QString());
    }
}

int  PreviewConfigurationWidget::PreviewConfigurationWidgetPrivate::browseSkin()
{
    QFileDialog dlg(m_parent);
    dlg.setFileMode(QFileDialog::DirectoryOnly);
    const QString title = tr("Load Custom Device Skin");
    dlg.setWindowTitle(title);
    dlg.setFilter(tr("All QVFB Skins (*.%1)").arg(QLatin1String(skinExtensionC)));

    int rc = m_lastSkinIndex;
    do {
        if (!dlg.exec())
            break;

        const QStringList directories =  dlg.selectedFiles();
        if (directories.size() != 1)
            break;

        // check: 1) name already there
        const QString directory = directories.front();
        const QString name = QFileInfo(directory).baseName();
        const int existingIndex = m_ui.m_skinCombo->findText(name);
        if (existingIndex != -1 && existingIndex != SkinComboNoneIndex &&  existingIndex != m_browseSkinIndex) {
            const QString msgTitle = tr("%1 - Duplicate Skin").arg(title);
            const QString msg = tr("The skin '%1' already exists.").arg(name);
            QMessageBox::information(m_parent, msgTitle, msg);
            break;
        }
        // check: 2) can read
        DeviceSkinParameters parameters;
        QString readError;
        if (parameters.read(directory, DeviceSkinParameters::ReadSizeOnly, &readError)) {
            const QString name = QFileInfo(directory).baseName();
            m_ui.m_skinCombo->insertItem(m_browseSkinIndex, name, QVariant(directory));
            rc = m_browseSkinIndex++;

            break;
        } else {
            const QString msgTitle = tr("%1 - Error").arg(title);
            const QString msg = tr("%1 is not a valid skin directory:\n%2").arg(directory).arg(readError);
            QMessageBox::warning (m_parent, msgTitle, msg);
        }
    } while (true);
    return rc;
}

// ------------- PreviewConfigurationWidget
PreviewConfigurationWidget::PreviewConfigurationWidget(QWidget *parent) :
    QGroupBox(parent),
    m_impl(new PreviewConfigurationWidgetPrivate(this))
{
    connect(m_impl->appStyleSheetChangeButton(), SIGNAL(clicked()), this, SLOT(slotEditAppStyleSheet()));
    connect(m_impl->skinRemoveButton(), SIGNAL(clicked()), this, SLOT(slotDeleteSkinEntry()));
    connect(m_impl->skinCombo(), SIGNAL(currentIndexChanged(int)), this, SLOT(slotSkinChanged(int)));
}

PreviewConfigurationWidget::~PreviewConfigurationWidget()
{
    delete m_impl;
}

void PreviewConfigurationWidget::setCore(QDesignerFormEditorInterface *core)
{
    m_impl->m_core = core;
}

PreviewConfigurationWidgetState PreviewConfigurationWidget::previewConfigurationWidgetState() const
{
    return m_impl->previewConfigurationWidgetState();
}

void PreviewConfigurationWidget::setPreviewConfigurationWidgetState(const PreviewConfigurationWidgetState &pc)
{
    m_impl->setPreviewConfigurationWidgetState(pc);
}

PreviewConfiguration PreviewConfigurationWidget::previewConfiguration() const
{
    return m_impl->previewConfiguration();
}

void PreviewConfigurationWidget::setPreviewConfiguration(const PreviewConfiguration &pc)
{
    m_impl->setPreviewConfiguration(pc);
}

void PreviewConfigurationWidget::slotEditAppStyleSheet()
{
    m_impl->slotEditAppStyleSheet();
}

void PreviewConfigurationWidget::slotDeleteSkinEntry()
{
    m_impl->slotDeleteSkinEntry();
}

void PreviewConfigurationWidget::slotSkinChanged(int index)
{
    m_impl->slotSkinChanged(index);
}
}

QT_END_NAMESPACE
