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

#ifndef Q3PROGRESSBAR_H
#define Q3PROGRESSBAR_H

#include <QtGui/qframe.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_PROGRESSBAR

class Q3ProgressBarPrivate;

class Q_COMPAT_EXPORT Q3ProgressBar : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int totalSteps READ totalSteps WRITE setTotalSteps)
    Q_PROPERTY(int progress READ progress WRITE setProgress)
    Q_PROPERTY(QString progressString READ progressString)
    Q_PROPERTY(bool centerIndicator READ centerIndicator WRITE setCenterIndicator)
    Q_PROPERTY(bool percentageVisible READ percentageVisible WRITE setPercentageVisible)

public:
    Q3ProgressBar(QWidget *parent, const char *name, Qt::WindowFlags f=0);
    Q3ProgressBar(int totalSteps, QWidget *parent, const char *name,
                  Qt::WindowFlags f=0);
    Q3ProgressBar(QWidget *parent = 0, Qt::WindowFlags f = 0);
    Q3ProgressBar(int totalSteps, QWidget *parent = 0, Qt::WindowFlags f=0);

    int totalSteps() const;
    int progress() const;
    const QString &progressString() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setCenterIndicator(bool on);
    bool centerIndicator() const;

    bool percentageVisible() const;
    void setPercentageVisible(bool);

    void setVisible(bool visible);

    void setMargin(int margin) { setContentsMargins(margin, margin, margin, margin); }
    int margin() const 
    { int margin; int dummy; getContentsMargins(&margin, &dummy, &dummy, &dummy);  return margin; }    

public Q_SLOTS:
    void reset();
    virtual void setTotalSteps(int totalSteps);
    virtual void setProgress(int progress);
    void setProgress(int progress, int totalSteps);

protected:
    void paintEvent(QPaintEvent *);
    virtual bool setIndicator(QString &progress_str, int progress, int totalSteps);
    void changeEvent(QEvent *);

private:
    Q_DISABLE_COPY(Q3ProgressBar)

    int total_steps;
    int progress_val;
    int percentage;
    QString progress_str;
    bool center_indicator : 1;
    bool percentage_visible : 1;
    Q3ProgressBarPrivate *d;
    void initFrame();
};


inline int Q3ProgressBar::totalSteps() const
{
    return total_steps;
}

inline int Q3ProgressBar::progress() const
{
    return progress_val;
}

inline const QString &Q3ProgressBar::progressString() const
{
    return progress_str;
}

inline bool Q3ProgressBar::centerIndicator() const
{
    return center_indicator;
}

inline bool Q3ProgressBar::percentageVisible() const
{
    return percentage_visible;
}

#endif // QT_NO_PROGRESSBAR

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PROGRESSBAR_H
