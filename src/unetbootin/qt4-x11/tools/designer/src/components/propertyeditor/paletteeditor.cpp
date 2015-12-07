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

/*
TRANSLATOR qdesigner_internal::PaletteEditor
*/
/*
TRANSLATOR qdesigner_internal::PaletteModel
*/

#include "paletteeditor.h"

#include <iconloader_p.h>
#include <qtcolorbutton.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerIconCacheInterface>

#include <QtCore/QMetaProperty>
#include <QtGui/QPainter>
#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

enum { BrushRole = 33 };

PaletteEditor::PaletteEditor(QDesignerFormEditorInterface *core, QWidget *parent) : 
    QDialog(parent),
    m_currentColorGroup(QPalette::Active),
    m_paletteModel(new PaletteModel(this)),
    m_modelUpdated(false),
    m_paletteUpdated(false),
    m_compute(true),
    m_core(core)
{
    ui.setupUi(this);
    ui.paletteView->setModel(m_paletteModel);
    updatePreviewPalette();
    updateStyledButton();
    ui.paletteView->setModel(m_paletteModel);
    ColorDelegate *delegate = new ColorDelegate(core, this);
    ui.paletteView->setItemDelegate(delegate);
    ui.paletteView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    connect(m_paletteModel, SIGNAL(paletteChanged(const QPalette &)),
                this, SLOT(paletteChanged(const QPalette &)));
    ui.paletteView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.paletteView->setDragEnabled(true);
    ui.paletteView->setDropIndicatorShown(true);
    ui.paletteView->setRootIsDecorated(false);
    ui.paletteView->setColumnHidden(2, true);
    ui.paletteView->setColumnHidden(3, true);
}

PaletteEditor::~PaletteEditor()
{
}

QPalette PaletteEditor::palette() const
{
    return m_editPalette;
}

void PaletteEditor::setPalette(const QPalette &palette)
{
    m_editPalette = palette;
    const uint mask = palette.resolve();
    for (int i = 0; i < (int)QPalette::NColorRoles; i++) {
        if (!(mask & (1 << i))) {
            m_editPalette.setBrush(QPalette::Active, static_cast<QPalette::ColorRole>(i),
                        m_parentPalette.brush(QPalette::Active, static_cast<QPalette::ColorRole>(i)));
            m_editPalette.setBrush(QPalette::Inactive, static_cast<QPalette::ColorRole>(i),
                        m_parentPalette.brush(QPalette::Inactive, static_cast<QPalette::ColorRole>(i)));
            m_editPalette.setBrush(QPalette::Disabled, static_cast<QPalette::ColorRole>(i),
                        m_parentPalette.brush(QPalette::Disabled, static_cast<QPalette::ColorRole>(i)));
        }
    }
    m_editPalette.resolve(mask);
    updatePreviewPalette();
    updateStyledButton();
    m_paletteUpdated = true;
    if (!m_modelUpdated)
        m_paletteModel->setPalette(m_editPalette, m_parentPalette);
    m_paletteUpdated = false;
}

void PaletteEditor::setPalette(const QPalette &palette, const QPalette &parentPalette)
{
    m_parentPalette = parentPalette;
    setPalette(palette);
}

void PaletteEditor::on_buildButton_colorChanged(const QColor &)
{
    buildPalette();
}

void PaletteEditor::on_activeRadio_clicked()
{
    m_currentColorGroup = QPalette::Active;
    updatePreviewPalette();
}

void PaletteEditor::on_inactiveRadio_clicked()
{
    m_currentColorGroup = QPalette::Inactive;
    updatePreviewPalette();
}

void PaletteEditor::on_disabledRadio_clicked()
{
    m_currentColorGroup = QPalette::Disabled;
    updatePreviewPalette();
}

void PaletteEditor::on_computeRadio_clicked()
{
    if (m_compute)
        return;
    ui.paletteView->setColumnHidden(2, true);
    ui.paletteView->setColumnHidden(3, true);
    m_compute = true;
    m_paletteModel->setCompute(true);
}

void PaletteEditor::on_detailsRadio_clicked()
{
    if (!m_compute)
        return;
    const int w = ui.paletteView->columnWidth(1);
    ui.paletteView->setColumnHidden(2, false);
    ui.paletteView->setColumnHidden(3, false);
    QHeaderView *header = ui.paletteView->header();
    header->resizeSection(1, w / 3);
    header->resizeSection(2, w / 3);
    header->resizeSection(3, w / 3);
    m_compute = false;
    m_paletteModel->setCompute(false);
}

void PaletteEditor::paletteChanged(const QPalette &palette)
{
    m_modelUpdated = true;
    if (!m_paletteUpdated)
        setPalette(palette);
    m_modelUpdated = false;
}

void PaletteEditor::buildPalette()
{
    const QColor btn = ui.buildButton->color();
    const QPalette temp = QPalette(btn);
    setPalette(temp);
}

void PaletteEditor::updatePreviewPalette()
{
    const QPalette::ColorGroup g = currentColorGroup();
    // build the preview palette
    const QPalette currentPalette = palette();
    QPalette previewPalette;
    for (int i = QPalette::WindowText; i < QPalette::NColorRoles; i++) {
        const QPalette::ColorRole r = static_cast<QPalette::ColorRole>(i);
        const QBrush br = currentPalette.brush(g, r);
        previewPalette.setBrush(QPalette::Active, r, br);
        previewPalette.setBrush(QPalette::Inactive, r, br);
        previewPalette.setBrush(QPalette::Disabled, r, br);
    }
    ui.previewFrame->setPreviewPalette(previewPalette);

    const bool enabled = g != QPalette::Disabled;
    ui.previewFrame->setEnabled(enabled);
    ui.previewFrame->setSubWindowActive(g != QPalette::Inactive);
}

void PaletteEditor::updateStyledButton()
{
    ui.buildButton->setColor(palette().color(QPalette::Active, QPalette::Button));
}

QPalette PaletteEditor::getPalette(QDesignerFormEditorInterface *core, QWidget* parent, const QPalette &init,
            const QPalette &parentPal, int *ok)
{
    PaletteEditor dlg(core, parent);
    QPalette parentPalette(parentPal);
    uint mask = init.resolve();
    for (int i = 0; i < (int)QPalette::NColorRoles; i++) {
        if (!(mask & (1 << i))) {
            parentPalette.setBrush(QPalette::Active, static_cast<QPalette::ColorRole>(i),
                        init.brush(QPalette::Active, static_cast<QPalette::ColorRole>(i)));
            parentPalette.setBrush(QPalette::Inactive, static_cast<QPalette::ColorRole>(i),
                        init.brush(QPalette::Inactive, static_cast<QPalette::ColorRole>(i)));
            parentPalette.setBrush(QPalette::Disabled, static_cast<QPalette::ColorRole>(i),
                        init.brush(QPalette::Disabled, static_cast<QPalette::ColorRole>(i)));
        }
    }
    dlg.setPalette(init, parentPalette);

    const int result = dlg.exec();
    if (ok) *ok = result;

    return result == QDialog::Accepted ? dlg.palette() : init;
}

//////////////////////

PaletteModel::PaletteModel(QObject *parent)  : 
    QAbstractTableModel(parent),
    m_compute(true)
{
    const QMetaObject *meta = metaObject();
    const int index = meta->indexOfProperty("colorRole");
    const QMetaProperty p = meta->property(index);
    const QMetaEnum e = p.enumerator();
    for (int r = QPalette::WindowText; r < QPalette::NColorRoles; r++) {
        m_roleNames[static_cast<QPalette::ColorRole>(r)] = QLatin1String(e.key(r));
    }
}

int PaletteModel::rowCount(const QModelIndex &) const
{
    return m_roleNames.count();
}

int PaletteModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() < 0 || index.row() >= QPalette::NColorRoles)
        return QVariant();
    if (index.column() < 0 || index.column() >= 4)
        return QVariant();

    if (index.column() == 0) {
        if (role == Qt::DisplayRole)
            return m_roleNames[static_cast<QPalette::ColorRole>(index.row())];
        if (role == Qt::EditRole) {
            const uint mask = m_palette.resolve();
            if (mask & (1 << index.row()))
                return true;
            return false;
        }
        return QVariant();
    }
    if (role == BrushRole)
        return m_palette.brush(columnToGroup(index.column()),
                    static_cast<QPalette::ColorRole>(index.row()));
    return QVariant();
}

bool PaletteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (index.column() != 0 && role == BrushRole) {
        const QBrush br = qVariantValue<QBrush>(value);
        const QPalette::ColorRole r = static_cast<QPalette::ColorRole>(index.row());
        const QPalette::ColorGroup g = columnToGroup(index.column());
        m_palette.setBrush(g, r, br);

        QModelIndex idxBegin = PaletteModel::index(r, 0);
        QModelIndex idxEnd = PaletteModel::index(r, 3);
        if (m_compute) {
            m_palette.setBrush(QPalette::Inactive, r, br);
            switch (r) {
                case QPalette::WindowText:
                case QPalette::Text:
                case QPalette::ButtonText:
                case QPalette::Base:
                    break;
                case QPalette::Dark:
                    m_palette.setBrush(QPalette::Disabled, QPalette::WindowText, br);
                    m_palette.setBrush(QPalette::Disabled, QPalette::Dark, br);
                    m_palette.setBrush(QPalette::Disabled, QPalette::Text, br);
                    m_palette.setBrush(QPalette::Disabled, QPalette::ButtonText, br);
                    idxBegin = PaletteModel::index(0, 0);
                    idxEnd = PaletteModel::index(m_roleNames.count() - 1, 3);
                    break;
                case QPalette::Window:
                    m_palette.setBrush(QPalette::Disabled, QPalette::Base, br);
                    m_palette.setBrush(QPalette::Disabled, QPalette::Window, br);
                    idxBegin = PaletteModel::index(QPalette::Base, 0);
                    break;
                case QPalette::Highlight:
                    //m_palette.setBrush(QPalette::Disabled, QPalette::Highlight, c.dark(120));
                    break;
                default:
                    m_palette.setBrush(QPalette::Disabled, r, br);
                    break;
            }
        }
        emit paletteChanged(m_palette);
        emit dataChanged(idxBegin, idxEnd);
        return true;
    }
    if (index.column() == 0 && role == Qt::EditRole) {
        uint mask = m_palette.resolve();
        const bool isMask = qVariantValue<bool>(value);
        const int r = index.row();
        if (isMask)
            mask |= (1 << r);
        else {
            m_palette.setBrush(QPalette::Active, static_cast<QPalette::ColorRole>(r),
                        m_parentPalette.brush(QPalette::Active, static_cast<QPalette::ColorRole>(r)));
            m_palette.setBrush(QPalette::Inactive, static_cast<QPalette::ColorRole>(r),
                        m_parentPalette.brush(QPalette::Inactive, static_cast<QPalette::ColorRole>(r)));
            m_palette.setBrush(QPalette::Disabled, static_cast<QPalette::ColorRole>(r),
                        m_parentPalette.brush(QPalette::Disabled, static_cast<QPalette::ColorRole>(r)));

            mask &= ~(1 << index.row());
        }
        m_palette.resolve(mask);
        emit paletteChanged(m_palette);
        const QModelIndex idxEnd = PaletteModel::index(r, 3);
        emit dataChanged(index, idxEnd);
        return true;
    }
    return false;
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant PaletteModel::headerData(int section, Qt::Orientation orientation,
                int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return tr("Color Role");
        if (section == groupToColumn(QPalette::Active))
            return tr("Active");
        if (section == groupToColumn(QPalette::Inactive))
            return tr("Inactive");
        if (section == groupToColumn(QPalette::Disabled))
            return tr("Disabled");
    }
    return QVariant();
}

QPalette PaletteModel::getPalette() const
{
    return m_palette;
}

void PaletteModel::setPalette(const QPalette &palette, const QPalette &parentPalette)
{
    m_parentPalette = parentPalette;
    m_palette = palette;
    const QModelIndex idxBegin = index(0, 0);
    const QModelIndex idxEnd = index(m_roleNames.count() - 1, 3);
    emit dataChanged(idxBegin, idxEnd);
}

QPalette::ColorGroup PaletteModel::columnToGroup(int index) const
{
    if (index == 1)
        return QPalette::Active;
    if (index == 2)
        return QPalette::Inactive;
    return QPalette::Disabled;
}

int PaletteModel::groupToColumn(QPalette::ColorGroup group) const
{
    if (group == QPalette::Active)
        return 1;
    if (group == QPalette::Inactive)
        return 2;
    return 3;
}

//////////////////////////

BrushEditor::BrushEditor(QDesignerFormEditorInterface *core, QWidget *parent) :
    QWidget(parent),
    m_button(new QtColorButton(this)),
    m_changed(false),
    m_core(core)    
{
    QLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_button);
    connect(m_button, SIGNAL(colorChanged(const QColor &)), this, SLOT(brushChanged()));
    setFocusProxy(m_button);
}

void BrushEditor::setBrush(const QBrush &brush)
{
    m_button->setColor(brush.color());
    m_changed = false;
}

QBrush BrushEditor::brush() const
{
    return QBrush(m_button->color());
}

void BrushEditor::brushChanged()
{
    m_changed = true;
    emit changed(this);
}

bool BrushEditor::changed() const
{
    return m_changed;
}

//////////////////////////

RoleEditor::RoleEditor(QWidget *parent) :
    QWidget(parent),
    m_label(new QLabel(this)),
    m_edited(false)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_label);
    m_label->setAutoFillBackground(true);
    m_label->setIndent(3); // ### hardcode it should have the same value of textMargin in QItemDelegate
    setFocusProxy(m_label);

    QToolButton *button = new QToolButton(this);
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIcon(createIconSet(QLatin1String("resetproperty.png")));
    button->setIconSize(QSize(8,8));
    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));
    layout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(emitResetProperty()));
}

void RoleEditor::setLabel(const QString &label)
{
    m_label->setText(label);
}

void RoleEditor::setEdited(bool on)
{
    QFont font;
    if (on == true) {
        font.setBold(on);
    }
    m_label->setFont(font);
    m_edited = on;
}

bool RoleEditor::edited() const
{
    return m_edited;
}

void RoleEditor::emitResetProperty()
{
    setEdited(false);
    emit changed(this);
}
        
//////////////////////////
ColorDelegate::ColorDelegate(QDesignerFormEditorInterface *core, QObject *parent) :
    QItemDelegate(parent),
    m_core(core)
{ 
}

QWidget *ColorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &,
                const QModelIndex &index) const
{
    QWidget *ed = 0;
    if (index.column() == 0) {
        RoleEditor *editor = new RoleEditor(parent);
        connect(editor, SIGNAL(changed(QWidget *)), this, SIGNAL(commitData(QWidget *)));
        //editor->setFocusPolicy(Qt::NoFocus);
        //editor->installEventFilter(const_cast<ColorDelegate *>(this));
        ed = editor;
    } else {
        BrushEditor *editor = new BrushEditor(m_core, parent);
        connect(editor, SIGNAL(changed(QWidget *)), this, SIGNAL(commitData(QWidget *)));
        editor->setFocusPolicy(Qt::NoFocus);
        editor->installEventFilter(const_cast<ColorDelegate *>(this));
        ed = editor;
    }
    return ed;
}

void ColorDelegate::setEditorData(QWidget *ed, const QModelIndex &index) const
{
    if (index.column() == 0) {
        const bool mask = qVariantValue<bool>(index.model()->data(index, Qt::EditRole));
        RoleEditor *editor = static_cast<RoleEditor *>(ed);
        editor->setEdited(mask);
        const QString colorName = qVariantValue<QString>(index.model()->data(index, Qt::DisplayRole));
        editor->setLabel(colorName);
    } else {
        const QBrush br = qVariantValue<QBrush>(index.model()->data(index, BrushRole));
        BrushEditor *editor = static_cast<BrushEditor *>(ed);
        editor->setBrush(br);
    }
}

void ColorDelegate::setModelData(QWidget *ed, QAbstractItemModel *model,
                const QModelIndex &index) const
{
    if (index.column() == 0) {
        RoleEditor *editor = static_cast<RoleEditor *>(ed);
        const bool mask = editor->edited();
        model->setData(index, mask, Qt::EditRole);
    } else {
        BrushEditor *editor = static_cast<BrushEditor *>(ed);
        if (editor->changed()) {
            QBrush br = editor->brush();
            model->setData(index, br, BrushRole);
        }
    }
}

void ColorDelegate::updateEditorGeometry(QWidget *ed,
                const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::updateEditorGeometry(ed, option, index);
    ed->setGeometry(ed->geometry().adjusted(0, 0, -1, -1));
}

void ColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt,
            const QModelIndex &index) const
{
    QStyleOptionViewItem option = opt;
    const bool mask = qVariantValue<bool>(index.model()->data(index, Qt::EditRole));
    if (index.column() == 0 && mask) {
        option.font.setBold(true);
    }
    QBrush br = qVariantValue<QBrush>(index.model()->data(index, BrushRole));
    if (br.style() == Qt::LinearGradientPattern ||
            br.style() == Qt::RadialGradientPattern ||
            br.style() == Qt::ConicalGradientPattern) {
        painter->save();
        painter->translate(option.rect.x(), option.rect.y());
        painter->scale(option.rect.width(), option.rect.height());
        QGradient gr = *(br.gradient());
        gr.setCoordinateMode(QGradient::LogicalMode);
        br = QBrush(gr);
        painter->fillRect(0, 0, 1, 1, br);
        painter->restore();
    } else {
        painter->save();
        painter->setBrushOrigin(option.rect.x(), option.rect.y());
        painter->fillRect(option.rect, br);
        painter->restore();
    }
    QItemDelegate::paint(painter, option, index);
    
    
    const QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
    const QPen oldPen = painter->pen();
    painter->setPen(QPen(color));

    painter->drawLine(option.rect.right(), option.rect.y(),
            option.rect.right(), option.rect.bottom());
    painter->drawLine(option.rect.x(), option.rect.bottom(),
            option.rect.right(), option.rect.bottom());
    painter->setPen(oldPen);
}

QSize ColorDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(opt, index) + QSize(4, 4);
}
}

QT_END_NAMESPACE
