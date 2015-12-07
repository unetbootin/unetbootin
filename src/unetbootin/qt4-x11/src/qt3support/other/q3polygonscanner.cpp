/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3polygonscanner.h"
#include "q3pointarray.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

// Based on Xserver code miFillGeneralPoly...
/*
 *
 *     Written by Brian Kelleher;  Oct. 1985
 *
 *     Routine to fill a polygon.  Two fill rules are
 *     supported: frWINDING and frEVENODD.
 *
 *     See fillpoly.h for a complete description of the algorithm.
 */

/*
 *     These are the data structures needed to scan
 *     convert regions.  Two different scan conversion
 *     methods are available -- the even-odd method, and
 *     the winding number method.
 *     The even-odd rule states that a point is inside
 *     the polygon if a ray drawn from that point in any
 *     direction will pass through an odd number of
 *     path segments.
 *     By the winding number rule, a point is decided
 *     to be inside the polygon if a ray drawn from that
 *     point in any direction passes through a different
 *     number of clockwise and counterclockwise path
 *     segments.
 *
 *     These data structures are adapted somewhat from
 *     the algorithm in (Foley/Van Dam) for scan converting
 *     polygons.
 *     The basic algorithm is to start at the top (smallest y)
 *     of the polygon, stepping down to the bottom of
 *     the polygon by incrementing the y coordinate.  We
 *     keep a list of edges which the current scanline crosses,
 *     sorted by x.  This list is called the Active Edge Table (AET)
 *     As we change the y-coordinate, we update each entry in
 *     in the active edge table to reflect the edges new xcoord.
 *     This list must be sorted at each scanline in case
 *     two edges intersect.
 *     We also keep a data structure known as the Edge Table (ET),
 *     which keeps track of all the edges which the current
 *     scanline has not yet reached.  The ET is basically a
 *     list of ScanLineList structures containing a list of
 *     edges which are entered at a given scanline.  There is one
 *     ScanLineList per scanline at which an edge is entered.
 *     When we enter a new edge, we move it from the ET to the AET.
 *
 *     From the AET, we can implement the even-odd rule as in
 *     (Foley/Van Dam).
 *     The winding number rule is a little trickier.  We also
 *     keep the EdgeTableEntries in the AET linked by the
 *     nextWETE (winding EdgeTableEntry) link.  This allows
 *     the edges to be linked just as before for updating
 *     purposes, but only uses the edges linked by the nextWETE
 *     link as edges representing spans of the polygon to
 *     drawn (as with the even-odd rule).
 */

/* $XConsortium: miscanfill.h,v 1.5 94/04/17 20:27:50 dpw Exp $ */
/*

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/


/*
 *     scanfill.h
 *
 *     Written by Brian Kelleher; Jan 1985
 *
 *     This file contains a few macros to help track
 *     the edge of a filled object.  The object is assumed
 *     to be filled in scanline order, and thus the
 *     algorithm used is an extension of Bresenham's line
 *     drawing algorithm which assumes that y is always the
 *     major axis.
 *     Since these pieces of code are the same for any filled shape,
 *     it is more convenient to gather the library in one
 *     place, but since these pieces of code are also in
 *     the inner loops of output primitives, procedure call
 *     overhead is out of the question.
 *     See the author for a derivation if needed.
 */

/*
 *  In scan converting polygons, we want to choose those pixels
 *  which are inside the polygon.  Thus, we add .5 to the starting
 *  x coordinate for both left and right edges.  Now we choose the
 *  first pixel which is inside the pgon for the left edge and the
 *  first pixel which is outside the pgon for the right edge.
 *  Draw the left pixel, but not the right.
 *
 *  How to add .5 to the starting x coordinate:
 *      If the edge is moving to the right, then subtract dy from the
 *  error term from the general form of the algorithm.
 *      If the edge is moving to the left, then add dy to the error term.
 *
 *  The reason for the difference between edges moving to the left
 *  and edges moving to the right is simple:  If an edge is moving
 *  to the right, then we want the algorithm to flip immediately.
 *  If it is moving to the left, then we don't want it to flip until
 *  we traverse an entire pixel.
 */
#define BRESINITPGON(dy, x1, x2, xStart, d, m, m1, incr1, incr2) { \
    int dx;      /* local storage */ \
\
    /* \
     *  if the edge is horizontal, then it is ignored \
     *  and assumed not to be processed.  Otherwise, do this stuff. \
     */ \
    if ((dy) != 0) { \
        xStart = (x1); \
        dx = (x2) - xStart; \
        if (dx < 0) { \
            m = dx / (dy); \
            m1 = m - 1; \
            incr1 = -2 * dx + 2 * (dy) * m1; \
            incr2 = -2 * dx + 2 * (dy) * m; \
            d = 2 * m * (dy) - 2 * dx - 2 * (dy); \
        } else { \
            m = dx / (dy); \
            m1 = m + 1; \
            incr1 = 2 * dx - 2 * (dy) * m1; \
            incr2 = 2 * dx - 2 * (dy) * m; \
            d = -2 * m * (dy) + 2 * dx; \
        } \
    } \
}

#define BRESINCRPGON(d, minval, m, m1, incr1, incr2) { \
    if (m1 > 0) { \
        if (d > 0) { \
            minval += m1; \
            d += incr1; \
        } \
        else { \
            minval += m; \
            d += incr2; \
        } \
    } else {\
        if (d >= 0) { \
            minval += m1; \
            d += incr1; \
        } \
        else { \
            minval += m; \
            d += incr2; \
        } \
    } \
}


/*
 *     This structure contains all of the information needed
 *     to run the bresenham algorithm.
 *     The variables may be hardcoded into the declarations
 *     instead of using this structure to make use of
 *     register declarations.
 */
typedef struct {
    int minor;         /* minor axis        */
    int d;           /* decision variable */
    int m, m1;       /* slope and slope+1 */
    int incr1, incr2; /* error increments */
} BRESINFO;


#define BRESINITPGONSTRUCT(dmaj, min1, min2, bres) \
        BRESINITPGON(dmaj, min1, min2, bres.minor, bres.d, \
                     bres.m, bres.m1, bres.incr1, bres.incr2)

#define BRESINCRPGONSTRUCT(bres) \
        BRESINCRPGON(bres.d, bres.minor, bres.m, bres.m1, bres.incr1, bres.incr2)


typedef struct _EdgeTableEntry {
     int ymax;             /* ycoord at which we exit this edge. */
     BRESINFO bres;        /* Bresenham info to run the edge     */
     struct _EdgeTableEntry *next;       /* next in the list     */
     struct _EdgeTableEntry *back;       /* for insertion sort   */
     struct _EdgeTableEntry *nextWETE;   /* for winding num rule */
     int ClockWise;        /* flag for winding number rule       */
} EdgeTableEntry;


typedef struct _ScanLineList{
     int scanline;              /* the scanline represented */
     EdgeTableEntry *edgelist;  /* header node              */
     struct _ScanLineList *next;  /* next in the list       */
} ScanLineList;


typedef struct {
     int ymax;                 /* ymax for the polygon     */
     int ymin;                 /* ymin for the polygon     */
     ScanLineList scanlines;   /* header node              */
} EdgeTable;


/*
 * Here is a struct to help with storage allocation
 * so we can allocate a big chunk at a time, and then take
 * pieces from this heap when we need to.
 */
#define SLLSPERBLOCK 25

typedef struct _ScanLineListBlock {
     ScanLineList SLLs[SLLSPERBLOCK];
     struct _ScanLineListBlock *next;
} ScanLineListBlock;

/*
 * number of points to buffer before sending them off
 * to scanlines() :  Must be an even number
 */
#define NUMPTSTOBUFFER 200

/*
 *
 *     a few macros for the inner loops of the fill code where
 *     performance considerations don't allow a procedure call.
 *
 *     Evaluate the given edge at the given scanline.
 *     If the edge has expired, then we leave it and fix up
 *     the active edge table; otherwise, we increment the
 *     x value to be ready for the next scanline.
 *     The winding number rule is in effect, so we must notify
 *     the caller when the edge has been removed so he
 *     can reorder the Winding Active Edge Table.
 */
#define EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET) { \
   if (pAET->ymax == y) {          /* leaving this edge */ \
      pPrevAET->next = pAET->next; \
      pAET = pPrevAET->next; \
      fixWAET = 1; \
      if (pAET) \
         pAET->back = pPrevAET; \
   } \
   else { \
      BRESINCRPGONSTRUCT(pAET->bres); \
      pPrevAET = pAET; \
      pAET = pAET->next; \
   } \
}


/*
 *     Evaluate the given edge at the given scanline.
 *     If the edge has expired, then we leave it and fix up
 *     the active edge table; otherwise, we increment the
 *     x value to be ready for the next scanline.
 *     The even-odd rule is in effect.
 */
#define EVALUATEEDGEEVENODD(pAET, pPrevAET, y) { \
   if (pAET->ymax == y) {          /* leaving this edge */ \
      pPrevAET->next = pAET->next; \
      pAET = pPrevAET->next; \
      if (pAET) \
         pAET->back = pPrevAET; \
   } \
   else { \
      BRESINCRPGONSTRUCT(pAET->bres) \
      pPrevAET = pAET; \
      pAET = pAET->next; \
   } \
}

/***********************************************************

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#define MAXINT 0x7fffffff
#define MININT -MAXINT

/*
 *     fillUtils.c
 *
 *     Written by Brian Kelleher;  Oct. 1985
 *
 *     This module contains all of the utility functions
 *     needed to scan convert a polygon.
 *
 */
/*
 *     InsertEdgeInET
 *
 *     Insert the given edge into the edge table.
 *     First we must find the correct bucket in the
 *     Edge table, then find the right slot in the
 *     bucket.  Finally, we can insert it.
 *
 */
static bool
miInsertEdgeInET(EdgeTable *ET, EdgeTableEntry *ETE,
        int scanline, ScanLineListBlock **SLLBlock, int *iSLLBlock)
{
    register EdgeTableEntry *start, *prev;
    register ScanLineList *pSLL, *pPrevSLL;
    ScanLineListBlock *tmpSLLBlock;

    /*
     * find the right bucket to put the edge into
     */
    pPrevSLL = &ET->scanlines;
    pSLL = pPrevSLL->next;
    while (pSLL && (pSLL->scanline < scanline))
    {
        pPrevSLL = pSLL;
        pSLL = pSLL->next;
    }

    /*
     * reassign pSLL (pointer to ScanLineList) if necessary
     */
    if ((!pSLL) || (pSLL->scanline > scanline))
    {
        if (*iSLLBlock > SLLSPERBLOCK-1)
        {
            tmpSLLBlock =
                  (ScanLineListBlock *)malloc(sizeof(ScanLineListBlock));
            if (!tmpSLLBlock)
                return false;
            (*SLLBlock)->next = tmpSLLBlock;
            tmpSLLBlock->next = 0;
            *SLLBlock = tmpSLLBlock;
            *iSLLBlock = 0;
        }
        pSLL = &((*SLLBlock)->SLLs[(*iSLLBlock)++]);

        pSLL->next = pPrevSLL->next;
        pSLL->edgelist = 0;
        pPrevSLL->next = pSLL;
    }
    pSLL->scanline = scanline;

    /*
     * now insert the edge in the right bucket
     */
    prev = 0;
    start = pSLL->edgelist;
    while (start && (start->bres.minor < ETE->bres.minor))
    {
        prev = start;
        start = start->next;
    }
    ETE->next = start;

    if (prev)
        prev->next = ETE;
    else
        pSLL->edgelist = ETE;
    return true;
}

/*
 *     CreateEdgeTable
 *
 *     This routine creates the edge table for
 *     scan converting polygons.
 *     The Edge Table (ET) looks like:
 *
 *    EdgeTable
 *     --------
 *    |  ymax  |        ScanLineLists
 *    |scanline|-->------------>-------------->...
 *     --------   |scanline|   |scanline|
 *                |edgelist|   |edgelist|
 *                ---------    ---------
 *                    |             |
 *                    |             |
 *                    V             V
 *              list of ETEs   list of ETEs
 *
 *     where ETE is an EdgeTableEntry data structure,
 *     and there is one ScanLineList per scanline at
 *     which an edge is initially entered.
 *
 */

typedef struct {
#if defined(Q_OS_MAC)
    int y, x;
#else
    int x, y;
#endif

} DDXPointRec, *DDXPointPtr;

/*
 *     Clean up our act.
 */
static void
miFreeStorage(ScanLineListBlock   *pSLLBlock)
{
    register ScanLineListBlock   *tmpSLLBlock;

    while (pSLLBlock)
    {
        tmpSLLBlock = pSLLBlock->next;
        free(pSLLBlock);
        pSLLBlock = tmpSLLBlock;
    }
}

static bool
miCreateETandAET(int count, DDXPointPtr pts, EdgeTable *ET,
        EdgeTableEntry *AET, EdgeTableEntry *pETEs, ScanLineListBlock *pSLLBlock)
{
    register DDXPointPtr top, bottom;
    register DDXPointPtr PrevPt, CurrPt;
    int iSLLBlock = 0;

    int dy;

    if (count < 2)  return true;

    /*
     *  initialize the Active Edge Table
     */
    AET->next = 0;
    AET->back = 0;
    AET->nextWETE = 0;
    AET->bres.minor = MININT;

    /*
     *  initialize the Edge Table.
     */
    ET->scanlines.next = 0;
    ET->ymax = MININT;
    ET->ymin = MAXINT;
    pSLLBlock->next = 0;

    PrevPt = &pts[count-1];

    /*
     *  for each vertex in the array of points.
     *  In this loop we are dealing with two vertices at
     *  a time -- these make up one edge of the polygon.
     */
    while (count--)
    {
        CurrPt = pts++;

        /*
         *  find out which point is above and which is below.
         */
        if (PrevPt->y > CurrPt->y)
        {
            bottom = PrevPt, top = CurrPt;
            pETEs->ClockWise = 0;
        }
        else
        {
            bottom = CurrPt, top = PrevPt;
            pETEs->ClockWise = 1;
        }

        /*
         * don't add horizontal edges to the Edge table.
         */
        if (bottom->y != top->y)
        {
            pETEs->ymax = bottom->y-1;  /* -1 so we don't get last scanline */

            /*
             *  initialize integer edge algorithm
             */
            dy = bottom->y - top->y;
            BRESINITPGONSTRUCT(dy, top->x, bottom->x, pETEs->bres)

            if (!miInsertEdgeInET(ET, pETEs, top->y, &pSLLBlock, &iSLLBlock))
            {
                miFreeStorage(pSLLBlock->next);
                return false;
            }

            ET->ymax = qMax(ET->ymax, PrevPt->y);
            ET->ymin = qMin(ET->ymin, PrevPt->y);
            pETEs++;
        }

        PrevPt = CurrPt;
    }
    return true;
}

/*
 *     loadAET
 *
 *     This routine moves EdgeTableEntries from the
 *     EdgeTable into the Active Edge Table,
 *     leaving them sorted by smaller x coordinate.
 *
 */

static void
miloadAET(EdgeTableEntry *AET, EdgeTableEntry *ETEs)
{
    register EdgeTableEntry *pPrevAET;
    register EdgeTableEntry *tmp;

    pPrevAET = AET;
    AET = AET->next;
    while (ETEs)
    {
        while (AET && (AET->bres.minor < ETEs->bres.minor))
        {
            pPrevAET = AET;
            AET = AET->next;
        }
        tmp = ETEs->next;
        ETEs->next = AET;
        if (AET)
            AET->back = ETEs;
        ETEs->back = pPrevAET;
        pPrevAET->next = ETEs;
        pPrevAET = ETEs;

        ETEs = tmp;
    }
}

/*
 *     computeWAET
 *
 *     This routine links the AET by the
 *     nextWETE (winding EdgeTableEntry) link for
 *     use by the winding number rule.  The final
 *     Active Edge Table (AET) might look something
 *     like:
 *
 *     AET
 *     ----------  ---------   ---------
 *     |ymax    |  |ymax    |  |ymax    |
 *     | ...    |  |...     |  |...     |
 *     |next    |->|next    |->|next    |->...
 *     |nextWETE|  |nextWETE|  |nextWETE|
 *     ---------   ---------   ^--------
 *         |                   |       |
 *         V------------------->       V---> ...
 *
 */
static void
micomputeWAET(EdgeTableEntry *AET)
{
    register EdgeTableEntry *pWETE;
    register int inside = 1;
    register int isInside = 0;

    AET->nextWETE = 0;
    pWETE = AET;
    AET = AET->next;
    while (AET)
    {
        if (AET->ClockWise)
            isInside++;
        else
            isInside--;

        if ((!inside && !isInside) ||
            (inside &&  isInside))
        {
            pWETE->nextWETE = AET;
            pWETE = AET;
            inside = !inside;
        }
        AET = AET->next;
    }
    pWETE->nextWETE = 0;
}

/*
 *     InsertionSort
 *
 *     Just a simple insertion sort using
 *     pointers and back pointers to sort the Active
 *     Edge Table.
 *
 */

static int
miInsertionSort(EdgeTableEntry *AET)
{
    register EdgeTableEntry *pETEchase;
    register EdgeTableEntry *pETEinsert;
    register EdgeTableEntry *pETEchaseBackTMP;
    register int changed = 0;

    AET = AET->next;
    while (AET)
    {
        pETEinsert = AET;
        pETEchase = AET;
        while (pETEchase->back->bres.minor > AET->bres.minor)
            pETEchase = pETEchase->back;

        AET = AET->next;
        if (pETEchase != pETEinsert)
        {
            pETEchaseBackTMP = pETEchase->back;
            pETEinsert->back->next = AET;
            if (AET)
                AET->back = pETEinsert->back;
            pETEinsert->next = pETEchase;
            pETEchase->back->next = pETEinsert;
            pETEchase->back = pETEinsert;
            pETEinsert->back = pETEchaseBackTMP;
            changed = 1;
        }
    }
    return changed;
}

/*!
    \overload
*/
void Q3PolygonScanner::scan(const Q3PointArray& pa, bool winding, int index, int npoints)
{
    scan(pa, winding, index, npoints, true);
}

/*!
    \overload

    If \a stitchable is false, the right and bottom edges of the
    polygon are included. This causes adjacent polygons to overlap.
*/
void Q3PolygonScanner::scan(const Q3PointArray& pa, bool winding, int index, int npoints, bool stitchable)
{
    scan(pa, winding, index, npoints,
        stitchable ? Edge(Left+Top) : Edge(Left+Right+Top+Bottom));
}

/*!
    Calls processSpans() for all scanlines of the polygon defined by
    \a npoints starting at \a index in \a pa.

    If \a winding is true, the Winding algorithm rather than the
    Odd-Even rule is used.

    The \a edges is any bitwise combination of:
    \list
    \i Q3PolygonScanner::Left
    \i Q3PolygonScanner::Right
    \i Q3PolygonScanner::Top
    \i Q3PolygonScanner::Bottom
    \endlist
    \a edges determines which edges are included.

    \warning The edges feature does not work properly.

*/
void Q3PolygonScanner::scan(const Q3PointArray& pa, bool winding, int index, int npoints, Edge edges)
{


    DDXPointPtr ptsIn = (DDXPointPtr)pa.data();
    ptsIn += index;
    register EdgeTableEntry *pAET;  /* the Active Edge Table   */
    register int y;                 /* the current scanline    */
    register int nPts = 0;          /* number of pts in buffer */
    register EdgeTableEntry *pWETE; /* Winding Edge Table      */
    register ScanLineList *pSLL;    /* Current ScanLineList    */
    register DDXPointPtr ptsOut;      /* ptr to output buffers   */
    int *width;
    DDXPointRec FirstPoint[NUMPTSTOBUFFER]; /* the output buffers */
    int FirstWidth[NUMPTSTOBUFFER];
    EdgeTableEntry *pPrevAET;       /* previous AET entry      */
    EdgeTable ET;                   /* Edge Table header node  */
    EdgeTableEntry AET;             /* Active ET header node   */
    EdgeTableEntry *pETEs;          /* Edge Table Entries buff */
    ScanLineListBlock SLLBlock;     /* header for ScanLineList */
    int fixWAET = 0;
    int edge_l = (edges & Left) ? 1 : 0;
    int edge_r = (edges & Right) ? 1 : 0;
    int edge_t = 1; //#### (edges & Top) ? 1 : 0;
    int edge_b = (edges & Bottom) ? 1 : 0;

    if (npoints == -1)
        npoints = pa.size();

    if (npoints < 3)
        return;

    if(!(pETEs = (EdgeTableEntry *)
        malloc(sizeof(EdgeTableEntry) * npoints)))
        return;
    ptsOut = FirstPoint;
    width = FirstWidth;
    if (!miCreateETandAET(npoints, ptsIn, &ET, &AET, pETEs, &SLLBlock))
    {
        free(pETEs);
        return;
    }
    pSLL = ET.scanlines.next;

    if (!winding)
    {
        /*
         *  for each scanline
         */
        for (y = ET.ymin+1-edge_t; y < ET.ymax+edge_b; y++)
        {
            /*
             *  Add a new edge to the active edge table when we
             *  get to the next edge.
             */
            if (pSLL && y == pSLL->scanline)
            {
                miloadAET(&AET, pSLL->edgelist);
                pSLL = pSLL->next;
            }
            pPrevAET = &AET;
            pAET = AET.next;

            /*
             *  for each active edge
             */
            while (pAET)
            {
                ptsOut->x = pAET->bres.minor + 1 - edge_l;
                ptsOut++->y = y;
                *width++ = pAET->next->bres.minor - pAET->bres.minor
                    - 1 + edge_l + edge_r;
                nPts++;

                /*
                 *  send out the buffer when its full
                 */
                if (nPts == NUMPTSTOBUFFER)
                {
                    processSpans(nPts, (QPoint*)FirstPoint, FirstWidth);
                    ptsOut = FirstPoint;
                    width = FirstWidth;
                    nPts = 0;
                }
                EVALUATEEDGEEVENODD(pAET, pPrevAET, y)
                EVALUATEEDGEEVENODD(pAET, pPrevAET, y)
            }
            miInsertionSort(&AET);
        }
    }
    else      /* default to WindingNumber */
    {
        /*
         *  for each scanline
         */
        for (y = ET.ymin+1-edge_t; y < ET.ymax+edge_b; y++)
        {
            /*
             *  Add a new edge to the active edge table when we
             *  get to the next edge.
             */
            if (pSLL && y == pSLL->scanline)
            {
                miloadAET(&AET, pSLL->edgelist);
                micomputeWAET(&AET);
                pSLL = pSLL->next;
            }
            pPrevAET = &AET;
            pAET = AET.next;
            pWETE = pAET;

            /*
             *  for each active edge
             */
            while (pAET)
            {
                /*
                 *  if the next edge in the active edge table is
                 *  also the next edge in the winding active edge
                 *  table.
                 */
                if (pWETE == pAET)
                {
                    ptsOut->x = pAET->bres.minor + 1 - edge_l;
                    ptsOut++->y = y;
                    *width++ = pAET->nextWETE->bres.minor - pAET->bres.minor - 1 + edge_l + edge_r;
                    nPts++;

                    /*
                     *  send out the buffer
                     */
                    if (nPts == NUMPTSTOBUFFER)
                    {
                        processSpans(nPts, (QPoint*)FirstPoint, FirstWidth);
                        ptsOut = FirstPoint;
                        width  = FirstWidth;
                        nPts = 0;
                    }

                    pWETE = pWETE->nextWETE;
                    while (pWETE != pAET) {
                        EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET)
                    }
                    pWETE = pWETE->nextWETE;
                }
                EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET)
            }

            /*
             *  reevaluate the Winding active edge table if we
             *  just had to resort it or if we just exited an edge.
             */
            if (miInsertionSort(&AET) || fixWAET)
            {
                micomputeWAET(&AET);
                fixWAET = 0;
            }
        }
    }

    /*
     *     Get any spans that we missed by buffering
     */


    processSpans(nPts, (QPoint*)FirstPoint, FirstWidth);
    free(pETEs);
    miFreeStorage(SLLBlock.next);
}
/***** END OF X11-based CODE *****/

QT_END_NAMESPACE
