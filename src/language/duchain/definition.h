/* This file is part of KDevelop
    Copyright (C) 2006 Hamish Rodda <rodda@kde.org>

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

#ifndef DEFINITION_H
#define DEFINITION_H

#include <documentrangeobject.h>
#include <duchainbase.h>
#include <languageexport.h>

namespace KDevelop
{

class Declaration;
class DUContext;

/**
 * Represents a definition of a previously encountered declaration in a definition-use chain.
 */
class KDEVPLATFORMLANGUAGE_EXPORT Definition : public DUChainBase
{
friend class Declaration;
public:
  Definition(KTextEditor::Range* range, DUContext* context);
  virtual ~Definition();

  virtual TopDUContext* topContext() const;

  DUContext* context() const;
  void setContext(DUContext* context);

  /**
   * Retrieve the declaration for this use.
   */
  Declaration* declaration() const;

private:
  class DefinitionPrivate* const d;
};
}
#endif // DEFINITION_H

// kate: space-indent on; indent-width 2; tab-width: 4; replace-tabs on; auto-insert-doxygen on
