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

/*
 *
 * implementation of recursive shadowcasting
 *
 * 020125: Bjoern Bergstroem - changed from float to double to remove compiler
 *         warnings
 * 020125: Bjoern Bergstroem - included a check to avoid orthogonal edges to be
 *         scanned more than once
 * 020125: Greg McIntyre - declared the nwL, neL etc in FOV::start outside the
 *         for loops
 *
 */

#include "fov.h"
#include "oublietteplan.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

double FOV::slope(double x1, double y1, double x2, double y2)
{
	double xDiff=x1-x2;
	double yDiff=y1-y2;

	if(yDiff != 0)
	{
		return xDiff/yDiff;
	}
	else
	{
		return 0;
	}
}

double FOV::invSlope(double x1, double y1, double x2, double y2)
{
	double slope=this->slope(x1,y1,x2,y2);

	if(slope != 0)
	{
		return 1/slope;
	}
	else
	{
		return 0;
	}
}


/* scanNW2N
	scans the octant covering the area from north west to north from left to 
	right
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanNW2N(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int xStart=(int)((double)xCenter + 0.5 - (startSlope * distance));
	int xEnd=(int)((double)xCenter + 0.5 - (endSlope * distance));
	int yCheck=yCenter - distance;

	// is the starting cell the leftmost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(xStart != xCenter-(1*distance))
	{
		this->applyCell(map,xStart,yCheck);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xStart,yCheck);

	// scan from the cell after the starting cell (xStart+1) to end cell of
	// scan (xCheck<=xEnd)
	for(int xCheck=xStart+1; xCheck<=xEnd; xCheck++)
	{
		// is the current cell the rightmost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(xCheck != xCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by to the left of the blocking cell
		//
		// +---+a####+---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   |#####|   |  a = old [xCheck,yCheck]
		// |   |#####|   |  b = new [xCheck-0.00001,yCheck+0.99999]
		// |   |#####|   |
		// +---b#####+---+
		// +---++---++---+
		// |   ||   ||   |
		// |   ||   || @ |
		// |   ||   ||   |
		// +---++---++---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanNW2N(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck-0.000001,(double)yCheck+0.999999));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by to the right
		// of the blocking cells
		//
		// #####a---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// #####|   ||   |  a = new and old [xCheck,yCheck]
		// #####|   ||   |
		// #####|   ||   |
		// #####+---++---+
		// +---++---++---+
		// |   ||   ||   |
		// |   ||   || @ |
		// |   ||   ||   |
		// +---++---++---+
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck,(double)yCheck);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanNW2N(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}


/* scanNE2N
	scans the octant covering the area from north east to north from right to 
	left
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanNE2N(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int xStart=(int)((double)xCenter + 0.5 - (startSlope * distance));
	int xEnd=(int)((double)xCenter + 0.5 - (endSlope * distance));
	int yCheck=yCenter - distance;

	// is starting cell the rightmost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(xStart != xCenter-(-1*distance))
	{
		this->applyCell(map,xStart,yCheck);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xStart,yCheck);

	// scan from the cell after the starting cell (xStart-1) to end cell of
	// scan (xCheck>=xEnd)
	for(int xCheck=xStart-1; xCheck>=xEnd; xCheck--)
	{
		// is the current cell the leftmost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(xCheck != xCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by to the right of the blocking cell
		//
		// +---+a####+---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   |#####|   |  a = old [xCheck,yCheck]
		// |   |#####|   |  b = new [xCheck+0.9999,yCheck-0.00001]
		// |   |#####|   |
		// +---+#####b---+
		// +---++---++---+
		// |   ||   ||   |
		// | @ ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanNE2N(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+1,(double)yCheck+0.99999));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by to the left
		// of the blocking cells
		//
		// +---+a---b#####  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   |#####  a = old [xCheck,yCheck]
		// |   ||   |#####  b = new [xCheck+0.99999,yCheck]
		// |   ||   |#####
		// +---++---+#####
		// +---++---++---+
		// |   ||   ||   |
		// | @ ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+0.9999999,(double)yCheck);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanNE2N(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}


/* scanNW2W
	scans the octant covering the area from north west to west from top to 
	bottom
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanNW2W(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int yStart=(int)((double)yCenter + 0.5 - (startSlope * distance));
	int yEnd=(int)((double)yCenter + 0.5 - (endSlope * distance));
	int xCheck=xCenter - distance;

	// is starting cell the topmost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(yStart != yCenter-(1*distance))
	{
		this->applyCell(map,xCheck,yStart);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xCheck,yStart);

	// scan from the cell after the starting cell (yStart+1) to end cell of
	// scan (yCheck<=yEnd)
	for(int yCheck=yStart+1; yCheck<=yEnd; yCheck++)
	{
		// is the current cell the bottommost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(yCheck != yCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by the top of the blocking cell (see fig.)
		//
		// +---++---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   ||   |  a = old [xCheck,yCheck]
		// |   ||   ||   |  b = new [xCheck+0.99999,yCheck-0.00001]
		// |   ||   ||   |
		// +---b+---++---+
		// a####+---++---+
		// #####|   ||   |
		// #####|   ||   |
		// #####|   ||   |
		// #####+---++---+
		// +---++---++---+
		// |   ||   ||   |
		// |   ||   || @ |
		// |   ||   ||   |
		// +---++---++---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanNW2W(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+0.99999,(double)yCheck-0.00001));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by the bottom
		// of the blocking cells
		//
		// #####+---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// #####|   ||   |  a = old and new [xCheck,yCheck]
		// #####|   ||   |
		// #####|   ||   |
		// #####+---++---+
		// a---++---++---+
		// |   ||   ||   |
		// |   ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		// +---++---++---+
		// |   ||   ||   |
		// |   ||   || @ |
		// |   ||   ||   |
		// +---++---++---+
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck,(double)yCheck);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanNW2W(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}

/* scanSW2W
	scans the octant covering the area from southe west to west from bottom to 
	top
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanSW2W(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int yStart=(int)((double)yCenter + 0.5 - (startSlope * distance));
	int yEnd=(int)((double)yCenter + 0.5 - (endSlope * distance));
	int xCheck=xCenter - distance;

	// is starting cell the bottommost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(yStart != yCenter-(-1*distance))
	{
		this->applyCell(map,xCheck,yStart);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xCheck,yStart);

	// scan from the cell after the starting cell (yStart-1) to end cell of
	// scan (yCheck>=yEnd)
	for(int yCheck=yStart-1; yCheck>=yEnd; yCheck--)
	{
		// is the current cell the topmost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(yCheck != yCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by the bottom of the blocking cell
		//
		// +---++---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   ||   |  a = old [xCheck,yCheck]
		// |   ||   || @ |  b = new [xCheck+0.99999,yCheck+1]
		// |   ||   ||   |
		// +---++---++---+
		// a####+---++---+
		// #####|   ||   |
		// #####|   ||   |
		// #####|   ||   |
		// #####+---++---+
		// +---b+---++---+
		// |   ||   ||   |
		// |   ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanSW2W(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+0.99999,(double)yCheck+1));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by the top of
		// the blocking cells
		//
		// +---++---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   ||   |  a = old [xCheck,yCheck]
		// |   ||   || @ |  b = new [xCheck,yCheck+0.99999]
		// |   ||   ||   |
		// +---++---++---+
		// a---++---++---+
		// |   ||   ||   |
		// |   ||   ||   |
		// |   ||   ||   |
		// b---++---++---+
		// #####+---++---+
		// #####|   ||   |
		// #####|   ||   |
		// #####|   ||   |
		// #####+---++---+
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck,(double)yCheck+0.99999);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanSW2W(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}


/* scanSW2S
	scans the octant covering the area from south west to south from left to 
	right
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanSW2S(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int xStart=(int)((double)xCenter + 0.5 + (startSlope * distance));
	int xEnd=(int)((double)xCenter + 0.5 + (endSlope * distance));
	int yCheck=yCenter + distance;

	// is the starting cell the leftmost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(xStart != xCenter+(-1*distance))
	{
		this->applyCell(map,xStart,yCheck);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xStart,yCheck);

	// scan from the cell after the starting cell (xStart+1) to end cell of
	// scan (xCheck<=xEnd)
	for(int xCheck=xStart+1; xCheck<=xEnd; xCheck++)
	{
		// is the current cell the rightmost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(xCheck != xCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by to the left of the blocking cell
		//
		// +---++---++---+
		// |   ||   ||   |
		// |   ||   || @ |
		// |   ||   ||   |
		// +---++---++---+
		// +---ba####+---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   |#####|   |  a = old [xCheck,yCheck]
		// |   |#####|   |  b = new [xCheck-0.00001,yCheck]
		// |   |#####|   |
		// +---+#####+---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanSW2S(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck-0.00001,(double)yCheck));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by to the right
		// of the blocking cells
		//
		// +---++---++---+
		// |   ||   ||   |
		// |   ||   || @ |
		// |   ||   ||   |
		// +---++---++---+
		// #####a---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// #####|   ||   |  a = old [xCheck,yCheck]
		// #####|   ||   |  b = new [xCheck,yCheck+0.99999]
		// #####|   ||   |
		// #####b---++---+
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck,(double)yCheck+0.99999);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanSW2S(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}


/* scanSE2S
	scans the octant covering the area from south east to south from right to 
	left
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanSE2S(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int xStart=(int)((double)xCenter + 0.5 + (startSlope * distance));
	int xEnd=(int)((double)xCenter + 0.5 + (endSlope * distance));
	int yCheck=yCenter + distance;

	// is starting cell the rightmost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(xStart != xCenter+(1*distance))
	{
		this->applyCell(map,xStart,yCheck);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xStart,yCheck);

	// scan from the cell after the starting cell (xStart-1) to end cell of
	// scan (xCheck>=xEnd)
	for(int xCheck=xStart-1; xCheck>=xEnd; xCheck--)
	{
		// is the current cell the leftmost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(xCheck != xCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by to the right of the blocking cell
		//
		// +---++---++---+
		// |   ||   ||   |
		// | @ ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		// +---+a####b---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   |#####|   |  a = old [xCheck,yCheck]
		// |   |#####|   |  b = new [xCheck+1,yCheck]
		// |   |#####|   |
		// +---+#####+---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanSE2S(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+1,(double)yCheck));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by to the left
		// of the blocking cells
		//
		// +---++---++---+
		// |   ||   ||   |
		// | @ ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		// +---+a---+#####  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   |#####  a = old [xCheck,yCheck]
		// |   ||   |#####  b = new [xCheck+0.99999,yCheck+0.99999]
		// |   ||   |#####
		// +---++---b#####
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->slope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+0.99999,(double)yCheck+0.99999);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanSE2S(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}


/* scanNE2E
	scans the octant covering the area from north east to east from top to 
	bottom
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanNE2E(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int yStart=(int)((double)yCenter + 0.5 + (startSlope * distance));
	int yEnd=(int)((double)yCenter + 0.5 + (endSlope * distance));
	int xCheck=xCenter + distance;

	// is starting cell the topmost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(yStart != yCenter+(-1*distance))
	{
		this->applyCell(map,xCheck,yStart);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xCheck,yStart);

	// scan from the cell after the starting cell (yStart+1) to end cell of
	// scan (yCheck<=yEnd)
	for(int yCheck=yStart+1; yCheck<=yEnd; yCheck++)
	{
		// is the current cell the bottommost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(yCheck != yCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by the top of the blocking cell (see fig.)
		//
		// +---++---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   ||   |  a = old [xCheck,yCheck]
		// |   ||   ||   |  b = new [xCheck,yCheck-0.00001]
		// |   ||   ||   |
		// +---++---+b---+
		// +---++---+a####
		// |   ||   |#####
		// |   ||   |#####
		// |   ||   |#####
		// +---++---+#####
		// +---++---++---+
		// |   ||   ||   |
		// | @ ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanNE2E(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck,(double)yCheck-0.00001));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by the bottom
		// of the blocking cells
		//
		// +---++---+#####  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   |#####  a = old [xCheck,yCheck]
		// |   ||   |#####  b = new [xCheck+0.99999,yCheck]
		// |   ||   |#####
		// +---++---+#####
		// +---++---+a---b
		// |   ||   ||   |
		// |   ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		// +---++---++---+
		// |   ||   ||   |
		// | @ ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+0.99999,(double)yCheck);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanNE2E(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}

/* scanSE2E
	scans the octant covering the area from south east to east from bottom to 
	top
	the method ignores the octants starting and ending cells since they have
	been applied in FOV::start
*/
void FOV::scanSE2E(OublietteLevel *map, int xCenter, int yCenter, int distance, int maxRadius, double startSlope, double endSlope)
{
	if(distance > maxRadius)
	{
		return;
	}

	// calculate start and end cell of the scan
	int yStart=(int)((double)yCenter + 0.5 + (startSlope * distance));
	int yEnd=(int)((double)yCenter + 0.5 + (endSlope * distance));
	int xCheck=xCenter + distance;

	// is starting cell the bottommost cell in the octant?
	// NO: call applyCell() to starting cell 
	// YES: it has already been applied in FOV::start()
	if(yStart != yCenter+(1*distance))
	{
		this->applyCell(map,xCheck,yStart);
	}

	// find out if starting cell blocks LOS
	bool prevBlocked=this->scanCell(map,xCheck,yStart);

	// scan from the cell after the starting cell (yStart-1) to end cell of
	// scan (yCheck>=yEnd)
	for(int yCheck=yStart-1; yCheck>=yEnd; yCheck--)
	{
		// is the current cell the topmost cell in the octant?
		// NO: call applyCell() to current cell 
		// YES: it has already been applied in FOV::start()
		if(yCheck != yCenter)
		{
			// apply cell
			this->applyCell(map,xCheck,yCheck);
		}
		
		// cell blocks LOS
		// if previous cell didn't block LOS (prevBlocked==false) we have
		// hit a 'new' section of walls. a new scan will be started with an
		// endSlope that 'brushes' by the bottom of the blocking cell
		//
		// +---++---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   ||   |  a = old [xCheck,yCheck]
		// | @ ||   ||   |  b = new [xCheck,yCheck+1]
		// |   ||   ||   |
		// +---++---++---+
		// +---++---+a####
		// |   ||   |#####
		// |   ||   |#####
		// |   ||   |#####
		// +---++---+#####
		// +---++---+b---+
		// |   ||   ||   |
		// |   ||   ||   |
		// |   ||   ||   |
		// +---++---++---+
		//
		if(this->scanCell(map,xCheck,yCheck))
		{
			if(!prevBlocked)
			{
				this->scanSE2E(map,xCenter,yCenter,distance+1,maxRadius,startSlope,this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck,(double)yCheck+1));
			}
			prevBlocked=true;
		}

		// cell doesn't block LOS
		// if the cell is the first non-blocking cell after a section of walls
		// we need to calculate a new startSlope that 'brushes' by the top of
		// the blocking cells
		//
		// +---++---++---+  @ = [xCenter+0.5,yCenter+0.5]
		// |   ||   ||   |  a = old [xCheck,yCheck]
		// | @ ||   ||   |  b = new [xCheck+0.99999,yCheck+0.99999]
		// |   ||   ||   |
		// +---++---++---+
		// +---++---+a---+
		// |   ||   ||   |
		// |   ||   ||   |
		// |   ||   ||   |
		// +---++---++---b
		// +---++---+#####
		// |   ||   |#####
		// |   ||   |#####
		// |   ||   |#####
		// +---++---+#####
		//
		else
		{
			if(prevBlocked)
			{
				startSlope=this->invSlope((double)xCenter+0.5,(double)yCenter+0.5,(double)xCheck+0.99999,(double)yCheck+0.99999);
			}
			prevBlocked=false;
		}
	}

	// if the last cell of the scan didn't block LOS a new scan should be
	// started
	if(!prevBlocked)
	{
		this->scanSE2E(map,xCenter,yCenter,distance+1,maxRadius,startSlope,endSlope);
	}
}


void FOV::start(OublietteLevel *map, unsigned int x, unsigned int y, int maxRadius)
{
	if(map == NULL)
	{
		return;
	}

	// apply starting cell
	this->applyCell(map,x,y);

	if(maxRadius > 0)
	{
		// scan and apply north
		// until a blocking cell is hit or
		// until maxRadius is reached
		int nL;
		for(nL=1; nL<=maxRadius; nL++)
		{
			this->applyCell(map,x,y-nL);
			if(this->scanCell(map,x,y-nL))
			{
				break;
			}
		}

		// scan and apply north east
		// until a blocking cell is hit or
		// until maxRadius is reached
		int neL;
		for(neL=1; neL<=maxRadius; neL++)
		{
			this->applyCell(map,x+neL,y-neL);
			if(this->scanCell(map,x+neL,y-neL))
			{
				break;
			}
		}

		// scan and apply east
		// until a blocking cell is hit or
		// until maxRadius is reached
		int eL;
		for(eL=1; eL<=maxRadius; eL++)
		{
			this->applyCell(map,x+eL,y);
			if(this->scanCell(map,x+eL,y))
			{
				break;
			}
		}

		// scan and apply south east
		// until a blocking cell is hit or
		// until maxRadius is reached
		int seL;
		for(seL=1; seL<=maxRadius; seL++)
		{
			this->applyCell(map,x+seL,y+seL);
			if(this->scanCell(map,x+seL,y+seL))
			{
				break;
			}
		}

		// scan and apply south
		// until a blocking cell is hit or
		// until maxRadius is reached
		int sL;
		for(sL=1; sL<=maxRadius; sL++)
		{
			this->applyCell(map,x,y+sL);
			if(this->scanCell(map,x,y+sL))
			{
				break;
			}
		}

		// scan and apply south west
		// until a blocking cell is hit or
		// until maxRadius is reached
		int swL;
		for(swL=1; swL<=maxRadius; swL++)
		{
			this->applyCell(map,x-swL,y+swL);
			if(this->scanCell(map,x-swL,y+swL))
			{
				break;
			}
		}

		// scan and apply west
		// until a blocking cell is hit or
		// until maxRadius is reached
		int wL;
		for(wL=1; wL<=maxRadius; wL++)
		{
			this->applyCell(map,x-wL,y);
			if(this->scanCell(map,x-wL,y))
			{
				break;
			}
		}

		// scan and apply north west
		// until a blocking cell is hit or
		// until maxRadius is reached
		int nwL;
		for(nwL=1; nwL<=maxRadius; nwL++)
		{
			this->applyCell(map,x-nwL,y-nwL);
			if(this->scanCell(map,x-nwL,y-nwL))
			{
				break;
			}
		}


		// scan the octant covering the area from north west to north
		// if it isn't blocked
		if(nL!=1 || nwL!=1)
		{
			this->scanNW2N(map,x,y,1,maxRadius,1,0);
		}

		// scan the octant covering the area from north east to north
		// if it isn't blocked
		if(nL!=1 || neL!=1)
		{
			this->scanNE2N(map,x,y,1,maxRadius,-1,0);
		}

		// scan the octant covering the area from north west to west
		// if it isn't blocked
		if(nwL!=1 || wL!=1)
		{
			this->scanNW2W(map,x,y,1,maxRadius,1,0);
		}

		// scan the octant covering the area from south west to west
		// if it isn't blocked
		if(swL!=1 || wL!=1)
		{
			this->scanSW2W(map,x,y,1,maxRadius,-1,0);
		}

		// scan the octant covering the area from south west to south
		// if it isn't blocked
		if(swL!=1 || sL!=1)
		{
			this->scanSW2S(map,x,y,1,maxRadius,-1,0);
		}

		// scan the octant covering the area from south east to south
		// if it isn't blocked
		if(seL!=1 || sL!=1)
		{
			this->scanSE2S(map,x,y,1,maxRadius,1,0);
		}

		// scan the octant covering the area from north east to east
		// if it isn't blocked
		if(neL!=1 || eL!=1)
		{
			this->scanNE2E(map,x,y,1,maxRadius,-1,0);
		}

		// scan the octant covering the area from south east to east
		// if it isn't blocked
		if(seL!=1 || eL!=1)
		{
			this->scanSE2E(map,x,y,1,maxRadius,1,0);
		}

	}
}

bool SIMPLEFOV::scanCell(OublietteLevel *map, int x, int y)
{
	return map->blockLOS(x,y);
}

void SIMPLEFOV::applyCell(OublietteLevel *map, int x, int y)
{
	map->updateTileFlags(QPoint(x, y), Tile::Explored);
}

QT_END_NAMESPACE
