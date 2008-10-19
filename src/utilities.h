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

#ifndef __UTILITIES_H__
#define __UTILITIES_H__ 1



/// Logs/prints an error message.
#define ERRLOG(msg)                                                           \
  { GePrint(msg); }

/// Logs/prints an error message and returns (without a return value).
#define ERRLOG_RETURN(msg)                                                    \
  { ERRLOG(msg);  return; }

/// Logs/prints an error message and returns with the return value FALSE.
#define ERRLOG_RETURN_FALSE(msg)                                              \
  { ERRLOG(msg);  return FALSE; }

/// Logs/prints an error message, defined by an ID and returns
/// (without a return value).
#define ERRLOG_ID_RETURN_FALSE(id,msg)                                        \
  { mErrorStringID=(id);  ERRLOG(msg);  return FALSE; }



#endif  // #ifndef __UTILITIES_H__
