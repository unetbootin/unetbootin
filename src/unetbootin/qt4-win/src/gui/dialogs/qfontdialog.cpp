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

#include "qwindowdefs.h"

#ifndef QT_NO_FONTDIALOG

#include "qfontdialog.h"

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qfontdatabase.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qstyle.h>
#include <qdialogbuttonbox.h>
#include <qheaderview.h>
#include <qlistview.h>
#include <qstringlistmodel.h>
#include <qvalidator.h>
#include <private/qdialog_p.h>
#include <private/qfont_p.h>

QT_BEGIN_NAMESPACE

class QFontListView : public QListView
{
    Q_OBJECT
public:
    QFontListView(QWidget *parent);
    inline QStringListModel *model() const {
        return static_cast<QStringListModel *>(QListView::model());
    }
    inline void setCurrentItem(int item) {
        QListView::setCurrentIndex(static_cast<QAbstractListModel*>(model())->index(item));
    }
    inline int currentItem() const {
        return QListView::currentIndex().row();
    }
    inline int count() const {
        return model()->rowCount();
    }
    inline QString currentText() const {
        int row = QListView::currentIndex().row();
        return row < 0 ? QString() : model()->stringList().at(row);
    }
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) {
        QListView::currentChanged(current, previous);
        if (current.isValid())
            emit highlighted(current.row());
    }
    QString text(int i) const {
        return model()->stringList().at(i);
    }
signals:
    void highlighted(int);
};

QFontListView::QFontListView(QWidget *parent)
    : QListView(parent)
{
    setModel(new QStringListModel(parent));
    setEditTriggers(NoEditTriggers);
}

/*!
  \class QFontDialog
  \ingroup dialogs
  \ingroup text
  \mainclass
  \brief The QFontDialog class provides a dialog widget for selecting a font.

    A font dialog is created through one of the static getFont()
    functions.

  Examples:

  \snippet doc/src/snippets/code/src.gui.dialogs.qfontdialog.cpp 0

    The dialog can also be used to set a widget's font directly:
  \snippet doc/src/snippets/code/src.gui.dialogs.qfontdialog.cpp 1
  If the user clicks OK the font they chose will be used for myWidget,
  and if they click Cancel the original font is used.

  \image plastique-fontdialog.png A font dialog in the Plastique widget style.

  \sa QFont, QFontInfo, QFontMetrics, QColorDialog, QFileDialog, QPrintDialog,
      {Standard Dialogs Example}
*/

class QFontDialogPrivate : public QDialogPrivate
{
    Q_DECLARE_PUBLIC(QFontDialog)
public:
    inline QFontDialogPrivate()
        : writingSystem(QFontDatabase::Any)
    { }

    static QFont getFont(bool *ok, const QFont *def, QWidget* parent, const QString &caption = QString());

    void _q_sizeChanged(const QString &);
    void _q_familyHighlighted(int);
    void _q_writingSystemHighlighted(int);
    void _q_styleHighlighted(int);
    void _q_sizeHighlighted(int);
    void _q_updateSample();
    void retranslateStrings();

    QLabel * familyAccel;
    QLineEdit * familyEdit;
    QFontListView * familyList;

    QLabel * styleAccel;
    QLineEdit * styleEdit;
    QFontListView * styleList;

    QLabel * sizeAccel;
    QLineEdit * sizeEdit;
    QFontListView * sizeList;

    QGroupBox * effects;
    QCheckBox * strikeout;
    QCheckBox * underline;
    QComboBox * color;

    QGroupBox * sample;
    QLineEdit * sampleEdit;

    QLabel * writingSystemAccel;
    QComboBox * writingSystemCombo;

    QBoxLayout * buttonLayout;
    QBoxLayout * effectsLayout;
    QBoxLayout * sampleLayout;
    QBoxLayout * sampleEditLayout;

    QFontDatabase fdb;

    QString       family;
    QFontDatabase::WritingSystem writingSystem;
    QString       style;
    int           size;

    bool smoothScalable;
};


/*!
  \internal
  Constructs a standard font dialog.

  Use setFont() to set the initial font attributes.

  The \a parent, \a name, \a modal and \a f parameters are passed to
  the QDialog constructor.

  \sa getFont()
*/

QFontDialog::QFontDialog(QWidget *parent, bool modal, Qt::WindowFlags f)
    : QDialog(*new QFontDialogPrivate, parent, f)
{
    Q_D(QFontDialog);
    setModal(modal);
    setSizeGripEnabled(true);
    // grid
    d->familyEdit = new QLineEdit(this);
    d->familyEdit->setReadOnly(true);
    d->familyList = new QFontListView(this);
    d->familyEdit->setFocusProxy(d->familyList);

    d->familyAccel = new QLabel(this);
#ifndef QT_NO_SHORTCUT
    d->familyAccel->setBuddy(d->familyList);
#endif
    d->familyAccel->setIndent(2);

    d->styleEdit = new QLineEdit(this);
    d->styleEdit->setReadOnly(true);
    d->styleList = new QFontListView(this);
    d->styleEdit->setFocusProxy(d->styleList);

    d->styleAccel = new QLabel(this);
#ifndef QT_NO_SHORTCUT
    d->styleAccel->setBuddy(d->styleList);
#endif
    d->styleAccel->setIndent(2);

    d->sizeEdit = new QLineEdit(this);
    d->sizeEdit->setFocusPolicy(Qt::ClickFocus);
    QIntValidator *validator = new QIntValidator(1, 512, this);
    d->sizeEdit->setValidator(validator);
    d->sizeList = new QFontListView(this);

    d->sizeAccel = new QLabel(this);
#ifndef QT_NO_SHORTCUT
    d->sizeAccel->setBuddy(d->sizeEdit);
#endif
    d->sizeAccel->setIndent(2);

    // effects box
    d->effects = new QGroupBox(this);
    QVBoxLayout *vbox = new QVBoxLayout(d->effects);
    d->strikeout = new QCheckBox(d->effects);
    vbox->addWidget(d->strikeout);
    d->underline = new QCheckBox(d->effects);
    vbox->addWidget(d->underline);

    d->sample = new QGroupBox(this);
    QHBoxLayout *hbox = new QHBoxLayout(d->sample);
    d->sampleEdit = new QLineEdit(d->sample);
    d->sampleEdit->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    d->sampleEdit->setAlignment(Qt::AlignCenter);
    // Note that the sample text is *not* translated with tr(), as the
    // characters used depend on the charset encoding.
    d->sampleEdit->setText(QLatin1String("AaBbYyZz"));
    hbox->addWidget(d->sampleEdit);

    d->writingSystemCombo = new QComboBox(this);

    d->writingSystemAccel = new QLabel(this);
#ifndef QT_NO_SHORTCUT
    d->writingSystemAccel->setBuddy(d->writingSystemCombo);
#endif
    d->writingSystemAccel->setIndent(2);

    d->size = 0;
    d->smoothScalable = false;

    connect(d->writingSystemCombo, SIGNAL(activated(int)),
            SLOT(_q_writingSystemHighlighted(int)));
    connect(d->familyList, SIGNAL(highlighted(int)),
            SLOT(_q_familyHighlighted(int)));
    connect(d->styleList, SIGNAL(highlighted(int)),
            SLOT(_q_styleHighlighted(int)));
    connect(d->sizeList, SIGNAL(highlighted(int)),
            SLOT(_q_sizeHighlighted(int)));
    connect(d->sizeEdit, SIGNAL(textChanged(QString)),
            SLOT(_q_sizeChanged(QString)));

    connect(d->strikeout, SIGNAL(clicked()),
            SLOT(_q_updateSample()));
    connect(d->underline, SIGNAL(clicked()),
            SLOT(_q_updateSample()));

    for (int i = 0; i < QFontDatabase::WritingSystemsCount; ++i) {
        QFontDatabase::WritingSystem ws = QFontDatabase::WritingSystem(i);
        QString writingSystemName = QFontDatabase::writingSystemName(ws);
        if (writingSystemName.isEmpty())
            break;
        d->writingSystemCombo->addItem(writingSystemName);
    }

    updateFamilies();
    if (d->familyList->count() != 0)
        d->familyList->setCurrentItem(0);

    // grid layout
    QGridLayout *mainGrid = new QGridLayout(this);

    int spacing = mainGrid->spacing();
    if (spacing >= 0) {     // uniform spacing
       mainGrid->setSpacing(0);

       mainGrid->setColumnMinimumWidth(1, spacing);
       mainGrid->setColumnMinimumWidth(3, spacing);

       int margin = 0;
       mainGrid->getContentsMargins(0, 0, 0, &margin);

       mainGrid->setRowMinimumHeight(3, margin);
       mainGrid->setRowMinimumHeight(6, 2);
       mainGrid->setRowMinimumHeight(8, margin);
    }

    mainGrid->addWidget(d->familyAccel, 0, 0);
    mainGrid->addWidget(d->familyEdit, 1, 0);
    mainGrid->addWidget(d->familyList, 2, 0);

    mainGrid->addWidget(d->styleAccel, 0, 2);
    mainGrid->addWidget(d->styleEdit, 1, 2);
    mainGrid->addWidget(d->styleList, 2, 2);

    mainGrid->addWidget(d->sizeAccel, 0, 4);
    mainGrid->addWidget(d->sizeEdit, 1, 4);
    mainGrid->addWidget(d->sizeList, 2, 4);

    mainGrid->setColumnStretch(0, 38);
    mainGrid->setColumnStretch(2, 24);
    mainGrid->setColumnStretch(4, 10);

    mainGrid->addWidget(d->effects, 4, 0);

    mainGrid->addWidget(d->sample, 4, 2, 4, 3);

    mainGrid->addWidget(d->writingSystemAccel, 5, 0);
    mainGrid->addWidget(d->writingSystemCombo, 7, 0);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    mainGrid->addWidget(buttonBox, 9, 0, 1, 5);

    QPushButton *button
            = static_cast<QPushButton *>(buttonBox->addButton(modal ? QDialogButtonBox::Ok
                                                                    : QDialogButtonBox::Apply));
    if (modal)
        connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    button->setDefault(true);

    buttonBox->addButton(modal ? QDialogButtonBox::Cancel : QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

#if defined(Q_OS_WINCE)
    resize(180, 120);
#else
    resize(500, 360);
#endif //Q_OS_WINCE

    d->sizeEdit->installEventFilter(this);
    d->familyList->installEventFilter(this);
    d->styleList->installEventFilter(this);
    d->sizeList->installEventFilter(this);

    d->familyList->setFocus();
    d->retranslateStrings();
}

/*!
  \internal
 Destroys the font dialog and frees up its storage.
*/

QFontDialog::~QFontDialog()
{
}

/*!
  Executes a modal font dialog and returns a font.

  If the user clicks \gui OK, the selected font is returned. If the user
  clicks \gui Cancel, the \a initial font is returned.

  The dialog is constructed with the given \a parent. \a caption is
  shown as the window title of the dialog and  \a initial is the
  initially selected font. If the \a ok parameter is not-null, the
  value it refers to is set to true if the user clicks \gui OK, and
  set to false if the user clicks \gui Cancel.

  Examples:
  \snippet doc/src/snippets/code/src.gui.dialogs.qfontdialog.cpp 2

    The dialog can also be used to set a widget's font directly:
  \snippet doc/src/snippets/code/src.gui.dialogs.qfontdialog.cpp 3
  In this example, if the user clicks OK the font they chose will be
  used, and if they click Cancel the original font is used.
*/
QFont QFontDialog::getFont(bool *ok, const QFont &initial, QWidget* parent, const QString &caption)
{
    return QFontDialogPrivate::getFont(ok, &initial, parent, caption);
}

/*!
  \overload
*/
QFont QFontDialog::getFont(bool *ok, const QFont &initial,
                            QWidget *parent)
{
    return QFontDialogPrivate::getFont(ok, &initial, parent);
}


/*!
    \overload

  Executes a modal font dialog and returns a font.

  If the user clicks \gui OK, the selected font is returned. If the user
  clicks \gui Cancel, the Qt default font is returned.

  The dialog is constructed with the given \a parent.
  If the \a ok parameter is not-null, the value it refers to is set
  to true if the user clicks \gui OK, and false if the user clicks
  \gui Cancel.

  Example:
  \snippet doc/src/snippets/code/src.gui.dialogs.qfontdialog.cpp 4

*/
QFont QFontDialog::getFont(bool *ok, QWidget *parent)
{
    return QFontDialogPrivate::getFont(ok, 0, parent);
}

QFont QFontDialogPrivate::getFont(bool *ok, const QFont *def, QWidget *parent,
                                  const QString &caption)
{
    QFont result;
    if (def)
        result = *def;

    QFontDialog *dlg = new QFontDialog(parent, true);

    dlg->setFont((def ? *def : QFont()));
    dlg->setWindowTitle(!caption.isEmpty() ? caption : QFontDialog::tr("Select Font"));

    bool res = (dlg->exec() == QDialog::Accepted);
    if (res)
        result = dlg->font();
    if (ok)
        *ok = res;
    delete dlg;
    return result;
}

/*!
  \internal
  ### Qt5 - remove me
*/
QFont QFontDialog::getFont(bool *ok, const QFont *def, QWidget* parent)
{
    return getFont(ok, *def, parent, QString());
}

/*!
    \internal
    An event filter to make the Up, Down, PageUp and PageDown keys work
    correctly in the line edits. The source of the event is the object
    \a o and the event is \a e.
*/

bool QFontDialog::eventFilter(QObject * o , QEvent * e)
{
    Q_D(QFontDialog);
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent * k = (QKeyEvent *)e;
        if (o == d->sizeEdit &&
        (k->key() == Qt::Key_Up ||
             k->key() == Qt::Key_Down ||
         k->key() == Qt::Key_PageUp ||
         k->key() == Qt::Key_PageDown)) {

            int ci = d->sizeList->currentItem();
            (void)QApplication::sendEvent(d->sizeList, k);

            if (ci != d->sizeList->currentItem() &&
                style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this))
                d->sizeEdit->selectAll();
            return true;
        } else if ((o == d->familyList || o == d->styleList) &&
                    (k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter)) {
            k->accept();
        accept();
            return true;
        }
    } else if (e->type() == QEvent::FocusIn &&
                style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this)) {
        if (o == d->familyList)
            d->familyEdit->selectAll();
        else if (o == d->styleList)
            d->styleEdit->selectAll();
        else if (o == d->sizeList)
            d->sizeEdit->selectAll();
    } else if (e->type() == QEvent::MouseButtonPress && o == d->sizeList) {
            d->sizeEdit->setFocus();
    }
    return QDialog::eventFilter(o, e);
}

/*!
  \internal
    Updates the contents of the "font family" list box. This
  function can be reimplemented if you have special requirements.
*/

void QFontDialog::updateFamilies()
{
    Q_D(QFontDialog);
    d->familyList->blockSignals(true);

    enum match_t { MATCH_NONE=0, MATCH_LAST_RESORT=1, MATCH_APP=2, MATCH_FALLBACK, MATCH_FAMILY=3 };

    QStringList familyNames = d->fdb.families(d->writingSystem);

    d->familyList->model()->setStringList(familyNames);

    QString foundryName1, familyName1, foundryName2, familyName2;
    int bestFamilyMatch = -1;
    match_t bestFamilyType = MATCH_NONE;

    QFont f;

    // ##### do the right thing for a list of family names in the font.
    QFontDatabase::parseFontName(d->family, foundryName1, familyName1);

    QStringList::Iterator it = familyNames.begin();
    int i = 0;
    for(; it != familyNames.end(); ++it, ++i) {

        QFontDatabase::parseFontName(*it, foundryName2, familyName2);

        //try to match..
        if (familyName1 == familyName2) {
            bestFamilyType = MATCH_FAMILY;
            if (foundryName1 == foundryName2) {
                bestFamilyMatch = i;
                break;
            }
            if (bestFamilyMatch < MATCH_FAMILY)
                bestFamilyMatch = i;
        }

        //and try some fall backs
        match_t type = MATCH_NONE;
        if (bestFamilyType <= MATCH_NONE && familyName2 == f.lastResortFamily())
            type = MATCH_LAST_RESORT;
        if (bestFamilyType <= MATCH_LAST_RESORT && familyName2 == f.family())
            type = MATCH_APP;
        // ### add fallback for writingSystem
        if (type != MATCH_NONE) {
            bestFamilyType = type;
            bestFamilyMatch = i;
        }
    }

    if (i != -1 && bestFamilyType != MATCH_NONE)
        d->familyList->setCurrentItem(bestFamilyMatch);
    else
        d->familyList->setCurrentItem(0);
    d->familyEdit->setText(d->familyList->currentText());
    if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
        d->familyList->hasFocus())
        d->familyEdit->selectAll();

    d->familyList->blockSignals(false);
    updateStyles();
}

/*!
    \internal
    Updates the contents of the "font style" list box. This
  function can be reimplemented if you have special requirements.
*/

void QFontDialog::updateStyles()
{
    Q_D(QFontDialog);
    d->styleList->blockSignals(true);


    QStringList styles = d->fdb.styles(d->familyList->currentText());
    d->styleList->model()->setStringList(styles);

    if (styles.isEmpty()) {
        d->styleEdit->clear();
        d->smoothScalable = false;
    } else {
        if (!d->style.isEmpty()) {
            bool found = false;
            bool first = true;
            QString cstyle = d->style;
        redo:
            for ( int i = 0 ; i < (int)d->styleList->count() ; i++ ) {
                if ( cstyle == d->styleList->text(i) ) {
                     d->styleList->setCurrentItem( i );
                     found = true;
                     break;
                 }
            }
            if (!found && first) {
                if (cstyle.contains(QLatin1String("Italic"))) {
                    cstyle.replace(QLatin1String("Italic"), QLatin1String("Oblique"));
                    first = false;
                    goto redo;
                } else if (cstyle.contains(QLatin1String("Oblique"))) {
                    cstyle.replace(QLatin1String("Oblique"), QLatin1String("Italic"));
                    first = false;
                    goto redo;
                }
            }
            if (!found)
                d->styleList->setCurrentItem(0);
        }

        d->styleEdit->setText(d->styleList->currentText());
        if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
            d->styleList->hasFocus())
            d->styleEdit->selectAll();

        d->smoothScalable = d->fdb.isSmoothlyScalable(d->familyList->currentText(), d->styleList->currentText());
    }

    d->styleList->blockSignals(false);

    updateSizes();
}

/*!
    \internal
    Updates the contents of the "font size" list box. This
  function can be reimplemented if you have special requirements.
*/

void QFontDialog::updateSizes()
{
    Q_D(QFontDialog);
    d->sizeList->blockSignals(true);

    if (!d->familyList->currentText().isEmpty()) {
        QList<int> sizes = d->fdb.pointSizes(d->familyList->currentText(), d->styleList->currentText());

        int i = 0;
        int current = -1;
        QStringList str_sizes;
        for(QList<int>::const_iterator it = sizes.constBegin() ; it != sizes.constEnd(); ++it) {
            str_sizes.append(QString::number(*it));
            if (current == -1 && *it >= d->size)
                current = i;
            ++i;
        }
        d->sizeList->model()->setStringList(str_sizes);
        if (current == -1)
            // we request a size bigger than the ones in the list, select the biggest one
            current = d->sizeList->count() - 1;
        d->sizeList->setCurrentItem(current);

        d->sizeEdit->blockSignals(true);
        d->sizeEdit->setText((d->smoothScalable ? QString::number(d->size) : d->sizeList->currentText()));
        if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
             d->sizeList->hasFocus())
            d->sizeEdit->selectAll();
        d->sizeEdit->blockSignals(false);
    } else {
        d->sizeEdit->clear();
    }

    d->sizeList->blockSignals(false);
    d->_q_updateSample();
}

void QFontDialogPrivate::_q_updateSample()
{
    Q_Q(QFontDialog);
    if (familyList->currentText().isEmpty())
        sampleEdit->clear();
    else
        sampleEdit->setFont(q->font());
}

/*!
    \internal
*/
void QFontDialogPrivate::_q_writingSystemHighlighted(int index)
{
    Q_Q(QFontDialog);
    writingSystem = QFontDatabase::WritingSystem(index);
    sampleEdit->setText(fdb.writingSystemSample(writingSystem));
    q->updateFamilies();
}

/*!
    \internal
*/
void QFontDialogPrivate::_q_familyHighlighted(int i)
{
    Q_Q(QFontDialog);
    family = familyList->text(i);
    familyEdit->setText(family);
    if (q->style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, q) &&
         familyList->hasFocus())
        familyEdit->selectAll();

    q->updateStyles();
}


/*!
    \internal
*/

void QFontDialogPrivate::_q_styleHighlighted(int index)
{
    Q_Q(QFontDialog);
    QString s = styleList->text(index);
    styleEdit->setText(s);
    if (q->style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, q) &&
         styleList->hasFocus())
        styleEdit->selectAll();

    style = s;

    q->updateSizes();
}


/*!
    \internal
*/

void QFontDialogPrivate::_q_sizeHighlighted(int index)
{
    Q_Q(QFontDialog);
    QString s = sizeList->text(index);
    sizeEdit->setText(s);
    if (q->style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, q) &&
         sizeEdit->hasFocus())
        sizeEdit->selectAll();

    size = s.toInt();
    _q_updateSample();
}

/*!
    \internal
    This slot is called if the user changes the font size.
    The size is passed in the \a s argument as a \e string.
*/

void QFontDialogPrivate::_q_sizeChanged(const QString &s)
{
    // no need to check if the conversion is valid, since we have an QIntValidator in the size edit
    int size = s.toInt();
    if (this->size == size)
        return;

    this->size = size;
    if (sizeList->count() != 0) {
        int i;
        for (i = 0 ; i < sizeList->count() - 1 ; i++) {
            if (sizeList->text(i).toInt() >= this->size)
                break;
        }
        sizeList->blockSignals(true);
        sizeList->setCurrentItem(i);
        sizeList->blockSignals(false);
    }
    _q_updateSample();
}

void QFontDialogPrivate::retranslateStrings()
{
    familyAccel->setText(QFontDialog::tr("&Font"));
    styleAccel->setText(QFontDialog::tr("Font st&yle"));
    sizeAccel->setText(QFontDialog::tr("&Size"));
    effects->setTitle(QFontDialog::tr("Effects"));
    strikeout->setText(QFontDialog::tr("Stri&keout"));
    underline->setText(QFontDialog::tr("&Underline"));
    sample->setTitle(QFontDialog::tr("Sample"));
    writingSystemAccel->setText(QFontDialog::tr("Wr&iting System"));
}


/*!
    \reimp
*/
void QFontDialog::changeEvent(QEvent *e)
{
    Q_D(QFontDialog);
    if (e->type() == QEvent::LanguageChange) {
        d->retranslateStrings();
    }
    QDialog::changeEvent(e);
}

/*!
  \internal
  Sets the font highlighted in the QFontDialog to font \a f.

  \sa font()
*/

void QFontDialog::setFont(const QFont &f)
{
    Q_D(QFontDialog);
    d->family = f.family();
    d->style = d->fdb.styleString(f);
    d->size = f.pointSize();
    if (d->size == -1) {
            QFontInfo fi(f);
            d->size = fi.pointSize();
    }
    d->strikeout->setChecked(f.strikeOut());
    d->underline->setChecked(f.underline());

    updateFamilies();
}

/*!
  \internal
  Returns the font which the user has chosen.

  \sa setFont()
*/

QFont QFontDialog::font() const
{
    Q_D(const QFontDialog);
    int pSize = d->sizeEdit->text().toInt();

    QFont f = d->fdb.font(d->familyList->currentText(), d->style, pSize);
    f.setStrikeOut(d->strikeout->isChecked());
    f.setUnderline(d->underline->isChecked());
    return f;
}

/*!
    \fn QFont QFontDialog::getFont(bool *ok, const QFont &def, QWidget* parent, const char* name)

    Call getFont(\a ok, \a def, \a parent) instead.

    The \a name parameter is ignored.
*/

/*!
    \fn QFont QFontDialog::getFont(bool *ok, QWidget* parent, const char* name)

    Call getFont(\a ok, \a parent) instead.

    The \a name parameter is ignored.
*/

QT_END_NAMESPACE

#include "qfontdialog.moc"
#include "moc_qfontdialog.cpp"

#endif // QT_NO_FONTDIALOG
