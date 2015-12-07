/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
** Copyright (C) 2002-2008 Bjoern Bergstroem
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef FOV_H
#define FOV_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class OublietteLevel;

class FOV
{
protected:
	virtual ~FOV() {}

	virtual bool scanCell(OublietteLevel *map, int x, int y) = 0;
	virtual void applyCell(OublietteLevel *map, int x, int y) = 0;

	double slope(double x1, double y1, double x2, double y2);
	double invSlope(double x1, double y1, double x2, double y2);

	void scanNW2N(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);
	void scanNE2N(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);
	void scanNW2W(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);
	void scanSW2W(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);
	void scanNE2E(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);
	void scanSE2E(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);

	void scanSW2S(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);
	void scanSE2S(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope);
public:
	void start(OublietteLevel *map, unsigned int x, unsigned int y, int maxRadius);
};


class SIMPLEFOV : public FOV
{
private:
	bool scanCell(OublietteLevel *map, int x, int y);
	void applyCell(OublietteLevel *map, int x, int y);
};

QT_END_NAMESPACE

#endif
