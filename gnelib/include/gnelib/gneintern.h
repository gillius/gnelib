#ifndef _GNEINTERN_H_
#define _GNEINTERN_H_

/* GNE - Game Networking Engine, a portable multithreaded networking library.
 * Copyright (C) 2001 Jason Winnebeck (gillius@mail.rit.edu)
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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#endif

//Standard C includes -- for Console class and debugging functions
#include <cstdio>
#include <cstdarg>

//Include the HawkNL library
#include <nl.h>

//Standard C++ includes
//We don't include the iostream includes because not all compilers support
//pre-compiled headers, and the iostream library is quite large, and is used
//very sparsely (mainly just for the streambuf replacements in
//gstreambufs.cpp)
#include <string>
#include <map>
#include <queue>

//Other standard includes
#include <assert.h>

//Include pthreads, disabling the catch warning.
#ifndef WIN32
#include <pthread.h>
#include <sched.h>
#endif

//Define a custom "assert" macro
#ifdef _DEBUG
#define valassert(CALL, VAL)        assert((CALL) == (VAL))
#else
#define valassert(CALL, VAL)        CALL
#endif

//Typdefs shared across all of GNE.
namespace GNE {
  /**
   * Typedefs for the variable types that are common to all GNE
   * implementations (as defined in the GNE protocol document).
   * "u" type variables denote unsigned versions.
   */
  //##ModelId=3C65C7A20192
  typedef signed   char  gint8;
  //##ModelId=3C65C7A201A4
  typedef unsigned char  guint8;
  //##ModelId=3C65C7A20182
  typedef guint8         gbyte;

  /**
   * A gbool is 'false' only when its value is 0, else it is 'true'.
   */
  //##ModelId=3C65C7A20180
  typedef guint8         gbool;

  /**
   * One possible true value for a gbool.
   */
  const gbool gTrue = 1;
  /**
   * The only false value for a gbool.
   */
  const gbool gFalse = 0;
  
  //##ModelId=3C65C7A20189
  typedef signed   short gint16;
  //##ModelId=3C65C7A2019B
  typedef unsigned short guint16;
  
  //##ModelId=3C65C7A20190
  typedef signed   int   gint32;
  //##ModelId=3C65C7A2019D
  typedef unsigned int   guint32;
  
  /**
   * A single precision 32-bit IEEE 754 floating point variable.
   */
  //##ModelId=3C65C7A20194
  typedef float          gsingle;
  /**
   * A double precision 64-bit IEEE 754 floating point variable.
   */
  //##ModelId=3C65C7A20187
  typedef double         gdouble;
};

//Define debugging functions
#ifdef _DEBUG
#include "GNEDebug.h"
/**
 * Helper macro for reporting an error in HawkNL.  Displays the HawkNL
 * error code with string, and if it is a system error, reports the system
 * error and string.
 */
#define gnedbg(LEVEL, MESSAGE)           GNE::doTrace(LEVEL, __FILE__, __LINE__, MESSAGE)
#define gnedbg1(LEVEL, MESSAGE, P1)      GNE::doTrace(LEVEL, __FILE__, __LINE__, MESSAGE, P1)
#define gnedbg2(LEVEL, MESSAGE, P1, P2)  GNE::doTrace(LEVEL, __FILE__, __LINE__, MESSAGE, P1, P2)
#define gnedbgo(LEVEL, MESSAGE)          GNE::doTrace(LEVEL, __FILE__, __LINE__, "%x: "MESSAGE, this)
#define gnedbgo1(LEVEL, MESSAGE, P1)     GNE::doTrace(LEVEL, __FILE__, __LINE__, "%x: "MESSAGE, this, P1)
#define gnedbgo2(LEVEL, MESSAGE, P1, P2) GNE::doTrace(LEVEL, __FILE__, __LINE__, "%x: "MESSAGE, this, P1, P2)
#else
#define gnedbg(LEVEL, MESSAGE)           (void(0))
#define gnedbg1(LEVEL, MESSAGE, P1)      (void(0))
#define gnedbg2(LEVEL, MESSAGE, P1, P2)  (void(0))
#define gnedbgo(LEVEL, MESSAGE)          (void(0))
#define gnedbgo1(LEVEL, MESSAGE, P1)     (void(0))
#define gnedbgo2(LEVEL, MESSAGE, P1, P2) (void(0))
#endif

#endif // _GNEINTERN_H_



