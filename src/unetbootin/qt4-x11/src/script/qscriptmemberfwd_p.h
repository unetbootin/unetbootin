/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTMEMBERFWD_P_H
#define QSCRIPTMEMBERFWD_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SCRIPT

class QScriptNameIdImpl;

namespace QScript {

    class Member
    {
    public:
        enum PropertyFlag {
            ObjectProperty      = 0x00000100, // Stored in the member table
            NativeProperty      = 0x00000200,

            UninitializedConst  = 0x00000800, // NB: shared with QScriptValue::KeepExistingFlags

            InternalRange       = 0x0000ff00  // Not user-accessible (read as 0, don't change on write)
        };

        inline void resetFlags(uint flags);
        inline void setFlags(uint flags);
        inline void unsetFlags(uint flags);
        inline uint flags() const;
        inline bool testFlags(uint mask) const;

        inline bool isValid() const;

        inline bool isWritable() const;
        inline bool isDeletable() const;

        inline bool dontEnum() const;

        inline bool isObjectProperty() const;
        inline bool isNativeProperty() const;

        inline bool isUninitializedConst() const;

        inline bool isGetter() const;
        inline bool isSetter() const;
        inline bool isGetterOrSetter() const;

        inline int id() const;
        inline QScriptNameIdImpl *nameId() const;

        inline bool operator==(const Member &other) const;
        inline bool operator!=(const Member &other) const;

        inline static Member invalid();
        inline void invalidate();

        inline void native(QScriptNameIdImpl *nameId, int id, uint flags);
        inline void object(QScriptNameIdImpl *nameId, int id, uint flags);

    private:
        QScriptNameIdImpl *m_nameId;
        int m_id;
        uint m_flags;
    };

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
