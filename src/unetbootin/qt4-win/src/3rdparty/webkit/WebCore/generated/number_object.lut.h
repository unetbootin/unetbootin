/* Automatically generated from ../../JavaScriptCore/kjs/number_object.cpp using JavaScriptCore/kjs/create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry numberTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { "MAX_VALUE", NumberObjectImp::MaxValue, DontEnum|DontDelete|ReadOnly, 0, 0 }/* 2846567726 */ ,
   { "NaN", NumberObjectImp::NaNValue, DontEnum|DontDelete|ReadOnly, 0, 0 }/* 2899381417 */ ,
   { 0, 0, 0, 0, 0 },
   { "NEGATIVE_INFINITY", NumberObjectImp::NegInfinity, DontEnum|DontDelete|ReadOnly, 0, &numberTableEntries[5] }/* 1655285119 */ ,
   { "POSITIVE_INFINITY", NumberObjectImp::PosInfinity, DontEnum|DontDelete|ReadOnly, 0, &numberTableEntries[6] }/* 2941182689 */ ,
   { "MIN_VALUE", NumberObjectImp::MinValue, DontEnum|DontDelete|ReadOnly, 0, 0 }/* 3873332699 */ 
};

const struct HashTable numberTable = { 2, 7, numberTableEntries, 5 };

} // namespace
