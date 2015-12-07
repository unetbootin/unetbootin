// Automatically generated from ../runtime/ArrayPrototype.cpp using JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue arrayTableValues[20] = {
   { "toString", DontEnum|Function, (intptr_t)arrayProtoFuncToString, (intptr_t)0 },
   { "toLocaleString", DontEnum|Function, (intptr_t)arrayProtoFuncToLocaleString, (intptr_t)0 },
   { "concat", DontEnum|Function, (intptr_t)arrayProtoFuncConcat, (intptr_t)1 },
   { "join", DontEnum|Function, (intptr_t)arrayProtoFuncJoin, (intptr_t)1 },
   { "pop", DontEnum|Function, (intptr_t)arrayProtoFuncPop, (intptr_t)0 },
   { "push", DontEnum|Function, (intptr_t)arrayProtoFuncPush, (intptr_t)1 },
   { "reverse", DontEnum|Function, (intptr_t)arrayProtoFuncReverse, (intptr_t)0 },
   { "shift", DontEnum|Function, (intptr_t)arrayProtoFuncShift, (intptr_t)0 },
   { "slice", DontEnum|Function, (intptr_t)arrayProtoFuncSlice, (intptr_t)2 },
   { "sort", DontEnum|Function, (intptr_t)arrayProtoFuncSort, (intptr_t)1 },
   { "splice", DontEnum|Function, (intptr_t)arrayProtoFuncSplice, (intptr_t)2 },
   { "unshift", DontEnum|Function, (intptr_t)arrayProtoFuncUnShift, (intptr_t)1 },
   { "every", DontEnum|Function, (intptr_t)arrayProtoFuncEvery, (intptr_t)1 },
   { "forEach", DontEnum|Function, (intptr_t)arrayProtoFuncForEach, (intptr_t)1 },
   { "some", DontEnum|Function, (intptr_t)arrayProtoFuncSome, (intptr_t)1 },
   { "indexOf", DontEnum|Function, (intptr_t)arrayProtoFuncIndexOf, (intptr_t)1 },
   { "lastIndexOf", DontEnum|Function, (intptr_t)arrayProtoFuncLastIndexOf, (intptr_t)1 },
   { "filter", DontEnum|Function, (intptr_t)arrayProtoFuncFilter, (intptr_t)1 },
   { "map", DontEnum|Function, (intptr_t)arrayProtoFuncMap, (intptr_t)1 },
   { 0, 0, 0, 0 }
};

extern const struct HashTable arrayTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 127, arrayTableValues, 0 };
#else
    { 65, 63, arrayTableValues, 0 };
#endif

} // namespace
