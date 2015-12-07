/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#include "JSCSSImportRule.h"

#include <wtf/GetPtr.h>

#include "CSSImportRule.h"
#include "CSSStyleSheet.h"
#include "JSCSSStyleSheet.h"
#include "JSMediaList.h"
#include "KURL.h"
#include "MediaList.h"

#include <runtime/JSNumberCell.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSCSSImportRule)

/* Hash table */

static const HashTableValue JSCSSImportRuleTableValues[5] =
{
    { "href", DontDelete|ReadOnly, (intptr_t)jsCSSImportRuleHref, (intptr_t)0 },
    { "media", DontDelete|ReadOnly, (intptr_t)jsCSSImportRuleMedia, (intptr_t)0 },
    { "styleSheet", DontDelete|ReadOnly, (intptr_t)jsCSSImportRuleStyleSheet, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsCSSImportRuleConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSCSSImportRuleTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 7, JSCSSImportRuleTableValues, 0 };
#else
    { 8, 7, JSCSSImportRuleTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSCSSImportRuleConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSCSSImportRuleConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSCSSImportRuleConstructorTableValues, 0 };
#else
    { 1, 0, JSCSSImportRuleConstructorTableValues, 0 };
#endif

class JSCSSImportRuleConstructor : public DOMObject {
public:
    JSCSSImportRuleConstructor(ExecState* exec)
        : DOMObject(JSCSSImportRuleConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSCSSImportRulePrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSCSSImportRuleConstructor::s_info = { "CSSImportRuleConstructor", 0, &JSCSSImportRuleConstructorTable, 0 };

bool JSCSSImportRuleConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSCSSImportRuleConstructor, DOMObject>(exec, &JSCSSImportRuleConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSCSSImportRulePrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSCSSImportRulePrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSCSSImportRulePrototypeTableValues, 0 };
#else
    { 1, 0, JSCSSImportRulePrototypeTableValues, 0 };
#endif

const ClassInfo JSCSSImportRulePrototype::s_info = { "CSSImportRulePrototype", 0, &JSCSSImportRulePrototypeTable, 0 };

JSObject* JSCSSImportRulePrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSCSSImportRule>(exec);
}

const ClassInfo JSCSSImportRule::s_info = { "CSSImportRule", &JSCSSRule::s_info, &JSCSSImportRuleTable, 0 };

JSCSSImportRule::JSCSSImportRule(PassRefPtr<Structure> structure, PassRefPtr<CSSImportRule> impl)
    : JSCSSRule(structure, impl)
{
}

JSObject* JSCSSImportRule::createPrototype(ExecState* exec)
{
    return new (exec) JSCSSImportRulePrototype(JSCSSImportRulePrototype::createStructure(JSCSSRulePrototype::self(exec)));
}

bool JSCSSImportRule::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSCSSImportRule, Base>(exec, &JSCSSImportRuleTable, this, propertyName, slot);
}

JSValuePtr jsCSSImportRuleHref(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    CSSImportRule* imp = static_cast<CSSImportRule*>(static_cast<JSCSSImportRule*>(asObject(slot.slotBase()))->impl());
    return jsStringOrNull(exec, imp->href());
}

JSValuePtr jsCSSImportRuleMedia(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    CSSImportRule* imp = static_cast<CSSImportRule*>(static_cast<JSCSSImportRule*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->media()));
}

JSValuePtr jsCSSImportRuleStyleSheet(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    CSSImportRule* imp = static_cast<CSSImportRule*>(static_cast<JSCSSImportRule*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->styleSheet()));
}

JSValuePtr jsCSSImportRuleConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSCSSImportRule*>(asObject(slot.slotBase()))->getConstructor(exec);
}
JSValuePtr JSCSSImportRule::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSCSSImportRuleConstructor>(exec);
}


}