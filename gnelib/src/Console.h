#ifndef CONSOLE_H_INCLUDED_C507037C
#define CONSOLE_H_INCLUDED_C507037C

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

#include "gneintern.h"
#include "Mutex.h"

/**
 * Class for providing very basic console support, meant for use in the
 * example and test programs, and in console-only servers using GNE.  This
 * class's behavior is undefined when running in a Win32 application (not a
 * Win32 console) project.
 */
//##ModelId=3AF8A1A0033E
class Console {
public:
  /**
   * Initalizes the console part of GNE.  This may be called multiple times.
   * Pass it your atexit function so the console will be shutdown
   * automatically on program exit.
   * @return true if the console could not be initalized.
   */
  //##ModelId=3AF8A1E2038E
  static bool init(int (*atexit_ptr)(void (*func)(void)));

  /**
   * Shuts down the console part of GNE.  This may be called multiple times.
   */
  //##ModelId=3AF8A1ED037A
  static void shutdown();

  /**
   * Returns non-zero if a key is waiting in the buffer to be received by
   * getch.
   * @see getch
   */
  //##ModelId=3AF8AADE038E
  static int kbhit();

  /**
   * Returns the next character in the input, blocking if no character is
   * ready to be returned.
   * @return the next character
   * @see kbhit
   */
  //##ModelId=3AF8AADF0050
  static int getch();

  /**
   * Function to sync on printf.
   * @return number of characters written.
   */
  //##ModelId=3AF8A7EC026C
  static int mprintf(char* format, ...);
  
  /**
   * Function to sync on printf, and print to a certain location.
   * @return number of characters written.
   * @see mprintf
   */
  //##ModelId=3AF8A7ED00AA
  static int mlprintf(int x, int y, char* format, ...);

private:
  /**
   * Moves the cursor on the screen to (X, Y).
   */
  //##ModelId=3AF8A3AE001E
  static void gotoxy(int x, int y);

  //##ModelId=3AF8A1F80226
  static Mutex outSync;

  //##ModelId=3AF8ADDB0280
  static bool initialized;
};

#endif
