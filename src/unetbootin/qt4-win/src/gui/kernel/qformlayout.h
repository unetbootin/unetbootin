/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFORMLAYOUT_H
#define QFORMLAYOUT_H

#include <QtGui/QLayout>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QFormLayoutPrivate;

class Q_GUI_EXPORT QFormLayout : public QLayout
{
    Q_OBJECT
    Q_ENUMS(FormStyle FieldGrowthPolicy RowWrapPolicy ItemRole)
    Q_DECLARE_PRIVATE(QFormLayout)
    Q_PROPERTY(FieldGrowthPolicy fieldGrowthPolicy READ fieldGrowthPolicy WRITE setFieldGrowthPolicy RESET resetFieldGrowthPolicy)
    Q_PROPERTY(RowWrapPolicy rowWrapPolicy READ rowWrapPolicy WRITE setRowWrapPolicy RESET resetRowWrapPolicy)
    Q_PROPERTY(Qt::Alignment labelAlignment READ labelAlignment WRITE setLabelAlignment RESET resetLabelAlignment)
    Q_PROPERTY(Qt::Alignment formAlignment READ formAlignment WRITE setFormAlignment RESET resetFormAlignment)
    Q_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
    Q_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)

public:
    enum FieldGrowthPolicy {
        FieldsStayAtSizeHint,
        ExpandingFieldsGrow,
        AllNonFixedFieldsGrow
    };

    enum RowWrapPolicy {
        DontWrapRows,
        WrapLongRows,
        WrapAllRows
    };

    enum ItemRole {
        LabelRole = 0,
        FieldRole = 1
    };

    explicit QFormLayout(QWidget *parent = 0);
    ~QFormLayout();

    void setFieldGrowthPolicy(FieldGrowthPolicy policy);
    FieldGrowthPolicy fieldGrowthPolicy() const;
    void setRowWrapPolicy(RowWrapPolicy policy);
    RowWrapPolicy rowWrapPolicy() const;
    void setLabelAlignment(Qt::Alignment alignment);
    Qt::Alignment labelAlignment() const;
    void setFormAlignment(Qt::Alignment alignment);
    Qt::Alignment formAlignment() const;

    void setHorizontalSpacing(int spacing);
    int horizontalSpacing() const;
    void setVerticalSpacing(int spacing);
    int verticalSpacing() const;

    int spacing() const;
    void setSpacing(int);

    void addRow(QWidget *label, QWidget *field);
    void addRow(QWidget *label, QLayout *field);
    void addRow(const QString &labelText, QWidget *field);
    void addRow(const QString &labelText, QLayout *field);
    void addRow(QWidget *widget);
    void addRow(QLayout *layout);

    void insertRow(int row, QWidget *label, QWidget *field);
    void insertRow(int row, QWidget *label, QLayout *field);
    void insertRow(int row, const QString &labelText, QWidget *field);
    void insertRow(int row, const QString &labelText, QLayout *field);
    void insertRow(int row, QWidget *widget);
    void insertRow(int row, QLayout *layout);

    void setItem(int row, ItemRole role, QLayoutItem *item);
    void setWidget(int row, ItemRole role, QWidget *widget);
    void setLayout(int row, ItemRole role, QLayout *layout);

    QLayoutItem *itemAt(int row, ItemRole role) const;
    void getItemPosition(int index, int *rowPtr, ItemRole *rolePtr) const;
    void getWidgetPosition(QWidget *widget, int *rowPtr, ItemRole *rolePtr) const;
    void getLayoutPosition(QLayout *layout, int *rowPtr, ItemRole *rolePtr) const;
    QWidget *labelForField(QWidget *field) const;
    QWidget *labelForField(QLayout *field) const;

    // reimplemented from QLayout
    void addItem(QLayoutItem *item);
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);

    void setGeometry(const QRect &rect);
    QSize minimumSize() const;
    QSize sizeHint() const;
    void invalidate();

    bool hasHeightForWidth() const;
    int heightForWidth(int width) const;
    Qt::Orientations expandingDirections() const;
    int count() const;

    int rowCount() const;

#if 0
	void dump() const;
#endif

private:
    void resetFieldGrowthPolicy();
    void resetRowWrapPolicy();
    void resetLabelAlignment();
    void resetFormAlignment();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
