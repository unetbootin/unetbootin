/* Automatically generated from ../../JavaScriptCore/kjs/regexp_object.cpp using JavaScriptCore/kjs/create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry RegExpTableEntries[] = {
   { "input", RegExpObjectImp::Input, None, 0, 0 }/* 916335100 */ ,
   { "$5", RegExpObjectImp::Dollar5, DontDelete|ReadOnly, 0, 0 }/* 1294203281 */ ,
   { "$3", RegExpObjectImp::Dollar3, DontDelete|ReadOnly, 0, 0 }/* 1474083822 */ ,
   { "$1", RegExpObjectImp::Dollar1, DontDelete|ReadOnly, 0, &RegExpTableEntries[26] }/* 1905495623 */ ,
   { 0, 0, 0, 0, 0 },
   { "$_", RegExpObjectImp::Input, DontEnum, 0, 0 }/* 1478551605 */ ,
   { "$+", RegExpObjectImp::LastParen, DontDelete|ReadOnly|DontEnum, 0, &RegExpTableEntries[23] }/* 2230578446 */ ,
   { "multiline", RegExpObjectImp::Multiline, None, 0, 0 }/* 167225807 */ ,
   { "lastParen", RegExpObjectImp::LastParen, DontDelete|ReadOnly, 0, &RegExpTableEntries[21] }/* 1570511608 */ ,
   { "$*", RegExpObjectImp::Multiline, DontEnum, 0, &RegExpTableEntries[20] }/* 2273268249 */ ,
   { 0, 0, 0, 0, 0 },
   { "rightContext", RegExpObjectImp::RightContext, DontDelete|ReadOnly, 0, 0 }/* 2723842571 */ ,
   { "$&", RegExpObjectImp::LastMatch, DontDelete|ReadOnly|DontEnum, 0, &RegExpTableEntries[25] }/* 3834535012 */ ,
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "$`", RegExpObjectImp::LeftContext, DontDelete|ReadOnly|DontEnum, 0, &RegExpTableEntries[22] }/* 2990930896 */ ,
   { "lastMatch", RegExpObjectImp::LastMatch, DontDelete|ReadOnly, 0, 0 }/* 1773827617 */ ,
   { "$4", RegExpObjectImp::Dollar4, DontDelete|ReadOnly, 0, 0 }/* 1436644538 */ ,
   { 0, 0, 0, 0, 0 },
   { "leftContext", RegExpObjectImp::LeftContext, DontDelete|ReadOnly, 0, 0 }/* 3329414529 */ ,
   { "$'", RegExpObjectImp::RightContext, DontDelete|ReadOnly|DontEnum, 0, 0 }/* 3924383088 */ ,
   { "$2", RegExpObjectImp::Dollar2, DontDelete|ReadOnly, 0, 0 }/* 1984886116 */ ,
   { "$6", RegExpObjectImp::Dollar6, DontDelete|ReadOnly, 0, &RegExpTableEntries[24] }/* 1220309546 */ ,
   { "$7", RegExpObjectImp::Dollar7, DontDelete|ReadOnly, 0, 0 }/* 3710803966 */ ,
   { "$8", RegExpObjectImp::Dollar8, DontDelete|ReadOnly, 0, 0 }/* 3492788052 */ ,
   { "$9", RegExpObjectImp::Dollar9, DontDelete|ReadOnly, 0, 0 }/* 1575317823 */ 
};

const struct HashTable RegExpTable = { 2, 27, RegExpTableEntries, 20 };

} // namespace
