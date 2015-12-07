// Automatically generated from ../bindings/js/JSDOMWindowBase.cpp using WebCore/../JavaScriptCore/create_hash_table. DO NOT EDIT!

namespace WebCore {

using namespace JSC;

static const struct HashTableValue JSDOMWindowBaseTableValues[14] = {
   { "open", DontDelete|Function, (intptr_t)windowProtoFuncOpen, (intptr_t)3 },
   { "showModalDialog", DontDelete|Function, (intptr_t)windowProtoFuncShowModalDialog, (intptr_t)1 },
   { "captureEvents", DontDelete|Function, (intptr_t)windowProtoFuncNotImplemented, (intptr_t)0 },
   { "releaseEvents", DontDelete|Function, (intptr_t)windowProtoFuncNotImplemented, (intptr_t)0 },
   { "crypto", DontDelete|ReadOnly, (intptr_t)jsDOMWindowBaseCrypto, (intptr_t)0 },
   { "event", DontDelete, (intptr_t)jsDOMWindowBaseEvent, (intptr_t)setJSDOMWindowBaseEvent },
   { "Audio", DontDelete, (intptr_t)jsDOMWindowBaseAudio, (intptr_t)setJSDOMWindowBaseAudio },
   { "Image", DontDelete, (intptr_t)jsDOMWindowBaseImage, (intptr_t)setJSDOMWindowBaseImage },
   { "MessageChannel", DontDelete, (intptr_t)jsDOMWindowBaseMessageChannel, (intptr_t)setJSDOMWindowBaseMessageChannel },
   { "Option", DontDelete, (intptr_t)jsDOMWindowBaseOption, (intptr_t)setJSDOMWindowBaseOption },
   { "Worker", DontDelete, (intptr_t)jsDOMWindowBaseWorker, (intptr_t)setJSDOMWindowBaseWorker },
   { "XMLHttpRequest", DontDelete, (intptr_t)jsDOMWindowBaseXMLHttpRequest, (intptr_t)setJSDOMWindowBaseXMLHttpRequest },
   { "XSLTProcessor", DontDelete, (intptr_t)jsDOMWindowBaseXSLTProcessor, (intptr_t)setJSDOMWindowBaseXSLTProcessor },
   { 0, 0, 0, 0 }
};

extern const struct HashTable JSDOMWindowBaseTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 63, JSDOMWindowBaseTableValues, 0 };
#else
    { 34, 31, JSDOMWindowBaseTableValues, 0 };
#endif

} // namespace
