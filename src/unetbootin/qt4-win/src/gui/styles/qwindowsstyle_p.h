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

#ifndef QWINDOWSSTYLE_P_H
#define QWINDOWSSTYLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "qwindowsstyle.h"
#include "qcommonstyle_p.h"

#ifndef QT_NO_STYLE_WINDOWS
#include <qlist.h>
#include <qdatetime.h>
#include <qhash.h>

QT_BEGIN_NAMESPACE

class QStringList;
class QTime;
class QProgressBar;

class QIconTheme
{

public:
    QIconTheme(QHash <int, QString> dirList, QStringList parents) :
          _dirList(dirList), _parents(parents), _valid(true){ }
    QIconTheme() : _valid(false){ }

    QHash <int, QString> dirList() {return _dirList;}
    QStringList parents() {return _parents;}
    bool isValid() {return _valid;}

private:
    QHash <int, QString> _dirList;
    QStringList _parents;
    bool _valid;
};

class QWindowsStylePrivate : public QCommonStylePrivate
{
    Q_DECLARE_PUBLIC(QWindowsStyle)
public:
    QWindowsStylePrivate();
    bool hasSeenAlt(const QWidget *widget) const;
    bool altDown() const { return alt_down; }
    bool alt_down;
    QList<const QWidget *> seenAlt;
    int menuBarTimer;

    QList<QProgressBar *> bars;
    int animationFps;
    int animateTimer;
    QTime startTime;
    int animateStep;    
    QColor inactiveCaptionText;
    QColor activeCaptionColor;
    QColor activeGradientCaptionColor;
    QColor inactiveCaptionColor;
    QColor inactiveGradientCaptionColor;
    
    //icon detection on X11
    QPixmap findIcon(int size, const QString &) const;
#ifdef Q_WS_X11
    QPixmap findIconHelper(int size, const QString &, const QString &, QStringList &visited) const;
    QIconTheme parseIndexFile(const QString &themeName) const;
    mutable QString themeName;
    QStringList iconDirs;
    mutable QHash <QString, QIconTheme> themeList;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_STYLE_WINDOWS

#endif //QWINDOWSSTYLE_P_H
