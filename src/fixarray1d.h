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

#ifndef __FIXARRAY1D_H__
#define __FIXARRAY1D_H__



#include "c4d.h"



/***************************************************************************//*!
 This class implements a really simple array class. It doesn't offer much more
 functionality than POD arrays, except safety checks and automatic
 deallocation.

 The template type has to support the following functions:
 - default constructor ( T::T() )
 - default destructor ( T::~T() )
 - copy operator ( T::operator=(const T&) )
*//****************************************************************************/

template <class T>
class FixArray1D
{
  public:

    /// This type will be used for specifying array size and locations.
    typedef VULONG  SizeT;


    FixArray1D(SizeT size=0);
    ~FixArray1D(void);

    FixArray1D(const FixArray1D& other);
    FixArray1D& operator=(const FixArray1D& other);

    Bool init(SizeT size);
    void fill(const T& value);
    void fillWithZero(void);
    void erase(void);

    inline SizeT size(void) const;

    inline const T& operator[](SizeT pos) const;
    inline T&       operator[](SizeT pos);

    inline const T& front(void) const;
    inline T&       front(void);
    inline const T& back(void) const;
    inline T&       back(void);

    inline const T* getArrayAddress(void) const;
    inline T*       getArrayAddress(void);

    void setArrayAddress(T* data, SizeT size);

    void adopt(FixArray1D& other);


  private:

    SizeT mSize;
    T*    mData;
};

#include "fixarray1d_impl.h"



#endif
