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

#ifndef QDESIGNER_PROPERTYSHEET_H
#define QDESIGNER_PROPERTYSHEET_H

#include "shared_global_p.h"
#include "dynamicpropertysheet.h"
#include <QtDesigner/propertysheet.h>
#include <QtDesigner/default_extensionfactory.h>
#include <QtDesigner/QExtensionManager>

#include <QtCore/QVariant>
#include <QtCore/QPair>

#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QLayout;
class QDesignerFormEditorInterface;
class QDesignerPropertySheetPrivate;

namespace qdesigner_internal
{
    class DesignerPixmapCache;
    class DesignerIconCache;
    class FormWindowBase;
}

class QDESIGNER_SHARED_EXPORT QDesignerPropertySheet: public QObject, public QDesignerPropertySheetExtension, public QDesignerDynamicPropertySheetExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerPropertySheetExtension QDesignerDynamicPropertySheetExtension)
public:
    explicit QDesignerPropertySheet(QObject *object, QObject *parent = 0);
    virtual ~QDesignerPropertySheet();

    virtual int indexOf(const QString &name) const;

    virtual int count() const;
    virtual QString propertyName(int index) const;

    virtual QString propertyGroup(int index) const;
    virtual void setPropertyGroup(int index, const QString &group);

    virtual bool hasReset(int index) const;
    virtual bool reset(int index);

    virtual bool isAttribute(int index) const;
    virtual void setAttribute(int index, bool b);

    virtual bool isVisible(int index) const;
    virtual void setVisible(int index, bool b);

    virtual QVariant property(int index) const;
    virtual void setProperty(int index, const QVariant &value);

    virtual bool isChanged(int index) const;

    virtual void setChanged(int index, bool changed);

    virtual bool dynamicPropertiesAllowed() const;
    virtual int addDynamicProperty(const QString &propertyName, const QVariant &value);
    virtual bool removeDynamicProperty(int index);
    virtual bool isDynamicProperty(int index) const;
    virtual bool canAddDynamicProperty(const QString &propertyName) const;

    bool isDefaultDynamicProperty(int index) const;

    bool isResourceProperty(int index) const;
    QVariant defaultResourceProperty(int index) const;

    qdesigner_internal::DesignerPixmapCache *pixmapCache() const;
    void setPixmapCache(qdesigner_internal::DesignerPixmapCache *cache);
    qdesigner_internal::DesignerIconCache *iconCache() const;
    void setIconCache(qdesigner_internal::DesignerIconCache *cache);
    int createFakeProperty(const QString &propertyName, const QVariant &value = QVariant());

    virtual bool isEnabled(int index) const;
    QObject *object() const;

protected:
    bool isAdditionalProperty(int index) const;
    bool isFakeProperty(int index) const;
    QVariant resolvePropertyValue(int index, const QVariant &value) const;
    QVariant metaProperty(int index) const;
    void setFakeProperty(int index, const QVariant &value);
    void clearFakeProperties();

    bool isFakeLayoutProperty(int index) const;
    bool isDynamic(int index) const;
    qdesigner_internal::FormWindowBase *formWindowBase() const;
    QDesignerFormEditorInterface *core() const;

public:
    enum PropertyType { PropertyNone,
                        PropertyLayoutObjectName,
                        PropertyLayoutLeftMargin,
                        PropertyLayoutTopMargin,
                        PropertyLayoutRightMargin,
                        PropertyLayoutBottomMargin,
                        PropertyLayoutSpacing,
                        PropertyLayoutHorizontalSpacing,
                        PropertyLayoutVerticalSpacing,
                        PropertyLayoutSizeConstraint,
                        PropertyLayoutFieldGrowthPolicy,
                        PropertyLayoutRowWrapPolicy,
                        PropertyLayoutLabelAlignment,
                        PropertyLayoutFormAlignment,
                        PropertyBuddy,
                        PropertyAccessibility,
                        PropertyGeometry,
                        PropertyCheckable,
                        PropertyWindowTitle,
                        PropertyWindowIcon,
                        PropertyWindowFilePath,
                        PropertyWindowOpacity,
                        PropertyWindowIconText,
                     PropertyWindowModified
    };

    enum ObjectType { ObjectNone, ObjectLabel, ObjectLayout, ObjectLayoutWidget, ObjectQ3GroupBox };

    static ObjectType objectTypeFromObject(const QObject *o);
    static PropertyType propertyTypeFromName(const QString &name);

protected:
    PropertyType propertyType(int index) const;
    ObjectType objectType() const;

private:
    QDesignerPropertySheetPrivate *d;
};

/* Abstract base class for factories that register a property sheet that implements
 * both QDesignerPropertySheetExtension and QDesignerDynamicPropertySheetExtension
 * by multiple inheritance. The factory maintains ownership of
 * the extension and returns it for both id's. */

class QDESIGNER_SHARED_EXPORT QDesignerAbstractPropertySheetFactory: public QExtensionFactory
{
    Q_OBJECT
    Q_INTERFACES(QAbstractExtensionFactory)
public:
    explicit QDesignerAbstractPropertySheetFactory(QExtensionManager *parent = 0);
    virtual ~QDesignerAbstractPropertySheetFactory();

    QObject *extension(QObject *object, const QString &iid) const;

private slots:
    void objectDestroyed(QObject *object);

private:
    virtual QObject *createPropertySheet(QObject *qObject, QObject *parent) const = 0;

    struct PropertySheetFactoryPrivate;
    PropertySheetFactoryPrivate *m_impl;
};

/* Convenience factory template for property sheets that implement
 * QDesignerPropertySheetExtension and QDesignerDynamicPropertySheetExtension
 * by multiple inheritance. */

template <class Object, class PropertySheet>
class QDesignerPropertySheetFactory : public QDesignerAbstractPropertySheetFactory {
public:
    explicit QDesignerPropertySheetFactory(QExtensionManager *parent = 0);

    static void registerExtension(QExtensionManager *mgr);

private:
    // Does a  qobject_cast on  the object.
    virtual QObject *createPropertySheet(QObject *qObject, QObject *parent) const;
};

template <class Object, class PropertySheet>
QDesignerPropertySheetFactory<Object, PropertySheet>::QDesignerPropertySheetFactory(QExtensionManager *parent) :
    QDesignerAbstractPropertySheetFactory(parent)
{
}

template <class Object, class PropertySheet>
QObject *QDesignerPropertySheetFactory<Object, PropertySheet>::createPropertySheet(QObject *qObject, QObject *parent) const
{
    Object *object = qobject_cast<Object *>(qObject);
    if (!object)
        return 0;
    return new PropertySheet(object, parent);
}

template <class Object, class PropertySheet>
void QDesignerPropertySheetFactory<Object, PropertySheet>::registerExtension(QExtensionManager *mgr)
{
    QDesignerPropertySheetFactory *factory = new QDesignerPropertySheetFactory(mgr);
    mgr->registerExtensions(factory, Q_TYPEID(QDesignerPropertySheetExtension));
    mgr->registerExtensions(factory, Q_TYPEID(QDesignerDynamicPropertySheetExtension));
}


// Standard property sheet
typedef QDesignerPropertySheetFactory<QObject, QDesignerPropertySheet> QDesignerDefaultPropertySheetFactory;

QT_END_NAMESPACE

#endif // QDESIGNER_PROPERTYSHEET_H
