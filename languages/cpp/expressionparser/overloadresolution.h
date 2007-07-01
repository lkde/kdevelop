/* 
   Copyright (C) 2007 David Nolden <user@host.de>
   (where user = david.nolden.kdevelop, host = art-master)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef OVERLOADRESOLUTION_H
#define OVERLOADRESOLUTION_H

#include <typesystem.h>
#include "typeconversion.h"
#include <QList>
#include "cppexpressionparserexport.h"

class CppFunctionType;

namespace KDevelop {
  class DUContext;
  class Declaration;
}

namespace Cpp {
using namespace KDevelop;
  class ViableFunction;
/**
 * Models overloaded function resolution
 * du-chain must be locked whenever this class is used
 * */
class KDEVCPPEXPRESSIONPARSER_EXPORT OverloadResolver {
  public:

    struct Parameter {
      Parameter() : type(0), lValue(false) {
      }
      Parameter( AbstractType* t, bool isLValue ) : type(t), lValue( isLValue ) {
      }
      AbstractType* type;
      bool lValue;
    };
    struct ParameterList {
      QList<Parameter> parameters;

      ParameterList() {
      }
      
      ParameterList( const QList<Parameter>& params ) : parameters(params) {
      }
      
      ParameterList( const Parameter& param ) {
        parameters << param;
      }

      ParameterList( AbstractType* param, bool isLValue ) {
        parameters << Parameter(param, isLValue);
      }
    };

    /**
     * @param container The container in which to search for the functions. If it is a class, base-classes will be respected too.
     * */
    OverloadResolver( DUContext* context );

    /**
     * Resolve one function with the given name that matches the given parameters.
     *
     * When classes are found under the given name, their constructors will be considered as functions.
     *
     * @warning du-chain must be locked
     * @param params The parameters
     * @param functionName name of the function
     * @param noUserDefinedConversion should be true when no user-defined conversions are allowed for parameters
     * */
    Declaration* resolve( const ParameterList& params, const QualifiedIdentifier& functionName, bool noUserDefinedConversion = false );

    /**
     * The worst conversion-rank achieved while matching the parameters of the last overload-resolution.
     * It is valued like the results of TypeConversion::implicitConversion(..)
     * */
    uint worstConversionRank();
    
    /**
     * Tries to find a constructor of the class represented by the current context
     * that matches the given parameter-list
     * @warning du-chain must be locked
     * @param implicit When this is true, constructors with the keyword "explicit" are ignored
     * @param noUserDefinedConversion When this is true, user-defined conversions(constructor- or conversion-function conversion) are not allowed while matching the parameters
     * */
    Declaration* resolveConstructor( const ParameterList& params, bool implicit = false, bool noUserDefinedConversion = false );
  
    /**
     * Tries to choose the correct function out of a given list of function-declarations.
     * If one of those declarations is a class-declaration, it will be substituted by it's constructors.
     * If one of those declarations is an instance of a class, it will be substituted with it's operator()
     * functions. Constant references to class-instances are treated correctly.
     * 
     * @warning du-chain must be locked
     
    * @param params parameters to match
     * @param declarations list of declarations
     * @param noUserDefinedConversion should be true if user-defined conversions(conversion-operators and constructor-conversion) are not allowed when matching the parameters
     * */
    Declaration* resolveList( const ParameterList& params, const QList<Declaration*>& declarations, bool noUserDefinedConversion = false );

    /**
     * Matches the given functions with the given parameters. Only does partial matching, by considering only those parameters that were
     * actually given.
     * Returns a sorted list containing all given declarations.
     * 
     * @warning du-chain must be locked
     *
     * @return List of all given functions. The list is sorted by viability(the first item is most viable). Non-viable functions are also included.
     * */
    QList< ViableFunction > resolveListPartial( const ParameterList& params, const QList<QPair<OverloadResolver::ParameterList, Declaration*> >& declarations );
  private:
    ///Replace class-instances with operator() functions, and pure classes with their constructors
    void expandDeclarations( const QList<Declaration*>& from, QList<Declaration*>& to );
    void expandDeclarations( const QList<QPair<OverloadResolver::ParameterList, Declaration*> >& from, QList<QPair<OverloadResolver::ParameterList, Declaration*> >& to );

    DUContext* m_context;
    uint m_worstConversionRank;
};

}

#endif
