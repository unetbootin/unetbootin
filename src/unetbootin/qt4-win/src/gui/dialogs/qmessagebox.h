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

#ifndef QMESSAGEBOX_H
#define QMESSAGEBOX_H

#include <QtGui/qdialog.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_MESSAGEBOX

class QLabel;
class QMessageBoxPrivate;
class QAbstractButton;

class Q_GUI_EXPORT QMessageBox : public QDialog
{
    Q_OBJECT
    Q_ENUMS(Icon)
    Q_FLAGS(StandardButtons)
    Q_PROPERTY(QString text READ text WRITE setText)
    // ### Qt 5: Rename 'icon' 'standardIcon' and 'iconPixmap' 'icon' (and use QIcon?)
    Q_PROPERTY(Icon icon READ icon WRITE setIcon)
    Q_PROPERTY(QPixmap iconPixmap READ iconPixmap WRITE setIconPixmap)
    Q_PROPERTY(Qt::TextFormat textFormat READ textFormat WRITE setTextFormat)
    Q_PROPERTY(StandardButtons standardButtons READ standardButtons WRITE setStandardButtons)
#ifndef QT_NO_TEXTEDIT
    Q_PROPERTY(QString detailedText READ detailedText WRITE setDetailedText)
#endif
    Q_PROPERTY(QString informativeText READ informativeText WRITE setInformativeText)

public:
    enum Icon {
        NoIcon = 0,
        Information = 1,
        Warning = 2,
        Critical = 3,
        Question = 4
    };

    enum ButtonRole {
        // keep this in sync with QDialogButtonBox::ButtonRole
        InvalidRole = -1,
        AcceptRole,
        RejectRole,
        DestructiveRole,
        ActionRole,
        HelpRole,
        YesRole,
        NoRole,
        ResetRole,
        ApplyRole,

        NRoles
    };

    enum StandardButton {
        // keep this in sync with QDialogButtonBox::StandardButton
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000,

        FirstButton        = Ok,                // internal
        LastButton         = RestoreDefaults,   // internal

        YesAll             = YesToAll,          // obsolete
        NoAll              = NoToAll,           // obsolete

        Default            = 0x00000100,        // obsolete
        Escape             = 0x00000200,        // obsolete
        FlagMask           = 0x00000300,        // obsolete
        ButtonMask         = ~FlagMask          // obsolete
    };
    typedef StandardButton Button;  // obsolete

    Q_DECLARE_FLAGS(StandardButtons, StandardButton)

    explicit QMessageBox(QWidget *parent = 0);
    QMessageBox(Icon icon, const QString &title, const QString &text,
                  StandardButtons buttons = NoButton, QWidget *parent = 0,
                  Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ~QMessageBox();

    void addButton(QAbstractButton *button, ButtonRole role);
    QPushButton *addButton(const QString &text, ButtonRole role);
    QPushButton *addButton(StandardButton button);
    void removeButton(QAbstractButton *button);
#ifdef Q_OS_WINCE
    void setVisible(bool visible);
#endif

/*
    These are probably overkill:

    QList<QAbstractButton *> buttons() const;
    ButtonRole buttonRole(QAbstractButton *button) const;
*/

    void setStandardButtons(StandardButtons buttons);
    StandardButtons standardButtons() const;
    StandardButton standardButton(QAbstractButton *button) const;
    QAbstractButton *button(StandardButton which) const;

    QPushButton *defaultButton() const;
    void setDefaultButton(QPushButton *button);
    void setDefaultButton(StandardButton button);

    QAbstractButton *escapeButton() const;
    void setEscapeButton(QAbstractButton *button);
    void setEscapeButton(StandardButton button);

    QAbstractButton *clickedButton() const;

    QString text() const;
    void setText(const QString &text);

    // ### QString informativeText() const;
    // void setInformativeText(const QString &text);

    Icon icon() const;
    void setIcon(Icon);

    QPixmap iconPixmap() const;
    void setIconPixmap(const QPixmap &pixmap);

    Qt::TextFormat textFormat() const;
    void setTextFormat(Qt::TextFormat format);

    static StandardButton information(QWidget *parent, const QString &title,
         const QString &text, StandardButtons buttons = Ok,
         StandardButton defaultButton = NoButton);
    static StandardButton question(QWidget *parent, const QString &title,
         const QString &text, StandardButtons buttons = Ok,
         StandardButton defaultButton = NoButton);
    static StandardButton warning(QWidget *parent, const QString &title,
         const QString &text, StandardButtons buttons = Ok,
         StandardButton defaultButton = NoButton);
    static StandardButton critical(QWidget *parent, const QString &title,
         const QString &text, StandardButtons buttons = Ok,
         StandardButton defaultButton = NoButton);
    static void about(QWidget *parent, const QString &title, const QString &text);
    static void aboutQt(QWidget *parent, const QString &title = QString());

    QSize sizeHint() const;

    // the following functions are obsolete:

    QMessageBox(const QString &title, const QString &text, Icon icon,
                  int button0, int button1, int button2,
                  QWidget *parent = 0,
                  Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    static int information(QWidget *parent, const QString &title,
                           const QString& text,
                           int button0, int button1 = 0, int button2 = 0);
    static int information(QWidget *parent, const QString &title,
                           const QString& text,
                           const QString& button0Text,
                           const QString& button1Text = QString(),
                           const QString& button2Text = QString(),
                           int defaultButtonNumber = 0,
                           int escapeButtonNumber = -1);
    inline static StandardButton information(QWidget *parent, const QString &title,
                                  const QString& text,
                                  StandardButton button0, StandardButton button1 = NoButton)
    { return information(parent, title, text, StandardButtons(button0), button1); }

    static int question(QWidget *parent, const QString &title,
                        const QString& text,
                        int button0, int button1 = 0, int button2 = 0);
    static int question(QWidget *parent, const QString &title,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text = QString(),
                        const QString& button2Text = QString(),
                        int defaultButtonNumber = 0,
                        int escapeButtonNumber = -1);
    inline static int question(QWidget *parent, const QString &title,
                               const QString& text,
                               StandardButton button0, StandardButton button1)
    { return question(parent, title, text, StandardButtons(button0), button1); }

    static int warning(QWidget *parent, const QString &title,
                       const QString& text,
                       int button0, int button1, int button2 = 0);
    static int warning(QWidget *parent, const QString &title,
                       const QString& text,
                       const QString& button0Text,
                       const QString& button1Text = QString(),
                       const QString& button2Text = QString(),
                       int defaultButtonNumber = 0,
                       int escapeButtonNumber = -1);
    inline static int warning(QWidget *parent, const QString &title,
                              const QString& text,
                              StandardButton button0, StandardButton button1)
    { return warning(parent, title, text, StandardButtons(button0), button1); }

    static int critical(QWidget *parent, const QString &title,
                        const QString& text,
                        int button0, int button1, int button2 = 0);
    static int critical(QWidget *parent, const QString &title,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text = QString(),
                        const QString& button2Text = QString(),
                        int defaultButtonNumber = 0,
                        int escapeButtonNumber = -1);
    inline static int critical(QWidget *parent, const QString &title,
                               const QString& text,
                               StandardButton button0, StandardButton button1)
    { return critical(parent, title, text, StandardButtons(button0), button1); }

    QString buttonText(int button) const;
    void setButtonText(int button, const QString &text);

    QString informativeText() const;
    void setInformativeText(const QString &text);

#ifndef QT_NO_TEXTEDIT
    QString detailedText() const;
    void setDetailedText(const QString &text);
#endif

    void setWindowTitle(const QString &title);
    void setWindowModality(Qt::WindowModality windowModality);

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QMessageBox(const QString &title, const QString &text, Icon icon,
                                          int button0, int button1, int button2,
                                          QWidget *parent, const char *name, bool modal,
                                           Qt::WindowFlags f =  Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    QT3_SUPPORT_CONSTRUCTOR QMessageBox(QWidget *parent, const char *name);

    static QT3_SUPPORT QPixmap standardIcon(Icon icon, Qt::GUIStyle);
    static QT3_SUPPORT int message(const QString &title,
                                   const QString& text,
                                   const QString& buttonText=QString(),
                                   QWidget *parent = 0, const char * = 0) {
        return QMessageBox::information(parent, title, text,
                                        buttonText.isEmpty() ? tr("OK") : buttonText) == 0;
    }
    static QT3_SUPPORT bool query(const QString &title,
                                  const QString& text,
                                  const QString& yesButtonText = QString(),
                                  const QString& noButtonText = QString(),
                                  QWidget *parent = 0, const char * = 0) {
        return QMessageBox::information(parent, title, text,
                                        yesButtonText.isEmpty() ? tr("OK") : yesButtonText,
                                        noButtonText) == 0;
    }
#endif

    static QPixmap standardIcon(Icon icon);

#ifdef qdoc
public Q_SLOTS:
    int exec();
#endif

protected:
    bool event(QEvent *e);
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void changeEvent(QEvent *event);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_buttonClicked(QAbstractButton *))

    Q_DISABLE_COPY(QMessageBox)
    Q_DECLARE_PRIVATE(QMessageBox)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QMessageBox::StandardButtons)

#define QT_REQUIRE_VERSION(argc, argv, str) { QString s = QString::fromLatin1(str);\
QString sq = QString::fromLatin1(qVersion()); \
if ((sq.section(QChar::fromLatin1('.'),0,0).toInt()<<16)+\
(sq.section(QChar::fromLatin1('.'),1,1).toInt()<<8)+\
sq.section(QChar::fromLatin1('.'),2,2).toInt()<(s.section(QChar::fromLatin1('.'),0,0).toInt()<<16)+\
(s.section(QChar::fromLatin1('.'),1,1).toInt()<<8)+\
s.section(QChar::fromLatin1('.'),2,2).toInt()) { \
if (!qApp){ \
    new QApplication(argc,argv); \
} \
QString s = QApplication::tr("Executable '%1' requires Qt "\
 "%2, found Qt %3.").arg(qAppName()).arg(QString::fromLatin1(\
str)).arg(QString::fromLatin1(qVersion())); QMessageBox::critical(0, QApplication::tr(\
"Incompatible Qt Library Error"), s, QMessageBox::Abort, 0); qFatal(s.toLatin1().data()); }}

#endif // QT_NO_MESSAGEBOX

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMESSAGEBOX_H
