#include "data_resources_parser.h"

namespace CrawlerEngine
{


DataResourcesParser::DataResourcesParser(ResourceType resourceType)
{
	Q_UNUSED(resourceType);
}

void DataResourcesParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(output);
	Q_UNUSED(page);
	Q_UNUSED(headers);
	// TODO: implement
	// TODO: remove the following comment
	/*
	data:[<media type>][;base64],<data>
	
	 * The scheme, data. It is followed by a colon (:).
	   An optional media type. The media type part may include one or more parameters, in the format attribute=value, 
	   separated by semicolons (;) . A common media type parameter is charset, specifying the character set of the media type, 
	   where the value is from the IANA list of character set names.[4] If one is not specified, the media type of the data 
	   URI is assumed to be text/plain;charset=US-ASCII.

	* An optional base64 extension base64, separated from the preceding part by a semicolon. When present, this indicates 
	  that the data content of the URI is binary data, encoded in ASCII format using the Base64 scheme for binary-to-text encoding. 
	  The base64 extension is distinguished from any media type parameters by virtue of not having a =value component and by coming 
	 after any media type parameters.

	* The data, separated from the preceding part by a comma (,). The data is a sequence of zero or more octets represented 
	  as characters. The comma is required in a data URI, even when the data part has zero length. The characters permitted 
	  within the data part include ASCII upper and lowercase letters, digits, and many ASCII punctuation and special characters. 
	  Note that this may include characters, such as colon, semicolon, and comma which are delimiters in the URI components 
	  preceding the data part. Other octets must be percent-encoded. If the data is Base64-encoded, then the data part may contain 
	  only valid Base64 characters.[5] Note that Base64-encoded data: URIs use the standard Base64 character set 
	 (with '+' and '/' as characters 62 and 63) rather than the so-called "URL-safe Base64" character set.

	Examples of data URIs showing most of the features are:

	data:text/vnd-example+xyz;foo=bar;base64,R0lGODdh
	
	data:text/plain;charset=UTF-8;page=21,the%20data:1234,5678

	The minimal data URI is data:,, consisting of the scheme, no media-type, and zero-length data.

	Thus, within the overall URI syntax, a data URI consists of a scheme and a path, with no authority part, query string, or fragment. 
	The optional media type, the optional base64 indicator, and the data are all parts of the URI path.
	 */
}

}
