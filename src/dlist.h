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

#ifndef __DLIST_H__
#define __DLIST_H__  1



#include <c4d.h>

#include "utilities.h"



/* Class prototypes */
template <class T> class DListIterator;
template <class T> class ConstDListIterator;



///
template<class T>
class DList
{
  friend class ConstDListIterator<T>;
  friend class DListIterator<T>;


  public:

    typedef ConstDListIterator<T> ConstIteratorT;
    typedef DListIterator<T>      IteratorT;


    DList();
    ~DList();

    DList(const DList& other);
    DList& operator=(const DList& other);

    Bool appendFront();
    Bool appendBack();
    void removeFront();
    void removeBack();

    Bool pushFront(const T& value);
    Bool pushBack(const T& value);

    Bool pushFront(ConstIteratorT& start,
                   ConstIteratorT& stop);
    Bool pushBack(ConstIteratorT& start,
                  ConstIteratorT& stop);

    Bool pushFront(const DList& other);
    Bool pushBack(const DList& other);

    T popFront();
    T popBack();

    void remove(IteratorT& iter);

    void adopt(IteratorT& entry);
    void adopt(IteratorT& entry,
               IteratorT& at);

    void adoptFront(DList& other);
    void adoptBack(DList& other);

    inline SizeT size() const;

    void erase();

    inline const T& front() const;
    inline T&       front();
    inline const T& back() const;
    inline T&       back();

    inline ConstIteratorT begin() const;
    inline IteratorT      begin();
    inline ConstIteratorT end() const;
    inline IteratorT      end();


private:

    struct Node {
      Node* mPrevious;
      Node* mNext;
      T     mData;

      Node(Node* previous,
           Node* next)
      : mPrevious(previous),
        mNext(next)
      {}

      Node(Node*    previous,
           Node*    next,
           const T& value)
      : mPrevious(previous),
        mNext(next),
        mData(value)
      {}
    };


    Node* mFront;
    Node* mBack;
    SizeT mSize;
};



/**************************************************************************//*!
*//***************************************************************************/

template <class T>
class DListIterator
{
  friend class DList<T>;
  friend class ConstDListIterator<T>;


  public:

    typedef DList<T> ContainerT;


    DListIterator();

    inline Bool isValid() const;
    inline Bool isFront() const;
    inline Bool isBack() const;

    inline ContainerT* container();
    
    inline T& operator*() const;
    inline T* operator->() const;
    inline DListIterator& operator++();
    inline DListIterator& operator--();

    inline Bool operator==(const ConstDListIterator<T>& other) const;
    inline Bool operator==(const DListIterator<T>& other) const;
    inline Bool operator!=(const ConstDListIterator<T>& other) const;
    inline Bool operator!=(const DListIterator<T>& other) const;


  private:

    typedef typename ContainerT::Node  NodeT;

    ContainerT* mList;
    NodeT*      mNode;


    DListIterator(ContainerT& list,
                  NodeT*      node);
};



/**************************************************************************//*!
*//***************************************************************************/

template <class T>
class ConstDListIterator
{
  friend class DList<T>;
  friend class DListIterator<T>;


  public:

    typedef DList<T> ContainerT;


    ConstDListIterator();

    ConstDListIterator(const DListIterator<T>& other);
    ConstDListIterator& operator=(const DListIterator<T>& other);

    inline Bool isValid() const;
    inline Bool isFront() const;
    inline Bool isBack() const;

    inline const ContainerT* container() const;

    inline const T& operator*() const;
    inline const T* operator->() const;
    inline ConstDListIterator& operator++();
    inline ConstDListIterator& operator--();

    inline Bool operator==(const ConstDListIterator<T>& other) const;
    inline Bool operator==(const DListIterator<T>& other) const;
    inline Bool operator!=(const ConstDListIterator<T>& other) const;
    inline Bool operator!=(const DListIterator<T>& other) const;


  private:

    typedef typename ContainerT::Node  NodeT;

    const ContainerT* mList;
    const NodeT*      mNode;


    ConstDListIterator(const ContainerT& list,
                       const NodeT*      node);
};

#include "DList_impl.h"



#endif  // #ifndef __DLIST_H__
