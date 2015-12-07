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

#ifndef QSIZEPOLICY_H
#define QSIZEPOLICY_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QVariant;

class Q_GUI_EXPORT QSizePolicy
{
    Q_GADGET
    Q_ENUMS(Policy)

private:
    enum SizePolicyMasks {
        HSize = 4,
        HMask = 0x0f,
        VMask = HMask << HSize,
		CTShift = 9,
		CTSize = 5,
		CTMask = ((0x1 << CTSize) - 1) << CTShift,
		UnusedShift = CTShift + CTSize,
		UnusedSize = 2
    };

public:
    enum PolicyFlag {
        GrowFlag = 1,
        ExpandFlag = 2,
        ShrinkFlag = 4,
        IgnoreFlag = 8
    };

    enum Policy {
        Fixed = 0,
        Minimum = GrowFlag,
        Maximum = ShrinkFlag,
        Preferred = GrowFlag | ShrinkFlag,
        MinimumExpanding = GrowFlag | ExpandFlag,
        Expanding = GrowFlag | ShrinkFlag | ExpandFlag,
        Ignored = ShrinkFlag | GrowFlag | IgnoreFlag
    };

    enum ControlType {
        DefaultType      = 0x00000001,
        ButtonBox        = 0x00000002,
        CheckBox         = 0x00000004,
        ComboBox         = 0x00000008,
        Frame            = 0x00000010,
        GroupBox         = 0x00000020,
        Label            = 0x00000040,
        Line             = 0x00000080,
        LineEdit         = 0x00000100,
        PushButton       = 0x00000200,
        RadioButton      = 0x00000400,
        Slider           = 0x00000800,
        SpinBox          = 0x00001000,
        TabWidget        = 0x00002000,
        ToolButton       = 0x00004000
    };
    Q_DECLARE_FLAGS(ControlTypes, ControlType)

    QSizePolicy() : data(0) { }

    // ### Qt 5: merge these two constructors (with type == DefaultType)
    QSizePolicy(Policy horizontal, Policy vertical)
        : data(horizontal | (vertical << HSize)) { }
    QSizePolicy(Policy horizontal, Policy vertical, ControlType type)
        : data(horizontal | (vertical << HSize)) { setControlType(type); }

    Policy horizontalPolicy() const { return static_cast<Policy>(data & HMask); }
    Policy verticalPolicy() const { return static_cast<Policy>((data & VMask) >> HSize); }
    ControlType controlType() const;

    void setHorizontalPolicy(Policy d) { data = (data & ~HMask) | d; }
    void setVerticalPolicy(Policy d) { data = (data & ~(HMask << HSize)) | (d << HSize); }
    void setControlType(ControlType type);

    Qt::Orientations expandingDirections() const {
        Qt::Orientations result;
        if (verticalPolicy() & ExpandFlag)
            result |= Qt::Vertical;
        if (horizontalPolicy() & ExpandFlag)
            result |= Qt::Horizontal;
        return result;
    }

    void setHeightForWidth(bool b) { data = b ? (data | (1 << 2*HSize)) : (data & ~(1 << 2*HSize));  }
    bool hasHeightForWidth() const { return data & (1 << 2*HSize); }

    bool operator==(const QSizePolicy& s) const { return data == s.data; }
    bool operator!=(const QSizePolicy& s) const { return data != s.data; }
    operator QVariant() const; // implemented in qabstractlayout.cpp

    int horizontalStretch() const { return data >> 24; }
    int verticalStretch() const { return (data >> 16) & 0xff; }
    void setHorizontalStretch(uchar stretchFactor) { data = (data&0x00ffffff) | (uint(stretchFactor)<<24); }
    void setVerticalStretch(uchar stretchFactor) { data = (data&0xff00ffff) | (uint(stretchFactor)<<16); }

    void transpose();

#ifdef QT3_SUPPORT
    typedef Policy SizeType;
#ifndef qdoc
    typedef Qt::Orientations ExpandData;
    enum {
        NoDirection = 0,
        Horizontally = 1,
        Vertically = 2,
        BothDirections = Horizontally | Vertically
    };
#else
    enum ExpandData {
        NoDirection = 0x0,
        Horizontally = 0x1,
        Vertically = 0x2,
        BothDirections = 0x3
    };
#endif // qdoc

    inline QT3_SUPPORT bool mayShrinkHorizontally() const
        { return horizontalPolicy() & ShrinkFlag; }
    inline QT3_SUPPORT bool mayShrinkVertically() const { return verticalPolicy() & ShrinkFlag; }
    inline QT3_SUPPORT bool mayGrowHorizontally() const { return horizontalPolicy() & GrowFlag; }
    inline QT3_SUPPORT bool mayGrowVertically() const { return verticalPolicy() & GrowFlag; }
    inline QT3_SUPPORT Qt::Orientations expanding() const { return expandingDirections(); }

    QT3_SUPPORT_CONSTRUCTOR QSizePolicy(Policy hor, Policy ver, bool hfw)
        : data(hor | (ver<<HSize) | (hfw ? (1U<<2*HSize) : 0)) { }

    QT3_SUPPORT_CONSTRUCTOR QSizePolicy(Policy hor, Policy ver, uchar hors, uchar vers, bool hfw = false)
        : data(hor | (ver<<HSize) | (hfw ? (1U<<2*HSize) : 0)) {
        setHorizontalStretch(hors);
        setVerticalStretch(vers);
    }

    inline QT3_SUPPORT Policy horData() const { return static_cast<Policy>(data & HMask); }
    inline QT3_SUPPORT Policy verData() const { return static_cast<Policy>((data & VMask) >> HSize); }
    inline QT3_SUPPORT void setHorData(Policy d) { setHorizontalPolicy(d); }
    inline QT3_SUPPORT void setVerData(Policy d) { setVerticalPolicy(d); }

    inline QT3_SUPPORT uint horStretch() const { return horizontalStretch(); }
    inline QT3_SUPPORT uint verStretch() const { return verticalStretch(); }
    inline QT3_SUPPORT void setHorStretch(uchar sf) { setHorizontalStretch(sf); }
    inline QT3_SUPPORT void setVerStretch(uchar sf) { setVerticalStretch(sf); }
#endif

private:
#ifndef QT_NO_DATASTREAM
    friend Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QSizePolicy &);
    friend Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QSizePolicy &);
#endif
    QSizePolicy(int i) : data(i) { }

    quint32 data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSizePolicy::ControlTypes)

// implemented in qlayout.cpp
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QSizePolicy &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QSizePolicy &);

inline void QSizePolicy::transpose() {
    Policy hData = horizontalPolicy();
    Policy vData = verticalPolicy();
    uchar hStretch = horizontalStretch();
    uchar vStretch = verticalStretch();
    setHorizontalPolicy(vData);
    setVerticalPolicy(hData);
    setHorizontalStretch(vStretch);
    setVerticalStretch(hStretch);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSIZEPOLICY_H
