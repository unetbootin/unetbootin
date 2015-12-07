// Automatically generated from ../runtime/RegExpConstructor.cpp using JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct HashTableValue regExpConstructorTableValues[22] = {
   { "input", None, (intptr_t)regExpConstructorInput, (intptr_t)setRegExpConstructorInput },
   { "$_", DontEnum, (intptr_t)regExpConstructorInput, (intptr_t)setRegExpConstructorInput },
   { "multiline", None, (intptr_t)regExpConstructorMultiline, (intptr_t)setRegExpConstructorMultiline },
   { "$*", DontEnum, (intptr_t)regExpConstructorMultiline, (intptr_t)setRegExpConstructorMultiline },
   { "lastMatch", DontDelete|ReadOnly, (intptr_t)regExpConstructorLastMatch, (intptr_t)0 },
   { "$&", DontDelete|ReadOnly|DontEnum, (intptr_t)regExpConstructorLastMatch, (intptr_t)0 },
   { "lastParen", DontDelete|ReadOnly, (intptr_t)regExpConstructorLastParen, (intptr_t)0 },
   { "$+", DontDelete|ReadOnly|DontEnum, (intptr_t)regExpConstructorLastParen, (intptr_t)0 },
   { "leftContext", DontDelete|ReadOnly, (intptr_t)regExpConstructorLeftContext, (intptr_t)0 },
   { "$`", DontDelete|ReadOnly|DontEnum, (intptr_t)regExpConstructorLeftContext, (intptr_t)0 },
   { "rightContext", DontDelete|ReadOnly, (intptr_t)regExpConstructorRightContext, (intptr_t)0 },
   { "$'", DontDelete|ReadOnly|DontEnum, (intptr_t)regExpConstructorRightContext, (intptr_t)0 },
   { "$1", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar1, (intptr_t)0 },
   { "$2", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar2, (intptr_t)0 },
   { "$3", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar3, (intptr_t)0 },
   { "$4", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar4, (intptr_t)0 },
   { "$5", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar5, (intptr_t)0 },
   { "$6", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar6, (intptr_t)0 },
   { "$7", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar7, (intptr_t)0 },
   { "$8", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar8, (intptr_t)0 },
   { "$9", DontDelete|ReadOnly, (intptr_t)regExpConstructorDollar9, (intptr_t)0 },
   { 0, 0, 0, 0 }
};

extern const struct HashTable regExpConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 511, regExpConstructorTableValues, 0 };
#else
    { 65, 63, regExpConstructorTableValues, 0 };
#endif

} // namespace
