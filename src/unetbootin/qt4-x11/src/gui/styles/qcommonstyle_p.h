/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCOMMONSTYLE_P_H
#define QCOMMONSTYLE_P_H

#include "qcommonstyle.h"
#include "qstyle_p.h"

#include "qstyleoption.h"

QT_BEGIN_NAMESPACE

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

class QStringList;

#ifdef Q_WS_X11
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
#endif

// Private class
class QCommonStylePrivate : public QStylePrivate
{
    Q_DECLARE_PUBLIC(QCommonStyle)
public:
    inline QCommonStylePrivate()
#ifndef QT_NO_ITEMVIEWS
    : cachedOption(0)
#endif
    { }

#ifndef QT_NO_ITEMVIEWS
    ~QCommonStylePrivate()
    {
        delete cachedOption;
    }
    void viewItemDrawText(QPainter *p, const QStyleOptionViewItemV4 *option, const QRect &rect) const;
    void viewItemLayout(const QStyleOptionViewItemV4 *opt,  QRect *checkRect,
                        QRect *pixmapRect, QRect *textRect, bool sizehint) const;
    QSize viewItemSize(const QStyleOptionViewItemV4 *option, int role) const;

    mutable QRect decorationRect, displayRect, checkRect;
    mutable QStyleOptionViewItemV4 *cachedOption;
    bool isViewItemCached(const QStyleOptionViewItemV4 &option) const {
        return cachedOption && (option.rect == cachedOption->rect
               && option.direction == cachedOption->direction
               && option.state == cachedOption->state
               && option.displayAlignment == cachedOption->displayAlignment
               && option.decorationAlignment == cachedOption->decorationAlignment
               && option.decorationPosition == cachedOption->decorationPosition
               && option.decorationSize == cachedOption->decorationSize
               && option.font == cachedOption->font
               && option.features == cachedOption->features
               && option.widget == cachedOption->widget
               && option.index == cachedOption->index
               && option.icon.isNull() == cachedOption->icon.isNull()
               && option.text == cachedOption->text
               && option.viewItemPosition == cachedOption->viewItemPosition);
    }
#endif
    mutable QIcon tabBarcloseButtonIcon;

//icon detection on X11
#ifdef Q_WS_X11
    void lookupIconTheme() const;
    QIcon createIcon(const QString &) const;
    QPixmap findIcon(int size, const QString &) const;
    QPixmap findIconHelper(int size, const QString &, const QString &, QStringList &visited) const;
    QIconTheme parseIndexFile(const QString &themeName) const;
    mutable QString themeName;
    mutable QStringList iconDirs;
    mutable QHash <QString, QIconTheme> themeList;
#endif

};

QT_END_NAMESPACE

#endif //QCOMMONSTYLE_P_H
