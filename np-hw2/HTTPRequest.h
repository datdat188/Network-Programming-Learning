//
//  HTTPRequest.h
//  Proxy Server
//
//  Created by Matt Zanchelli on 3/1/14.
//  Copyright (c) 2014 Matt Zanchelli. All rights reserved.
//

#ifndef Proxy_Server_HTTPRequest_h
#define Proxy_Server_HTTPRequest_h

#include "StringFunctions.h"

/// The header fields of an HTTP Request.
/// @discussion The index of each field can be found using @c HTTPRequestHeaderField.
typedef char ** HTTPRequest;

/// Create an HTTPRequest.
/// @return A newly allocated HTTPRequest.
HTTPRequest HTTPRequestCreate();

/// Deallocate an instance of HTTPRequest.
void HTTPRequestFree(HTTPRequest request);

/// Validate an instance of HTTPRequest.
/// @param request The HTTPRequest to validate.
/// @return Whether or not the request is valid.
bool validateRequest(HTTPRequest request);

/// Create a request string from a given HTTP Request.
/// @param request The HTTPRequest to translate into a string.
/// @return A newly allocated string containing the request.
char *requestStringFromRequest(HTTPRequest request);

/// A header field name for an HTTP Request.
typedef const char * const HTTPRequestHeaderFieldName;

static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Accept = "Accept";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Accept_Charset = "Accept-Charset";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Accept_Encoding = "Accept-Encoding";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Accept_Language = "Accept-Language";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Accept_Datetime = "Accept-Datetime";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Authorization = "Authorization";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Cache_Control = "Cache-Control";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Connection = "Connection";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Proxy_Connection = "Proxy-Connection";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Cookie = "Cookie";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Content_Length = "Content-Length";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Content_MD5 = "Content-MD5";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Content_Type = "Content-Type";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Date = "Date";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Expect = "Expect";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_From = "From";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Host = "Host";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_If_Match = "If-Match";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_If_Modified_Since = "If-Modified-Since";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_If_None_Match = "If-None-Match";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_If_Range = "If-Range";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_If_Unmodified_Since = "If-Unmodified-Since";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Max_Forwards = "Max-Forwards";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Origin = "Origin";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Pragma = "Pragma";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Proxy_Authorization = "Proxy-Authorization";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Range = "Range";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Referer = "Referer";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_TE = "TE";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_User_Agent = "User-Agent";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Via = "Via";
static HTTPRequestHeaderFieldName HTTPRequestHeaderFieldName_Warning = "Warning";

/// Enumerated header fields for an HTTP Request.
typedef enum {
	HTTPRequestHeaderField_Request_Line,
	HTTPRequestHeaderField_Accept,
	HTTPRequestHeaderField_Accept_Charset,
	HTTPRequestHeaderField_Accept_Encoding,
	HTTPRequestHeaderField_Accept_Language,
	HTTPRequestHeaderField_Accept_Datetime,
	HTTPRequestHeaderField_Authorization,
	HTTPRequestHeaderField_Cache_Control,
	HTTPRequestHeaderField_Connection,
	HTTPRequestHeaderField_Proxy_Connection,
	HTTPRequestHeaderField_Cookie,
	HTTPRequestHeaderField_Content_Length,
	HTTPRequestHeaderField_Content_MD5,
	HTTPRequestHeaderField_Content_Type,
	HTTPRequestHeaderField_Date,
	HTTPRequestHeaderField_Expect,
	HTTPRequestHeaderField_From,
	HTTPRequestHeaderField_Host,
	HTTPRequestHeaderField_If_Match,
	HTTPRequestHeaderField_If_Modified_Since,
	HTTPRequestHeaderField_If_None_Match,
	HTTPRequestHeaderField_If_Range,
	HTTPRequestHeaderField_If_Unmodified_Since,
	HTTPRequestHeaderField_Max_Forwards,
	HTTPRequestHeaderField_Origin,
	HTTPRequestHeaderField_Pragma,
	HTTPRequestHeaderField_Proxy_Authorization,
	HTTPRequestHeaderField_Range,
	HTTPRequestHeaderField_Referer,
	HTTPRequestHeaderField_TE,
	HTTPRequestHeaderField_User_Agent,
	HTTPRequestHeaderField_Via,
	HTTPRequestHeaderField_Warning,
	
	/// The total number of @c HTTPRequestHeaderField
	HTTPRequestHeaderFieldsCount
} HTTPRequestHeaderField;

/// Get the HTTPRequestHeaderField enum value for an HTTPRequestHeaderFieldName.
/// @param fieldName The @c HTTPRequestHeaderFieldName.
/// @return The @c HTTPRequestHeaderField enumeration value for the given field name or -1 if the conversion failed.
HTTPRequestHeaderField HTTPRequestHeaderFieldForFieldNamed(HTTPRequestHeaderFieldName fieldName);

/// Get the HTTPRequestHeaderFieldName for an HTTPRequestHeaderField.
/// @param field The @c HTTPRequestHeaderField value.
/// @return The corresponding @c HTTPRequestHeaderField value for the given field or @c NULL if the conversion failed.
const char *HTTPRequestHeaderFieldNameForField(HTTPRequestHeaderField field);

#endif
