#ifndef _GNEINTERN_H_
#define _GNEINTERN_H_

/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@webzone.net)
 * Project website: http://www.rit.edu/~jpw9607/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//This file is included everywhere in the program and is meant to be a
//pre-compiled header for compilers that support it.

#ifdef _MSC_VER
/*
 * Disable following warnings:
 *
 * warning C4786: identifier was truncated to '255' characters in the debug information
 */
#pragma warning( disable : 4786)
#endif

//Include system headers
#ifdef WIN32
#include <sys/timeb.h>
#include <windows.h>
#endif

//Include the HawkNL library
#include <nl.h>

//Standard C++ includes
#include <string>
#include <map>
#include <queue>

//Other standard includes
#include <assert.h>

//Include pthreads, disabling the catch warning.
#define PtW32NoCatchWarn
#include <pthread.h>
#include <sched.h>
#undef PtW32NoCatchWarn

//Define a custom "assert" macro
#ifdef _DEBUG
#define valassert(CALL, VAL)   assert((CALL) == (VAL))
#else
#define valassert(CALL, VAL)   CALL
#endif

#endif // _GNEINTERN_H_


