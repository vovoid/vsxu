/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2002 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include <ctype.h>
#include "tinyxml.h"

#ifdef vsxTiXml_USE_STL
#include <sstream>
#endif

bool vsxTiXmlBase::condenseWhiteSpace = true;

void vsxTiXmlBase::PutString( const vsxTiXml_STRING& str, vsxTiXml_OSTREAM* stream )
{
  vsxTiXml_STRING buffer;
	PutString( str, &buffer );
	(*stream) << buffer;
}

void vsxTiXmlBase::PutString( const vsxTiXml_STRING& str, vsxTiXml_STRING* outString )
{
	int i=0;

	while( i<(int)str.length() )
	{
		int c = str[i];

		if (    c == '&' 
		     && i < ( (int)str.length() - 2 )
			 && str[i+1] == '#'
			 && str[i+2] == 'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			while ( i<(int)str.length() )
			{
				outString->append( str.c_str() + i, 1 );
				++i;
				if ( str[i] == ';' )
					break;
			}
		}
		else if ( c == '&' )
		{
			outString->append( entity[0].str, entity[0].strLength );
			++i;
		}
		else if ( c == '<' )
		{
			outString->append( entity[1].str, entity[1].strLength );
			++i;
		}
		else if ( c == '>' )
		{
			outString->append( entity[2].str, entity[2].strLength );
			++i;
		}
		else if ( c == '\"' )
		{
			outString->append( entity[3].str, entity[3].strLength );
			++i;
		}
		else if ( c == '\'' )
		{
			outString->append( entity[4].str, entity[4].strLength );
			++i;
		}
		else if ( c < 32 || c > 126 )
		{
			// Easy pass at non-alpha/numeric/symbol
			// 127 is the delete key. Below 32 is symbolic.
			char buf[ 32 ];
			sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
			outString->append( buf, strlen( buf ) );
			++i;
		}
		else
		{
			char realc = (char) c;
			outString->append( &realc, 1 );
			++i;
		}
	}
}


// <-- Strange class for a bug fix. Search for STL_STRING_BUG
vsxTiXmlBase::StringToBuffer::StringToBuffer( const vsxTiXml_STRING& str )
{
	buffer = new char[ str.length()+1 ];
	if ( buffer )
	{
		strcpy( buffer, str.c_str() );
	}
}


vsxTiXmlBase::StringToBuffer::~StringToBuffer()
{
	delete [] buffer;
}
// End strange bug fix. -->


vsxTiXmlNode::vsxTiXmlNode( NodeType _type )
{
	parent = 0;
	type = _type;
	firstChild = 0;
	lastChild = 0;
	prev = 0;
	next = 0;
	userData = 0;
}


vsxTiXmlNode::~vsxTiXmlNode()
{
  vsxTiXmlNode* node = firstChild;
  vsxTiXmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}	
}


void vsxTiXmlNode::Clear()
{
  vsxTiXmlNode* node = firstChild;
  vsxTiXmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}	

	firstChild = 0;
	lastChild = 0;
}


vsxTiXmlNode* vsxTiXmlNode::LinkEndChild( vsxTiXmlNode* node )
{
	node->parent = this;

	node->prev = lastChild;
	node->next = 0;

	if ( lastChild )
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}


vsxTiXmlNode* vsxTiXmlNode::InsertEndChild( const vsxTiXmlNode& addThis )
{
  vsxTiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;

	return LinkEndChild( node );
}


vsxTiXmlNode* vsxTiXmlNode::InsertBeforeChild( vsxTiXmlNode* beforeThis, const vsxTiXmlNode& addThis )
{	
	if ( !beforeThis || beforeThis->parent != this )
		return 0;

  vsxTiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->next = beforeThis;
	node->prev = beforeThis->prev;
	if ( beforeThis->prev )
	{
		beforeThis->prev->next = node;
	}
	else
	{
		assert( firstChild == beforeThis );
		firstChild = node;
	}
	beforeThis->prev = node;
	return node;
}


vsxTiXmlNode* vsxTiXmlNode::InsertAfterChild( vsxTiXmlNode* afterThis, const vsxTiXmlNode& addThis )
{
	if ( !afterThis || afterThis->parent != this )
		return 0;

  vsxTiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->prev = afterThis;
	node->next = afterThis->next;
	if ( afterThis->next )
	{
		afterThis->next->prev = node;
	}
	else
	{
		assert( lastChild == afterThis );
		lastChild = node;
	}
	afterThis->next = node;
	return node;
}


vsxTiXmlNode* vsxTiXmlNode::ReplaceChild( vsxTiXmlNode* replaceThis, const vsxTiXmlNode& withThis )
{
	if ( replaceThis->parent != this )
		return 0;

  vsxTiXmlNode* node = withThis.Clone();
	if ( !node )
		return 0;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if ( replaceThis->next )
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if ( replaceThis->prev )
		replaceThis->prev->next = node;
	else
		firstChild = node;

	delete replaceThis;
	node->parent = this;
	return node;
}


bool vsxTiXmlNode::RemoveChild( vsxTiXmlNode* removeThis )
{
	if ( removeThis->parent != this )
	{	
		assert( 0 );
		return false;
	}

	if ( removeThis->next )
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if ( removeThis->prev )
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	delete removeThis;
	return true;
}

vsxTiXmlNode* vsxTiXmlNode::FirstChild( const char * _value ) const
{
  vsxTiXmlNode* node;
	for ( node = firstChild; node; node = node->next )
	{
    if ( node->SValue() == vsxTiXml_STRING( _value ))
			return node;
	}
	return 0;
}

vsxTiXmlNode* vsxTiXmlNode::LastChild( const char * _value ) const
{
  vsxTiXmlNode* node;
	for ( node = lastChild; node; node = node->prev )
	{
    if ( node->SValue() == vsxTiXml_STRING (_value))
			return node;
	}
	return 0;
}

vsxTiXmlNode* vsxTiXmlNode::IterateChildren( vsxTiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild();
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling();
	}
}

vsxTiXmlNode* vsxTiXmlNode::IterateChildren( const char * val, vsxTiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild( val );
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling( val );
	}
}

vsxTiXmlNode* vsxTiXmlNode::NextSibling( const char * _value ) const
{
  vsxTiXmlNode* node;
	for ( node = next; node; node = node->next )
	{
    if ( node->SValue() == vsxTiXml_STRING (_value))
			return node;
	}
	return 0;
}


vsxTiXmlNode* vsxTiXmlNode::PreviousSibling( const char * _value ) const
{
  vsxTiXmlNode* node;
	for ( node = prev; node; node = node->prev )
	{
    if ( node->SValue() == vsxTiXml_STRING (_value))
			return node;
	}
	return 0;
}

void vsxTiXmlElement::RemoveAttribute( const char * name )
{
  vsxTiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
	{
		attributeSet.Remove( node );
		delete node;
	}
}

vsxTiXmlElement* vsxTiXmlNode::FirstChildElement() const
{
  vsxTiXmlNode* node;

	for (	node = FirstChild();
	node;
	node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

vsxTiXmlElement* vsxTiXmlNode::FirstChildElement( const char * _value ) const
{
  vsxTiXmlNode* node;

	for (	node = FirstChild( _value );
	node;
	node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


vsxTiXmlElement* vsxTiXmlNode::NextSiblingElement() const
{
  vsxTiXmlNode* node;

	for (	node = NextSibling();
	node;
	node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

vsxTiXmlElement* vsxTiXmlNode::NextSiblingElement( const char * _value ) const
{
  vsxTiXmlNode* node;

	for (	node = NextSibling( _value );
	node;
	node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}



vsxTiXmlDocument* vsxTiXmlNode::GetDocument() const
{
  const vsxTiXmlNode* node;

	for( node = this; node; node = node->parent )
	{
		if ( node->ToDocument() )
			return node->ToDocument();
	}
	return 0;
}


vsxTiXmlElement::vsxTiXmlElement (const char * _value)
: vsxTiXmlNode( vsxTiXmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	value = _value;
}

vsxTiXmlElement::~vsxTiXmlElement()
{
	while( attributeSet.First() )
	{
    vsxTiXmlAttribute* node = attributeSet.First();
		attributeSet.Remove( node );
		delete node;
	}
}

const char * vsxTiXmlElement::Attribute( const char * name ) const
{
  vsxTiXmlAttribute* node = attributeSet.Find( name );

	if ( node )
		return node->Value();

	return 0;
}


const char * vsxTiXmlElement::Attribute( const char * name, int* i ) const
{
	const char * s = Attribute( name );
	if ( i )
	{
		if ( s )
			*i = atoi( s );
		else
			*i = 0;
	}
	return s;
}


const char * vsxTiXmlElement::Attribute( const char * name, double* d ) const
{
	const char * s = Attribute( name );
	if ( d )
	{
		if ( s )
			*d = atof( s );
		else
			*d = 0;
	}
	return s;
}


int vsxTiXmlElement::QueryIntAttribute( const char* name, int* ival ) const
{
  vsxTiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
    return vsxTiXml_NO_ATTRIBUTE;

	return node->QueryIntValue( ival );
}


int vsxTiXmlElement::QueryDoubleAttribute( const char* name, double* dval ) const
{
  vsxTiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
    return vsxTiXml_NO_ATTRIBUTE;

	return node->QueryDoubleValue( dval );
}


void vsxTiXmlElement::SetAttribute( const char * name, int val )
{	
	char buf[64];
	sprintf( buf, "%d", val );
	SetAttribute( name, buf );
}


void vsxTiXmlElement::SetAttribute( const char * name, const char * _value )
{
  vsxTiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
	{
		node->SetValue( _value );
		return;
	}

  vsxTiXmlAttribute* attrib = new vsxTiXmlAttribute( name, _value );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
    vsxTiXmlDocument* document = GetDocument();
    if ( document ) document->SetError( vsxTiXml_ERROR_OUT_OF_MEMORY, 0, 0 );
	}
}

void vsxTiXmlElement::Print( FILE* cfile, int depth ) const
{
	int i;
	for ( i=0; i<depth; i++ )
	{
		fprintf( cfile, "    " );
	}

	fprintf( cfile, "<%s", value.c_str() );

  vsxTiXmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{
		fprintf( cfile, " " );
		attrib->Print( cfile, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
  vsxTiXmlNode* node;
	if ( !firstChild )
	{
		fprintf( cfile, " />" );
	}
	else if ( firstChild == lastChild && firstChild->ToText() )
	{
		fprintf( cfile, ">" );
		firstChild->Print( cfile, depth + 1 );
		fprintf( cfile, "</%s>", value.c_str() );
	}
	else
	{
		fprintf( cfile, ">" );

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			if ( !node->ToText() )
			{
				fprintf( cfile, "\n" );
			}
			node->Print( cfile, depth+1 );
		}
		fprintf( cfile, "\n" );
		for( i=0; i<depth; ++i )
		fprintf( cfile, "    " );
		fprintf( cfile, "</%s>", value.c_str() );
	}
}

void vsxTiXmlElement::StreamOut( vsxTiXml_OSTREAM * stream ) const
{
	(*stream) << "<" << value;

  vsxTiXmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{	
		(*stream) << " ";
		attrib->StreamOut( stream );
	}

	// If this node has children, give it a closing tag. Else
	// make it an empty tag.
  vsxTiXmlNode* node;
	if ( firstChild )
	{ 		
		(*stream) << ">";

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			node->StreamOut( stream );
		}
		(*stream) << "</" << value << ">";
	}
	else
	{
		(*stream) << " />";
	}
}

vsxTiXmlNode* vsxTiXmlElement::Clone() const
{
  vsxTiXmlElement* clone = new vsxTiXmlElement( Value() );
	if ( !clone )
		return 0;

	CopyToClone( clone );

	// Clone the attributes, then clone the children.
  vsxTiXmlAttribute* attribute = 0;
	for(	attribute = attributeSet.First();
	attribute;
	attribute = attribute->Next() )
	{
		clone->SetAttribute( attribute->Name(), attribute->Value() );
	}

  vsxTiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		clone->LinkEndChild( node->Clone() );
	}
	return clone;
}


vsxTiXmlDocument::vsxTiXmlDocument() : vsxTiXmlNode( vsxTiXmlNode::DOCUMENT )
{
	tabsize = 4;
	ClearError();
}

vsxTiXmlDocument::vsxTiXmlDocument( const char * documentName ) : vsxTiXmlNode( vsxTiXmlNode::DOCUMENT )
{
	tabsize = 4;
	value = documentName;
	ClearError();
}

bool vsxTiXmlDocument::LoadFile()
{
	// See STL_STRING_BUG below.
	StringToBuffer buf( value );

	if ( buf.buffer && LoadFile( buf.buffer ) )
		return true;

	return false;
}


bool vsxTiXmlDocument::SaveFile() const
{
	// See STL_STRING_BUG below.
	StringToBuffer buf( value );

	if ( buf.buffer && SaveFile( buf.buffer ) )
		return true;

	return false;
}

bool vsxTiXmlDocument::LoadFile( const char* filename )
{
	// Delete the existing data:
	Clear();
	location.Clear();

	// There was a really terrifying little bug here. The code:
	//		value = filename
	// in the STL case, cause the assignment method of the std::string to
	// be called. What is strange, is that the std::string had the same
	// address as it's c_str() method, and so bad things happen. Looks
	// like a bug in the Microsoft STL implementation.
	// See STL_STRING_BUG above.
	// Fixed with the StringToBuffer class.
	value = filename;

	FILE* file = fopen( value.c_str (), "r" );

	if ( file )
	{
		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		long length = 0;
		fseek( file, 0, SEEK_END );
		length = ftell( file );
		fseek( file, 0, SEEK_SET );

		// Strange case, but good to handle up front.
		if ( length == 0 )
		{
			fclose( file );
			return false;
		}

		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.
    vsxTiXml_STRING data;
		data.reserve( length );

		const int BUF_SIZE = 2048;
		char buf[BUF_SIZE];

		while( fgets( buf, BUF_SIZE, file ) )
		{
			data += buf;
		}
		fclose( file );

		Parse( data.c_str(), 0 );

		if (  Error() )
            return false;
        else
			return true;
	}
  SetError( vsxTiXml_ERROR_OPENING_FILE, 0, 0 );
	return false;
}

bool vsxTiXmlDocument::SaveFile( const char * filename ) const
{
	// The old c stuff lives on...
	FILE* fp = fopen( filename, "w" );
	if ( fp )
	{
		Print( fp, 0 );
		fclose( fp );
		return true;
	}
	return false;
}


vsxTiXmlNode* vsxTiXmlDocument::Clone() const
{
  vsxTiXmlDocument* clone = new vsxTiXmlDocument();
	if ( !clone )
		return 0;

	CopyToClone( clone );
	clone->error = error;
	clone->errorDesc = errorDesc.c_str ();

  vsxTiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		clone->LinkEndChild( node->Clone() );
	}
	return clone;
}


void vsxTiXmlDocument::Print( FILE* cfile, int depth ) const
{
  vsxTiXmlNode* node;
	for ( node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( cfile, depth );
		fprintf( cfile, "\n" );
	}
}

void vsxTiXmlDocument::StreamOut( vsxTiXml_OSTREAM * out ) const
{
  vsxTiXmlNode* node;
	for ( node=FirstChild(); node; node=node->NextSibling() )
	{
		node->StreamOut( out );

		// Special rule for streams: stop after the root element.
		// The stream in code will only read one element, so don't
		// write more than one.
		if ( node->ToElement() )
			break;
	}
}


vsxTiXmlAttribute* vsxTiXmlAttribute::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}


vsxTiXmlAttribute* vsxTiXmlAttribute::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}


void vsxTiXmlAttribute::Print( FILE* cfile, int /*depth*/ ) const
{
  vsxTiXml_STRING n, v;

	PutString( Name(), &n );
	PutString( Value(), &v );

  if (value.find ('\"') == vsxTiXml_STRING::npos)
		fprintf (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
	else
		fprintf (cfile, "%s='%s'", n.c_str(), v.c_str() );
}


void vsxTiXmlAttribute::StreamOut( vsxTiXml_OSTREAM * stream ) const
{
  if (value.find( '\"' ) != vsxTiXml_STRING::npos)
	{
		PutString( name, stream );
		(*stream) << "=" << "'";
		PutString( value, stream );
		(*stream) << "'";
	}
	else
	{
		PutString( name, stream );
		(*stream) << "=" << "\"";
		PutString( value, stream );
		(*stream) << "\"";
	}
}

int vsxTiXmlAttribute::QueryIntValue( int* ival ) const
{
	if ( sscanf( value.c_str(), "%d", ival ) == 1 )
    return vsxTiXml_SUCCESS;
  return vsxTiXml_WRONG_TYPE;
}

int vsxTiXmlAttribute::QueryDoubleValue( double* dval ) const
{
	if ( sscanf( value.c_str(), "%lf", dval ) == 1 )
    return vsxTiXml_SUCCESS;
  return vsxTiXml_WRONG_TYPE;
}

void vsxTiXmlAttribute::SetIntValue( int _value )
{
	char buf [64];
	sprintf (buf, "%d", _value);
	SetValue (buf);
}

void vsxTiXmlAttribute::SetDoubleValue( double _value )
{
	char buf [64];
	sprintf (buf, "%lf", _value);
	SetValue (buf);
}

int vsxTiXmlAttribute::IntValue()
{
	return atoi (value.c_str ());
}

double  vsxTiXmlAttribute::DoubleValue()
{
	return atof (value.c_str ());
}

void vsxTiXmlComment::Print( FILE* cfile, int depth ) const
{
	for ( int i=0; i<depth; i++ )
	{
		fputs( "    ", cfile );
	}
	fprintf( cfile, "<!--%s-->", value.c_str() );
}

void vsxTiXmlComment::StreamOut( vsxTiXml_OSTREAM * stream ) const
{
	(*stream) << "<!--";
	PutString( value, stream );
	(*stream) << "-->";
}

vsxTiXmlNode* vsxTiXmlComment::Clone() const
{
  vsxTiXmlComment* clone = new vsxTiXmlComment();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


void vsxTiXmlText::Print( FILE* cfile, int /*depth*/ ) const
{
  vsxTiXml_STRING buffer;
	PutString( value, &buffer );
	fprintf( cfile, "%s", buffer.c_str() );
}


void vsxTiXmlText::StreamOut( vsxTiXml_OSTREAM * stream ) const
{
	PutString( value, stream );
}


vsxTiXmlNode* vsxTiXmlText::Clone() const
{	
  vsxTiXmlText* clone = 0;
  clone = new vsxTiXmlText( "" );

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


vsxTiXmlDeclaration::vsxTiXmlDeclaration( const char * _version,
	const char * _encoding,
	const char * _standalone )
: vsxTiXmlNode( vsxTiXmlNode::DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


void vsxTiXmlDeclaration::Print( FILE* cfile, int /*depth*/ ) const
{
	fprintf (cfile, "<?xml ");

	if ( !version.empty() )
		fprintf (cfile, "version=\"%s\" ", version.c_str ());
	if ( !encoding.empty() )
		fprintf (cfile, "encoding=\"%s\" ", encoding.c_str ());
	if ( !standalone.empty() )
		fprintf (cfile, "standalone=\"%s\" ", standalone.c_str ());
	fprintf (cfile, "?>");
}

void vsxTiXmlDeclaration::StreamOut( vsxTiXml_OSTREAM * stream ) const
{
	(*stream) << "<?xml ";

	if ( !version.empty() )
	{
		(*stream) << "version=\"";
		PutString( version, stream );
		(*stream) << "\" ";
	}
	if ( !encoding.empty() )
	{
		(*stream) << "encoding=\"";
		PutString( encoding, stream );
		(*stream ) << "\" ";
	}
	if ( !standalone.empty() )
	{
		(*stream) << "standalone=\"";
		PutString( standalone, stream );
		(*stream) << "\" ";
	}
	(*stream) << "?>";
}

vsxTiXmlNode* vsxTiXmlDeclaration::Clone() const
{	
  vsxTiXmlDeclaration* clone = new vsxTiXmlDeclaration();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	clone->version = version;
	clone->encoding = encoding;
	clone->standalone = standalone;
	return clone;
}


void vsxTiXmlUnknown::Print( FILE* cfile, int depth ) const
{
	for ( int i=0; i<depth; i++ )
		fprintf( cfile, "    " );
	fprintf( cfile, "%s", value.c_str() );
}

void vsxTiXmlUnknown::StreamOut( vsxTiXml_OSTREAM * stream ) const
{
	(*stream) << "<" << value << ">";		// Don't use entities hear! It is unknown.
}

vsxTiXmlNode* vsxTiXmlUnknown::Clone() const
{
  vsxTiXmlUnknown* clone = new vsxTiXmlUnknown();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


vsxTiXmlAttributeSet::vsxTiXmlAttributeSet()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


vsxTiXmlAttributeSet::~vsxTiXmlAttributeSet()
{
	assert( sentinel.next == &sentinel );
	assert( sentinel.prev == &sentinel );
}


void vsxTiXmlAttributeSet::Add( vsxTiXmlAttribute* addMe )
{
	assert( !Find( addMe->Name() ) );	// Shouldn't be multiply adding to the set.

	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev      = addMe;
}

void vsxTiXmlAttributeSet::Remove( vsxTiXmlAttribute* removeMe )
{
  vsxTiXmlAttribute* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node == removeMe )
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = 0;
			node->prev = 0;
			return;
		}
	}
	assert( 0 );		// we tried to remove a non-linked attribute.
}

vsxTiXmlAttribute*	vsxTiXmlAttributeSet::Find( const char * name ) const
{
  vsxTiXmlAttribute* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}


#ifdef vsxTiXml_USE_STL
vsxTiXml_ISTREAM & operator >> (vsxTiXml_ISTREAM & in, vsxTiXmlNode & base)
{
  vsxTiXml_STRING tag;
	tag.reserve( 8 * 1000 );
	base.StreamIn( &in, &tag );

	base.Parse( tag.c_str(), 0 );
	return in;
}
#endif


vsxTiXml_OSTREAM & operator<< (vsxTiXml_OSTREAM & out, const vsxTiXmlNode & base)
{
	base.StreamOut (& out);
	return out;
}


#ifdef vsxTiXml_USE_STL
std::string & operator<< (std::string& out, const vsxTiXmlNode& base )
{
   std::ostringstream os_stream( std::ostringstream::out );
   base.StreamOut( &os_stream );
   
   out.append( os_stream.str() );
   return out;
}
#endif


vsxTiXmlHandle vsxTiXmlHandle::FirstChild() const
{
	if ( node )
	{
    vsxTiXmlNode* child = node->FirstChild();
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}


vsxTiXmlHandle vsxTiXmlHandle::FirstChild( const char * value ) const
{
	if ( node )
	{
    vsxTiXmlNode* child = node->FirstChild( value );
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}


vsxTiXmlHandle vsxTiXmlHandle::FirstChildElement() const
{
	if ( node )
	{
    vsxTiXmlElement* child = node->FirstChildElement();
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}


vsxTiXmlHandle vsxTiXmlHandle::FirstChildElement( const char * value ) const
{
	if ( node )
	{
    vsxTiXmlElement* child = node->FirstChildElement( value );
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}

vsxTiXmlHandle vsxTiXmlHandle::Child( int count ) const
{
	if ( node )
	{
		int i;
    vsxTiXmlNode* child = node->FirstChild();
		for (	i=0;
				child && i<count;
				child = child->NextSibling(), ++i )
		{
			// nothing
		}
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}


vsxTiXmlHandle vsxTiXmlHandle::Child( const char* value, int count ) const
{
	if ( node )
	{
		int i;
    vsxTiXmlNode* child = node->FirstChild( value );
		for (	i=0;
				child && i<count;
				child = child->NextSibling( value ), ++i )
		{
			// nothing
		}
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}


vsxTiXmlHandle vsxTiXmlHandle::ChildElement( int count ) const
{
	if ( node )
	{
		int i;
    vsxTiXmlElement* child = node->FirstChildElement();
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement(), ++i )
		{
			// nothing
		}
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}


vsxTiXmlHandle vsxTiXmlHandle::ChildElement( const char* value, int count ) const
{
	if ( node )
	{
		int i;
    vsxTiXmlElement* child = node->FirstChildElement( value );
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement( value ), ++i )
		{
			// nothing
		}
		if ( child )
      return vsxTiXmlHandle( child );
	}
  return vsxTiXmlHandle( 0 );
}
