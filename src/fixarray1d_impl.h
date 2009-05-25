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

#ifndef __FIXARRAY1D_IMPL_H__
#define __FIXARRAY1D_IMPL_H__ 1



/*******************************************************************************
 * Implementation of template class FixArray1D
 *******************************************************************************/

/// Constructs a new instance. Allocated memory won't get cleared (except
/// T initialises itself in the default constructor).
///
/// @param[in]  size
///   If set to !=0 the internal array will be allocated.
template <class T>
FixArray1D<T>::FixArray1D(SizeT size)
: mSize(0), mData(0)
{
  init(size);
}


/// Destroys a FixArray1D instance and frees all allocated resources.
template <class T>
FixArray1D<T>::~FixArray1D(void)
{
  erase();
}


/// Constructs a new instance with a bitwise copy of the content of
/// another instance.
///
/// @param[in]  other
///   The other array to copy the content from.
template <class T>
FixArray1D<T>::FixArray1D(const FixArray1D& other)
: mSize(0), mData(0)
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
FixArray1D<T>& FixArray1D<T>::operator=(const FixArray1D& other)
{
  if (init(other.mSize) && other.mSize) {
    memcpy(mData, other.mData, other.mSize*sizeof(T));
  }
  return *this;
}


/// Deallocates current content and allocates a new array. The new memory
/// won't get cleared (except T initialises itself in the default
/// constructor).
///
/// @param[in]  size
///   The size of the new array.
/// @return
///   FALSE if allocation failed, otherwise TRUE.
template <class T>
Bool FixArray1D<T>::init(SizeT size)
{
  GeAssert(size >= 0);
  erase();
  if (size > 0) {
    if ((mData = bNewNC T[size]) == 0) {
      ERRLOG("FixArray1D::init(): could not allocate array of size " + LLongToString(size));
      return FALSE;
    }
    mSize = size;
  }
  return TRUE;
}


/// Copies a value to all entries of array.
///
/// @param[in]  value
///   The value to copy.
template <class T>
void FixArray1D<T>::fill(const T& value)
{
  for (SizeT c=0; c<mSize; ++c)  mData[c] = value;
}


/// Sets all bytes of the array to 0.
template <class T>
void FixArray1D<T>::fillWithZero(void)
{
  if (mSize) {
    memset(mData, 0, sizeof(T)*mSize);
  }
}


/// Deallocates the current content.
template <class T>
void FixArray1D<T>::erase(void)
{
  bDelete(mData);
  mSize = 0;
}


/// Returns the size of the array.
template <class T>
inline SizeT FixArray1D<T>::size(void) const  
{ 
  return mSize; 
}


/// Constant random access operator.
///
/// @param[in]  pos
///   The position of the value to retrieve. Must be >=0 and <size()!
/// @return
///   A constant reference to the value.
template <class T>
inline const T& FixArray1D<T>::operator[](SizeT pos) const
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
inline T& FixArray1D<T>::operator[](SizeT pos)
{
  GeAssert((pos >= 0) && (pos < mSize));
  return mData[pos];
}


/// Returns a constant reference to the first entry of the array.
/// (size() must be !=0)
template <class T>
inline const T& FixArray1D<T>::front(void) const
{
  GeAssert(mSize >= 0);
  return mData[0];
}


/// Returns a non-constant reference to the first entry of the array.
/// (size() must be !=0)
template <class T>
inline T& FixArray1D<T>::front(void)
{
  GeAssert(mSize >= 0);
  return mData[0];
}


/// Returns a constant reference to the last entry of the array.
/// (size() must be !=0)
template <class T>
inline const T& FixArray1D<T>::back(void) const
{
  GeAssert(mSize >= 0);
  return mData[mSize-1];
}


/// Returns a non-constant reference to the last entry of the array.
/// (size() must be !=0)
template <class T>
inline T& FixArray1D<T>::back(void)
{
  GeAssert(mSize >= 0);
  return mData[mSize-1];
}


/// Returns a constant pointer to the internal data.
template <class T>
inline const T* FixArray1D<T>::arrayAddress(void) const  
{ 
  return mData; 
}


/// Returns a pointer to the internal data.
template <class T>
inline T* FixArray1D<T>::arrayAddress(void)  
{ 
  return mData; 
}


/// Allows you to specify the internal data. After that the instance owns
/// the memory. You can pass in a NULL pointer, but the specified size must
/// be 0 then.
///
/// @param[in]  data
///   The pointer to the array to pass to instance.
/// @param[in]  size
///   The size of the passed in array.
template <class T>
void FixArray1D<T>::setArrayAddress(T* data, SizeT size)
{
  GeAssert((!data && !size) || (data && size));
  erase();
  mData = data;
  mSize = size;
}


/// Deallocates the current content and adopts the data from another instance.
/// The memory will be owned by this instance and the other one will be empty
/// afterwards.
///
/// @param[in]  other
///   The instance from where the data shall be adapted from.
template <class T>
void FixArray1D<T>::adopt(FixArray1D& other)
{
  erase();
  mData = other.mData;
  mSize = other.mSize;
  other.mData = 0;
  other.mSize = 0;
}



#endif
