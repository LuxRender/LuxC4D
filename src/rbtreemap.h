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

#ifndef __RBTREEMAP_H__
#define __RBTREEMAP_H__ 1



/***************************************************************************//*!
 This class implments a map, based on the very cool "simplified"
 left-leaning red-black tree of Robert Sedgewick:
   http://www.cs.princeton.edu/~rs/

 The template key type K has to support the following functions:
  - Less operator     bool K::operator<(const K&)

 The template type has to support the following functions:
  - Copy constructor  T::T(const T&)
  - Copy operator     T::operator=(const T&)
*//****************************************************************************/

template <class K, class T>
class RBTreeMap
{
public:

  /// The key type.
  typedef K       KeyT;
  /// The value type.
  typedef T       ValueT;
  /// The size type.
  typedef VULONG  SizeT;


  RBTreeMap(void);
  ~RBTreeMap(void);

  void erase(void);

  inline SizeT size(void) const;

  ValueT* add(const KeyT& key, const ValueT& value);

  const ValueT* get(const KeyT& key) const;
  ValueT*       get(const KeyT& key);


private:


  /// This helper structure implements a single node of a red-black tree.
  struct Node 
  {
    Node*  mLeft;
    Node*  mRight;
    Bool   mIsRed;
    KeyT   mKey;
    ValueT mValue;


    /// Constructs a new node and sets its value.
    /// @param[in]  value
    ///   The value of which we store a copy in the new node.
    inline Node(const KeyT& key, const ValueT& value)
    : mLeft(0),
      mRight(0),
      mIsRed(TRUE),
      mKey(key),
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


  // At the moment, we don't allow copying of RBTreeMaps.
  RBTreeMap(const RBTreeMap& other) {}
  RBTreeMap& operator==(const RBTreeMap& other)  {}

  inline static Node* rotateLeft(Node* node);
  inline static Node* rotateRight(Node* node);
  inline static void  colorFlip(Node* node);
  
  Node* insert(Node* node,
               const KeyT&   key,
               const ValueT& value);

  static void deleteRecursive(Node* node);
};


#include "rbtreemap_impl.h"



#endif  // #ifndef __RBTREEMAP_H__
