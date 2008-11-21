/************************************************************************
 * LuxC4D - CINEMA 4D plug-in for export to LuxRender                   *
 * (http://www.luxrender.net)                                           *
 *                                                                      *
 * Author:                                                              *
 * Marcus Spranger (abstrax)                                            *
 *                                                                      *
 ************************************************************************
 *                                                                      *
 * This file is part of LuxC4D.                                         *
 *                                                                      *
 * LuxC4D is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * LuxC4D is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with LuxC4D.  If not, see <http://www.gnu.org/licenses/>.      *
 ************************************************************************/

#ifndef __RBTREESET_H__
#define __RBTREESET_H__ 1



/***************************************************************************//*!
 This class implments a set, based on the very cool "simplified"
 left-leaning red-black tree of Robert Sedgewick:
   http://www.cs.princeton.edu/~rs/

 The template type has to support the following functions:
  - Copy constructor  T::T(const T&)
  - Copy operator     T::operator=(const T&)
  - Less operator     bool T::operator<(const T&)
*//****************************************************************************/

template <class T>
class RBTreeSet
{
public:

  /// The value type.
  typedef T       ValueT;
  /// The size type.
  typedef VULONG  SizeT;


  RBTreeSet(void);
  ~RBTreeSet(void);

  void erase(void);

  inline SizeT size(void) const;

  ValueT* add(const ValueT& value);

  const ValueT* get(const ValueT& value) const;
  ValueT*       get(const ValueT& value);


private:


  /// This helper structure implements a single node of a red-black tree.
  struct Node 
  {
    Node*  mLeft;
    Node*  mRight;
    Bool   mIsRed;
    ValueT mValue;


    /// Constructs a new node and sets its value.
    /// @param[in]  value
    ///   The value of which we store a copy in the new node.
    inline Node(const ValueT& value)
    : mLeft(0),
      mRight(0),
      mIsRed(TRUE),
      mValue(value)
    {}

    /// Static helper function that checks if a pointer points to a black node.
    /// @param[in]  node
    ///   Pointer to a node, which can be NULL.
    /// @return 
    ///   TRUE if the pointer is valid and is pointing to a black node.
    inline static Bool isBlack(const Node* node)
    {
      return (node && !node->mIsRed);
    }

    /// Static helper function that checks if a pointer points to a red node.
    /// @param[in]  node
    ///   Pointer to a node, which can be NULL.
    /// @return 
    ///   TRUE if the pointer is valid and is pointing to a red node.
    inline static Bool isRed(const Node* node)
    {
      return (node && node->mIsRed);
    }
  };


  Node* mRoot;
  SizeT mSize;
  Node* mNewNode;


  RBTreeSet(const RBTreeSet& other) {}
  RBTreeSet& operator==(const RBTreeSet& other)  {}

  inline static Node* rotateLeft(Node* node);
  inline static Node* rotateRight(Node* node);
  inline static void  colorFlip(Node* node);
  
  Node* insert(Node* node,
               const ValueT& value);

  static void deleteRecursive(Node* node);
};


#include "rbtreeset_impl.h"



#endif  // #ifndef __RBTREESET_H__
