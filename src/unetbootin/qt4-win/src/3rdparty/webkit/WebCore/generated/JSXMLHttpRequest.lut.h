/* Automatically generated from ../bindings/js/JSXMLHttpRequest.cpp using JavaScriptCore/kjs/create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry JSXMLHttpRequestPrototypeTableEntries[] = {
   { "getResponseHeader", JSXMLHttpRequest::GetResponseHeader, DontDelete|Function, 1, &JSXMLHttpRequestPrototypeTableEntries[9] }/* 3719932930 */ ,
   { 0, 0, 0, 0, 0 },
   { "setRequestHeader", JSXMLHttpRequest::SetRequestHeader, DontDelete|Function, 2, 0 }/* 447965870 */ ,
   { "send", JSXMLHttpRequest::Send, DontDelete|Function, 1, 0 }/* 1580270940 */ ,
   { 0, 0, 0, 0, 0 },
   { "open", JSXMLHttpRequest::Open, DontDelete|Function, 5, &JSXMLHttpRequestPrototypeTableEntries[8] }/* 1674648015 */ ,
   { "abort", JSXMLHttpRequest::Abort, DontDelete|Function, 0, &JSXMLHttpRequestPrototypeTableEntries[7] }/* 3823818319 */ ,
   { "getAllResponseHeaders", JSXMLHttpRequest::GetAllResponseHeaders, DontDelete|Function, 0, &JSXMLHttpRequestPrototypeTableEntries[11] }/* 571638136 */ ,
   { "overrideMimeType", JSXMLHttpRequest::OverrideMIMEType, DontDelete|Function, 1, 0 }/* 4250026191 */ ,
   { "addEventListener", JSXMLHttpRequest::AddEventListener, DontDelete|Function, 3, &JSXMLHttpRequestPrototypeTableEntries[10] }/* 75269726 */ ,
   { "removeEventListener", JSXMLHttpRequest::RemoveEventListener, DontDelete|Function, 3, 0 }/* 1047744376 */ ,
   { "dispatchEvent", JSXMLHttpRequest::DispatchEvent, DontDelete|Function, 1, 0 }/* 3265753021 */ 
};

const struct HashTable JSXMLHttpRequestPrototypeTable = { 2, 12, JSXMLHttpRequestPrototypeTableEntries, 7 };

} // namespace

#include "lookup.h"

namespace KJS {

static const struct HashEntry JSXMLHttpRequestTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { "responseText", JSXMLHttpRequest::ResponseText, DontDelete|ReadOnly, 0, 0 }/* 287730045 */ ,
   { "status", JSXMLHttpRequest::Status, DontDelete|ReadOnly, 0, &JSXMLHttpRequestTableEntries[8] }/* 704781072 */ ,
   { "readyState", JSXMLHttpRequest::ReadyState, DontDelete|ReadOnly, 0, &JSXMLHttpRequestTableEntries[7] }/* 2142707136 */ ,
   { 0, 0, 0, 0, 0 },
   { "responseXML", JSXMLHttpRequest::ResponseXML, DontDelete|ReadOnly, 0, 0 }/* 406434600 */ ,
   { 0, 0, 0, 0, 0 },
   { "statusText", JSXMLHttpRequest::StatusText, DontDelete|ReadOnly, 0, 0 }/* 158010352 */ ,
   { "onreadystatechange", JSXMLHttpRequest::Onreadystatechange, DontDelete, 0, &JSXMLHttpRequestTableEntries[9] }/* 95629655 */ ,
   { "onload", JSXMLHttpRequest::Onload, DontDelete, 0, 0 }/* 3960668140 */ 
};

const struct HashTable JSXMLHttpRequestTable = { 2, 10, JSXMLHttpRequestTableEntries, 7 };

} // namespace
