// Automatically generated from ../runtime/MathObject.cpp using JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue mathTableValues[19] = {
   { "abs", DontEnum|Function, (intptr_t)mathProtoFuncAbs, (intptr_t)1 },
   { "acos", DontEnum|Function, (intptr_t)mathProtoFuncACos, (intptr_t)1 },
   { "asin", DontEnum|Function, (intptr_t)mathProtoFuncASin, (intptr_t)1 },
   { "atan", DontEnum|Function, (intptr_t)mathProtoFuncATan, (intptr_t)1 },
   { "atan2", DontEnum|Function, (intptr_t)mathProtoFuncATan2, (intptr_t)2 },
   { "ceil", DontEnum|Function, (intptr_t)mathProtoFuncCeil, (intptr_t)1 },
   { "cos", DontEnum|Function, (intptr_t)mathProtoFuncCos, (intptr_t)1 },
   { "exp", DontEnum|Function, (intptr_t)mathProtoFuncExp, (intptr_t)1 },
   { "floor", DontEnum|Function, (intptr_t)mathProtoFuncFloor, (intptr_t)1 },
   { "log", DontEnum|Function, (intptr_t)mathProtoFuncLog, (intptr_t)1 },
   { "max", DontEnum|Function, (intptr_t)mathProtoFuncMax, (intptr_t)2 },
   { "min", DontEnum|Function, (intptr_t)mathProtoFuncMin, (intptr_t)2 },
   { "pow", DontEnum|Function, (intptr_t)mathProtoFuncPow, (intptr_t)2 },
   { "random", DontEnum|Function, (intptr_t)mathProtoFuncRandom, (intptr_t)0 },
   { "round", DontEnum|Function, (intptr_t)mathProtoFuncRound, (intptr_t)1 },
   { "sin", DontEnum|Function, (intptr_t)mathProtoFuncSin, (intptr_t)1 },
   { "sqrt", DontEnum|Function, (intptr_t)mathProtoFuncSqrt, (intptr_t)1 },
   { "tan", DontEnum|Function, (intptr_t)mathProtoFuncTan, (intptr_t)1 },
   { 0, 0, 0, 0 }
};

extern const struct HashTable mathTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 511, mathTableValues, 0 };
#else
    { 67, 63, mathTableValues, 0 };
#endif

} // namespace
