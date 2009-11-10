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

#ifndef __DYNARRAY1D_H__
#define __DYNARRAY1D_H__  1



#include <c4d.h>

#include "utilities.h"



/***************************************************************************//*!
 This class implements a dynamic array container, that can grow if it's internal
 capacity is exhausted. Shrinking is not done autmatically but can be done
 manually, if needed.

 WARNING: During a grow operation the array data will be copied into a new
          (larger array). This copy is done bitwise, i.e. not using the copy
          operator. As a consequence you should not keep any pointers to the
          entries after a append(), push(), reserve() or adaptCapacity(). The
          same applies to linking entries against each other using memory
          addresses (i.e. pointers).

 The template type has to support the following functions:
 - default constructor ( T::T() )
 - default destructor ( T::~T() )
 - copy operator ( T::operator=(const T&) )
*//****************************************************************************/
template <class T>
class DynArray1D
{
  public:

    DynArray1D(SizeT size=0,
               SizeT capacity=0);
    ~DynArray1D(void);

    DynArray1D(const DynArray1D& other);
    DynArray1D& operator=(const DynArray1D& other);

    Bool init(SizeT size=0,
              SizeT capacity=0);
    Bool reserve(SizeT capacity);
    Bool adaptCapacity(void);

    void fill(const T& value);
    void clear(void);
    void erase(void);

    inline SizeT size(void) const;
    inline SizeT capacity(void) const;

    inline const T& operator[](SizeT pos) const;
    inline T&       operator[](SizeT pos);

    inline const T& front(void) const;
    inline T&       front(void);
    inline const T& back(void) const;
    inline T&       back(void);

    inline Bool append(void);
    inline Bool push(const T& value);
    inline T    pop(void);

    void remove(SizeT pos);

    inline const T* arrayAddress(void) const;
    inline T*       arrayAddress(void);


  private:

    SizeT mSize;
    SizeT mCapacity;
    T*    mData;

    Bool increaseCapacity(void);
    Bool newCapacity(SizeT size);
};

#include "dynarray1d_impl.h"



#endif  // #ifndef __DYNARRAY1D_H__
