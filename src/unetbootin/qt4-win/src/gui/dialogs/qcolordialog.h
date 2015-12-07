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

#ifndef QCOLORDIALOG_H
#define QCOLORDIALOG_H

#include <QtGui/qdialog.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_COLORDIALOG

class QColorDialogPrivate;

class Q_GUI_EXPORT QColorDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QColorDialog)

public:
    static QColor getColor(const QColor& init = Qt::white, QWidget* parent=0);
    static QRgb getRgba(QRgb = 0xffffffff, bool* ok = 0, QWidget* parent=0);

    static int customCount();
    static QRgb customColor(int);
    static void setCustomColor(int, QRgb);
    static void setStandardColor(int, QRgb);

#ifdef QT3_SUPPORT
    static QColor getColor(const QColor& init, QWidget* parent, const char* name)
        { Q_UNUSED(name); return getColor(init, parent); }
    static QRgb getRgba(QRgb rgba, bool* ok, QWidget* parent, const char* name)
        { Q_UNUSED(name); return getRgba(rgba, ok, parent); }
#endif

protected:
    void changeEvent(QEvent *e);
private:
    ~QColorDialog();
    explicit QColorDialog(QWidget* parent=0, bool modal=false);

    void setColor(const QColor&);
    QColor color() const;

    bool selectColor(const QColor&);

    void setSelectedAlpha(int);
    int selectedAlpha() const;

    void showCustom(bool=true);

private:
    Q_DISABLE_COPY(QColorDialog)

    Q_PRIVATE_SLOT(d_func(), void _q_addCustom())

    Q_PRIVATE_SLOT(d_func(), void _q_newHsv(int h, int s, int v))
    Q_PRIVATE_SLOT(d_func(), void _q_newColorTypedIn(QRgb rgb))
    Q_PRIVATE_SLOT(d_func(), void _q_newCustom(int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_newStandard(int, int))

    friend class QColorShower;
};

#endif // QT_NO_COLORDIALOG

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCOLORDIALOG_H
