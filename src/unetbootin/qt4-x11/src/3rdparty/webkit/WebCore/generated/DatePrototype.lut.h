// Automatically generated from ../../JavaScriptCore/runtime/DatePrototype.cpp using JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue dateTableValues[45] = {
   { "toString", DontEnum|Function, (intptr_t)dateProtoFuncToString, (intptr_t)0 },
   { "toUTCString", DontEnum|Function, (intptr_t)dateProtoFuncToUTCString, (intptr_t)0 },
   { "toDateString", DontEnum|Function, (intptr_t)dateProtoFuncToDateString, (intptr_t)0 },
   { "toTimeString", DontEnum|Function, (intptr_t)dateProtoFuncToTimeString, (intptr_t)0 },
   { "toLocaleString", DontEnum|Function, (intptr_t)dateProtoFuncToLocaleString, (intptr_t)0 },
   { "toLocaleDateString", DontEnum|Function, (intptr_t)dateProtoFuncToLocaleDateString, (intptr_t)0 },
   { "toLocaleTimeString", DontEnum|Function, (intptr_t)dateProtoFuncToLocaleTimeString, (intptr_t)0 },
   { "valueOf", DontEnum|Function, (intptr_t)dateProtoFuncValueOf, (intptr_t)0 },
   { "getTime", DontEnum|Function, (intptr_t)dateProtoFuncGetTime, (intptr_t)0 },
   { "getFullYear", DontEnum|Function, (intptr_t)dateProtoFuncGetFullYear, (intptr_t)0 },
   { "getUTCFullYear", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCFullYear, (intptr_t)0 },
   { "toGMTString", DontEnum|Function, (intptr_t)dateProtoFuncToGMTString, (intptr_t)0 },
   { "getMonth", DontEnum|Function, (intptr_t)dateProtoFuncGetMonth, (intptr_t)0 },
   { "getUTCMonth", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCMonth, (intptr_t)0 },
   { "getDate", DontEnum|Function, (intptr_t)dateProtoFuncGetDate, (intptr_t)0 },
   { "getUTCDate", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCDate, (intptr_t)0 },
   { "getDay", DontEnum|Function, (intptr_t)dateProtoFuncGetDay, (intptr_t)0 },
   { "getUTCDay", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCDay, (intptr_t)0 },
   { "getHours", DontEnum|Function, (intptr_t)dateProtoFuncGetHours, (intptr_t)0 },
   { "getUTCHours", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCHours, (intptr_t)0 },
   { "getMinutes", DontEnum|Function, (intptr_t)dateProtoFuncGetMinutes, (intptr_t)0 },
   { "getUTCMinutes", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCMinutes, (intptr_t)0 },
   { "getSeconds", DontEnum|Function, (intptr_t)dateProtoFuncGetSeconds, (intptr_t)0 },
   { "getUTCSeconds", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCSeconds, (intptr_t)0 },
   { "getMilliseconds", DontEnum|Function, (intptr_t)dateProtoFuncGetMilliSeconds, (intptr_t)0 },
   { "getUTCMilliseconds", DontEnum|Function, (intptr_t)dateProtoFuncGetUTCMilliseconds, (intptr_t)0 },
   { "getTimezoneOffset", DontEnum|Function, (intptr_t)dateProtoFuncGetTimezoneOffset, (intptr_t)0 },
   { "setTime", DontEnum|Function, (intptr_t)dateProtoFuncSetTime, (intptr_t)1 },
   { "setMilliseconds", DontEnum|Function, (intptr_t)dateProtoFuncSetMilliSeconds, (intptr_t)1 },
   { "setUTCMilliseconds", DontEnum|Function, (intptr_t)dateProtoFuncSetUTCMilliseconds, (intptr_t)1 },
   { "setSeconds", DontEnum|Function, (intptr_t)dateProtoFuncSetSeconds, (intptr_t)2 },
   { "setUTCSeconds", DontEnum|Function, (intptr_t)dateProtoFuncSetUTCSeconds, (intptr_t)2 },
   { "setMinutes", DontEnum|Function, (intptr_t)dateProtoFuncSetMinutes, (intptr_t)3 },
   { "setUTCMinutes", DontEnum|Function, (intptr_t)dateProtoFuncSetUTCMinutes, (intptr_t)3 },
   { "setHours", DontEnum|Function, (intptr_t)dateProtoFuncSetHours, (intptr_t)4 },
   { "setUTCHours", DontEnum|Function, (intptr_t)dateProtoFuncSetUTCHours, (intptr_t)4 },
   { "setDate", DontEnum|Function, (intptr_t)dateProtoFuncSetDate, (intptr_t)1 },
   { "setUTCDate", DontEnum|Function, (intptr_t)dateProtoFuncSetUTCDate, (intptr_t)1 },
   { "setMonth", DontEnum|Function, (intptr_t)dateProtoFuncSetMonth, (intptr_t)2 },
   { "setUTCMonth", DontEnum|Function, (intptr_t)dateProtoFuncSetUTCMonth, (intptr_t)2 },
   { "setFullYear", DontEnum|Function, (intptr_t)dateProtoFuncSetFullYear, (intptr_t)3 },
   { "setUTCFullYear", DontEnum|Function, (intptr_t)dateProtoFuncSetUTCFullYear, (intptr_t)3 },
   { "setYear", DontEnum|Function, (intptr_t)dateProtoFuncSetYear, (intptr_t)1 },
   { "getYear", DontEnum|Function, (intptr_t)dateProtoFuncGetYear, (intptr_t)0 },
   { 0, 0, 0, 0 }
};

extern const struct HashTable dateTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 2047, dateTableValues, 0 };
#else
    { 134, 127, dateTableValues, 0 };
#endif

} // namespace
