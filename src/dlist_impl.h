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

#ifndef __DLIST_IMPL_H__
#define __DLIST_IMPL_H__  1



#include "utilities.h"



/*****************************************************************************
 * Implementation of DList
 *****************************************************************************/

template <class T>
DList<T>::DList()
: mFront(0),
  mBack(0),
  mSize(0)
{}


template <class T>
DList<T>::~DList()
{
  erase();
}


template <class T>
DList<T>::DList(const DList& other)
: mFront(0),
  mBack(0),
  mSize(0)
{
  *this = other;
}


template <class T>
DList<T>& DList<T>::operator=(const DList& other)
{
  erase();
  for (ConstIteratorT iter=other.begin(); iter.isValid(); ++iter) {
    if (!pushBack(*iter))  break;
  }
  return *this;
}


template <class T>
Bool DList<T>::appendFront()
{
  if (mFront) {
    mFront->mPrevious = gNewNC Node(0, mFront);
    if (!mFront->mPrevious) {
      ERRLOG("DList::appendFront(): could not allocate new node for list");
      return FALSE;
    }
    mFront = mFront->mPrevious;
  } else {
    mFront = mBack = gNewNC Node(0, 0);
    if (!mFront) {
      ERRLOG("DList::appendFront(): could not allocate new node for list");
      return FALSE;
    }
  }
  ++mSize;
  return TRUE;
}


template <class T>
Bool DList<T>::appendBack()
{
  if (mBack) {
    mBack->mNext = gNewNC Node(mBack, 0);
    if (!mBack->mNext) {
      ERRLOG("DList::appendBack(): could not allocate new node for list");
      return FALSE;
    }
    mBack = mBack->mNext;
  } else {
    mFront = mBack = gNewNC Node(0, 0);
    if (!mFront) {
      ERRLOG("DList::appendBack(): could not allocate new node for list");
      return FALSE;
    }
  }
  ++mSize;
  return TRUE;
}


template <class T>
void DList<T>::removeFront()
{
  if (!mFront) { return; }

  if (mFront == mBack) {
    gDelete(mFront);
    mBack = 0;
  } else {
    mFront = mFront->mNext;
    gDelete(mFront->mPrevious);
  }
  --mSize;
}


template <class T>
void DList<T>::removeBack()
{
  if (!mBack) { return; }

  if (mFront == mBack) {
    gDelete(mFront);
    mBack = 0;
  } else {
    mBack = mBack->mPrevious;
    gDelete(mBack->mNext);
  }
  --mSize;
}


template <class T>
Bool DList<T>::pushFront(const T& value)
{
  if (mFront) {
    mFront->mPrevious = gNewNC Node(0, mFront, value);
    if (!mFront->mPrevious) {
      ERRLOG("DList::pushFront(): could not allocate new node for list");
      return FALSE;
    }
    mFront = mFront->mPrevious;
  } else {
    mFront = mBack = gNewNC Node(0, 0, value);
    if (!mFront) {
      ERRLOG("DList::pushFront(): could not allocate new node for list");
      return FALSE;
    }
  }
  ++mSize;
  return TRUE;
}


template <class T>
Bool DList<T>::pushBack(const T& value)
{
  if (mBack) {
    mBack->mNext = gNewNC Node(mBack, 0, value);
    if (!mBack->mNext) {
      ERRLOG("DList::pushBack(): could not allocate new node for list");
      return FALSE;
    }
    mBack = mBack->mNext;
  } else {
    mFront = mBack = gNewNC Node(0, 0, value);
    if (!mFront) {
      ERRLOG("DList::pushBack(): could not allocate new node for list");
      return FALSE;
    }
  }
  ++mSize;
  return TRUE;
}


template <class T>
Bool DList<T>::pushFront(ConstIteratorT& start,
                         ConstIteratorT& stop)
{
  GeAssert(start.isValid());
  GeAssert(stop.isValid());

  for (const Node* node=start.mNode; node; node=node->mNext) {
    if (!pushFront(node->mData)) { return FALSE; }
    if (node == stop.mNode) { return TRUE; }
  }
  ERRLOG("DList::pushFront(): stop iterator wasn't reached");
  return FALSE;
}


template <class T>
Bool DList<T>::pushBack(ConstIteratorT& start,
                        ConstIteratorT& stop)
{
  GeAssert(start.isValid());
  GeAssert(stop.isValid());

  for (const Node* node=start.mNode; node; node=node->mNext) {
    if (!pushBack(node->mData)) { return FALSE; }
    if (node == stop.mNode) { return TRUE; }
  }
  ERRLOG("DList::pushBack(): stop iterator wasn't reached");
  return FALSE;
}


template <class T>
Bool DList<T>::pushFront(const DList& other)
{
  for (const Node* node=other.mFront; node; node=node->mNext) {
    if (!pushFront(node->mData)) { return FALSE; }
  }
  return TRUE;
}


template <class T>
Bool DList<T>::pushBack(const DList& other)
{
  for (const Node* node=other.mFront; node; node=node->mNext) {
    if (!pushBack(node->mData)) { return FALSE; }
  }
  return TRUE;
}


template <class T>
T DList<T>::popFront()
{
  GeAssert(mFront);

  T retValue = mFront->mData;
  if (mFront == mBack) {
    gDelete(mFront);
    mBack = 0;
  } else {
    mFront = mFront->mNext;
    gDelete(mFront->mPrevious);
  }
  --mSize;
  return retValue;
}


template <class T>
T DList<T>::popBack()
{
  GeAssert(mBack);

  T retValue = mBack->mData;
  if (mFront == mBack) {
    gDelete(mFront);
    mBack = 0;
  } else {
    mBack = mBack->mPrevious;
    gDelete(mBack->mNext);
  }
  --mSize;
  return retValue;
}


template <class T>
void DList<T>::remove(IteratorT& iter)
{
  GeAssert(iter.mNode);
  GeAssert(iter.mList == this);

  Node* node(iter.mNode);
  ++iter;
  if (node->mPrevious) {
    node->mPrevious->mNext = node->mNext;
  } else {
    mFront = node->mNext;
  }
  if (node->mNext) {
    node->mNext->mPrevious = node->mPrevious;
  } else {
    mBack = node->mPrevious;
  }
  gDelete(node);
  --iter.mList->mSize;
}


template <class T>
void DList<T>::adopt(IteratorT& entry)
{
  GeAssert(entry.mNode);
  GeAssert(entry.mList);
  GeAssert(entry.mList != this);

  Node* node(entry.mNode);

  // move node out of source list
  if (node->mPrevious) {
    node->mPrevious->mNext = node->mNext;
  } else {
    entry.mList->mFront = node->mNext;
  }
  if (node->mNext) {
    node->mNext->mPrevious = node->mPrevious;
  } else {
    entry.mList->mBack = node->mPrevious;
  }
  --entry.mList->mSize;

  // append node to list
  entry->mList = this;
  node->mPrevious = mBack;
  node->mNext = 0;
  if (mBack) {
    mBack->mNext = node;
    mBack = node;
  } else {
    mFront = mBack = node;
  }
  ++mSize;
}


template <class T>
void DList<T>::adopt(IteratorT& entry,
                     IteratorT& at)
{
  GeAssert(entry.mNode);
  GeAssert(entry.mList);
  GeAssert(entry.mList != this);
  GeAssert((at.mList == this) || !at.mNode);

  Node* node(entry.mNode);
  Node* postNode(at.mNode);

  // move node out of source list
  ++entry;
  if (node->mPrevious) {
    node->mPrevious->mNext = node->mNext;
  } else {
    entry.mList->mFront = node->mNext;
  }
  if (node->mNext) {
    node->mNext->mPrevious = node->mPrevious;
  } else {
    entry.mList->mBack = node->mPrevious;
  }
  --entry.mList->mSize;

  // append node to list
  if (!postNode) {
    node->mPrevious = mBack;
    node->mNext = 0;
    if (mBack) {
      mBack->mNext = node;
      mBack = node;
    } else {
      mFront = mBack = node;
    }
  } else {
    node->mPrevious = postNode->mPrevious;
    node->mNext = postNode;
    postNode->mPrevious = node;
    if (node->mPrevious) {
      postNode->mPrevious->mNext = node;
    } else {
      mFront = node;
    }
    if (!node->mNext)  mBack = node;
  }
  ++mSize;
}


template <class T>
void DList<T>::adoptFront(DList& other)
{
  GeAssert(&other != this);

  if (!other.mFront) { return; }
  if (mFront) {
    mFront->mPrevious = other.mBack;
    other.mBack->mNext = mFront;
  } else
    mBack = other.mBack;
  mFront = other.mFront;
  mSize += other.mSize;
  other.mFront = 0;
  other.mBack = 0;
  other.mSize = 0;
}


template <class T>
void DList<T>::adoptBack(DList& other)
{
  GeAssert(&other != this);

  if (!other.mFront) { return; }
  if (mBack) {
    mBack->mNext = other.mFront;
    other.mFront->mPrevious = mBack;
  } else
    mFront = other.mFront;
  mBack = other.mBack;
  mSize += other.mSize;
  other.mFront = 0;
  other.mBack = 0;
  other.mSize = 0;
}


template <class T>
inline SizeT DList<T>::size() const
{
  return mSize;
}


template <class T>
void DList<T>::erase()
{
  Node* node(mFront);
  Node* temp;
  while (node) {
    temp = node->mNext;
    gDelete(node);
    node = temp;
  }
  mFront = 0;
  mBack = 0;
  mSize = 0;
}


template <class T>
inline const T& DList<T>::front() const
{
  GeAssert(mFront);
  return mFront->mData;
}


template <class T>
inline T& DList<T>::front()
{
  GeAssert(mFront);
  return mFront->mData;
}


template <class T>
inline const T& DList<T>::back() const
{
  GeAssert(mBack);
  return mBack->mData;
}


template <class T>
inline T& DList<T>::back()
{
  GeAssert(mBack);
  return mBack->mData;
}


template <class T>
inline typename DList<T>::ConstIteratorT DList<T>::begin() const
{
  return ConstIteratorT(*this, mFront);
}


template <class T>
inline typename DList<T>::IteratorT DList<T>::begin()
{
  return IteratorT(*this, mFront);
}


template <class T>
inline typename DList<T>::ConstIteratorT DList<T>::end() const
{
  return ConstIteratorT(*this, mBack);
}


template <class T>
inline typename DList<T>::IteratorT DList<T>::end()
{
  return IteratorT(*this, mBack);
}



/*****************************************************************************
 * Implementation of DListIterator
 *****************************************************************************/

template <class T>
DListIterator<T>::DListIterator()
: mList(0), mNode(0)
{}


template <class T>
DListIterator<T>::DListIterator(ContainerT& list,
                                NodeT*      node)
: mList(&list), mNode(node)
{}


template <class T>
inline Bool DListIterator<T>::isValid() const
{
  return (mNode != 0);
}


template <class T>
inline Bool DListIterator<T>::isFront() const
{
  if (!mList) { return FALSE; }
  return (mNode == mList->mFront);
}


template <class T>
inline Bool DListIterator<T>::isBack() const
{
  if (!mList) { return FALSE; }
  return (mNode == mList->mBack);
}


template <class T>
inline typename DListIterator<T>::ContainerT* DListIterator<T>::container()
{
  return mList;
}


template <class T>
inline T& DListIterator<T>::operator*() const
{
  GeAssert(isValid());
  return mNode->mData;
}


template <class T>
inline T* DListIterator<T>::operator->() const
{
  GeAssert(isValid());
  return &(mNode->mData);
}


template <class T>
inline DListIterator<T>& DListIterator<T>::operator++()
{
  GeAssert(isValid());
  mNode = mNode->mNext;
  return *this;
}


template <class T>
inline DListIterator<T>& DListIterator<T>::operator--()
{
  GeAssert(isValid());
  mNode = mNode->mPrevious;
  return *this;
}


template <class T>
inline Bool DListIterator<T>::operator==(const ConstDListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode == mNode);
}


template <class T>
inline Bool DListIterator<T>::operator==(const DListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode == mNode);
}


template <class T>
inline Bool DListIterator<T>::operator!=(const ConstDListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode != mNode);
}


template <class T>
inline Bool DListIterator<T>::operator!=(const DListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode != mNode);
}



/*****************************************************************************
 * Implementation of ConstDListIterator
 *****************************************************************************/

template <class T>
ConstDListIterator<T>::ConstDListIterator()
: mList(0),mNode(0)
{}


template <class T>
ConstDListIterator<T>::ConstDListIterator(const ContainerT& list,
                                          const NodeT*      node)
: mList(&list), mNode(node)
{}


template <class T>
ConstDListIterator<T>::ConstDListIterator(const DListIterator<T>& other)
: mList(other.mList), mNode(other.mNode)
{}


template <class T>
ConstDListIterator<T>& ConstDListIterator<T>::operator=(const DListIterator<T>& other)
{
  mList = other.mList;
  mNode = other.mNode;
  return *this;
}


template <class T>
inline Bool ConstDListIterator<T>::isValid() const
{
  return (mNode != 0);
}


template <class T>
inline Bool ConstDListIterator<T>::isFront() const
{
  if (!mList) { return FALSE; }
  return (mNode == mList->mFront);
}


template <class T>
inline Bool ConstDListIterator<T>::isBack() const
{
  if (!mList) { return FALSE; }
  return (mNode == mList->mBack);
}


template <class T>
inline const typename ConstDListIterator<T>::ContainerT* ConstDListIterator<T>::container() const
{
  return mList;
}


template <class T>
inline const T& ConstDListIterator<T>::operator*() const
{
  GeAssert(isValid());
  return mNode->mData;
}


template <class T>
inline const T* ConstDListIterator<T>::operator->() const
{
  GeAssert(isValid());
  return &(mNode->mData);
}


template <class T>
inline ConstDListIterator<T>& ConstDListIterator<T>::operator++()
{
  GeAssert(isValid());
  mNode = mNode->mNext;
  return *this;
}


template <class T>
inline ConstDListIterator<T>& ConstDListIterator<T>::operator--()
{
  GeAssert(isValid());
  mNode = mNode->mPrevious;
  return *this;
}


template <class T>
inline Bool ConstDListIterator<T>::operator==(const ConstDListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode != mNode);
}


template <class T>
inline Bool ConstDListIterator<T>::operator==(const DListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode != mNode);
}


template <class T>
inline Bool ConstDListIterator<T>::operator!=(const ConstDListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode != mNode);
}


template <class T>
inline Bool ConstDListIterator<T>::operator!=(const DListIterator<T>& other) const
{
  GeAssert(mList == other.mList);
  return (other.mNode != mNode);
}



#endif  // #ifndef __DLIST_IMPL_H__
