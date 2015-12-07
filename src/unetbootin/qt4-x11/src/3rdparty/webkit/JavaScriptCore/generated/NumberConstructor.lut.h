// Automatically generated from ../runtime/NumberConstructor.cpp using JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue numberTableValues[6] = {
   { "NaN", DontEnum|DontDelete|ReadOnly, (intptr_t)numberConstructorNaNValue, (intptr_t)0 },
   { "NEGATIVE_INFINITY", DontEnum|DontDelete|ReadOnly, (intptr_t)numberConstructorNegInfinity, (intptr_t)0 },
   { "POSITIVE_INFINITY", DontEnum|DontDelete|ReadOnly, (intptr_t)numberConstructorPosInfinity, (intptr_t)0 },
   { "MAX_VALUE", DontEnum|DontDelete|ReadOnly, (intptr_t)numberConstructorMaxValue, (intptr_t)0 },
   { "MIN_VALUE", DontEnum|DontDelete|ReadOnly, (intptr_t)numberConstructorMinValue, (intptr_t)0 },
   { 0, 0, 0, 0 }
};

extern const struct HashTable numberTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 15, numberTableValues, 0 };
#else
    { 16, 15, numberTableValues, 0 };
#endif

} // namespace
