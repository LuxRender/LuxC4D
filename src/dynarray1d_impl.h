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

#ifndef __DYNARRAY1D_IMPL_H__
#define __DYNARRAY1D_IMPL_H__ 1



#include "utilities.h"



/*****************************************************************************
 * Implementation of template class DynArray1D
 *****************************************************************************/

template <class T>
DynArray1D<T>::DynArray1D(SizeT size, SizeT capacity)
: mSize(0), mCapacity(0), mData(0)
{
  init(size, capacity);
}


template <class T>
DynArray1D<T>::~DynArray1D()
{
  erase();
}


template <class T>
DynArray1D<T>::DynArray1D(const DynArray1D& other)
: mSize(0), mCapacity(0), mData(0)
{
  *this = other;
}


template <class T>
DynArray1D<T>& DynArray1D<T>::operator=(const DynArray1D& other)
{
  if (init(other.mSize, other.mCapacity) && other.mSize) {
    memcpy(mData, other.mData, other.mSize*sizeof(T));
  }
  return *this;
}


template <class T>
Bool DynArray1D<T>::init(SizeT size, SizeT capacity)
{
  erase();

  if (capacity < size)  capacity = size;
  if (capacity > 0) {
    if ((mData = bNewNC T[capacity]) == 0) {
      ERRLOG("DynArray1D::Init(): could not allocate array of size " + LLongToString(capacity));
      return FALSE;
    }
    mSize     = size;
    mCapacity = capacity;
  }
  return TRUE;
}


template <class T>
Bool DynArray1D<T>::reserve(SizeT size)
{
  if (mCapacity >= size)  return TRUE;

  T* newData = bNewNC T[size];
  if (!newData) {
    ERRLOG("DynArray1D::reserve(): could not allocate new arrayof size " + LLongToString(size));
    return FALSE;
  }

  memcpy(mData, newData, mSize*sizeof(T));
  bDelete(mData);
  mData = newData;
  mCapacity = size;
  return TRUE;
}


template <class T>
Bool DynArray1D<T>::adaptCapacity()
{
  if (mSize <= 0) {
    erase();
  } else if (mSize != mCapacity) {
    T* newData = bNewNC T[mSize];
    if (!newData) {
      ERRLOG("DynArray1D::adaptCapacity(): could not allocate new array of size " + LLongToString(mSize));
      return FALSE;
    }
    memcpy(newData, mData, mSize*sizeof(T));
    bDelete(mData);
    mData = newData;
    mCapacity = mSize;
  }
  return TRUE;
}


template <class T>
void DynArray1D<T>::fill(const T& value)
{
  for (SizeT c=0; c<mSize; ++c)  mData[c] = value;
}


template <class T>
void DynArray1D<T>::clear()
{
  mSize = 0;
}


template <class T>
void DynArray1D<T>::erase()
{
  mSize = 0;
  mCapacity = 0;
  bDelete(mData);
  mData = 0;
}


template <class T>
inline SizeT DynArray1D<T>::size() const  
{ 
  return mSize; 
}


template <class T>
inline SizeT DynArray1D<T>::capacity() const  
{ 
  return mCapacity; 
}


template <class T>
inline const T& DynArray1D<T>::operator[](SizeT pos) const
{
  GeAssert((pos >= 0) && (pos < mSize));
  return mData[pos];
}


template <class T>
inline T& DynArray1D<T>::operator[](SizeT pos)
{
  GeAssert((pos >= 0) && (pos < mSize));
  return mData[pos];
}


template <class T>
inline const T& DynArray1D<T>::front() const
{
  GeAssert(mSize > 0);
  return mData[0];
}


template <class T>
inline T& DynArray1D<T>::front()
{
  GeAssert(mSize > 0);
  return mData[0];
}


template <class T>
inline const T& DynArray1D<T>::back() const
{
  GeAssert(mSize > 0);
  return mData[mSize-1];
}


template <class T>
inline T& DynArray1D<T>::back()
{
  GeAssert(mSize > 0);
  return mData[mSize-1];
}


template <class T>
inline Bool DynArray1D<T>::append()
{
  if (mSize == mCapacity) {
    if (!increaseCapacity())  return FALSE;
  }
  ++mSize;
  return TRUE;
}


template <class T>
inline Bool DynArray1D<T>::push(const T& value)
{
  if (mSize == mCapacity) {
    if (!increaseCapacity())  return FALSE;
  }
  mData[mSize] = value;
  ++mSize;
  return TRUE;
}


template <class T>
inline T DynArray1D<T>::pop()
{
  T value = back();
  if (mSize > 0)  --mSize;
  return value;
}


template <class T>
inline const T* DynArray1D<T>::arrayAddress() const  
{ 
  return mData; 
}


template <class T>
inline T* DynArray1D<T>::arrayAddress()  
{ 
  return mData; 
}


template <class T>
void DynArray1D<T>::remove(SizeT pos)
{
  GeAssert(mSize > 0);
  for (SizeT c=pos+1; c<mSize; ++c)  mData[c-1] = mData[c];
  if (mSize)  --mSize;
}



/*****************************************************************************
 * Private Functions of DynArray1D
 *****************************************************************************/

template <class T>
Bool DynArray1D<T>::increaseCapacity()
{
  SizeT newCapacity = (mCapacity < 1) ? 16 : mCapacity<<1;
  if (newCapacity <= mCapacity) {
    ERRLOG("DynArray1D::increaseCapacity(): could not increase capacity variable - end of value range reached");
    return FALSE;
  }

  T* newData = bNewNC T[newCapacity];
  if (!newData) {
    ERRLOG("DynArray1D<T>::increaseCapacity(): could not allocate new array of size " + LLongToString(newCapacity));
    return FALSE;
  }
  memcpy(newData, mData, mSize*sizeof(T));
  bDelete(mData);
  mData = newData;
  mCapacity = newCapacity;
  return TRUE;
}



#endif
