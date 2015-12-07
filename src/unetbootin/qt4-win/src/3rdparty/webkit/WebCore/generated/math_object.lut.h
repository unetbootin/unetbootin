/* Automatically generated from ../../JavaScriptCore/kjs/math_object.cpp using JavaScriptCore/kjs/create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry mathTableEntries[] = {
   { "LN2", MathObjectImp::Ln2, DontEnum|DontDelete|ReadOnly, 0, &mathTableEntries[25] }/* 2671667313 */ ,
   { "round", MathObjectImp::Round, DontEnum|Function, 1, &mathTableEntries[30] }/* 368505691 */ ,
   { "abs", MathObjectImp::Abs, DontEnum|Function, 1, &mathTableEntries[23] }/* 4031145932 */ ,
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "LOG10E", MathObjectImp::Log10E, DontEnum|DontDelete|ReadOnly, 0, &mathTableEntries[21] }/* 3985658276 */ ,
   { "pow", MathObjectImp::Pow, DontEnum|Function, 2, 0 }/* 195216468 */ ,
   { "exp", MathObjectImp::Exp, DontEnum|Function, 1, 0 }/* 3909690232 */ ,
   { "E", MathObjectImp::Euler, DontEnum|DontDelete|ReadOnly, 0, 0 }/* 4247163005 */ ,
   { "floor", MathObjectImp::Floor, DontEnum|Function, 1, 0 }/* 1931817057 */ ,
   { "LOG2E", MathObjectImp::Log2E, DontEnum|DontDelete|ReadOnly, 0, &mathTableEntries[29] }/* 1065392359 */ ,
   { "atan2", MathObjectImp::ATan2, DontEnum|Function, 2, &mathTableEntries[24] }/* 240016088 */ ,
   { "acos", MathObjectImp::ACos, DontEnum|Function, 1, 0 }/* 2091015246 */ ,
   { "PI", MathObjectImp::Pi, DontEnum|DontDelete|ReadOnly, 0, &mathTableEntries[22] }/* 2281976437 */ ,
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "tan", MathObjectImp::Tan, DontEnum|Function, 1, 0 }/* 1495917418 */ ,
   { "log", MathObjectImp::Log, DontEnum|Function, 1, 0 }/* 3271412756 */ ,
   { "LN10", MathObjectImp::Ln10, DontEnum|DontDelete|ReadOnly, 0, 0 }/* 3786008769 */ ,
   { "SQRT2", MathObjectImp::Sqrt2, DontEnum|DontDelete|ReadOnly, 0, 0 }/* 670080997 */ ,
   { 0, 0, 0, 0, 0 },
   { "SQRT1_2", MathObjectImp::Sqrt1_2, DontEnum|DontDelete|ReadOnly, 0, &mathTableEntries[28] }/* 2399638022 */ ,
   { "asin", MathObjectImp::ASin, DontEnum|Function, 1, 0 }/* 41780542 */ ,
   { "atan", MathObjectImp::ATan, DontEnum|Function, 1, &mathTableEntries[26] }/* 1142128808 */ ,
   { "ceil", MathObjectImp::Ceil, DontEnum|Function, 1, &mathTableEntries[27] }/* 2762729288 */ ,
   { "cos", MathObjectImp::Cos, DontEnum|Function, 1, 0 }/* 2954861070 */ ,
   { "max", MathObjectImp::Max, DontEnum|Function, 2, 0 }/* 389325134 */ ,
   { "min", MathObjectImp::Min, DontEnum|Function, 2, 0 }/* 1361150774 */ ,
   { "random", MathObjectImp::Random, DontEnum|Function, 0, 0 }/* 549648839 */ ,
   { "sin", MathObjectImp::Sin, DontEnum|Function, 1, 0 }/* 1589798521 */ ,
   { "sqrt", MathObjectImp::Sqrt, DontEnum|Function, 1, 0 }/* 4154444533 */ 
};

const struct HashTable mathTable = { 2, 31, mathTableEntries, 21 };

} // namespace
