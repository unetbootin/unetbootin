/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include "qinputdialog.h"

#ifndef QT_NO_INPUTDIALOG

#include "qapplication.h"
#include "qcombobox.h"
#include "qdialogbuttonbox.h"
#include "qlabel.h"
#include "qlayout.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "qstackedlayout.h"
#include "qvalidator.h"
#include "qevent.h"
#include "qdialog_p.h"

QT_BEGIN_NAMESPACE

// This internal class adds extra validation to a QSpinBox by emitting textChanged(bool)
// after events that may potentially change the visible text. Return or Enter key presses
// are not propagated if the visible text is invalid. Instead, the visible text is modified
// to the last valid value.
class QInputDialogValidatedSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    QInputDialogValidatedSpinBox(int minValue, int maxValue, int step, int value)
        : QSpinBox(0)
    {
        setRange(minValue, maxValue);
        setSingleStep(step);
        setValue(value);
        selectAll();
        validator = new QIntValidator(minValue, maxValue, this);
        QObject::connect(
            lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(notifyTextChanged()));
        QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(notifyTextChanged()));
    }

private:
    QIntValidator *validator;
    void keyPressEvent(QKeyEvent *event)
    {
        if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && !textValid()) {
#ifndef QT_NO_PROPERTIES
            setProperty("value", property("value"));
#endif
        } else {
            QSpinBox::keyPressEvent(event);
        }
        notifyTextChanged();
    }

    void mousePressEvent(QMouseEvent *event)
    {
        QSpinBox::mousePressEvent(event);
        notifyTextChanged();
    }

    bool textValid() const
    {
        QString t = text();
        int pos = 0;
        return validate(t, pos) == QValidator::Acceptable;
    }

private slots:
    void notifyTextChanged() { emit textChanged(textValid()); }

signals:
    void textChanged(bool);
};


// This internal class adds extra validation to a QDoubleSpinBox by emitting textChanged(bool)
// after events that may potentially change the visible text. Return or Enter key presses
// are not propagated if the visible text is invalid. Instead, the visible text is modified
// to the last valid value.
class QInputDialogValidatedDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    QInputDialogValidatedDoubleSpinBox(
        double minValue, double maxValue, int decimals, double value)
        : QDoubleSpinBox(0)
    {
        setDecimals(decimals);
        setRange(minValue, maxValue);
        setValue(value);
        selectAll();
        validator = new QDoubleValidator(minValue, maxValue, decimals, this);
        QObject::connect(
            lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(notifyTextChanged()));
        QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(notifyTextChanged()));
    }
private:
    QDoubleValidator *validator;
    void keyPressEvent(QKeyEvent *event)
    {
        if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && !textValid()) {
#ifndef QT_NO_PROPERTIES
            setProperty("value", property("value"));
#endif
        } else {
            QDoubleSpinBox::keyPressEvent(event);
        }
        notifyTextChanged();
    }

    void mousePressEvent(QMouseEvent *event)
    {
        QDoubleSpinBox::mousePressEvent(event);
        notifyTextChanged();
    }

    bool textValid() const
    {
        QString t = text();
        int pos = 0;
        return validate(t, pos) == QValidator::Acceptable;
    }

private slots:
    void notifyTextChanged() { emit textChanged(textValid()); }

signals:
    void textChanged(bool);
};

QT_BEGIN_INCLUDE_NAMESPACE
#include "qinputdialog.moc"
QT_END_INCLUDE_NAMESPACE

class QInputDialogPrivate : public QDialogPrivate
{
    Q_DECLARE_PUBLIC(QInputDialog)
public:
    QInputDialogPrivate();
    QLabel *label; // ### Qt 5: remove
    QPushButton *okButton;
    QWidget *input; // ### Qt 5: remove
    void init(const QString &label, QInputDialog::Type); // ### Qt 5: remove
    void init(const QString &title, const QString &label, QWidget *input);
    void tryAccept();
};

QInputDialogPrivate::QInputDialogPrivate()
    : QDialogPrivate()
{
}

// ### Qt 5: remove
void QInputDialogPrivate::init(const QString &lbl, QInputDialog::Type type)
{
    Q_Q(QInputDialog);
    QVBoxLayout *vbox = new QVBoxLayout(q);

    label = new QLabel(lbl, q);
    vbox->addWidget(label);
    vbox->addStretch(1);

    switch (type) {
    case QInputDialog::LineEdit:
        input = new QLineEdit(q);
        break;
    case QInputDialog::SpinBox:
        input = new QSpinBox(q);
        break;
    case QInputDialog::DoubleSpinBox:
        input = new QDoubleSpinBox(q);
        break;
    case QInputDialog::ComboBox:
    case QInputDialog::EditableComboBox: {
        QComboBox *combo = new QComboBox(q);
        if (type == QInputDialog::EditableComboBox)
            combo->setEditable(true);
        input = combo;
    }
        break;
    }
    vbox->addWidget(input);
    vbox->addStretch(1);

#ifndef QT_NO_SHORTCUT
    label->setBuddy(input);
#endif

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel,
                                                        Qt::Horizontal, q);
    buttonBox->setObjectName(QLatin1String("qt_inputdlg_buttonbox"));
    QPushButton *okButton =
        static_cast<QPushButton *>(buttonBox->addButton(QDialogButtonBox::Ok));
    okButton->setDefault(true);
    vbox->addWidget(buttonBox);

    QObject::connect(buttonBox, SIGNAL(accepted()), q, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), q, SLOT(reject()));

}

void QInputDialogPrivate::init(const QString &title, const QString &lbl, QWidget *input)
{
    Q_Q(QInputDialog);
    q->setWindowTitle(title);
    QVBoxLayout *vbox = new QVBoxLayout(q);

    QLabel *label = new QLabel(lbl, q);
    vbox->addWidget(label);
    vbox->addStretch(1);

    input->setParent(q);
    vbox->addWidget(input);
    vbox->addStretch(1);

#ifndef QT_NO_SHORTCUT
    label->setBuddy(input);
#endif

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, q);
    buttonBox->setObjectName(QLatin1String("qt_inputdlg_buttonbox"));
    okButton = static_cast<QPushButton *>(buttonBox->addButton(QDialogButtonBox::Ok));
    okButton->setDefault(true);
    vbox->addWidget(buttonBox);

    QObject::connect(buttonBox, SIGNAL(accepted()), q, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), q, SLOT(reject()));

    q->setSizeGripEnabled(true);
}

/*!
    \class QInputDialog
    \brief The QInputDialog class provides a simple convenience dialog to get a single value from the user.
    \ingroup dialogs
    \mainclass

    The input value can be a string, a number or an item from a list. A
    label must be set to tell the user what they should enter.

    Four static convenience functions are provided:
    getText(), getInteger(), getDouble() and getItem(). All the
    functions can be used in a similar way, for example:

    \snippet examples/dialogs/standarddialogs/dialog.cpp 3

    The \c ok variable is set to true if the user clicks \gui OK;
    otherwise it is set to false.

    \img inputdialogs.png Input Dialogs

    The \l{dialogs/standarddialogs}{Standard Dialogs} example shows
    how to use QInputDialog as well as other built-in Qt dialogs.

    \sa QMessageBox, {Standard Dialogs Example}
*/

/*!
  \enum QInputDialog::Type

  This enum specifies the type of the dialog, i.e. what kind of data you
  want the user to input:

  \value LineEdit  A QLineEdit is used for obtaining string or numeric
  input. The QLineEdit can be accessed using lineEdit().

  \value SpinBox  A QSpinBox is used for obtaining integer input.
  Use spinBox() to access the QSpinBox.

  \value ComboBox  A read-only QComboBox is used to provide a fixed
  list of choices from which the user can choose.
  Use comboBox() to access the QComboBox.

  \value EditableComboBox  An editable QComboBox is used to provide a fixed
  list of choices from which the user can choose, but which also
  allows the user to enter their own value instead.
  Use editableComboBox() to access the QComboBox.
*/

/*!
  Constructs an input dialog of the specified \a type with the given \a parent
  widget and window \a flags. The \a text is shown in the dialog with the input
  field, and is typically used to tell the user what they are expected to enter.

  \sa getText(), getInteger(), getDouble(), getItem()
*/
// ### Qt 5: remove
QInputDialog::QInputDialog(const QString &text, QWidget* parent, Type type, Qt::WindowFlags f)
    : QDialog(*new QInputDialogPrivate, parent, f)
{
    Q_D(QInputDialog);
    d->init(text, type);
}

/*!
  Constructs the dialog. The \a title is the text which is displayed in
  the title bar of the dialog. The \a label is the text which is shown to
  the user (it should tell the user what they are expected to enter).
  The \a parent is the dialog's parent widget. The \a input parameter
  is the dialog to be used. The \a f parameter is passed on to the
  QDialog constructor.

  \sa getText(), getInteger(), getDouble(), getItem()
*/
QInputDialog::QInputDialog(
    const QString &title, const QString &label, QWidget *parent, QWidget *input,
    Qt::WindowFlags f)
    : QDialog(*new QInputDialogPrivate, parent, f)
{
    Q_D(QInputDialog);
    d->init(title, label, input);
}

/*!
    Destroys the input dialog.
*/
QInputDialog::~QInputDialog()
{
}

/*!
    Static convenience function to get a string from the user. \a
    title is the text which is displayed in the title bar of the
    dialog. \a label is the text which is shown to the user (it should
    say what should be entered). \a text is the default text which is
    placed in the line edit. The \a mode is the echo mode the line
    edit will use. If \a ok is non-null \e *\a ok will be set to true
    if the user pressed \gui OK and to false if the user pressed
    \gui Cancel. The dialog's parent is \a parent. The dialog will be
    modal and uses the widget flags \a f.

    This function returns the text which has been entered in the line
    edit. It will not return an empty string.

    Use this static function like this:

    \snippet examples/dialogs/standarddialogs/dialog.cpp 3

    \sa getInteger(), getDouble(), getItem()
*/

QString QInputDialog::getText(QWidget *parent, const QString &title, const QString &label,
                               QLineEdit::EchoMode mode, const QString &text,
                               bool *ok, Qt::WindowFlags f)
{
    QLineEdit *le = new QLineEdit;
    le->setText(text);
    le->setEchoMode(mode);
    le->setFocus();
    le->selectAll();

    QInputDialog dlg(title, label, parent, le, f);

    QString result;
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
        *ok = accepted;
    if (accepted)
        result = le->text();

    return result;
}

/*!
    Static convenience function to get an integer input from the
    user. \a title is the text which is displayed in the title bar
    of the dialog.  \a label is the text which is shown to the user
    (it should say what should be entered). \a value is the default
    integer which the spinbox will be set to.  \a minValue and \a
    maxValue are the minimum and maximum values the user may choose,
    and \a step is the amount by which the values change as the user
    presses the arrow buttons to increment or decrement the value.

    If \a ok is non-null *\a ok will be set to true if the user
    pressed \gui OK and to false if the user pressed \gui Cancel. The
    dialog's parent is \a parent. The dialog will be modal and uses
    the widget flags \a f.

    This function returns the integer which has been entered by the user.

    Use this static function like this:

    \snippet examples/dialogs/standarddialogs/dialog.cpp 0

    \sa getText(), getDouble(), getItem()
*/

int QInputDialog::getInteger(QWidget *parent, const QString &title, const QString &label,
                             int value, int minValue, int maxValue, int step, bool *ok,
                             Qt::WindowFlags f)
{
    QInputDialogValidatedSpinBox *sb =
        new QInputDialogValidatedSpinBox(minValue, maxValue, step, value);
    QInputDialog dlg(title, label, parent, sb, f);
    QObject::connect(
        sb, SIGNAL(textChanged(bool)), qobject_cast<QPushButton*>(dlg.d_func()->okButton),
        SLOT(setEnabled(bool)));
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
        *ok = accepted;
    return sb->value();
}

/*!
    Static convenience function to get a floating point number from
    the user. \a title is the text which is displayed in the title
    bar of the dialog. \a label is the text which is shown to the user
    (it should say what should be entered). \a value is the default
    floating point number that the line edit will be set to. \a
    minValue and \a maxValue are the minimum and maximum values the
    user may choose, and \a decimals is the maximum number of decimal
    places the number may have.

    If \a ok is non-null, *\a ok will be set to true if the user
    pressed \gui OK and to false if the user pressed \gui Cancel. The
    dialog's parent is \a parent. The dialog will be modal and uses
    the widget flags \a f.

    This function returns the floating point number which has been
    entered by the user.

    Use this static function like this:

    \snippet examples/dialogs/standarddialogs/dialog.cpp 1

    \sa getText(), getInteger(), getItem()
*/

double QInputDialog::getDouble( QWidget *parent, const QString &title, const QString &label,
                                double value, double minValue, double maxValue,
                                int decimals, bool *ok, Qt::WindowFlags f)
{
    QInputDialogValidatedDoubleSpinBox *sb =
        new QInputDialogValidatedDoubleSpinBox(minValue, maxValue, decimals, value);
    QInputDialog dlg(title, label, parent, sb, f);
    QObject::connect(
        sb, SIGNAL(textChanged(bool)), qobject_cast<QPushButton*>(dlg.d_func()->okButton),
        SLOT(setEnabled(bool)));
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
        *ok = accepted;
    return sb->value();
}

/*!
    Static convenience function to let the user select an item from a
    string list. \a title is the text which is displayed in the title
    bar of the dialog. \a label is the text which is shown to the user (it
    should say what should be entered). \a list is the
    string list which is inserted into the combobox, and \a current is the number
    of the item which should be the current item. If \a editable is true
    the user can enter their own text; if \a editable is false the user
    may only select one of the existing items.

    If \a ok is non-null \e *\a ok will be set to true if the user
    pressed \gui OK and to false if the user pressed \gui Cancel. The
    dialog's parent is \a parent. The dialog will be modal and uses
    the widget flags \a f.

    This function returns the text of the current item, or if \a
    editable is true, the current text of the combobox.

    Use this static function like this:

    \snippet examples/dialogs/standarddialogs/dialog.cpp 2

    \sa getText(), getInteger(), getDouble()
*/

QString QInputDialog::getItem(QWidget *parent, const QString &title, const QString &label, const QStringList &list,
                              int current, bool editable, bool *ok, Qt::WindowFlags f)
{
    QComboBox *combo = new QComboBox;
    combo->addItems(list);
    combo->setCurrentIndex(current);
    combo->setEditable(editable);
    QInputDialog dlg(title, label, parent, combo, f);
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
        *ok = accepted;
    return combo->currentText();
}

/*!
    \fn QString QInputDialog::getText(const QString &title, const QString &label,
                                      QLineEdit::EchoMode echo = QLineEdit::Normal,
                                      const QString &text = QString(), bool *ok = 0,
                                      QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)

    Call getText(\a parent, \a title, \a label, \a echo, \a text, \a
    ok, \a f) instead.

    The \a name parameter is ignored.
*/

/*!
    \fn int QInputDialog::getInteger(const QString &title, const QString &label, int value = 0,
                                     int minValue = -2147483647, int maxValue = 2147483647,
                                     int step = 1, bool *ok = 0,
                                     QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)


    Call getInteger(\a parent, \a title, \a label, \a value, \a
    minValue, \a maxValue, \a step, \a ok, \a f) instead.

    The \a name parameter is ignored.
*/

/*!
    \fn double QInputDialog::getDouble(const QString &title, const QString &label, double value = 0,
                                       double minValue = -2147483647, double maxValue = 2147483647,
                                       int decimals = 1, bool *ok = 0,
                                       QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)

    Call getDouble(\a parent, \a title, \a label, \a value, \a
    minValue, \a maxValue, \a decimals, \a ok, \a f).

    The \a name parameter is ignored.
*/

/*!
    \fn QString QInputDialog::getItem(const QString &title, const QString &label, const QStringList &list,
                                      int current = 0, bool editable = true, bool *ok = 0,
                                      QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)

    Call getItem(\a parent, \a title, \a label, \a list, \a current,
    \a editable, \a ok, \a f) instead.

    The \a name parameter is ignored.
*/

QT_END_NAMESPACE

#endif // QT_NO_INPUTDIALOG
