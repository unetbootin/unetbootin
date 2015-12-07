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

#include "JSTreeWalker.h"

#include <wtf/GetPtr.h>

#include "JSNode.h"
#include "JSNodeFilter.h"
#include "Node.h"
#include "NodeFilter.h"
#include "TreeWalker.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashEntry JSTreeWalkerTableEntries[] =
{
    { "filter", JSTreeWalker::FilterAttrNum, DontDelete|ReadOnly, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "root", JSTreeWalker::RootAttrNum, DontDelete|ReadOnly, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "whatToShow", JSTreeWalker::WhatToShowAttrNum, DontDelete|ReadOnly, 0, &JSTreeWalkerTableEntries[5] },
    { "expandEntityReferences", JSTreeWalker::ExpandEntityReferencesAttrNum, DontDelete|ReadOnly, 0, &JSTreeWalkerTableEntries[6] },
    { "currentNode", JSTreeWalker::CurrentNodeAttrNum, DontDelete, 0, 0 }
};

static const HashTable JSTreeWalkerTable = 
{
    2, 7, JSTreeWalkerTableEntries, 5
};

/* Hash table for prototype */

static const HashEntry JSTreeWalkerPrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "nextSibling", JSTreeWalker::NextSiblingFuncNum, DontDelete|Function, 0, 0 },
    { "parentNode", JSTreeWalker::ParentNodeFuncNum, DontDelete|Function, 0, &JSTreeWalkerPrototypeTableEntries[7] },
    { "nextNode", JSTreeWalker::NextNodeFuncNum, DontDelete|Function, 0, 0 },
    { "lastChild", JSTreeWalker::LastChildFuncNum, DontDelete|Function, 0, &JSTreeWalkerPrototypeTableEntries[8] },
    { "previousNode", JSTreeWalker::PreviousNodeFuncNum, DontDelete|Function, 0, 0 },
    { "firstChild", JSTreeWalker::FirstChildFuncNum, DontDelete|Function, 0, 0 },
    { "previousSibling", JSTreeWalker::PreviousSiblingFuncNum, DontDelete|Function, 0, 0 }
};

static const HashTable JSTreeWalkerPrototypeTable = 
{
    2, 9, JSTreeWalkerPrototypeTableEntries, 7
};

const ClassInfo JSTreeWalkerPrototype::info = { "TreeWalkerPrototype", 0, &JSTreeWalkerPrototypeTable, 0 };

JSObject* JSTreeWalkerPrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSTreeWalkerPrototype>(exec, "[[JSTreeWalker.prototype]]");
}

bool JSTreeWalkerPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSTreeWalkerPrototypeFunction, JSObject>(exec, &JSTreeWalkerPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSTreeWalker::info = { "TreeWalker", 0, &JSTreeWalkerTable, 0 };

JSTreeWalker::JSTreeWalker(ExecState* exec, TreeWalker* impl)
    : m_impl(impl)
{
    setPrototype(JSTreeWalkerPrototype::self(exec));
}

JSTreeWalker::~JSTreeWalker()
{
    ScriptInterpreter::forgetDOMObject(m_impl.get());
}

bool JSTreeWalker::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTreeWalker, KJS::DOMObject>(exec, &JSTreeWalkerTable, this, propertyName, slot);
}

JSValue* JSTreeWalker::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case RootAttrNum: {
        TreeWalker* imp = static_cast<TreeWalker*>(impl());

        return toJS(exec, WTF::getPtr(imp->root()));
    }
    case WhatToShowAttrNum: {
        TreeWalker* imp = static_cast<TreeWalker*>(impl());

        return jsNumber(imp->whatToShow());
    }
    case FilterAttrNum: {
        TreeWalker* imp = static_cast<TreeWalker*>(impl());

        return toJS(exec, WTF::getPtr(imp->filter()));
    }
    case ExpandEntityReferencesAttrNum: {
        TreeWalker* imp = static_cast<TreeWalker*>(impl());

        return jsBoolean(imp->expandEntityReferences());
    }
    case CurrentNodeAttrNum: {
        TreeWalker* imp = static_cast<TreeWalker*>(impl());

        return toJS(exec, WTF::getPtr(imp->currentNode()));
    }
    }
    return 0;
}

void JSTreeWalker::put(ExecState* exec, const Identifier& propertyName, JSValue* value, int attr)
{
    lookupPut<JSTreeWalker, KJS::DOMObject>(exec, propertyName, value, attr, &JSTreeWalkerTable, this);
}

void JSTreeWalker::putValueProperty(ExecState* exec, int token, JSValue* value, int /*attr*/)
{
    switch (token) {
    case CurrentNodeAttrNum: {
        TreeWalker* imp = static_cast<TreeWalker*>(impl());

        ExceptionCode ec = 0;
        imp->setCurrentNode(toNode(value), ec);
        setDOMException(exec, ec);
        break;
    }
    }
}

JSValue* JSTreeWalkerPrototypeFunction::callAsFunction(ExecState* exec, JSObject* thisObj, const List& args)
{
    if (!thisObj->inherits(&JSTreeWalker::info))
      return throwError(exec, TypeError);

    TreeWalker* imp = static_cast<TreeWalker*>(static_cast<JSTreeWalker*>(thisObj)->impl());

    switch (id) {
    case JSTreeWalker::ParentNodeFuncNum: {


        KJS::JSValue* result = toJS(exec, WTF::getPtr(imp->parentNode()));
        return result;
    }
    case JSTreeWalker::FirstChildFuncNum: {


        KJS::JSValue* result = toJS(exec, WTF::getPtr(imp->firstChild()));
        return result;
    }
    case JSTreeWalker::LastChildFuncNum: {


        KJS::JSValue* result = toJS(exec, WTF::getPtr(imp->lastChild()));
        return result;
    }
    case JSTreeWalker::PreviousSiblingFuncNum: {


        KJS::JSValue* result = toJS(exec, WTF::getPtr(imp->previousSibling()));
        return result;
    }
    case JSTreeWalker::NextSiblingFuncNum: {


        KJS::JSValue* result = toJS(exec, WTF::getPtr(imp->nextSibling()));
        return result;
    }
    case JSTreeWalker::PreviousNodeFuncNum: {


        KJS::JSValue* result = toJS(exec, WTF::getPtr(imp->previousNode()));
        return result;
    }
    case JSTreeWalker::NextNodeFuncNum: {


        KJS::JSValue* result = toJS(exec, WTF::getPtr(imp->nextNode()));
        return result;
    }
    }
    return 0;
}
KJS::JSValue* toJS(KJS::ExecState* exec, TreeWalker* obj)
{
    return KJS::cacheDOMObject<TreeWalker, JSTreeWalker>(exec, obj);
}
TreeWalker* toTreeWalker(KJS::JSValue* val)
{
    return val->isObject(&JSTreeWalker::info) ? static_cast<JSTreeWalker*>(val)->impl() : 0;
}

}