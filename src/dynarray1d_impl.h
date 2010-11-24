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

/// Constructs a new instance and allocates a memory block of size CAPACITY.
/// The accessible size will be of SIZE.
/// NOTE: Allocated memory won't get cleared (except T initialises itself in
///       the default constructor).
///
/// @param[in]  size
///   The size of the accessible range after construction.
/// @param[in]  capacity
///   The actual size of the allocated memory. If CAPACITY < SIZE, CAPACITY
///   will be set to SIZE.
template <class T>
DynArray1D<T>::DynArray1D(SizeT size, SizeT capacity)
: mSize(0), mCapacity(0), mData(0)
{
  init(size, capacity);
}


/// Destroys a DynArray1D instance and frees all allocated resources.
template <class T>
DynArray1D<T>::~DynArray1D(void)
{
  erase();
}


/// Constructs a new instance with a bitwise copy of the content of
/// another instance.
///
/// @param[in]  other
///   The other array to copy the content from.
template <class T>
DynArray1D<T>::DynArray1D(const DynArray1D& other)
: mSize(0), mCapacity(0), mData(0)
{
  *this = other;
}


/// Deallocates current content and creates a bitwise copy of the content
/// of the other instance.
///
/// @param[in]  other
///   The other array to copy the content from.
/// @return
///   A reference to this instance.
template <class T>
DynArray1D<T>& DynArray1D<T>::operator=(const DynArray1D& other)
{
  if (init(other.mSize, other.mCapacity) && other.mSize) {
    memcpy(mData, other.mData, other.mSize*sizeof(T));
  }
  return *this;
}


/// Deallocates current content and allocates a new dynamic array. The
/// behaviour is the same as described for the constructor.
///
/// @param[in]  size
///   The size of the accessible range after initialisation.
/// @param[in]  capacity
///   The actual size of the allocated memory. If CAPACITY < SIZE, CAPACITY
///   will be set to SIZE.
/// @return
///   FALSE if allocation failed, otherwise TRUE. If this operation fails
///   the array will have size and capacity of 0.
template <class T>
Bool DynArray1D<T>::init(SizeT size, SizeT capacity)
{
  // first free old content
  erase();

  // make sure that capacity >= size
  if (capacity < size)  capacity = size;

  // if the array shoulf not be empty, allocate new memory block and update
  // members
  if (capacity > 0) {
    if ((mData = bNew T[capacity]) == 0) {
      ERRLOG("DynArray1D::Init(): could not allocate array of size " + LLongToString(capacity));
      return FALSE;
    }
    mSize     = size;
    mCapacity = capacity;
  }
  return TRUE;
}


/// Makes sure that the array has a capacity of at least SIZE. If the capacity
/// is smaller a larger memory block will be allocated. Otherwise nothing
/// changes.
///
/// @param[in]  size
///   The minimum capacity the array should have.
/// @return
///   FALSE if the capacity is not large enough, but no larger memory block
///   could be allocated - TRUE otherwise. If this function fails, the old
///   content is still available.
template <class T>
Bool DynArray1D<T>::reserve(SizeT capacity)
{
  // if we have enough memory allocated, return happily
  if (mCapacity >= capacity)  return TRUE;

  // allocate new memory block
  T* newData = bNew T[capacity];
  if (!newData) {
    ERRLOG("DynArray1D::reserve(): could not allocate new arrayof size " + LLongToString(size));
    return FALSE;
  }

  // copy data into new memory block and deallocate the old one
  memcpy(mData, newData, mSize*sizeof(T));
  bDelete(mData);
  mData = newData;

  // update capacity and return
  mCapacity = capacity;
  return TRUE;
}


/// Reduces the capacity of the array to the used size.
///
/// @return
///   TRUE if successful, FALSE if not. If this function fails, the old content
///   is still available.
template <class T>
Bool DynArray1D<T>::adaptCapacity(void)
{
  // if array empty, deallocate memory block
  if (mSize <= 0) {
    erase();
  // if the array is not empty, but its size doesn't match it's capacity:
  } else if (mSize != mCapacity) {
    // allocate new memory block of correct size
    T* newData = bNew T[mSize];
    if (!newData) {
      ERRLOG("DynArray1D::adaptCapacity(): could not allocate new array of size " + LLongToString(mSize));
      return FALSE;
    }
    // copy content from old to new arrat
    memcpy(newData, mData, mSize*sizeof(T));
    // deallocate old memory and update members
    bDelete(mData);
    mData = newData;
    mCapacity = mSize;
  }
  return TRUE;
}


/// Sets all entries of the array to a specified value.
///
/// @param[in]  value
///   The value the entries will be set to.
template <class T>
void DynArray1D<T>::fill(const T& value)
{
  for (SizeT c=0; c<mSize; ++c)  mData[c] = value;
}


/// Sets the array size to 0, but doesn't change its capacity.
template <class T>
void DynArray1D<T>::clear(void)
{
  mSize = 0;
}


/// Deallocates the internal memory. Size and(!) capacity will be 0 afterwards.
template <class T>
void DynArray1D<T>::erase(void)
{
  mSize = 0;
  mCapacity = 0;
  bDelete(mData);
  mData = 0;
}


/// Returns the size of the array.
template <class T>
inline SizeT DynArray1D<T>::size(void) const  
{ 
  return mSize; 
}


/// Returns the capacity of the array.
template <class T>
inline SizeT DynArray1D<T>::capacity(void) const  
{ 
  return mCapacity; 
}


/// Constant random access operator.
///
/// @param[in]  pos
///   The position of the value to retrieve. Must be >=0 and <size()!
/// @return
///   A const reference to the value.
template <class T>
inline const T& DynArray1D<T>::operator[](SizeT pos) const
{
  GeAssert((pos >= 0) && (pos < mSize));
  return mData[pos];
}


/// Non-constant random access operator.
///
/// @param[in]  pos
///   The position of the value to retrieve. Must be >=0 and <size()!
/// @return
///   A reference to the value.
template <class T>
inline T& DynArray1D<T>::operator[](SizeT pos)
{
  GeAssert((pos >= 0) && (pos < mSize));
  return mData[pos];
}


/// Returns a constant reference to the first entry of the array.
/// (size() must be !=0)
template <class T>
inline const T& DynArray1D<T>::front(void) const
{
  GeAssert(mSize > 0);
  return mData[0];
}


/// Returns a non-constant reference to the first entry of the array.
/// (size() must be !=0)
template <class T>
inline T& DynArray1D<T>::front(void)
{
  GeAssert(mSize > 0);
  return mData[0];
}


/// Returns a constant reference to the last entry of the array.
/// (size() must be !=0)
template <class T>
inline const T& DynArray1D<T>::back(void) const
{
  GeAssert(mSize > 0);
  return mData[mSize-1];
}


/// Returns a non-constant reference to the last entry of the array.
/// (size() must be !=0)
template <class T>
inline T& DynArray1D<T>::back(void)
{
  GeAssert(mSize > 0);
  return mData[mSize-1];
}


/// Increases the size of the array by 1.
///
/// @return
///   TRUE if successful, FALSE otherwise.
template <class T>
inline Bool DynArray1D<T>::append(void)
{
  if (mSize == mCapacity) {
    if (!increaseCapacity())  return FALSE;
  }
  ++mSize;
  return TRUE;
}


/// Copies a value to the end of the array. The array size will be increased
/// by 1.
///
/// @param[in]  value
///   The value to copy.
/// @return
///   TRUE if successful, FALSE if not.
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


/// Removes the last entry from the array and returns it's value.
template <class T>
inline T DynArray1D<T>::pop(void)
{
  T value = back();
  if (mSize > 0)  --mSize;
  return value;
}


/// Returns a constant pointer to the internal data.
template <class T>
inline const T* DynArray1D<T>::arrayAddress(void) const  
{ 
  return mData; 
}


/// Returns a pointer to the internal data.
template <class T>
inline T* DynArray1D<T>::arrayAddress(void)  
{ 
  return mData; 
}


/// Removes the entry at position POS. Size() will be one less afterwards.
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

/// Helper function that increases the capacity. It's is called, when the
/// current capacity is exhausted.
///
/// @return
///   TRUE if successful, FALSE if not.
template <class T>
Bool DynArray1D<T>::increaseCapacity(void)
{
  // calculate and check new capacity
  SizeT newCapacity = (mCapacity < 1) ? 16 : mCapacity<<1;
  if (newCapacity <= mCapacity) {
    ERRLOG("DynArray1D::increaseCapacity(): could not increase capacity variable - end of value range reached");
    return FALSE;
  }

  // allocate new (larger) memory block
  T* newData = bNew T[newCapacity];
  if (!newData) {
    ERRLOG("DynArray1D<T>::increaseCapacity(): could not allocate new array of size " + LLongToString(newCapacity));
    return FALSE;
  }

  // copy from to new memory
  memcpy(newData, mData, mSize*sizeof(T));

  // deallocate old memory, update members and return
  bDelete(mData);
  mData = newData;
  mCapacity = newCapacity;
  return TRUE;
}



#endif
