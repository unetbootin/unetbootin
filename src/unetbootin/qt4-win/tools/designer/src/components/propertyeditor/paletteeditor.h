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

#ifndef PALETTEEDITOR_H
#define PALETTEEDITOR_H

#include "ui_paletteeditor.h"
#include <QtGui/QItemDelegate>

QT_BEGIN_NAMESPACE

class QListView;
class QLabel;
class QtColorButton;
class QDesignerFormEditorInterface;

namespace qdesigner_internal {

class PaletteEditor: public QDialog
{
    Q_OBJECT
public:
    virtual ~PaletteEditor();

    static QPalette getPalette(QDesignerFormEditorInterface *core,
                QWidget* parent, const QPalette &init = QPalette(),
                const QPalette &parentPal = QPalette(), int *result = 0);

    QPalette palette() const;
    void setPalette(const QPalette &palette);
    void setPalette(const QPalette &palette, const QPalette &parentPalette);

private slots:

    void on_buildButton_colorChanged(const QColor &);
    void on_activeRadio_clicked();
    void on_inactiveRadio_clicked();
    void on_disabledRadio_clicked();
    void on_computeRadio_clicked();
    void on_detailsRadio_clicked();

    void paletteChanged(const QPalette &palette);

protected:

private:
    PaletteEditor(QDesignerFormEditorInterface *core, QWidget *parent);
    void buildPalette();

    void updatePreviewPalette();
    void updateStyledButton();

    QPalette::ColorGroup currentColorGroup() const
        { return m_currentColorGroup; }

    Ui::PaletteEditor ui;
    QPalette m_editPalette;
    QPalette m_parentPalette;
    QPalette::ColorGroup m_currentColorGroup;
    class PaletteModel *m_paletteModel;
    bool m_modelUpdated;
    bool m_paletteUpdated;
    bool m_compute;
    QDesignerFormEditorInterface *m_core;
};


class PaletteModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QPalette::ColorRole colorRole READ colorRole)
public:
    explicit PaletteModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                int role = Qt::DisplayRole) const;

    QPalette getPalette() const;
    void setPalette(const QPalette &palette, const QPalette &parentPalette);

    QPalette::ColorRole colorRole() const { return QPalette::NoRole; }
    void setCompute(bool on) { m_compute = on; }
signals:
    void paletteChanged(const QPalette &palette);
private:

    QPalette::ColorGroup columnToGroup(int index) const;
    int groupToColumn(QPalette::ColorGroup group) const;

    QPalette m_palette;
    QPalette m_parentPalette;
    QMap<QPalette::ColorRole, QString> m_roleNames;
    bool m_compute;
};

class BrushEditor : public QWidget
{
    Q_OBJECT
public:
    explicit BrushEditor(QDesignerFormEditorInterface *core, QWidget *parent = 0);

    void setBrush(const QBrush &brush);
    QBrush brush() const;
    bool changed() const;
signals:
    void changed(QWidget *widget);
private slots:
    void brushChanged();
private:
    QtColorButton *m_button;
    bool m_changed;
    QDesignerFormEditorInterface *m_core;
};

class RoleEditor : public QWidget
{
    Q_OBJECT
public:
    explicit RoleEditor(QWidget *parent = 0);

    void setLabel(const QString &label);
    void setEdited(bool on);
    bool edited() const;
signals:
    void changed(QWidget *widget);
private slots:
    void emitResetProperty();
private:
    QLabel *m_label;
    bool    m_edited;
};

class ColorDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit ColorDelegate(QDesignerFormEditorInterface *core, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;

    void setEditorData(QWidget *ed, const QModelIndex &index) const;
    void setModelData(QWidget *ed, QAbstractItemModel *model,
                const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *ed,
                const QStyleOptionViewItem &option, const QModelIndex &index) const;

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &opt,
                       const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const;
private:
    QDesignerFormEditorInterface *m_core;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // PALETTEEDITOR_H
