/***************************************************************************
                          ClassStore.cc  -  description
                             -------------------
    begin                : Fri Mar 19 1999
    copyright            : (C) 1999 by Jonas Nordin
    email                : jonas.nordin@cenacle.se
   
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include "ClassStore.h"
#include <iostream.h>
#include <assert.h>
#include <qregexp.h>

/*********************************************************************
 *                                                                   *
 *                     CREATION RELATED METHODS                      *
 *                                                                   *
 ********************************************************************/

/*---------------------------------------- CClassStore::CClassStore()
 * CClassStore()
 *   Constructor.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CClassStore::CClassStore() 
{
  // Initialize the persistant class store.
  //  globalStore.setPath( "/tmp"  );
  globalStore.setFilename( "classes.db" );

  // Open the store if it exists, else create it.
  globalStore.open();
}

/*---------------------------------------- CClassStore::~CClassStore()
 * ~CClassStore()
 *   Destructor.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CClassStore::~CClassStore()
{
}

/*********************************************************************
 *                                                                   *
 *                          PUBLIC METHODS                           *
 *                                                                   *
 ********************************************************************/

/*-------------------------------------------- CClassStore::wipeout()
 * wipeout()
 *   Remove all parsed classes and reset the state.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::wipeout()
{
  globalContainer.clear();
}


/*-------------------------------- CClassStore::removeWithReferences()
 * removeWithReferences()
 *   Remove all items in the store with references to the file.
 *
 * Parameters:
 *   aFile          The file.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::removeWithReferences( const char *aFile )
{
  CParsedClass *aClass;

  // Remove all classes with reference to this file.
  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();

    // Remove the class if any of the files are the supplied one.
    if( aClass->declaredInFile == aFile ||
        aClass->definedInFile == aFile )
      removeClass( aClass->name );
  }
  
  // Remove all global functions, variables and structures.
  globalContainer.removeWithReferences( aFile );
}

/*------------------------------------------- CClassStore::storeAll()
 * storeAll()
 *   Store all parsed classes as a database.
 *
 * Parameters:
 *   aClass        The class to add.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::storeAll()
{
  QString str;
  CParsedClass *aClass;

  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();
    aClass->asPersistantString( str );

    debug( "Storing:" );
    debug( "----------" );
    debug( str );
    globalStore.storeClass( aClass );
    debug( "----------" );
  }
}

/*------------------------------------------- CClassStore::addClass()
 * addClass()
 *   Add a class to the store.
 *
 * Parameters:
 *   aClass        The class to add.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::addClass( CParsedClass *aClass )
{
  assert( aClass != NULL );
  assert( !aClass->name.isEmpty() );
  assert( !hasClass( aClass->name ) );

  globalContainer.addClass( aClass );

  if( globalStore.isOpen )
    globalStore.storeClass( aClass );
}

/*---------------------------------------- CClassStore::removeClass()
 * removeClass()
 *   Remove a class from the store.
 *
 * Parameters:
 *   aName        Name of the class to remove
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::removeClass( const char *aName )
{
  assert( aName != NULL );
  assert( strlen( aName ) > 0 );
  assert( hasClass( aName ) );

  globalContainer.removeClass( aName );

  if( globalStore.isOpen )
    globalStore.removeClass( aName );
}

/*------------------------------------------------- CClassStore::out()
 * out()
 *   Output this object to stdout.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::out()
{
  QList<CParsedMethod> *globalMethods;
  QList<CParsedAttribute> *globalAttributes;
  QList<CParsedStruct> *globalStructs;
  QList<CParsedClass> *classes;
  CParsedClass *aClass;
  CParsedMethod *aMethod;
  CParsedAttribute *aAttr;
  CParsedStruct *aStruct;

  // Output all classes.
  classes = globalContainer.getSortedClassList();
  for( aClass = classes->first();
       aClass != NULL;
       aClass = classes->next() )
  {
    aClass->out();
  }
  delete classes;

  // Global methods
  cout << "Global functions\n";

  globalMethods = globalContainer.getSortedMethodList();
  for( aMethod = globalMethods->first();
       aMethod != NULL;
       aMethod = globalMethods->next() )
  {
    aMethod->out();
  }
  delete globalMethods;

  // Global structures
  cout << "Global variables\n";
  globalAttributes = globalContainer.getSortedAttributeList();
  for( aAttr = globalAttributes->first();
       aAttr != NULL;
       aAttr = globalAttributes->next() )
  {
    aAttr->out();
  }
  delete globalAttributes;  

  // Global structures
  cout << "Global structs\n";
  globalStructs = globalContainer.getSortedStructList();
  for( aStruct = globalStructs->first();
       aStruct != NULL;
       aStruct = globalStructs->next() )
  {
    aStruct->out();
  }
  delete globalStructs;  
}

/*********************************************************************
 *                                                                   *
 *                          PUBLIC QUERIES                           *
 *                                                                   *
 ********************************************************************/

/*-------------------------------------------- CClassStore::asForest()
 * asForest()
 *   Return the store as a forest(collection of trees).
 *
 * Parameters:
 *   -
 * Returns:
 *   QList<CClassTreeNode> List of trees.
 *-----------------------------------------------------------------*/
QList<CClassTreeNode> *CClassStore::asForest()
{
  CParsedClass *aClass;
  CParsedParent *aParent;  
  CClassTreeNode *childNode;
  CClassTreeNode *parentNode;
  QDict<CClassTreeNode> ctDict;
  QList<CClassTreeNode> *retVal = new QList<CClassTreeNode>;

  // Iterate over all parsed classes.
  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();

    // Check if we have added the child.
    childNode = ctDict.find( aClass->name );
    
    // If not in the table already, we add a new node.
    if( childNode == NULL )
    {
      childNode = new CClassTreeNode();

      ctDict.insert( aClass->name, childNode );
    }
    else if( !childNode->isInSystem )
      retVal->removeRef( childNode );
    
    // Set childnode values.
    childNode->setName( aClass->name );
    childNode->setClass( aClass );
    childNode->setIsInSystem( true );

    // If this class has no parent, we add it as a rootnode in the forest.
    if( aClass->parents.count() == 0)
      retVal->append( childNode );
    else // Has parents
    {
      // Add this class to its' parents.
      for( aParent = childNode->theClass->parents.first();
           aParent != NULL;
           aParent = childNode->theClass->parents.next() )
      {
        // Check if we have added the parent already.
        parentNode = ctDict.find( aParent->name );
        
        // Add a new node for the parent if not added already.
        if( parentNode == NULL )
        {
          // Create the parentnode.
          parentNode = new CClassTreeNode();
          parentNode->setName( aParent->name );
          parentNode->setIsInSystem( false );

          retVal->append( parentNode );
          ctDict.insert( parentNode->name, parentNode );
        }
        
        // Add the child to the parent node.
        parentNode->addChild( childNode );
      }
    }
  }

  return retVal;
}

/*-------------------------------------------- CClassStore::hasClass()
 * hasClass()
 *   Tells if a class exist in the store.
 *
 * Parameters:
 *   aName          Name of the class to check.
 *
 * Returns:
 *   bool           Result of the lookup.
 *-----------------------------------------------------------------*/
bool CClassStore::hasClass( const char *aName )
{
  return globalContainer.hasClass( aName ) || 
    ( globalStore.isOpen && globalStore.hasClass( aName ) );
  //return classes.find( aName ) != NULL;
}

/*-------------------------------------- CClassStore::getClassByName()
 * getClassByName()
 *   Get a class from the list by using its' name.
 *
 * Parameters:
 *   aName          Name of the class to fetch.
 *
 * Returns:
 *   CParsedClass * The class we looked for.
 *   NULL           Otherwise.
 *-----------------------------------------------------------------*/
CParsedClass *CClassStore::getClassByName( const char *aName )
{
  assert( aName != NULL );

  CParsedClass *aClass;

  if( globalStore.isOpen && globalStore.hasClass( aName ) )
    aClass = globalStore.getClassByName( aName );
  else
    aClass = globalContainer.getClassByName( aName );

  return aClass;
}

/*--------------------------------- CClassStore::getClassesByParent()
 * getClassesByParent()
 *   Get all classes with a certain parent.
 *
 * Parameters:
 *   aName             Name of the parent.
 *
 * Returns:
 *   QList<CParsedClass> * The classes with the desired parent.
 *-----------------------------------------------------------------*/
QList<CParsedClass> *CClassStore::getClassesByParent( const char *aName )
{
  QList<CParsedClass> *retVal = new QList<CParsedClass>();
  CParsedClass *aClass;

  retVal->setAutoDelete( false );
  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();
    if( aClass->hasParent( aName ) )
      retVal->append( aClass );
  }

  return retVal;
}

/*------------------------------------ CClassStore::getClassClients()
 * getClassClients()
 *   Fetches all clients of a named class.
 *
 * Parameters:
 *   aName             Name of the class.
 *
 * Returns:
 *   QList<CParsedClass> * The clients of the class.
 *-----------------------------------------------------------------*/
QList<CParsedClass> *CClassStore::getClassClients( const char *aName )
{
  assert( aName != NULL );

  bool exit;
  CParsedClass *aClass;
  CParsedAttribute *aAttr;
  QList<CParsedClass> *retVal = new QList<CParsedClass>();

  retVal->setAutoDelete( false );
  for( globalContainer.classIterator.toFirst();
       globalContainer.classIterator.current();
       ++globalContainer.classIterator )
  {
    aClass = globalContainer.classIterator.current();
    if( aClass->name != aName )
    {
      exit = false;
      for( aClass->attributeIterator.toFirst();
           aClass->attributeIterator.current() && !exit;
           ++(aClass->attributeIterator) )
      {
        aAttr = aClass->attributeIterator.current();
        exit = ( aAttr->type.find( aName ) != -1 );
      }

      if( exit )
        retVal->append( aClass );
    }
  }

  return retVal;
}

/*------------------------------------ CClassStore::getClassSuppliers()
 * getClassSuppliers()
 *   Fetches all suppliers of a named class.
 *
 * Parameters:
 *   aName             Name of the class.
 *
 * Returns:
 *   QList<CParsedClass> * The suppliers to the class.
 *-----------------------------------------------------------------*/
QList<CParsedClass> *CClassStore::getClassSuppliers( const char *aName )
{
  assert( aName != NULL );
  assert( hasClass( aName ) );

  CParsedClass *aClass;
  CParsedClass *toAdd;
  QString str;
  QList<CParsedClass> *retVal = new QList<CParsedClass>();

  retVal->setAutoDelete( false );

  aClass = getClassByName( aName );
  for( aClass->attributeIterator.toFirst();
       aClass->attributeIterator.current();
       ++aClass->attributeIterator )
  {
    str = aClass->attributeIterator.current()->type;

    // Remove all unwanted stuff.
    str = str.replace( "[\\*&]", "" );
    str = str.replace( "const", "" );
    str = str.replace( "void", "" );
    str = str.replace( "bool", "" );
    str = str.replace( "uint", "" );
    str = str.replace( "int", "" );
    str = str.replace( "char", "" );
    str = str.stripWhiteSpace();

    // If this isn't the class and the string contains data, we check for it.
    if( str != aName && !str.isEmpty() )
    {
      debug( "Checking if '%s' is a class", str.data() );
      toAdd = getClassByName( str );
      if( toAdd )
        retVal->append( toAdd );
    }
  }

  return retVal;
}

/*------------------------------------ CClassStore::getSortedClassList()
 * getSortedClassList()
 *   Get all classes in sorted order.
 *
 * Parameters:
 *   -
 * Returns:
 *   QList<CParsedClass> * The classes.
 *-----------------------------------------------------------------*/
QList<CParsedClass> *CClassStore::getSortedClassList()
{
  return globalContainer.getSortedClassList();
}

/*---------------------------- CClassStore::getSortedClassNameList()
 * getSortedClassNameList()
 *   Get all classnames in sorted order.
 *
 * Parameters:
 *   -
 * Returns:
 *   QStrList * The classnames.
 *-----------------------------------------------------------------*/
QStrList *CClassStore::getSortedClassNameList()
{
  return globalContainer.getSortedClassNameList();
}

/*-------------------------- CClassStore::getVirtualMethodsForClass()
 * getVirtualMethodsForClass()
 *   Fetch all virtual methods, both implemented and not.
 *
 * Parameters:
 *   aName      Name of the class.
 *   implList   The list that will contain the implemented virtual 
 *              methods.
 *   availList  The list hat will contain the available virtual
 *              methods.
 * 
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CClassStore::getVirtualMethodsForClass( const char *aName, 
                                             QList<CParsedMethod> *implList,
                                             QList<CParsedMethod> *availList )
{
  CParsedClass *aClass;
  CParsedParent *aParent;
  CParsedClass *parentClass;
  QList<CParsedMethod> *list;
  CParsedMethod *aMethod;
  QDict<char> added;
  QString str;

  // Start by reseting the lists.
  implList->setAutoDelete( false );
  availList->setAutoDelete( false );
  implList->clear();
  availList->clear();

  // Try to fetch the class
  aClass = getClassByName( aName );
  if( aClass != NULL )
  {
    // Iterate over all parents.
    for( aParent = aClass->parents.first();
         aParent != NULL;
         aParent = aClass->parents.next() )
    {
      // Try to fetch the parent.
      parentClass = getClassByName( aParent->name );
      if( parentClass != NULL )
      {
        list = parentClass->getVirtualMethodList();

        for( aMethod = list->first();
             aMethod != NULL;
             aMethod = list->next() )
        {
          // Check if we already have the method.
          if( aClass->getMethod( *aMethod ) != NULL )
          {
            implList->append( aMethod );
            added.insert( aMethod->asString( str ), "" );
          }
          else
            availList->append( aMethod );
        }
        
        delete list;
      }

    }

    // Add all virtual methods defined in THIS class.
    for( aClass->methodIterator.toFirst();
         aClass->methodIterator.current();
         ++aClass->methodIterator )
    {
      aMethod = aClass->methodIterator.current();
      if( aMethod->isVirtual && 
          added.find( aMethod->asString( str ) ) == NULL )
      {
        availList->append( aMethod );
      }
    }
  }
}
