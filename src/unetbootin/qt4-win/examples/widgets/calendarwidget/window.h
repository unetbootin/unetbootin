/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCalendarWidget;
class QCheckBox;
class QComboBox;
class QDate;
class QDateEdit;
class QGridLayout;
class QGroupBox;
class QLabel;
QT_END_NAMESPACE

//! [0]
class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

private slots:
    void localeChanged(int index);
    void firstDayChanged(int index);
    void selectionModeChanged(int index);
    void horizontalHeaderChanged(int index);
    void verticalHeaderChanged(int index);
    void selectedDateChanged();
    void minimumDateChanged(const QDate &date);
    void maximumDateChanged(const QDate &date);
    void weekdayFormatChanged();
    void weekendFormatChanged();
    void reformatHeaders();
    void reformatCalendarPage();

private:
    void createPreviewGroupBox();
    void createGeneralOptionsGroupBox();
    void createDatesGroupBox();
    void createTextFormatsGroupBox();
    QComboBox *createColorComboBox();

    QGroupBox *previewGroupBox;
    QGridLayout *previewLayout;
    QCalendarWidget *calendar;

    QGroupBox *generalOptionsGroupBox;
    QLabel *localeLabel;
    QLabel *firstDayLabel;
//! [0]
    QLabel *selectionModeLabel;
    QLabel *horizontalHeaderLabel;
    QLabel *verticalHeaderLabel;
    QComboBox *localeCombo;
    QComboBox *firstDayCombo;
    QComboBox *selectionModeCombo;
    QCheckBox *gridCheckBox;
    QCheckBox *navigationCheckBox;
    QComboBox *horizontalHeaderCombo;
    QComboBox *verticalHeaderCombo;

    QGroupBox *datesGroupBox;
    QLabel *currentDateLabel;
    QLabel *minimumDateLabel;
    QLabel *maximumDateLabel;
    QDateEdit *currentDateEdit;
    QDateEdit *minimumDateEdit;
    QDateEdit *maximumDateEdit;

    QGroupBox *textFormatsGroupBox;
    QLabel *weekdayColorLabel;
    QLabel *weekendColorLabel;
    QLabel *headerTextFormatLabel;
    QComboBox *weekdayColorCombo;
    QComboBox *weekendColorCombo;
    QComboBox *headerTextFormatCombo;

    QCheckBox *firstFridayCheckBox;
//! [1]
    QCheckBox *mayFirstCheckBox;
};
//! [1]

#endif
