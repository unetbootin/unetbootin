/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef TEXTPROPERTYEDITOR_H
#define TEXTPROPERTYEDITOR_H

#include "shared_global_p.h"
#include "shared_enums_p.h"

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

    class PropertyLineEdit;

    // Editor for text properties. Does escaping of newline characters
    // and provides validation modes. The interface corresponds to
    // that of QLineEdit.
    class QDESIGNER_SHARED_EXPORT TextPropertyEditor : public QWidget
    {
        TextPropertyEditor(const TextPropertyEditor &);
        TextPropertyEditor& operator=(const TextPropertyEditor &);
        Q_OBJECT
        Q_PROPERTY(QString text READ text WRITE setText USER true)
    public:
        enum EmbeddingMode {
            // Stand-alone widget
            EmbeddingNone,
                // Disable frame
                EmbeddingTreeView,
                // For editing in forms
                EmbeddingInPlace
        };

        enum UpdateMode {
            // Emit textChanged() as the user types
            UpdateAsYouType,
            // Emit textChanged() only when the user finishes
            UpdateOnFinished
        };

        TextPropertyEditor(QWidget *parent = 0, EmbeddingMode embeddingMode = EmbeddingNone, TextPropertyValidationMode validationMode = ValidationMultiLine);

        TextPropertyValidationMode textPropertyValidationMode() const { return m_validationMode; }
        void setTextPropertyValidationMode(TextPropertyValidationMode vm);

        UpdateMode updateMode() const                { return m_updateMode; }
        void setUpdateMode(UpdateMode um) { m_updateMode = um; }

        QString text() const;

        virtual QSize sizeHint () const;
        virtual QSize minimumSizeHint () const;

        void setAlignment(Qt::Alignment alignment);

        bool hasAcceptableInput() const;

        // installs an event filter object on the private QLineEdit
        void installEventFilter(QObject *filterObject);

        // Replace newline characters by literal "\n" for inline editing
        // in mode ValidationMultiLine
        static QString stringToEditorString(const QString &s, TextPropertyValidationMode validationMode = ValidationMultiLine);

        // Replace literal "\n"  by actual new lines in mode ValidationMultiLine
        static QString editorStringToString(const QString &s, TextPropertyValidationMode validationMode = ValidationMultiLine);

        // Returns whether newline characters are valid in validationMode.
        static bool multiLine(TextPropertyValidationMode validationMode);

    signals:
        void textChanged(const QString &text);
        void editingFinished();

    public slots:
        void setText(const QString &text);
        void selectAll();
        void clear();

    protected:
        void resizeEvent(QResizeEvent * event );

    private slots:
        void slotTextChanged(const QString &text);
        void slotEditingFinished();

    private:
        void setRegExpValidator(const QString &pattern);
        void markIntermediateState();

        TextPropertyValidationMode m_validationMode;
        UpdateMode m_updateMode;
        PropertyLineEdit* m_lineEdit;

        // Cached text containing real newline characters.
        QString m_cachedText;
    };
}

QT_END_NAMESPACE

#endif // TEXTPROPERTYEDITOR_H
