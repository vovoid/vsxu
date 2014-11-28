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


#ifndef TINYXML_INCLUDED
#define TINYXML_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cal3d/platform.h"

// Help out windows:
#if defined( _DEBUG ) && !defined( DEBUG )
#define DEBUG
#endif

#if defined( DEBUG ) && defined( _MSC_VER )
#include <windows.h>
#define vsxTiXml_LOG OutputDebugString
#else
#define vsxTiXml_LOG printf
#endif

#define vsxTiXml_USE_STL

#ifdef vsxTiXml_USE_STL
	#include <string>
 	#include <iostream>
    //#include <ostream>
  #define vsxTiXml_STRING	std::string
  #define vsxTiXml_ISTREAM	std::istream
  #define vsxTiXml_OSTREAM	std::ostream
#else
	#include "tinystr.h"
  #define vsxTiXml_STRING	vsxTiXmlString
  #define vsxTiXml_OSTREAM	vsxTiXmlOutStream
#endif

class vsxTiXmlDocument;
class vsxTiXmlElement;
class vsxTiXmlComment;
class vsxTiXmlUnknown;
class vsxTiXmlAttribute;
class vsxTiXmlText;
class vsxTiXmlDeclaration;

class vsxTiXmlParsingData;

/*	Internal structure for tracking location of items 
	in the XML file.
*/
struct CAL3D_API vsxTiXmlCursor
{
  vsxTiXmlCursor()		{ Clear(); }
	void Clear()		{ row = col = -1; }

	int row;	// 0 based.
	int col;	// 0 based.
};


// Only used by Attribute::Query functions
enum 
{ 
  vsxTiXml_SUCCESS,
  vsxTiXml_NO_ATTRIBUTE,
  vsxTiXml_WRONG_TYPE
};

/** vsxTiXmlBase is a base class for every class in TinyXml.
	It does little except to establish that TinyXml classes
	can be printed and provide some utility functions.

	In XML, the document and elements can contain
	other elements and other types of nodes.

	@verbatim
	A Document can contain:	Element	(container or leaf)
							Comment (leaf)
							Unknown (leaf)
							Declaration( leaf )

	An Element can contain:	Element (container or leaf)
							Text	(leaf)
							Attributes (not on tree)
							Comment (leaf)
							Unknown (leaf)

	A Decleration contains: Attributes (not on tree)
	@endverbatim
*/
class CAL3D_API vsxTiXmlBase
{
  friend class vsxTiXmlNode;
  friend class vsxTiXmlElement;
  friend class vsxTiXmlDocument;

public:
  vsxTiXmlBase()								{}
  virtual ~vsxTiXmlBase()					{}

	/**	All TinyXml classes can print themselves to a filestream.
		This is a formatted print, and will insert tabs and newlines.
		
		(For an unformatted stream, use the << operator.)
	*/
	virtual void Print( FILE* cfile, int depth ) const = 0;

	/**	The world does not agree on whether white space should be kept or
		not. In order to make everyone happy, these global, static functions
		are provided to set whether or not TinyXml will condense all white space
		into a single space or not. The default is to condense. Note changing this
		values is not thread safe.
	*/
	static void SetCondenseWhiteSpace( bool condense )		{ condenseWhiteSpace = condense; }

	/// Return the current white space setting.
	static bool IsWhiteSpaceCondensed()						{ return condenseWhiteSpace; }

	/** Return the position, in the original source file, of this node or attribute.
		The row and column are 1-based. (That is the first row and first column is
		1,1). If the returns values are 0 or less, then the parser does not have
		a row and column value.

    Generally, the row and column value will be set when the vsxTiXmlDocument::Load(),
    vsxTiXmlDocument::LoadFile(), or any vsxTiXmlNode::Parse() is called. It will NOT be set
		when the DOM was created from operator>>.

		The values reflect the initial load. Once the DOM is modified programmatically
		(by adding or changing nodes and attributes) the new values will NOT update to
		reflect changes in the document.

		There is a minor performance cost to computing the row and column. Computation
    can be disabled if vsxTiXmlDocument::SetTabSize() is called with 0 as the value.

    @sa vsxTiXmlDocument::SetTabSize()
	*/
	int Row() const			{ return location.row + 1; }
	int Column() const		{ return location.col + 1; }	///< See Row()

protected:
	// See STL_STRING_BUG
	// Utility class to overcome a bug.
	class StringToBuffer
	{
	  public:
    StringToBuffer( const vsxTiXml_STRING& str );
		~StringToBuffer();
		char* buffer;
	};

	static const char*	SkipWhiteSpace( const char* );
	inline static bool	IsWhiteSpace( int c )		{ return ( isspace( c ) || c == '\n' || c == '\r' ); }

  virtual void StreamOut (vsxTiXml_OSTREAM *) const = 0;

  #ifdef vsxTiXml_USE_STL
      static bool	StreamWhiteSpace( vsxTiXml_ISTREAM * in, vsxTiXml_STRING * tag );
      static bool StreamTo( vsxTiXml_ISTREAM * in, int character, vsxTiXml_STRING * tag );
	#endif

	/*	Reads an XML name into the string provided. Returns
		a pointer just past the last character of the name,
		or 0 if the function has an error.
	*/
  static const char* ReadName( const char* p, vsxTiXml_STRING* name );

	/*	Reads text. Returns a pointer past the given end tag.
		Wickedly complex options, but it keeps the (sensitive) code in one place.
	*/
	static const char* ReadText(	const char* in,				// where to start
                  vsxTiXml_STRING* text,			// the string read
									bool ignoreWhiteSpace,		// whether to keep the white space
									const char* endTag,			// what ends this text
									bool ignoreCase );			// whether to ignore case in the end tag

  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data ) = 0;

	// If an entity has been found, transform it into a character.
	static const char* GetEntity( const char* in, char* value );

	// Get a character, while interpreting entities.
	inline static const char* GetChar( const char* p, char* _value )
	{
		assert( p );
		if ( *p == '&' )
		{
			return GetEntity( p, _value );
		}
		else
		{
			*_value = *p;
			return p+1;
		}
	}

	// Puts a string to a stream, expanding entities as it goes.
	// Note this should not contian the '<', '>', etc, or they will be transformed into entities!
  static void PutString( const vsxTiXml_STRING& str, vsxTiXml_OSTREAM* out );

  static void PutString( const vsxTiXml_STRING& str, vsxTiXml_STRING* out );

	// Return true if the next characters in the stream are any of the endTag sequences.
	static bool StringEqual(	const char* p,
								const char* endTag,
								bool ignoreCase );


	enum
	{
    vsxTiXml_NO_ERROR = 0,
    vsxTiXml_ERROR,
    vsxTiXml_ERROR_OPENING_FILE,
    vsxTiXml_ERROR_OUT_OF_MEMORY,
    vsxTiXml_ERROR_PARSING_ELEMENT,
    vsxTiXml_ERROR_FAILED_TO_READ_ELEMENT_NAME,
    vsxTiXml_ERROR_READING_ELEMENT_VALUE,
    vsxTiXml_ERROR_READING_ATTRIBUTES,
    vsxTiXml_ERROR_PARSING_EMPTY,
    vsxTiXml_ERROR_READING_END_TAG,
    vsxTiXml_ERROR_PARSING_UNKNOWN,
    vsxTiXml_ERROR_PARSING_COMMENT,
    vsxTiXml_ERROR_PARSING_DECLARATION,
    vsxTiXml_ERROR_DOCUMENT_EMPTY,

    vsxTiXml_ERROR_STRING_COUNT
	};
  static const char* errorString[ vsxTiXml_ERROR_STRING_COUNT ];

  vsxTiXmlCursor location;

private:
	struct Entity
	{
		const char*     str;
		unsigned int	strLength;
		char		    chr;
	};
	enum
	{
		NUM_ENTITY = 5,
		MAX_ENTITY_LENGTH = 6

	};
	static Entity entity[ NUM_ENTITY ];
	static bool condenseWhiteSpace;
};


/** The parent class for everything in the Document Object Model.
	(Except for attributes).
	Nodes have siblings, a parent, and children. A node can be
  in a document, or stand on its own. The type of a vsxTiXmlNode
	can be queried, and it can be cast to its more defined type.
*/
class CAL3D_API vsxTiXmlNode : public vsxTiXmlBase
{
  friend class vsxTiXmlDocument;
  friend class vsxTiXmlElement;

public:
  #ifdef vsxTiXml_USE_STL

	    /** An input stream operator, for every class. Tolerant of newlines and
		    formatting, but doesn't expect them.
	    */
      friend std::istream& operator >> (std::istream& in, vsxTiXmlNode& base);

	    /** An output stream operator, for every class. Note that this outputs
		    without any newlines or formatting, as opposed to Print(), which
		    includes tabs and new lines.

		    The operator<< and operator>> are not completely symmetric. Writing
		    a node to a stream is very well defined. You'll get a nice stream
		    of output, without any extra whitespace or newlines.
		    
		    But reading is not as well defined. (As it always is.) If you create
        a vsxTiXmlElement (for example) and read that from an input stream,
		    the text needs to define an element or junk will result. This is
		    true of all input streams, but it's worth keeping in mind.

        A vsxTiXmlDocument will read nodes until it reads a root element, and
			all the children of that root element.
	    */	
      friend std::ostream& operator<< (std::ostream& out, const vsxTiXmlNode& base);

		/// Appends the XML node or attribute to a std::string.
    friend std::string& operator<< (std::string& out, const vsxTiXmlNode& base );

	#else
	    // Used internally, not part of the public API.
      friend vsxTiXml_OSTREAM& operator<< (vsxTiXml_OSTREAM& out, const vsxTiXmlNode& base);
	#endif

	/** The types of XML nodes supported by TinyXml. (All the
			unsupported types are picked up by UNKNOWN.)
	*/
	enum NodeType
	{
		DOCUMENT,
		ELEMENT,
		COMMENT,
		UNKNOWN,
		TEXT,
		DECLARATION,
		TYPECOUNT
	};

  virtual ~vsxTiXmlNode();

	/** The meaning of 'value' changes for the specific type of
    vsxTiXmlNode.
		@verbatim
		Document:	filename of the xml file
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text string
		@endverbatim

		The subclasses will wrap this function.
	*/
	const char * Value() const { return value.c_str (); }

	/** Changes the value of the node. Defined as:
		@verbatim
		Document:	filename of the xml file
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text string
		@endverbatim
	*/
	void SetValue(const char * _value) { value = _value;}

    #ifdef vsxTiXml_USE_STL
	/// STL std::string form.
	void SetValue( const std::string& _value )    
	{	  
		StringToBuffer buf( _value );
		SetValue( buf.buffer ? buf.buffer : "" );    	
	}	
	#endif

	/// Delete all the children of this node. Does not affect 'this'.
	void Clear();

	/// One step up the DOM.
  vsxTiXmlNode* Parent() const					{ return parent; }

  vsxTiXmlNode* FirstChild()	const	{ return firstChild; }		///< The first child of this node. Will be null if there are no children.
  vsxTiXmlNode* FirstChild( const char * value ) const;			///< The first child of this node with the matching 'value'. Will be null if none found.

  vsxTiXmlNode* LastChild() const	{ return lastChild; }		/// The last child of this node. Will be null if there are no children.
  vsxTiXmlNode* LastChild( const char * value ) const;			/// The last child of this node matching 'value'. Will be null if there are no children.

    #ifdef vsxTiXml_USE_STL
  vsxTiXmlNode* FirstChild( const std::string& _value ) const	{	return FirstChild (_value.c_str ());	}	///< STL std::string form.
  vsxTiXmlNode* LastChild( const std::string& _value ) const		{	return LastChild (_value.c_str ());	}	///< STL std::string form.
	#endif

	/** An alternate way to walk the children of a node.
		One way to iterate over nodes is:
		@verbatim
			for( child = parent->FirstChild(); child; child = child->NextSibling() )
		@endverbatim

		IterateChildren does the same thing with the syntax:
		@verbatim
			child = 0;
			while( child = parent->IterateChildren( child ) )
		@endverbatim

		IterateChildren takes the previous child as input and finds
		the next one. If the previous child is null, it returns the
		first. IterateChildren will return null when done.
	*/
  vsxTiXmlNode* IterateChildren( vsxTiXmlNode* previous ) const;

	/// This flavor of IterateChildren searches for children with a particular 'value'
  vsxTiXmlNode* IterateChildren( const char * value, vsxTiXmlNode* previous ) const;

    #ifdef vsxTiXml_USE_STL
  vsxTiXmlNode* IterateChildren( const std::string& _value, vsxTiXmlNode* previous ) const	{	return IterateChildren (_value.c_str (), previous);	}	///< STL std::string form.
	#endif

	/** Add a new node related to this. Adds a child past the LastChild.
		Returns a pointer to the new object or NULL if an error occured.
	*/
  vsxTiXmlNode* InsertEndChild( const vsxTiXmlNode& addThis );


	/** Add a new node related to this. Adds a child past the LastChild.

		NOTE: the node to be added is passed by pointer, and will be
		henceforth owned (and deleted) by tinyXml. This method is efficient
		and avoids an extra copy, but should be used with care as it
		uses a different memory model than the other insert functions.

		@sa InsertEndChild
	*/
  vsxTiXmlNode* LinkEndChild( vsxTiXmlNode* addThis );

	/** Add a new node related to this. Adds a child before the specified child.
		Returns a pointer to the new object or NULL if an error occured.
	*/
  vsxTiXmlNode* InsertBeforeChild( vsxTiXmlNode* beforeThis, const vsxTiXmlNode& addThis );

	/** Add a new node related to this. Adds a child after the specified child.
		Returns a pointer to the new object or NULL if an error occured.
	*/
  vsxTiXmlNode* InsertAfterChild(  vsxTiXmlNode* afterThis, const vsxTiXmlNode& addThis );

	/** Replace a child of this node.
		Returns a pointer to the new object or NULL if an error occured.
	*/
  vsxTiXmlNode* ReplaceChild( vsxTiXmlNode* replaceThis, const vsxTiXmlNode& withThis );

	/// Delete a child of this node.
  bool RemoveChild( vsxTiXmlNode* removeThis );

	/// Navigate to a sibling node.
  vsxTiXmlNode* PreviousSibling() const			{ return prev; }

	/// Navigate to a sibling node.
  vsxTiXmlNode* PreviousSibling( const char * ) const;

    #ifdef vsxTiXml_USE_STL
  vsxTiXmlNode* PreviousSibling( const std::string& _value ) const	{	return PreviousSibling (_value.c_str ());	}	///< STL std::string form.
  vsxTiXmlNode* NextSibling( const std::string& _value) const		{	return NextSibling (_value.c_str ());	}	///< STL std::string form.
	#endif

	/// Navigate to a sibling node.
  vsxTiXmlNode* NextSibling() const				{ return next; }

	/// Navigate to a sibling node with the given 'value'.
  vsxTiXmlNode* NextSibling( const char * ) const;

	/** Convenience function to get through elements.
		Calls NextSibling and ToElement. Will skip all non-Element
		nodes. Returns 0 if there is not another element.
	*/
  vsxTiXmlElement* NextSiblingElement() const;

	/** Convenience function to get through elements.
		Calls NextSibling and ToElement. Will skip all non-Element
		nodes. Returns 0 if there is not another element.
	*/
  vsxTiXmlElement* NextSiblingElement( const char * ) const;

    #ifdef vsxTiXml_USE_STL
  vsxTiXmlElement* NextSiblingElement( const std::string& _value) const	{	return NextSiblingElement (_value.c_str ());	}	///< STL std::string form.
	#endif

	/// Convenience function to get through elements.
  vsxTiXmlElement* FirstChildElement()	const;

	/// Convenience function to get through elements.
  vsxTiXmlElement* FirstChildElement( const char * value ) const;

    #ifdef vsxTiXml_USE_STL
  vsxTiXmlElement* FirstChildElement( const std::string& _value ) const	{	return FirstChildElement (_value.c_str ());	}	///< STL std::string form.
	#endif

	/** Query the type (as an enumerated value, above) of this node.
		The possible types are: DOCUMENT, ELEMENT, COMMENT,
								UNKNOWN, TEXT, and DECLARATION.
	*/
	virtual int Type() const	{ return type; }

	/** Return a pointer to the Document this node lives in.
		Returns null if not in a document.
	*/
  vsxTiXmlDocument* GetDocument() const;

	/// Returns true if this node has no children.
	bool NoChildren() const						{ return !firstChild; }

  vsxTiXmlDocument* ToDocument()	const		{ return ( this && type == DOCUMENT ) ? (vsxTiXmlDocument*) this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
  vsxTiXmlElement*  ToElement() const		{ return ( this && type == ELEMENT  ) ? (vsxTiXmlElement*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
  vsxTiXmlComment*  ToComment() const		{ return ( this && type == COMMENT  ) ? (vsxTiXmlComment*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
  vsxTiXmlUnknown*  ToUnknown() const		{ return ( this && type == UNKNOWN  ) ? (vsxTiXmlUnknown*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
  vsxTiXmlText*	   ToText()    const		{ return ( this && type == TEXT     ) ? (vsxTiXmlText*)     this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
  vsxTiXmlDeclaration* ToDeclaration() const	{ return ( this && type == DECLARATION ) ? (vsxTiXmlDeclaration*) this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.

  virtual vsxTiXmlNode* Clone() const = 0;

	void  SetUserData( void* user )			{ userData = user; }
	void* GetUserData()						{ return userData; }

protected:
  vsxTiXmlNode( NodeType type );

  #ifdef vsxTiXml_USE_STL
	    // The real work of the input operator.
      virtual void StreamIn( vsxTiXml_ISTREAM* in, vsxTiXml_STRING* tag ) = 0;
	#endif

	// Figure out what is at *p, and parse it. Returns null if it is not an xml node.
  vsxTiXmlNode* Identify( const char* start );
  void CopyToClone( vsxTiXmlNode* target ) const	{ target->SetValue (value.c_str() );
												  target->userData = userData; }

  // Internal Value function returning a vsxTiXml_STRING
  vsxTiXml_STRING SValue() const	{ return value ; }

  vsxTiXmlNode*		parent;
	NodeType		type;

  vsxTiXmlNode*		firstChild;
  vsxTiXmlNode*		lastChild;

  vsxTiXml_STRING	value;

  vsxTiXmlNode*		prev;
  vsxTiXmlNode*		next;
	void*			userData;
};


/** An attribute is a name-value pair. Elements have an arbitrary
	number of attributes, each with a unique name.

  @note The attributes are not vsxTiXmlNodes, since they are not
		  part of the tinyXML document object model. There are other
		  suggested ways to look at this problem.
*/
class CAL3D_API vsxTiXmlAttribute : public vsxTiXmlBase
{
  friend class vsxTiXmlAttributeSet;

public:
	/// Construct an empty attribute.
  vsxTiXmlAttribute()
	{
		document = 0;
		prev = next = 0;
	}

  #ifdef vsxTiXml_USE_STL
	/// std::string constructor.
  vsxTiXmlAttribute( const std::string& _name, const std::string& _value )
	{
		name = _name;
		value = _value;
		document = 0;
		prev = next = 0;
	}
	#endif

	/// Construct an attribute with a name and value.
  vsxTiXmlAttribute( const char * _name, const char * _value )
	{
		name = _name;
		value = _value;
		document = 0;
		prev = next = 0;
  }

  const char*		Name()  const		{ return name.c_str (); }		///< Return the name of this attribute.
	const char*		Value() const		{ return value.c_str (); }		///< Return the value of this attribute.
  int       IntValue();									///< Return the value of this attribute, converted to an integer.
  double	DoubleValue();								///< Return the value of this attribute, converted to a double.

	/** QueryIntValue examines the value string. It is an alternative to the
		IntValue() method with richer error checking.
		If the value is an integer, it is stored in 'value' and 
    the call returns vsxTiXml_SUCCESS. If it is not
    an integer, it returns vsxTiXml_WRONG_TYPE.

		A specialized but useful call. Note that for success it returns 0,
		which is the opposite of almost all other TinyXml calls.
	*/
	int QueryIntValue( int* value ) const;
	/// QueryDoubleValue examines the value string. See QueryIntValue().
	int QueryDoubleValue( double* value ) const;

	void SetName( const char* _name )	{ name = _name; }				///< Set the name of this attribute.
	void SetValue( const char* _value )	{ value = _value; }				///< Set the value.

	void SetIntValue( int value );										///< Set the value from an integer.
	void SetDoubleValue( double value );								///< Set the value from a double.

    #ifdef vsxTiXml_USE_STL
	/// STL std::string form.
	void SetName( const std::string& _name )	
	{	
		StringToBuffer buf( _name );
		SetName ( buf.buffer ? buf.buffer : "error" );	
	}
	/// STL std::string form.	
	void SetValue( const std::string& _value )	
	{	
		StringToBuffer buf( _value );
		SetValue( buf.buffer ? buf.buffer : "error" );	
	}
	#endif

	/// Get the next sibling attribute in the DOM. Returns null at end.
  vsxTiXmlAttribute* Next() const;
	/// Get the previous sibling attribute in the DOM. Returns null at beginning.
  vsxTiXmlAttribute* Previous() const;

  bool operator==( const vsxTiXmlAttribute& rhs ) const { return rhs.name == name; }
  bool operator<( const vsxTiXmlAttribute& rhs )	 const { return name < rhs.name; }
  bool operator>( const vsxTiXmlAttribute& rhs )  const { return name > rhs.name; }

	/*	[internal use]
		Attribtue parsing starts: first letter of the name
						 returns: the next char after the value end quote
	*/
  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data );

	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;

  virtual void StreamOut( vsxTiXml_OSTREAM * out ) const;
	// [internal use]
	// Set the document pointer so the attribute can report errors.
  void SetDocument( vsxTiXmlDocument* doc )	{ document = doc; }

private:
  vsxTiXmlDocument*	document;	// A pointer back to a document, for error reporting.
  vsxTiXml_STRING name;
  vsxTiXml_STRING value;
  vsxTiXmlAttribute*	prev;
  vsxTiXmlAttribute*	next;
};


/*	A class used to manage a group of attributes.
	It is only used internally, both by the ELEMENT and the DECLARATION.
	
	The set can be changed transparent to the Element and Declaration
	classes that use it, but NOT transparent to the Attribute
	which has to implement a next() and previous() method. Which makes
	it a bit problematic and prevents the use of STL.

	This version is implemented with circular lists because:
		- I like circular lists
		- it demonstrates some independence from the (typical) doubly linked list.
*/
class CAL3D_API vsxTiXmlAttributeSet
{
public:
  vsxTiXmlAttributeSet();
  ~vsxTiXmlAttributeSet();

  void Add( vsxTiXmlAttribute* attribute );
  void Remove( vsxTiXmlAttribute* attribute );

  vsxTiXmlAttribute* First() const	{ return ( sentinel.next == &sentinel ) ? 0 : sentinel.next; }
  vsxTiXmlAttribute* Last()  const	{ return ( sentinel.prev == &sentinel ) ? 0 : sentinel.prev; }
  vsxTiXmlAttribute*	Find( const char * name ) const;

private:
  vsxTiXmlAttribute sentinel;
};


/** The element is a container class. It has a value, the element name,
	and can contain other elements, text, comments, and unknowns.
	Elements also contain an arbitrary number of attributes.
*/
class CAL3D_API vsxTiXmlElement : public vsxTiXmlNode
{
public:
	/// Construct an element.
  vsxTiXmlElement (const char * in_value);

  #ifdef vsxTiXml_USE_STL
	/// std::string constructor.
  vsxTiXmlElement( const std::string& _value ) : 	vsxTiXmlNode( vsxTiXmlNode::ELEMENT )
	{
		firstChild = lastChild = 0;
		value = _value;
	}
	#endif

  virtual ~vsxTiXmlElement();

	/** Given an attribute name, Attribute() returns the value
		for the attribute of that name, or null if none exists.
	*/
	const char* Attribute( const char* name ) const;

	/** Given an attribute name, Attribute() returns the value
		for the attribute of that name, or null if none exists.
		If the attribute exists and can be converted to an integer,
		the integer value will be put in the return 'i', if 'i'
		is non-null.
	*/
	const char* Attribute( const char* name, int* i ) const;

	/** Given an attribute name, Attribute() returns the value
		for the attribute of that name, or null if none exists.
		If the attribute exists and can be converted to an double,
		the double value will be put in the return 'd', if 'd'
		is non-null.
	*/
	const char* Attribute( const char* name, double* d ) const;

	/** QueryIntAttribute examines the attribute - it is an alternative to the
		Attribute() method with richer error checking.
		If the attribute is an integer, it is stored in 'value' and 
    the call returns vsxTiXml_SUCCESS. If it is not
    an integer, it returns vsxTiXml_WRONG_TYPE. If the attribute
    does not exist, then vsxTiXml_NO_ATTRIBUTE is returned.
	*/	
	int QueryIntAttribute( const char* name, int* value ) const;
	/// QueryDoubleAttribute examines the attribute - see QueryIntAttribute().
	int QueryDoubleAttribute( const char* name, double* value ) const;

	/** Sets an attribute of name to a given value. The attribute
		will be created if it does not exist, or changed if it does.
	*/
	void SetAttribute( const char* name, const char * value );

    #ifdef vsxTiXml_USE_STL
	const char* Attribute( const std::string& name ) const				{ return Attribute( name.c_str() ); }
	const char* Attribute( const std::string& name, int* i ) const		{ return Attribute( name.c_str(), i ); }

	/// STL std::string form.
	void SetAttribute( const std::string& name, const std::string& _value )	
	{	
		StringToBuffer n( name );
		StringToBuffer v( _value );
		if ( n.buffer && v.buffer )
			SetAttribute (n.buffer, v.buffer );	
	}	
	///< STL std::string form.
	void SetAttribute( const std::string& name, int _value )	
	{	
		StringToBuffer n( name );
		if ( n.buffer )
			SetAttribute (n.buffer, _value);	
	}	
	#endif

	/** Sets an attribute of name to a given value. The attribute
		will be created if it does not exist, or changed if it does.
	*/
	void SetAttribute( const char * name, int value );

	/** Deletes an attribute with the given name.
	*/
	void RemoveAttribute( const char * name );
    #ifdef vsxTiXml_USE_STL
	void RemoveAttribute( const std::string& name )	{	RemoveAttribute (name.c_str ());	}	///< STL std::string form.
	#endif

  vsxTiXmlAttribute* FirstAttribute() const	{ return attributeSet.First(); }		///< Access the first attribute in this element.
  vsxTiXmlAttribute* LastAttribute()	const 	{ return attributeSet.Last(); }		///< Access the last attribute in this element.

	// [internal use] Creates a new Element and returs it.
  virtual vsxTiXmlNode* Clone() const;
	// [internal use]

	virtual void Print( FILE* cfile, int depth ) const;

protected:

	// Used to be public [internal use]
  #ifdef vsxTiXml_USE_STL
      virtual void StreamIn( vsxTiXml_ISTREAM * in, vsxTiXml_STRING * tag );
	#endif
  virtual void StreamOut( vsxTiXml_OSTREAM * out ) const;

	/*	[internal use]
		Attribtue parsing starts: next char past '<'
						 returns: next char past '>'
	*/
  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data );

	/*	[internal use]
		Reads the "value" of the element -- another element, or text.
		This should terminate with the current end tag.
	*/
  const char* ReadValue( const char* in, vsxTiXmlParsingData* prevData );

private:
  vsxTiXmlAttributeSet attributeSet;
};


/**	An XML comment.
*/
class CAL3D_API vsxTiXmlComment : public vsxTiXmlNode
{
public:
	/// Constructs an empty comment.
  vsxTiXmlComment() : vsxTiXmlNode( vsxTiXmlNode::COMMENT ) {}
  virtual ~vsxTiXmlComment()	{}

	// [internal use] Creates a new Element and returs it.
  virtual vsxTiXmlNode* Clone() const;
	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;
protected:
	// used to be public
  #ifdef vsxTiXml_USE_STL
      virtual void StreamIn( vsxTiXml_ISTREAM * in, vsxTiXml_STRING * tag );
	#endif
  virtual void StreamOut( vsxTiXml_OSTREAM * out ) const;
	/*	[internal use]
		Attribtue parsing starts: at the ! of the !--
						 returns: next char past '>'
	*/
  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data );
};


/** XML text. Contained in an element.
*/
class CAL3D_API vsxTiXmlText : public vsxTiXmlNode
{
  friend class vsxTiXmlElement;
public:
	/// Constructor.
  vsxTiXmlText (const char * initValue) : vsxTiXmlNode (vsxTiXmlNode::TEXT)
	{
		SetValue( initValue );
	}
  virtual ~vsxTiXmlText() {}

  #ifdef vsxTiXml_USE_STL
	/// Constructor.
  vsxTiXmlText( const std::string& initValue ) : vsxTiXmlNode (vsxTiXmlNode::TEXT)
	{
		SetValue( initValue );
	}
	#endif

	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;

protected :
	// [internal use] Creates a new Element and returns it.
  virtual vsxTiXmlNode* Clone() const;
  virtual void StreamOut ( vsxTiXml_OSTREAM * out ) const;
	// [internal use]
	bool Blank() const;	// returns true if all white space and new lines
	/*	[internal use]
			Attribtue parsing starts: First char of the text
							 returns: next char past '>'
	*/
  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data );
	// [internal use]
  #ifdef vsxTiXml_USE_STL
      virtual void StreamIn( vsxTiXml_ISTREAM * in, vsxTiXml_STRING * tag );
	#endif
};


/** In correct XML the declaration is the first entry in the file.
	@verbatim
		<?xml version="1.0" standalone="yes"?>
	@endverbatim

	TinyXml will happily read or write files without a declaration,
	however. There are 3 possible attributes to the declaration:
	version, encoding, and standalone.

	Note: In this version of the code, the attributes are
	handled as special cases, not generic attributes, simply
	because there can only be at most 3 and they are always the same.
*/
class CAL3D_API vsxTiXmlDeclaration : public vsxTiXmlNode
{
public:
	/// Construct an empty declaration.
  vsxTiXmlDeclaration()   : vsxTiXmlNode( vsxTiXmlNode::DECLARATION ) {}

#ifdef vsxTiXml_USE_STL
	/// Constructor.
  vsxTiXmlDeclaration(	const std::string& _version,
						const std::string& _encoding,
						const std::string& _standalone )
      : vsxTiXmlNode( vsxTiXmlNode::DECLARATION )
	{
		version = _version;
		encoding = _encoding;
		standalone = _standalone;
	}
#endif

	/// Construct.
  vsxTiXmlDeclaration(	const char* _version,
						const char* _encoding,
						const char* _standalone );

  virtual ~vsxTiXmlDeclaration()	{}

	/// Version. Will return empty if none was found.
	const char * Version() const		{ return version.c_str (); }
	/// Encoding. Will return empty if none was found.
	const char * Encoding() const		{ return encoding.c_str (); }
	/// Is this a standalone document?
	const char * Standalone() const		{ return standalone.c_str (); }

	// [internal use] Creates a new Element and returs it.
  virtual vsxTiXmlNode* Clone() const;
	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;

protected:
	// used to be public
  #ifdef vsxTiXml_USE_STL
      virtual void StreamIn( vsxTiXml_ISTREAM * in, vsxTiXml_STRING * tag );
	#endif
  virtual void StreamOut ( vsxTiXml_OSTREAM * out) const;
	//	[internal use]
	//	Attribtue parsing starts: next char past '<'
	//					 returns: next char past '>'

  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data );

private:
  vsxTiXml_STRING version;
  vsxTiXml_STRING encoding;
  vsxTiXml_STRING standalone;
};


/** Any tag that tinyXml doesn't recognize is saved as an
	unknown. It is a tag of text, but should not be modified.
	It will be written back to the XML, unchanged, when the file
	is saved.
*/
class CAL3D_API vsxTiXmlUnknown : public vsxTiXmlNode
{
public:
  vsxTiXmlUnknown() : vsxTiXmlNode( vsxTiXmlNode::UNKNOWN ) {}
  virtual ~vsxTiXmlUnknown() {}

	// [internal use]
  virtual vsxTiXmlNode* Clone() const;
	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;
protected:
  #ifdef vsxTiXml_USE_STL
      virtual void StreamIn( vsxTiXml_ISTREAM * in, vsxTiXml_STRING * tag );
	#endif
  virtual void StreamOut ( vsxTiXml_OSTREAM * out ) const;
	/*	[internal use]
		Attribute parsing starts: First char of the text
						 returns: next char past '>'
	*/
  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data );
};


/** Always the top level node. A document binds together all the
	XML pieces. It can be saved, loaded, and printed to the screen.
	The 'value' of a document node is the xml file name.
*/
class CAL3D_API vsxTiXmlDocument : public vsxTiXmlNode
{
public:
	/// Create an empty document, that has no name.
  vsxTiXmlDocument();
	/// Create a document with a name. The name of the document is also the filename of the xml.
  vsxTiXmlDocument( const char * documentName );

  #ifdef vsxTiXml_USE_STL
	/// Constructor.
  vsxTiXmlDocument( const std::string& documentName ) :
      vsxTiXmlNode( vsxTiXmlNode::DOCUMENT )
	{
        tabsize = 4;
        value = documentName;
		error = false;
	}
	#endif

  virtual ~vsxTiXmlDocument() {}

	/** Load a file using the current document value.
		Returns true if successful. Will delete any existing
		document data before loading.
	*/
	bool LoadFile();
	/// Save a file using the current document value. Returns true if successful.
	bool SaveFile() const;
	/// Load a file using the given filename. Returns true if successful.
	bool LoadFile( const char * filename );
	/// Save a file using the given filename. Returns true if successful.
	bool SaveFile( const char * filename ) const;

  #ifdef vsxTiXml_USE_STL
	bool LoadFile( const std::string& filename )			///< STL std::string version.
	{
		StringToBuffer f( filename );
		return ( f.buffer && LoadFile( f.buffer ));
	}
	bool SaveFile( const std::string& filename ) const		///< STL std::string version.
	{
		StringToBuffer f( filename );
		return ( f.buffer && SaveFile( f.buffer ));
	}
	#endif

	/** Parse the given null terminated block of xml data.
	*/
  virtual const char* Parse( const char* p, vsxTiXmlParsingData* data = 0 );

	/** Get the root element -- the only top level element -- of the document.
		In well formed XML, there should only be one. TinyXml is tolerant of
		multiple elements at the document level.
	*/
  vsxTiXmlElement* RootElement() const		{ return FirstChildElement(); }

	/** If an error occurs, Error will be set to true. Also,
		- The ErrorId() will contain the integer identifier of the error (not generally useful)
		- The ErrorDesc() method will return the name of the error. (very useful)
		- The ErrorRow() and ErrorCol() will return the location of the error (if known)
	*/	
	bool Error() const						{ return error; }

	/// Contains a textual (english) description of the error if one occurs.
	const char * ErrorDesc() const	{ return errorDesc.c_str (); }

	/** Generally, you probably want the error string ( ErrorDesc() ). But if you
		prefer the ErrorId, this function will fetch it.
	*/
  int ErrorId()					{ return errorId; }

	/** Returns the location (if known) of the error. The first column is column 1, 
		and the first row is row 1. A value of 0 means the row and column wasn't applicable
		(memory errors, for example, have no row/column) or the parser lost the error. (An
		error in the error reporting, in that case.)

		@sa SetTabSize, Row, Column
	*/
	int ErrorRow()	{ return errorLocation.row+1; }
	int ErrorCol()	{ return errorLocation.col+1; }	///< The column where the error occured. See ErrorRow()

	/** By calling this method, with a tab size
		greater than 0, the row and column of each node and attribute is stored
		when the file is loaded. Very useful for tracking the DOM back in to
		the source file.

		The tab size is required for calculating the location of nodes. If not
		set, the default of 4 is used. The tabsize is set per document. Setting
		the tabsize to 0 disables row/column tracking.

		Note that row and column tracking is not supported when using operator>>.

		The tab size needs to be enabled before the parse or load. Correct usage:
		@verbatim
    vsxTiXmlDocument doc;
		doc.SetTabSize( 8 );
		doc.Load( "myfile.xml" );
		@endverbatim

		@sa Row, Column
	*/
	void SetTabSize( int _tabsize )		{ tabsize = _tabsize; }

	int TabSize() const	{ return tabsize; }

	/** If you have handled the error, it can be reset with this call. The error
		state is automatically cleared if you Parse a new XML block.
	*/
	void ClearError()						{	error = false; 
												errorId = 0; 
												errorDesc = ""; 
												errorLocation.row = errorLocation.col = 0; 
												//errorLocation.last = 0; 
											}

	/** Dump the document to standard out. */
	void Print() const						{ Print( stdout, 0 ); }

	// [internal use]
	virtual void Print( FILE* cfile, int depth = 0 ) const;
	// [internal use]
  void SetError( int err, const char* errorLocation, vsxTiXmlParsingData* prevData );

protected :
  virtual void StreamOut ( vsxTiXml_OSTREAM * out) const;
	// [internal use]
  virtual vsxTiXmlNode* Clone() const;
  #ifdef vsxTiXml_USE_STL
      virtual void StreamIn( vsxTiXml_ISTREAM * in, vsxTiXml_STRING * tag );
	#endif

private:
	bool error;
	int  errorId;
  vsxTiXml_STRING errorDesc;
	int tabsize;
  vsxTiXmlCursor errorLocation;
};


/**
  A vsxTiXmlHandle is a class that wraps a node pointer with null checks; this is
  an incredibly useful thing. Note that vsxTiXmlHandle is not part of the TinyXml
	DOM structure. It is a separate utility class.

	Take an example:
	@verbatim
	<Document>
		<Element attributeA = "valueA">
			<Child attributeB = "value1" />
			<Child attributeB = "value2" />
		</Element>
	<Document>
	@endverbatim

	Assuming you want the value of "attributeB" in the 2nd "Child" element, it's very 
	easy to write a *lot* of code that looks like:

	@verbatim
  vsxTiXmlElement* root = document.FirstChildElement( "Document" );
	if ( root )
	{
    vsxTiXmlElement* element = root->FirstChildElement( "Element" );
		if ( element )
		{
      vsxTiXmlElement* child = element->FirstChildElement( "Child" );
			if ( child )
			{
        vsxTiXmlElement* child2 = child->NextSiblingElement( "Child" );
				if ( child2 )
				{
					// Finally do something useful.
	@endverbatim

  And that doesn't even cover "else" cases. vsxTiXmlHandle addresses the verbosity
  of such code. A vsxTiXmlHandle checks for null	pointers so it is perfectly safe
	and correct to use:

	@verbatim
  vsxTiXmlHandle docHandle( &document );
  vsxTiXmlElement* child2 = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).Child( "Child", 1 ).Element();
	if ( child2 )
	{
		// do something useful
	@endverbatim

	Which is MUCH more concise and useful.

	It is also safe to copy handles - internally they are nothing more than node pointers.
	@verbatim
  vsxTiXmlHandle handleCopy = handle;
	@endverbatim

	What they should not be used for is iteration:

	@verbatim
	int i=0; 
	while ( true )
	{
    vsxTiXmlElement* child = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).Child( "Child", i ).Element();
		if ( !child )
			break;
		// do something
		++i;
	}
	@endverbatim

	It seems reasonable, but it is in fact two embedded while loops. The Child method is 
	a linear walk to find the element, so this code would iterate much more than it needs 
	to. Instead, prefer:

	@verbatim
  vsxTiXmlElement* child = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).FirstChild( "Child" ).Element();

	for( child; child; child=child->NextSiblingElement() )
	{
		// do something
	}
	@endverbatim
*/
class CAL3D_API vsxTiXmlHandle
{
public:
	/// Create a handle from any node (at any depth of the tree.) This can be a null pointer.
  vsxTiXmlHandle( vsxTiXmlNode* node )			{ this->node = node; }
	/// Copy constructor
  vsxTiXmlHandle( const vsxTiXmlHandle& ref )	{ this->node = ref.node; }

	/// Return a handle to the first child node.
  vsxTiXmlHandle FirstChild() const;
	/// Return a handle to the first child node with the given name.
  vsxTiXmlHandle FirstChild( const char * value ) const;
	/// Return a handle to the first child element.
  vsxTiXmlHandle FirstChildElement() const;
	/// Return a handle to the first child element with the given name.
  vsxTiXmlHandle FirstChildElement( const char * value ) const;

	/** Return a handle to the "index" child with the given name. 
		The first child is 0, the second 1, etc.
	*/
  vsxTiXmlHandle Child( const char* value, int index ) const;
	/** Return a handle to the "index" child. 
		The first child is 0, the second 1, etc.
	*/
  vsxTiXmlHandle Child( int index ) const;
	/** Return a handle to the "index" child element with the given name. 
    The first child element is 0, the second 1, etc. Note that only vsxTiXmlElements
		are indexed: other types are not counted.
	*/
  vsxTiXmlHandle ChildElement( const char* value, int index ) const;
	/** Return a handle to the "index" child element. 
    The first child element is 0, the second 1, etc. Note that only vsxTiXmlElements
		are indexed: other types are not counted.
	*/
  vsxTiXmlHandle ChildElement( int index ) const;

  #ifdef vsxTiXml_USE_STL
  vsxTiXmlHandle FirstChild( const std::string& _value ) const			{ return FirstChild( _value.c_str() ); }
  vsxTiXmlHandle FirstChildElement( const std::string& _value ) const		{ return FirstChildElement( _value.c_str() ); }

  vsxTiXmlHandle Child( const std::string& _value, int index ) const			{ return Child( _value.c_str(), index ); }
  vsxTiXmlHandle ChildElement( const std::string& _value, int index ) const	{ return ChildElement( _value.c_str(), index ); }
	#endif

  /// Return the handle as a vsxTiXmlNode. This may return null.
  vsxTiXmlNode* Node() const			{ return node; }
  /// Return the handle as a vsxTiXmlElement. This may return null.
  vsxTiXmlElement* Element() const	{ return ( ( node && node->ToElement() ) ? node->ToElement() : 0 ); }
  /// Return the handle as a vsxTiXmlText. This may return null.
  vsxTiXmlText* Text() const			{ return ( ( node && node->ToText() ) ? node->ToText() : 0 ); }

private:
  vsxTiXmlNode* node;
};


#endif

