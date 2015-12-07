/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3PROGRESSDIALOG_H
#define Q3PROGRESSDIALOG_H

#include <QtGui/qdialog.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_PROGRESSDIALOG

class Q3ProgressDialogData;
class QLabel;
class QPushButton;
class QTimer;
class Q3ProgressBar;

class Q_COMPAT_EXPORT Q3ProgressDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool wasCanceled READ wasCanceled)
    Q_PROPERTY(bool wasCancelled READ wasCancelled DESIGNABLE false STORED false)
    Q_PROPERTY(int totalSteps READ totalSteps WRITE setTotalSteps)
    Q_PROPERTY(int progress READ progress WRITE setProgress)
    Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset)
    Q_PROPERTY(bool autoClose READ autoClose WRITE setAutoClose)
    Q_PROPERTY(int minimumDuration READ minimumDuration WRITE setMinimumDuration)
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)

public:
    Q3ProgressDialog(QWidget* parent, const char* name, bool modal=false,
                                           Qt::WindowFlags f=0);
    Q3ProgressDialog(const QString& labelText,
                                           const QString &cancelButtonText, int totalSteps,
                                           QWidget* parent=0, const char* name=0,
                                           bool modal=false, Qt::WindowFlags f=0);
    Q3ProgressDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
    Q3ProgressDialog(const QString& labelText, const QString &cancelButtonText,
                     int totalSteps, QWidget* parent=0, Qt::WindowFlags f=0);
    ~Q3ProgressDialog();

    void setLabel(QLabel *);
    void setCancelButton(QPushButton *);
    void setBar(Q3ProgressBar *);

    bool wasCanceled() const;
    bool wasCancelled() const;

    int totalSteps() const;
    int progress()   const;

    QSize sizeHint() const;

    QString labelText() const;

    void setAutoReset(bool b);
    bool autoReset() const;
    void setAutoClose(bool b);
    bool autoClose() const;

public Q_SLOTS:
    void cancel();
    void reset();
    void setTotalSteps(int totalSteps);
    void setProgress(int progress);
    void setProgress(int progress, int totalSteps);
    void setLabelText(const QString &);
    void setCancelButtonText(const QString &);

    void setMinimumDuration(int ms);
public:
    int minimumDuration() const;

Q_SIGNALS:
    void canceled();
    void cancelled();

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);
    void changeEvent(QEvent *);
    void showEvent(QShowEvent *e);

protected Q_SLOTS:
    void forceShow();

private:
    void init(QWidget *creator, const QString& lbl, const QString &canc,
              int totstps);
    void layout();
    QLabel *label()  const;
    Q3ProgressBar *bar()    const;
    Q3ProgressDialogData *d;
    QTimer *forceTimer;

private:
    Q_DISABLE_COPY(Q3ProgressDialog)
};

#endif // QT_NO_PROGRESSDIALOG

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PROGRESSDIALOG_H
