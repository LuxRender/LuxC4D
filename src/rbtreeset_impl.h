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

#ifndef __RBTREESET_IMPL_H__
#define __RBTREESET_IMPL_H__ 1



/*******************************************************************************
 * Implementation of public member functions of template class RBTreeSet.
 *******************************************************************************/

/// Constructs a new (empty) red-black tree.
template <class T>
RBTreeSet<T>::RBTreeSet(void)
: mRoot(0), mSize(0)
{}


/// Destroys a red-black tree and deallocates its resources.
template <class T>
RBTreeSet<T>::~RBTreeSet(void)
{
  erase();
}


/// Deallocates all resources of a red-black tree. The tree will be empty
/// afterwards.
template <class T>
void RBTreeSet<T>::erase(void)
{
  RBTreeSet<T>::deleteRecursive(mRoot);
  mRoot = 0;
  mSize = 0;
}


/// Returns the number of entries of red-black tree.
template <class T>
inline typename RBTreeSet<T>::SizeT RBTreeSet<T>::size(void) const
{
  return mSize;
}


/// Adds a new entry to the LLRB set. If there was already one with the same
/// rang (/key), we overwrite it.
///
/// @param[in]  value
///   The value to add to the red-black tree.
/// @return
///   NULL if we ran out of memory, otherwise the pointer to the stored entry.
template <class T>
T* RBTreeSet<T>::add(const ValueT& value)
{
  mNewNode = 0;
  mRoot = insert(mRoot, value);
  ++mSize;
  return mNewNode ? &mNewNode->mValue : 0;
}


/// Returns the (const) value from the set with the same order as the passed in
/// value.
///
/// @param[in]  value
///   The value that defines the rang of the value to retrieve.
/// @return
///   Pointer to the found value, or NULL if there is no matching value.
template <class T>
const T* RBTreeSet<T>::get(const ValueT& value) const
{
  const Node* node = mRoot;
  while (node) {
    if (value < node->mValue) {
      node = node->mLeft;
    } else if (node->mValue < value) {
      node = node->mRight;
    } else {
      return &node->mValue;
    }
  }
  return 0;
}


/// Returns the value from the set with the same order as the passed in value.
///
/// @param[in]  value
///   The value that defines the rang of the value to retrieve.
/// @return
///   Pointer to the found value, or NULL if there is no matching value.
template <class T>
T* RBTreeSet<T>::get(const ValueT& value)
{
  Node* node = mRoot;
  while (node) {
    if (value < node->mValue) {
      node = node->mLeft;
    } else if (node->mValue < value) {
      node = node->mRight;
    } else {
      return &node->mValue;
    }
  }
  return 0;
}


/*******************************************************************************
 * Implementation of private member functions of template class RBTreeSet.
 *******************************************************************************/

/// Rotates a sub-tree one step to the left without changing the order.
///
/// @param[in]  node
///   Pointer to the root node of the sub-tree (must not be NULL).
/// @return
///   Pointer to the new root node of the rotated sub-tree.
template <class T>
inline typename RBTreeSet<T>::Node* RBTreeSet<T>::rotateLeft(Node* node)
{
  Node* child   = node->mRight;
  node->mRight  = child->mLeft;
  child->mLeft  = node;
  child->mIsRed = node->mIsRed;
  node->mIsRed  = TRUE;
  return child;
}


/// Rotates a sub-tree one step to the right without changing the order.
///
/// @param[in]  node
///   Pointer to the root node of the sub-tree (must not be NULL).
/// @return
///   Pointer to the new root node of the rotated sub-tree.
template <class T>
inline typename RBTreeSet<T>::Node* RBTreeSet<T>::rotateRight(Node* node)
{
  Node* child   = node->mLeft;
  node->mLeft   = child->mRight;
  child->mRight = node;
  child->mIsRed = node->mIsRed;
  node->mIsRed  = TRUE;
  return child;
}


/// Flips the colors of a node and it's children. It's used to merge the
/// 2 red links of the children to one red link in the parent.
///
/// NOTE: This function can be called only, if the children of the node are
///       valid!
///
/// @param[in]  node
///   Pointer to the parent node (must nit be NULL).
template <class T>
inline void RBTreeSet<T>::colorFlip(Node* node)
{
  node->mIsRed         ^= 1;
  node->mLeft->mIsRed  ^= 1;
  node->mRight->mIsRed ^= 1;
}


/// Recursive function that inserts a node into a LLRB sub-tree. If a value
/// with the same rang already exists, it gets overwritten. While inserting
/// The properties of a LLRB tree will be preserved.
///
/// TODO: Make this function iterative using a stack.
///
/// @param[in]  node
///   Pointer to the root node of the sub-tree into which the new value should
///   be inserted (must not be NULL).
/// @param[in]  value
///   The new value to insert (gets copied).
/// @return
///   Pointer to the new root node of the sub-tree or NULL, if the allocation
///   of a new tree node failed.
template <class T>
typename RBTreeSet<T>::Node* RBTreeSet<T>::insert(Node*         node,
                                                  const ValueT& value)
{
  // if we have found a leave, add new node to it and return
  if (!node) {
    mNewNode = gNewNC Node(value);
    return mNewNode;
  }

  // traverse down the left sub-tree, if value is smaller than current node
  if (value < node->mValue) {
    node->mLeft = insert(node->mLeft, value);
  // traverse down the right sub-tree, if value is larger than current node
  } else if (node->mValue < value) {
    node->mRight = insert(node->mRight, value);
  // if we have found an already existing node with the same order, overwrite it
  } else {
    node->mValue = value;
  }

  // fix right-leaning leafs on the way up
  if (Node::isRed(node->mRight)) {
    node = rotateLeft(node);
  }

  // fix two reds in a row on the way up
  if (Node::isRed(node->mLeft) && Node::isRed(node->mLeft->mLeft)) {
    node = rotateRight(node);
  }

  // split 4-nodes on the way up
  if (Node::isRed(node->mLeft) && Node::isRed(node->mRight)) {
    colorFlip(node);
  }
  return node;
}


/// Deletes recursively all nodes of a sub-tree.
///
/// @param[in]  node
///   Pointer to the root node of the sub-tree (can be NULL).
template <class T>
void RBTreeSet<T>::deleteRecursive(Node* node)
{
  if (!node)  return;
  deleteRecursive(node->mLeft);
  deleteRecursive(node->mRight);
  gDelete(node);
}



#endif  // #ifndef __RBTREESET_IMPL_H__