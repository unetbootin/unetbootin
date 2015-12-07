/* Automatically generated from ../../JavaScriptCore/kjs/array_object.cpp using JavaScriptCore/kjs/create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry arrayTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "reverse", ArrayProtoFunc::Reverse, DontEnum|Function, 0, 0 }/* 1259697523 */ ,
   { "pop", ArrayProtoFunc::Pop, DontEnum|Function, 0, &arrayTableEntries[17] }/* 876916276 */ ,
   { "push", ArrayProtoFunc::Push, DontEnum|Function, 1, 0 }/* 55090581 */ ,
   { "toLocaleString", ArrayProtoFunc::ToLocaleString, DontEnum|Function, 0, 0 }/* 3561459206 */ ,
   { "shift", ArrayProtoFunc::Shift, DontEnum|Function, 0, &arrayTableEntries[19] }/* 2106316215 */ ,
   { "map", ArrayProtoFunc::Map, DontEnum|Function, 1, 0 }/* 2902200536 */ ,
   { "splice", ArrayProtoFunc::Splice, DontEnum|Function, 2, 0 }/* 3393300697 */ ,
   { "indexOf", ArrayProtoFunc::IndexOf, DontEnum|Function, 1, &arrayTableEntries[21] }/* 3409593642 */ ,
   { "slice", ArrayProtoFunc::Slice, DontEnum|Function, 2, 0 }/* 1302631211 */ ,
   { "toString", ArrayProtoFunc::ToString, DontEnum|Function, 0, 0 }/* 150235932 */ ,
   { "sort", ArrayProtoFunc::Sort, DontEnum|Function, 1, 0 }/* 3234276573 */ ,
   { "filter", ArrayProtoFunc::Filter, DontEnum|Function, 1, 0 }/* 1342131950 */ ,
   { "concat", ArrayProtoFunc::Concat, DontEnum|Function, 1, &arrayTableEntries[16] }/* 2774556463 */ ,
   { "join", ArrayProtoFunc::Join, DontEnum|Function, 1, &arrayTableEntries[18] }/* 74534303 */ ,
   { "unshift", ArrayProtoFunc::UnShift, DontEnum|Function, 1, 0 }/* 1292001012 */ ,
   { "every", ArrayProtoFunc::Every, DontEnum|Function, 1, &arrayTableEntries[20] }/* 4286540159 */ ,
   { "forEach", ArrayProtoFunc::ForEach, DontEnum|Function, 1, 0 }/* 2626804119 */ ,
   { "some", ArrayProtoFunc::Some, DontEnum|Function, 1, 0 }/* 2331092431 */ ,
   { "lastIndexOf", ArrayProtoFunc::LastIndexOf, DontEnum|Function, 1, 0 }/* 2145229914 */ 
};

const struct HashTable arrayTable = { 2, 22, arrayTableEntries, 16 };

} // namespace
