/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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
