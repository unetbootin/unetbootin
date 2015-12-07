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

#ifndef Q3GARRAY_H
#define Q3GARRAY_H

#include <Qt3Support/q3shared.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q_COMPAT_EXPORT Q3GArray					// generic array
{
    friend class QBuffer;
public:
    // do not use this, even though this is public
    struct array_data : public Q3Shared {	// shared array
	array_data():data(0),len(0)
#ifdef QT_QGARRAY_SPEED_OPTIM
		    ,maxl(0)
#endif
	    {}
	char *data;				// actual array data
	uint  len;
#ifdef QT_QGARRAY_SPEED_OPTIM
	uint maxl;
#endif
    };
    Q3GArray();
    enum Optimization { MemOptim, SpeedOptim };
protected:
    Q3GArray(int, int);			// dummy; does not alloc
    Q3GArray(int size);			// allocate 'size' bytes
    Q3GArray(const Q3GArray &a);		// shallow copy
    virtual ~Q3GArray();

    Q3GArray    &operator=(const Q3GArray &a) { return assign(a); }

    virtual void detach()	{ duplicate(*this); }

    // ### Qt 4.0: maybe provide two versions of data(), at(), etc.
    char       *data()	 const	{ return shd->data; }
    uint	nrefs()	 const	{ return shd->count; }
    uint	size()	 const	{ return shd->len; }
    bool	isEqual(const Q3GArray &a) const;

    bool	resize(uint newsize, Optimization optim);
    bool	resize(uint newsize);

    bool	fill(const char *d, int len, uint sz);

    Q3GArray    &assign(const Q3GArray &a);
    Q3GArray    &assign(const char *d, uint len);
    Q3GArray    &duplicate(const Q3GArray &a);
    Q3GArray    &duplicate(const char *d, uint len);
    void	store(const char *d, uint len);

    array_data *sharedBlock()	const		{ return shd; }
    void	setSharedBlock(array_data *p) { shd=(array_data*)p; }

    Q3GArray    &setRawData(const char *d, uint len);
    void	resetRawData(const char *d, uint len);

    int		find(const char *d, uint index, uint sz) const;
    int		contains(const char *d, uint sz) const;

    void	sort(uint sz);
    int		bsearch(const char *d, uint sz) const;

    char       *at(uint index) const;

    bool	setExpand(uint index, const char *d, uint sz);

protected:
    virtual array_data *newData();
    virtual void deleteData(array_data *p);

private:
    static void msg_index(uint);
    array_data *shd;
};


inline char *Q3GArray::at(uint index) const
{
#if defined(QT_CHECK_RANGE)
    if (index >= size()) {
	msg_index(index);
	index = 0;
    }
#endif
    return &shd->data[index];
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GARRAY_H
