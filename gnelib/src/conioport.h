//This code comes with no warranty.  The Linux portion of this code belongs
//to George foot gfoot@users.sourceforge.net.  The Windows portion of this
//code is public domain.

#ifdef WIN32
// *** WINDOWS API CONSOLE CODE ***

#include <conio.h>
#include <windows.h>

void conio_init (void) { system("cls"); }
void conio_exit (void) {}
int conio_kbhit (void) { return kbhit(); }
int conio_getch (void) { return getch(); }
void conio_gotoxy (int x, int y) {
  COORD pos = {x, y};
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

#else
// *** Try to use ncurses ***
//The following code was done by George Foot, and was part of the libnet
//library.

#include <ncurses.h>

static int __gf__cached__gf__ = 0, __gf__cached_value__gf__ = 0;

inline void conio_init (void)
{
	int x,y;
	initscr();
	start_color();
	cbreak();
	noecho();
	nonl();
//	intrflush (stdscr, FALSE);
	keypad (stdscr, TRUE);
	nodelay (stdscr, TRUE);
	scrollok (stdscr, TRUE);
}

inline void conio_exit (void)
{
	endwin();
}

inline int conio_kbhit (void)
{
  if (__gf__cached__gf__) return 1;
  __gf__cached_value__gf__ = getch();
  if (__gf__cached_value__gf__ != ERR) __gf__cached__gf__ = 1;
  return __gf__cached__gf__;
}

inline int conio_getch (void)
{
  if (__gf__cached__gf__) {
	__gf__cached__gf__ = 0;
	return __gf__cached_value__gf__;
  } else
	return getch();
}

inline void conio_gotoxy (int x, int y) { move (y-1, x-1); }

#endif