// Automatically generated from ../runtime/StringPrototype.cpp using JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue stringTableValues[33] = {
   { "toString", DontEnum|Function, (intptr_t)stringProtoFuncToString, (intptr_t)0 },
   { "valueOf", DontEnum|Function, (intptr_t)stringProtoFuncToString, (intptr_t)0 },
   { "charAt", DontEnum|Function, (intptr_t)stringProtoFuncCharAt, (intptr_t)1 },
   { "charCodeAt", DontEnum|Function, (intptr_t)stringProtoFuncCharCodeAt, (intptr_t)1 },
   { "concat", DontEnum|Function, (intptr_t)stringProtoFuncConcat, (intptr_t)1 },
   { "indexOf", DontEnum|Function, (intptr_t)stringProtoFuncIndexOf, (intptr_t)1 },
   { "lastIndexOf", DontEnum|Function, (intptr_t)stringProtoFuncLastIndexOf, (intptr_t)1 },
   { "match", DontEnum|Function, (intptr_t)stringProtoFuncMatch, (intptr_t)1 },
   { "replace", DontEnum|Function, (intptr_t)stringProtoFuncReplace, (intptr_t)2 },
   { "search", DontEnum|Function, (intptr_t)stringProtoFuncSearch, (intptr_t)1 },
   { "slice", DontEnum|Function, (intptr_t)stringProtoFuncSlice, (intptr_t)2 },
   { "split", DontEnum|Function, (intptr_t)stringProtoFuncSplit, (intptr_t)2 },
   { "substr", DontEnum|Function, (intptr_t)stringProtoFuncSubstr, (intptr_t)2 },
   { "substring", DontEnum|Function, (intptr_t)stringProtoFuncSubstring, (intptr_t)2 },
   { "toLowerCase", DontEnum|Function, (intptr_t)stringProtoFuncToLowerCase, (intptr_t)0 },
   { "toUpperCase", DontEnum|Function, (intptr_t)stringProtoFuncToUpperCase, (intptr_t)0 },
   { "localeCompare", DontEnum|Function, (intptr_t)stringProtoFuncLocaleCompare, (intptr_t)1 },
   { "toLocaleLowerCase", DontEnum|Function, (intptr_t)stringProtoFuncToLowerCase, (intptr_t)0 },
   { "toLocaleUpperCase", DontEnum|Function, (intptr_t)stringProtoFuncToUpperCase, (intptr_t)0 },
   { "big", DontEnum|Function, (intptr_t)stringProtoFuncBig, (intptr_t)0 },
   { "small", DontEnum|Function, (intptr_t)stringProtoFuncSmall, (intptr_t)0 },
   { "blink", DontEnum|Function, (intptr_t)stringProtoFuncBlink, (intptr_t)0 },
   { "bold", DontEnum|Function, (intptr_t)stringProtoFuncBold, (intptr_t)0 },
   { "fixed", DontEnum|Function, (intptr_t)stringProtoFuncFixed, (intptr_t)0 },
   { "italics", DontEnum|Function, (intptr_t)stringProtoFuncItalics, (intptr_t)0 },
   { "strike", DontEnum|Function, (intptr_t)stringProtoFuncStrike, (intptr_t)0 },
   { "sub", DontEnum|Function, (intptr_t)stringProtoFuncSub, (intptr_t)0 },
   { "sup", DontEnum|Function, (intptr_t)stringProtoFuncSup, (intptr_t)0 },
   { "fontcolor", DontEnum|Function, (intptr_t)stringProtoFuncFontcolor, (intptr_t)1 },
   { "fontsize", DontEnum|Function, (intptr_t)stringProtoFuncFontsize, (intptr_t)1 },
   { "anchor", DontEnum|Function, (intptr_t)stringProtoFuncAnchor, (intptr_t)1 },
   { "link", DontEnum|Function, (intptr_t)stringProtoFuncLink, (intptr_t)1 },
   { 0, 0, 0, 0 }
};

extern const struct HashTable stringTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 2047, stringTableValues, 0 };
#else
    { 71, 63, stringTableValues, 0 };
#endif

} // namespace
