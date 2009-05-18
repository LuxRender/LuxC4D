#ifndef __HANDLE_H__
#define __HANDLE_H__



#include "utilities.h"



/***************************************************************************//*!
  This class is to be used for reference counting and automatic deallocation.
  Such a handle object behaves like a normal pointer without pointer arithmetic.
  Each copy operation from one handle to another one increases the internal
  counter and each handle destruction decreases the internal counter. If the
  counter reaches zero the memory pointed by the stored pointer is deallocated.

  The array template parameter is needed to differ between object (de)allocation
  and array (de)allocation.
*//****************************************************************************/
template<class T, Bool array=FALSE>
class Handle
{
  public:

    inline Handle(void);
    inline Handle(T* data);
    inline Handle(const Handle& src);
    inline ~Handle(void);

    void Allocate(SizeT nmb=1);
    void Clear(void);

    inline Bool Bad(void) const;
    inline ULONG Count(void) const;

    inline operator Bool(void) const;

    inline const T& operator*(void) const;
    inline T&       operator*(void);

    inline const T* operator->(void) const;
    inline T*       operator->(void);

    inline const T* Ptr(void) const;
    inline T*       Ptr(void);

    Handle& operator=(T* data);
    Handle& operator=(const Handle& src);
    
    inline bool operator==(const Handle& other) const;

    template<class trgT>
    inline operator Handle<trgT,array>(void) const;

    /// NEVER EVER use this constructor directly!!!!!
    ///
    /// I had to define this function to allow type up-casting. The problem was,
    /// that different template classes are handled as different types that have
    /// no access to private/protected members of objects with other template
    /// parameters.
    inline Handle(T* data, ULONG* count);


  private :

    T*     mData;
    ULONG* mRefCount;
};



/*******************************************************************************
  Inlined Functions of Handle
*******************************************************************************/

/// Constructs an empty handle object. Bad() returns false afterwards.
template<class T, Bool array>
inline Handle<T,array>::Handle(void)
  : mData(0), mRefCount(0)
{}


/// Constructs a new handle object and initializes it with a pointer.
///
/// @param[in]  data
///   Pointer to the data the handle should be the reference of.
template<class T, Bool array>
inline Handle<T,array>::Handle(T* data)
  : mData(data), mRefCount(gNewNC ULONG(1))
{
  if (!mRefCount) {
    ERRLOG_RETURN("Handle::Handle(): Could not allocate reference counter.");
  }
  if (!mData) {
    gDelete(mRefCount);
  }
}


/// Copy constructor. Can be used for copying from one handle to another
/// one. The reference counter is increased, if the source handle referenced
/// some data.
///
/// @param[in]  src
///   Reference of the other instance to copy from.
template<class T, Bool array>
inline Handle<T,array>::Handle(const Handle& other)
  : mData(other.mData), mRefCount(other.mRefCount)
{
  if (mRefCount) {
    ++(*mRefCount);
  }
}


/// Destructs the handle object. Before construction the reference counter
/// is decreased. If it reaches zero the internal referenced data is
/// deallocated.
template<class T, Bool array>
inline Handle<T,array>::~Handle()
{
  Clear();
}


/// Allocates new object(s) T on heap.
///
/// @param[in]  nmb
///   The number of objects to create. If the template parameter ARRAY is FALSE,
///   we ignore the parameter NMB.
template<class T, Bool array>
void Handle<T,array>::Allocate(SizeT nmb)
{
  Clear();
  if (array) {
    if (!(mData = gNewNC T[nmb])) {
      ERRLOG_RETURN("Handle::Allocate(): Could not allocate data array.");
    }
  } else {
    if (!(mData = gNewNC T)) {
      ERRLOG_RETURN("Handle::Allocate(): Could not allocate data.");
    }
  }
  if (!(mRefCount = gNewNC ULONG(1))) {
    if (array) {
      bDelete(mData);
    } else {
      gDelete(mData);
    }
    ERRLOG_RETURN("Handle::Allocate(): Could not allocate reference counter.");
  }
}


/// Releases this handle from the referenced object and decreases the
/// reference counter. If it reaches zero the internal referenced data is
/// deallocated. The internal data pointer is always NULL afterwards.
template<class T, Bool array>
void Handle<T,array>::Clear(void)
{
  if (!mRefCount) {
    return;
  }
  if (--(*mRefCount) == 0) {
    gDelete(mRefCount);
    if (array) {
      bDelete(mData);
    } else {
      gDelete(mData);
    }
  }
}


/// Returns TRUE if the reference counter is not set and the handle is free.
template<class T, Bool array>
inline Bool Handle<T,array>::Bad(void) const
{
  return (!mRefCount);
}


/// Returns the internal reference counter.
template<class T, Bool array>
inline ULONG Handle<T,array>::Count(void) const
{
  return (mRefCount) ? (*mRefCount) : 0;
}



/// Returns TRUE if if the internal data pointer is not NULL.
template<class T, Bool array>
inline Handle<T,array>::operator Bool(void) const
{
  return !Bad();
}


/// Constant de-reference operator.
template<class T, Bool array>
inline const T& Handle<T,array>::operator*(void) const
{
  GeAssert(mData);
  return *mData;
}


/// De-reference operator.
template<class T, Bool array>
inline T& Handle<T,array>::operator*(void)
{
  GeAssert(mData);
  return *mData;
}


/// Constant "pointer-to-structure-member" operator.
template<class T, Bool array>
inline const T* Handle<T,array>::operator->(void) const
{
  GeAssert(mData);
  return mData;
}


/// "Pointer-to-structure-memeber" operator.
template<class T, Bool array>
inline T* Handle<T,array>::operator->(void)
{
  GeAssert(mData);
  return mData;
}


/// Returns the internally stored pointer as constant.
template<class T, Bool array>
inline const T* Handle<T,array>::Ptr(void) const
{
  return mData;
}


/// Returns the internally stored pointer.
template<class T, Bool array>
inline T* Handle<T,array>::Ptr(void)
{
  return mData;
}


/// Copy operator. Behaves like copy constructor.
template<class T, Bool array>
Handle<T,array>& Handle<T,array>::operator=(const Handle& src)
{
  if (src.mData != mData) {
    Clear();
    mData     = src.mData;
    mRefCount = src.mRefCount;
    if (mRefCount) {
      ++(*mRefCount);
    }
  }
  return *this;
}


/// Initializes the handle object with a new reference. If the operation fails,
/// the handle is not referencing the data. I.e. the data has to be deallocated
/// by the caller.
///
/// @param[in]  data
///   The pointer the handle shoud be assigned to.
/// @return
///   A reference of the Handle.
template<class T, Bool array>
Handle<T,array>& Handle<T,array>::operator=(T* data)
{
  if (data != mData) {
    Clear();
    if (!data)  {
      return *this;
    }
    mData = data;
    if (!(mRefCount = gNewNC SizeT(1)))  {
      ERRLOG("Handle::operator=(): Could not allocate reference counter.");
    }
  }
  return *this;
}


/// Returns TRUE if OTHER stores the same data pointer as this handle.
template<class T, Bool array>
inline bool Handle<T,array>::operator==(const Handle& other) const
{
  return (mData == other.mData);
}


/// Cast operator. Can be used for up-casting.
template<class T, Bool array>
template<class trgT>
inline Handle<T,array>::operator Handle<trgT,array>(void) const
{
  return Handle<trgT,array>(mData, mRefCount);
}


/// NEVER EVER use this constructor directly!!!!!
///
/// I had to define this function to allow type up-casting. The problem was,
/// that different template classes are handled as different types that have
/// no access to private/protected members of objects with other template
/// parameters.
template<class T, Bool array>
inline Handle<T,array>::Handle(T* data, ULONG* count)
  : mData(data), mRefCount(count)
{
  if (mRefCount) { ++(*mRefCount); }
}



#endif  // #ifndef __HANDLE_H__