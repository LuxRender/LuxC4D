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

#ifndef __RBTREEMAP_IMPL_H__
#define __RBTREEMAP_IMPL_H__ 1



/*******************************************************************************
 * Implementation of public member functions of template class RBTreeMap.
 *******************************************************************************/

/// Constructs a new (empty) red-black tree.
template <class K, class T>
RBTreeMap<K,T>::RBTreeMap(void)
: mRoot(0), mSize(0)
{}


/// Destroys a red-black tree and deallocates its resources.
template <class K, class T>
RBTreeMap<K,T>::~RBTreeMap(void)
{
  erase();
}


/// Deallocates all resources of a red-black tree. The tree will be empty
/// afterwards.
template <class K, class T>
void RBTreeMap<K,T>::erase(void)
{
  RBTreeMap<K,T>::deleteRecursive(mRoot);
  mRoot = 0;
  mSize = 0;
}


/// Returns the number of entries of red-black tree.
template <class K, class T>
inline typename RBTreeMap<K,T>::SizeT RBTreeMap<K,T>::size(void) const
{
  return mSize;
}


/// Adds a new key/entry pair to the LLRB set. If there was already one with
/// the same key, we overwrite it.
///
/// @param[in]  key
///   The key of the value to add to the rb-tree.
/// @param[in]  value
///   The value to add.
/// @return
///   NULL if we ran out of memory, otherwise the pointer to the stored value.
template <class K, class T>
typename RBTreeMap<K,T>::ValueT* RBTreeMap<K,T>::add(const KeyT& key,
                                                     const ValueT& value)
{
  mNewNode = 0;
  mRoot = insert(mRoot, key, value);
  ++mSize;
  return mNewNode ? &mNewNode->mValue : 0;
}


/// Returns the (const) value for a specified key from the map.
///
/// @param[in]  key
///   The key for which the value will be looked up.
/// @return
///   Pointer to the found value, or NULL if there is no matching value.
template <class K, class T>
const typename RBTreeMap<K,T>::ValueT* RBTreeMap<K,T>::get(const KeyT& key) const
{
  const Node* node = mRoot;
  while (node) {
    if (key < node->mKey) {
      node = node->mLeft;
    } else if (node->mKey < key) {
      node = node->mRight;
    } else {
      return &node->mValue;
    }
  }
  return 0;
}


/// Returns the (non-const) value for a specified key from the map.
///
/// @param[in]  key
///   The key for which the value will be looked up.
/// @return
///   Pointer to the found value, or NULL if there is no matching value.
template <class K, class T>
typename RBTreeMap<K,T>::ValueT* RBTreeMap<K,T>::get(const KeyT& key)
{
  Node* node = mRoot;
  while (node) {
    if (key < node->mKey) {
      node = node->mLeft;
    } else if (node->mKey < key) {
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
template <class K, class T>
inline typename RBTreeMap<K,T>::Node* RBTreeMap<K,T>::rotateLeft(Node* node)
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
template <class K, class T>
inline typename RBTreeMap<K,T>::Node* RBTreeMap<K,T>::rotateRight(Node* node)
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
template <class K, class T>
inline void RBTreeMap<K,T>::colorFlip(Node* node)
{
  node->mIsRed         ^= 1;
  node->mLeft->mIsRed  ^= 1;
  node->mRight->mIsRed ^= 1;
}


/// Recursive function that inserts a node into a LLRB sub-tree. If a node with
/// the same key already exists, it gets overwritten.
/// The properties of a LLRB tree will be preserved.
///
/// TODO: Make this function iterative using a stack.
///
/// @param[in]  node
///   Pointer to the root node of the sub-tree into which the new key/value pair
///   should be inserted (must not be NULL initially).
/// @param[in]  key
///   The key for which the value will be inserted.
/// @param[in]  value
///   The new value to insert (gets copied).
/// @return
///   Pointer to the new root node of the sub-tree or NULL, if the allocation
///   of a new tree node failed.
template <class K, class T>
typename RBTreeMap<K,T>::Node* RBTreeMap<K,T>::insert(Node*         node,
                                                      const KeyT&   key,
                                                      const ValueT& value)
{
  // if we have found a leave, add new node to it and return
  if (!node) {
    mNewNode = gNew Node(key, value);
    return mNewNode;
  }

  // traverse down the left sub-tree, if key is smaller than the key of the
  // current node
  if (key < node->mKey) {
    node->mLeft = insert(node->mLeft, key, value);
  // traverse down the right sub-tree, if key is larger than the key of the
  // current node
  } else if (node->mKey < key) {
    node->mRight = insert(node->mRight, key, value);
  // if we have found an already existing node with the same key, overwrite it
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
template <class K, class T>
void RBTreeMap<K,T>::deleteRecursive(Node* node)
{
  if (!node)  return;
  deleteRecursive(node->mLeft);
  deleteRecursive(node->mRight);
  gDelete(node);
}



#endif  // #ifndef __RBTREEMAP_IMPL_H__
