/* Automatically generated from ../bindings/js/kjs_events.cpp using JavaScriptCore/kjs/create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry JSClipboardTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { "effectAllowed", WebCore::JSClipboard::EffectAllowed, DontDelete, 0, &JSClipboardTableEntries[3] }/* 4031100625 */ ,
   { "dropEffect", WebCore::JSClipboard::DropEffect, DontDelete, 0, 0 }/* 2804509154 */ ,
   { "types", WebCore::JSClipboard::Types, DontDelete|ReadOnly, 0, 0 }/* 3287788138 */ 
};

const struct HashTable JSClipboardTable = { 2, 4, JSClipboardTableEntries, 3 };

} // namespace

#include "lookup.h"

namespace KJS {

static const struct HashEntry JSClipboardPrototypeTableEntries[] = {
   { "clearData", WebCore::JSClipboard::ClearData, DontDelete|Function, 0, 0 }/* 2521984052 */ ,
   { "getData", WebCore::JSClipboard::GetData, DontDelete|Function, 1, &JSClipboardPrototypeTableEntries[4] }/* 2662472317 */ ,
   { 0, 0, 0, 0, 0 },
   { "setDragImage", WebCore::JSClipboard::SetDragImage, DontDelete|Function, 3, 0 }/* 3024539715 */ ,
   { "setData", WebCore::JSClipboard::SetData, DontDelete|Function, 2, 0 }/* 3811369929 */ 
};

const struct HashTable JSClipboardPrototypeTable = { 2, 5, JSClipboardPrototypeTableEntries, 4 };

} // namespace
