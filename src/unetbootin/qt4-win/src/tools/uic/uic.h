/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef UIC_H
#define UIC_H

#include "databaseinfo.h"
#include "customwidgetsinfo.h"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QStack>

QT_BEGIN_NAMESPACE

class QTextStream;
class QIODevice;

class Driver;
class DomUI;
class DomWidget;
class DomSpacer;
class DomLayout;
class DomLayoutItem;
class DomItem;

struct Option;

class Uic
{
public:
    Uic(Driver *driver);
    ~Uic();

    bool printDependencies();

    inline Driver *driver() const
    { return drv; }

    inline QTextStream &output()
    { return out; }

    inline const Option &option() const
    { return opt; }

    inline QString pixmapFunction() const
    { return pixFunction; }

    inline void setPixmapFunction(const QString &f)
    { pixFunction = f; }

    inline bool hasExternalPixmap() const
    { return externalPix; }

    inline void setExternalPixmap(bool b)
    { externalPix = b; }

    inline const DatabaseInfo *databaseInfo() const
    { return &info; }

    inline const CustomWidgetsInfo *customWidgetsInfo() const
    { return &cWidgetsInfo; }

    bool write(QIODevice *in);

#ifdef QT_UIC_JAVA_GENERATOR
    bool jwrite(DomUI *ui);
#endif

#ifdef QT_UIC_CPP_GENERATOR
    bool write(DomUI *ui);
#endif

    bool isMainWindow(const QString &className) const;
    bool isToolBar(const QString &className) const;
    bool isStatusBar(const QString &className) const;
    bool isButton(const QString &className) const;
    bool isContainer(const QString &className) const;
    bool isMenuBar(const QString &className) const;
    bool isMenu(const QString &className) const;

private:
    // copyright header
    void writeCopyrightHeader(DomUI *ui);

#ifdef QT_UIC_CPP_GENERATOR
    // header protection
    void writeHeaderProtectionStart();
    void writeHeaderProtectionEnd();
#endif

private:
    Driver *drv;
    QTextStream &out;
    Option &opt;
    DatabaseInfo info;
    CustomWidgetsInfo cWidgetsInfo;
    QString pixFunction;
    bool externalPix;
};

QT_END_NAMESPACE

#endif // UIC_H
