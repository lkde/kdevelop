/***************************************************************************
                            ParsedItem.java -  description
                             -------------------
    begin                : Wed Jul  4 12:40:03 2001
    copyright            : (C) 2000-2001 Lost Highway Ltd.
    email                : Richard_Dale@tipitina.demon.co.uk
    generated by         : duke@tipitina on Wed Jul  4 12:40:03 2001, using kdoc parser/custom Java conversion backend.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
***************************************************************************/

package org.kde.koala;

import org.kde.qt.*;
import java.util.*;
import java.lang.Error;

/**

 Abstract class for all items that are parsed in the classparser. 
 The smallest common items shared by all items are name, export
 and the files/lines they are declared/defined from/to.

 @author Jonas Nordin
 
 @short 
 Abstract class for all items that are parsed in the classparser.
*/
public class  ParsedItem implements QtSupport {
	private long _qt;
	private boolean _allocatedInJavaWorld = true;

	protected ParsedItem(Class dummy){}

	protected native void finalize() throws InternalError;

/** 
 Is this a public item? 

*/
	public native boolean isPublic();

/** 
 Is this a protected item? 

*/
	public native boolean isProtected();

/** 
 Is this a private item?

*/
	public native boolean isPrivate();

/** 
 Is this a package item?

*/
	public native boolean isPackage();

/** 
 Is this a global variable?

*/
	public native boolean isGlobal();

/** 
 The path is the scope + "." + the name of the item.

*/
	public native String path();

/** 
 Sets the item type. 

*/
	public native void setItemType( int aType);
	public native int itemType();

/** 
 Sets the name. 

*/
	public native void setName( String aName);
	public native String name();

/** 
 Sets the acess (public/protected/private)

*/
	public native void setAccess( int aAccess);
	public native int access();

/** 
 Sets the scope this item is declared in.

*/
	public native void setDeclaredInScope( String aScope);
	public native String declaredInScope();

/** 
 Sets the line where the item was defined. 

*/
	public native void setDefinedOnLine( int aLine);
	public native int definedOnLine();

/** 
 Sets the line where the declaration ends.

*/
	public native void setDefinitionEndsOnLine( int aLine);
	public native int definitionEndsOnLine();

/** 
 Sets the line where the item was declared. 

*/
	public native void setDeclaredOnLine( int aLine);
	public native int declaredOnLine();

/** 
 Sets the line where the declaration ends.

*/
	public native void setDeclarationEndsOnLine( int aLine);
	public native int declarationEndsOnLine();

/** 
 Sets the line where the item was defined. 

*/
	public native void setDefinedInFile( String aFile);
	public native String definedInFile();

/** 
 Sets the file where the item was declared. 

*/
	public native void setDeclaredInFile( String aFile);
	public native String declaredInFile();

/** 
 Sets the comment of this item.

*/
	public native void setComment( String aComment);
	public native String comment();

/**  Clears all item declaration fields 
*/
	public native void clearDeclaration();

/**  Clears all item definition fields 
*/
	public native void clearDefinition();

/**  
 Makes this object a copy of the supplied object. 

*/
	public native void copy( ParsedItem anItem);

/** 
 Returns the object as a string(for tooltips etc) 

*/
	public native String asString();

/**  Outputs this object to stdout 
*/
	public native void out();

/** The type of a ParsedItem. */
	public static final int PIT_UNKNOWN = 0;
	public static final int PIT_CLASS = 1;
	public static final int PIT_METHOD = 2;
	public static final int PIT_ATTRIBUTE = 3;
	public static final int PIT_STRUCT = 4;
	public static final int PIT_SCOPE = 5;

/** Access of a ParsedItem. */
	public static final int PIE_GLOBAL = 0;
	public static final int PIE_PUBLIC = 1;
	public static final int PIE_PROTECTED = 2;
	public static final int PIE_PRIVATE = 3;

}
