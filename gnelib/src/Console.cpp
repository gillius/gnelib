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

#include "../include/gnelib/gneintern.h"
#include "../include/gnelib/Console.h"
#include "../include/gnelib/ConsoleStreambuf.h"
#include "../include/gnelib/ConsoleStream.h"
#include "../include/gnelib/Mutex.h"
#include <ostream>
#include <istream>

#include "../include/gnelib/conioport.h"

namespace GNE {
namespace Console {

static Mutex outSync;
static bool initialized = false;

//Global iostreams to replace cout/cin
Mutex goutSync;
ConsoleMutex acquire(true, goutSync);
ConsoleMutex release(false, goutSync);
static goutbuf outbuf;
std::ostream gout(&outbuf);
static ginbuf inbuf;
std::istream gin(&inbuf);

//Our global key codes for enter and backspace
int ENTER;
int BACKSPACE;

bool initConsole(int (*atexit_ptr)(void (*func)(void))) {
  if (!initialized) {
    conio_init(&ENTER, &BACKSPACE);
    assert(atexit_ptr);
    atexit_ptr(shutdownConsole);
    gin.tie(&gout); //tie the input and output together.
    initialized = true;
  }
  return false;
}

void shutdownConsole() {
  if (initialized) {
    gout.flush();
    conio_exit();
    initialized = false;
  }
}

int kbhit() {
  assert(initialized);
  return conio_kbhit();
}

int getch() {
  assert(initialized);
  return conio_getch();
}

int mprintf(const char* format, ...) {
  assert(initialized);
  va_list arg;
  
  va_start(arg, format);
  outSync.acquire();
  int ret = conio_vprintf(format, arg);
  outSync.release();
  va_end(arg);

  return ret;
}

int mlprintf(int x, int y, const char* format, ...) {
  assert(initialized);
  va_list arg;
  
  va_start(arg, format);
  outSync.acquire();
  conio_gotoxy(x, y);
  int ret = conio_vprintf(format, arg);
  outSync.release();
  va_end(arg);

  return ret;
}

void mputchar(int ch) {
  assert(initialized);
  outSync.acquire();
  conio_putchar(ch);
  outSync.release();
}

void mlputchar(int x, int y, int ch) {
  assert(initialized);
  outSync.acquire();
  conio_gotoxy(x, y);
  conio_putchar(ch);
  outSync.release();
}

int lgetString(int x, int y, char* str, int maxlen) {
  assert(initialized);
  int currpos = 0;          //The next char to be typed
  bool exit = false;
  while (!exit) {
    int ch = getch();
    if (ch == ENTER) {      //If user pressed RETURN
      exit = true;
    } else if (ch == BACKSPACE) {//If backspace
      currpos--;
      if (currpos < 0)
        currpos = 0;        //Hit the end
      else
        mlputchar(currpos+x, y, ' ');
    } else {
      currpos++;
      if (currpos > maxlen) {
        currpos = maxlen;   //Clip off at end
      } else {
        mlputchar(currpos-1+x, y, ch); //Display input
        str[currpos-1] = (char)ch;     //update input string
      }
    }
    str[currpos] = '\0';
  }
  return currpos;
}

int getString(char* str, int maxlen) {
  assert(initialized);
  int x, y;
  mgetPos(&x, &y);
  return lgetString(x, y, str, maxlen);
}

void setTitle(const char* title) {
  assert(initialized);
  conio_settitle(title);
}

void mgetConsoleSize(int* width, int* height) {
  assert(initialized);
  outSync.acquire();
  conio_getsize(width, height);
  outSync.release();
}

void mgetPos(int* x, int* y) {
  assert(initialized);
  outSync.acquire();
  conio_getxy(x, y);
  outSync.release();
}

}
}





