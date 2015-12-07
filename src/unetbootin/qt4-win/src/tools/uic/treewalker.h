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

#ifndef TREEWALKER_H
#define TREEWALKER_H

#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class DomUI;
class DomLayoutDefault;
class DomLayoutFunction;
class DomTabStops;
class DomLayout;
class DomLayoutItem;
class DomWidget;
class DomSpacer;
class DomColor;
class DomColorGroup;
class DomPalette;
class DomFont;
class DomPoint;
class DomRect;
class DomSizePolicy;
class DomSize;
class DomDate;
class DomTime;
class DomDateTime;
class DomProperty;
class DomCustomWidgets;
class DomCustomWidget;
class DomAction;
class DomActionGroup;
class DomActionRef;
class DomImages;
class DomImage;
class DomItem;
class DomIncludes;
class DomInclude;
class DomString;
class DomResourcePixmap;
class DomResources;
class DomResource;
class DomConnections;
class DomConnection;
class DomConnectionHints;
class DomConnectionHint;
class DomScript;

struct TreeWalker
{
    inline virtual ~TreeWalker() {}

    virtual void acceptUI(DomUI *ui);
    virtual void acceptLayoutDefault(DomLayoutDefault *layoutDefault);
    virtual void acceptLayoutFunction(DomLayoutFunction *layoutFunction);
    virtual void acceptTabStops(DomTabStops *tabStops);
    virtual void acceptCustomWidgets(DomCustomWidgets *customWidgets);
    virtual void acceptCustomWidget(DomCustomWidget *customWidget);
    virtual void acceptLayout(DomLayout *layout);
    virtual void acceptLayoutItem(DomLayoutItem *layoutItem);
    virtual void acceptWidget(DomWidget *widget);
    virtual void acceptSpacer(DomSpacer *spacer);
    virtual void acceptColor(DomColor *color);
    virtual void acceptColorGroup(DomColorGroup *colorGroup);
    virtual void acceptPalette(DomPalette *palette);
    virtual void acceptFont(DomFont *font);
    virtual void acceptPoint(DomPoint *point);
    virtual void acceptRect(DomRect *rect);
    virtual void acceptSizePolicy(DomSizePolicy *sizePolicy);
    virtual void acceptSize(DomSize *size);
    virtual void acceptDate(DomDate *date);
    virtual void acceptTime(DomTime *time);
    virtual void acceptDateTime(DomDateTime *dateTime);
    virtual void acceptProperty(DomProperty *property);
    typedef QList<DomScript *> DomScripts;
    typedef QList<DomWidget *> DomWidgets;
    virtual void acceptWidgetScripts(const DomScripts &, DomWidget *node, const  DomWidgets &childWidgets);
    virtual void acceptImages(DomImages *images);
    virtual void acceptImage(DomImage *image);
    virtual void acceptIncludes(DomIncludes *includes);
    virtual void acceptInclude(DomInclude *incl);
    virtual void acceptAction(DomAction *action);
    virtual void acceptActionGroup(DomActionGroup *actionGroup);
    virtual void acceptActionRef(DomActionRef *actionRef);
    virtual void acceptConnections(DomConnections *connections);
    virtual void acceptConnection(DomConnection *connection);
    virtual void acceptConnectionHints(DomConnectionHints *connectionHints);
    virtual void acceptConnectionHint(DomConnectionHint *connectionHint);
};

QT_END_NAMESPACE

#endif // TREEWALKER_H
