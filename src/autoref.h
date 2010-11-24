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

#ifndef __AUTOREF_H__
#define __AUTOREF_H__ 1



#include "utilities.h"



/***************************************************************************//*!
  This class is to be used for reference counting and automatic deallocation.
  Such a handle object behaves like a normal reference plus assignment operator.
  Each copy operation from one AutoRef to another one increases the internal
  counter and each AutoRef destruction decreases the internal counter. If the
  counter reaches zero the memory pointed by the internal pointer is deallocated.

  The array template parameter is needed to differ between object (de)allocation
  and array (de)allocation.
*//****************************************************************************/
template<class T, Bool array=FALSE>
class AutoRef
{
  public:

    inline AutoRef(void);
    inline AutoRef(T* data);
    inline AutoRef(const AutoRef& src);
    inline ~AutoRef(void);

    Bool allocate(SizeT nmb=1);
    void clear(void);

    inline Bool bad(void) const;
    inline ULONG count(void) const;

    inline operator Bool(void) const;

    inline const T& operator*(void) const;
    inline T&       operator*(void);

    inline const T* operator->(void) const;
    inline T*       operator->(void);

    inline const T* ptr(void) const;
    inline T*       ptr(void);

    AutoRef& operator=(T* data);
    AutoRef& operator=(const AutoRef& src);
    
    inline bool operator==(const AutoRef& other) const;

    template<class trgT>
    inline operator AutoRef<trgT,array>(void) const;

    /// NEVER EVER use this constructor directly!!!!!
    ///
    /// I had to define this function to allow type up-casting. The problem was,
    /// that different template classes are handled as different types that have
    /// no access to private/protected members of objects with other template
    /// parameters.
    inline AutoRef(T* data, SizeT* count);


  private :

    T*     mData;
    SizeT* mRefCount;
};



/*****************************************************************************
 * Inlined Functions of AutoRef
 *****************************************************************************/

/// Constructs an empty AutoRef object. Bad() returns TRUE afterwards.
template<class T, Bool array>
inline AutoRef<T,array>::AutoRef(void)
: mData(0), mRefCount(0)
{}


/// Constructs a new AutoRef object and initialises it with a pointer.
///
/// @param[in]  data
///   Pointer to the data the AutoRef should be the reference of.
template<class T, Bool array>
inline AutoRef<T,array>::AutoRef(T* data)
{
  if (data) {
    mRefCount = gNew SizeT(1);
    if (!mRefCount) {
      ERRLOG_RETURN("AutoRef::AutoRef(): Could not allocate reference counter.");
    }
    mData = data;
  } else {
    mData = 0;
    mRefCount = 0;
  }
}


/// Copy constructor. Can be used for copying from one AutoRef to another
/// one. The reference counter is increased, if the source AutoRef referenced
/// some data.
///
/// @param[in]  src
///   Reference of the other instance to copy from.
template<class T, Bool array>
inline AutoRef<T,array>::AutoRef(const AutoRef& other)
: mData(other.mData), mRefCount(other.mRefCount)
{
  if (mRefCount) { ++(*mRefCount); }
}


/// Destructs the handle object. Before construction the reference counter
/// is decreased. If it reaches zero the internal referenced data is
/// deallocated.
template<class T, Bool array>
inline AutoRef<T,array>::~AutoRef()
{
  clear();
}


/// Allocates new object(s) T on heap.
///
/// @param[in]  nmb
///   The number of objects to create. If the template parameter ARRAY is FALSE,
///   the parameter NMB will be ignored.
/// @return
///   TRUE if successful (bad() will return FALSE), FALSE otherwise (bad() will
///   return TRUE).
template<class T, Bool array>
Bool AutoRef<T,array>::allocate(SizeT nmb)
{
  clear();
  if (array) {
    if (!(mData = gNew T[nmb])) {
      ERRLOG_RETURN_VALUE(FALSE, "AutoRef::allocate(): Could not allocate data array.");
    }
  } else {
    if (!(mData = gNew T)) {
      ERRLOG_RETURN_VALUE(FALSE, "AutoRef::allocate(): Could not allocate data.");
    }
  }
  if (!(mRefCount = gNew SizeT(1))) {
    if (array) {
      bDelete(mData);
    } else {
      gDelete(mData);
    }
    ERRLOG_RETURN_VALUE(FALSE, "AutoRef::allocate(): Could not allocate reference counter.");
  }
  return TRUE;
}


/// Decreases the reference counter and then releases this AutoRef from the
/// referenced object. If the reference counter reaches zero the referenced
/// data is deallocated. The internal data pointer is always NULL afterwards.
template<class T, Bool array>
void AutoRef<T,array>::clear(void)
{
  if (mRefCount) {
    if (--(*mRefCount) == 0) {
      gDelete(mRefCount);
      if (array) {
        bDelete(mData);
      } else {
        gDelete(mData);
      }
    }
  }
}


/// Returns TRUE if the reference counter is not set and the handle is free.
template<class T, Bool array>
inline Bool AutoRef<T,array>::bad(void) const
{
  return (!mRefCount);
}


/// Returns the internal reference count.
template<class T, Bool array>
inline ULONG AutoRef<T,array>::count(void) const
{
  return (mRefCount) ? (*mRefCount) : 0;
}



/// Returns TRUE if if the internal data pointer is not NULL.
template<class T, Bool array>
inline AutoRef<T,array>::operator Bool(void) const
{
  return !bad();
}


/// Constant de-reference operator.
template<class T, Bool array>
inline const T& AutoRef<T,array>::operator*(void) const
{
  GeAssert(mData);
  return *mData;
}


/// Non-constant de-reference operator.
template<class T, Bool array>
inline T& AutoRef<T,array>::operator*(void)
{
  GeAssert(mData);
  return *mData;
}


/// Constant "pointer-to-structure-member" operator.
template<class T, Bool array>
inline const T* AutoRef<T,array>::operator->(void) const
{
  GeAssert(mData);
  return mData;
}


/// Non-constant "pointer-to-structure-memeber" operator.
template<class T, Bool array>
inline T* AutoRef<T,array>::operator->(void)
{
  GeAssert(mData);
  return mData;
}


/// Returns the internally stored pointer as constant.
template<class T, Bool array>
inline const T* AutoRef<T,array>::ptr(void) const
{
  return mData;
}


/// Returns the internally stored pointer.
template<class T, Bool array>
inline T* AutoRef<T,array>::ptr(void)
{
  return mData;
}


/// Copy operator. Behaves like the copy constructor.
template<class T, Bool array>
AutoRef<T,array>& AutoRef<T,array>::operator=(const AutoRef& src)
{
  if (src.mData != mData) {
    clear();
    mData     = src.mData;
    mRefCount = src.mRefCount;
    if (mRefCount) { ++(*mRefCount); }
  }
  return *this;
}


/// Initialises the AutoRef object with a new reference. If the operation fails,
/// the AutoRef is not referencing the data. I.e. the data has to be deallocated
/// by the caller.
///
/// @param[in]  data
///   The pointer the handle shoud be assigned to.
/// @return
///   A reference of this AutoRef. If DATA is NULL or the assignment operation
///   failed (because the a new reference counter could not be allocated), the
///   AutoRef will be empty afterwards and Bad() will return TRUE.
template<class T, Bool array>
AutoRef<T,array>& AutoRef<T,array>::operator=(T* data)
{
  if (data != mData) {
    clear();
    if (!data)  { return *this; }
    mData = data;
    if (!(mRefCount = gNew SizeT(1)))  {
      ERRLOG("AutoRef::operator=(): Could not allocate reference counter.");
    }
  }
  return *this;
}


/// Returns TRUE if OTHER stores the same data pointer as this handle.
template<class T, Bool array>
inline bool AutoRef<T,array>::operator==(const AutoRef& other) const
{
  return (mData == other.mData);
}


/// Cast operator. Can be used for up-casting.
template<class T, Bool array>
template<class trgT>
inline AutoRef<T,array>::operator AutoRef<trgT,array>(void) const
{
  return AutoRef<trgT,array>(mData, mRefCount);
}


/// NEVER EVER use this constructor directly!!!!!
///
/// I had to define this function to allow type up-casting. The problem was,
/// that different template classes are handled as different types that have
/// no access to private/protected members of objects with other template
/// parameters.
template<class T, Bool array>
inline AutoRef<T,array>::AutoRef(T* data, SizeT* count)
: mData(data), mRefCount(count)
{
  if (mRefCount) { ++(*mRefCount); }
}



#endif  // #ifndef __AUTOREF_H__
