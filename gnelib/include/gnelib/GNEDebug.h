#ifndef _GNEDEBUG_H_
#define _GNEDEBUG_H_

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

//A header file to help do some trace debugging.

namespace GNE {
/**
 * Debugging level constants for the levelMask.
 */
const int DLEVEL1 = 1;
const int DLEVEL2 = 2;
const int DLEVEL3 = 4;
const int DLEVEL4 = 8;
const int DLEVEL5 = 16;
/**
 * Function to initalize debugging.  This is safe to call before
 * initialization of GNE, but can be called at any time (but not multiple
 * times).  No debugging messages will be displayed until this function is
 * called, and it is save to use the debugging macros and doTrace before
 * calling this -- just nothing will be displayed.
 * @param fileName an optional fileName to log to.  If you pass NULL for
 *        this parameter, then GNE will pick a temp file name that is safe
 *        for multiple instances.  The filename will be based on time:
 *        gneHH'MM'SS.log
 * @param levelMask sets just how detailed the trace is, and will display
 *        all messages that match this parameter.  You should bitwise OR
 *        together the levels you want to see.
 * @return false on error
 */
bool initDebug(int levelMask, const char* fileName = NULL);

/**
 * Function to deinitialize debugging.  This will be called automatically on
 * program exit, and the GNE user need not call it.
 */
void killDebug();

/**
 * A function available in a debug compile that does trace logging to a file.
 * This function is thread-safe.
 * @param level the "level" of detail for this trace statement, 1 being the
 *              most basic and should always be displayed.  The higher you
 *              go, the more detailed the trace should get.  This is so to
 *              allow debugging but not totally flood the logfile unless you
 *              need to get this detailed.\n
 *              These general standards are followed:\n
 *              1 - startup, shutdown, and explicit error messages.\n
 *              2 - connection, disconnection, and other "normal" operation.\n
 *              3 - average level.  packet registration, socket registration\n
 *              4 - events like onReceive and such, and some reporting.\n
 *              5 - very low level -- thread creation, memory allocation.
 * @param fn The file name this trace was called from
 * @param lineno The line number in the file
 * @param msg The message to display to the logfile, in printf style,
 *            followed by the data.
 */
void doTrace(int level, const char* fn, int lineno, const char* msg, ...);
}

#endif
