// Allegro example program, using the GNElib Threads/Timers/Networking engine //
#include <allegro.h>
#include <winalleg.h>
#include "../AllegroSample.h"
#include "../../include/gnelib.h"

const int BALLSNUM = 50;
int NEWCOUNT = 0;
int DELETECOUNT = 0;
int STARTCOUNT = 0;
int STOPCOUNT = 0;

// ThreadBalls is just an example to better understand what threads are. You
// would never want to use threads for the purpose it is used here, because it's
// not a good idea to access the graphics output function of Allegro from within
// different threads. But maybe i'm not the only one who needs to *see*
// something to understand it :)
// Every ThreadBall creates a bitmap in its constructor, and destroys the bitmap
// in its destructor. The run() method is from the Thread class, and is the
// actual thread to be executed. It's just a normal while loop, with calls to
// sleep() which wait the specified time. You can already see in this example
// what a powerfull tool threads are.
class ThreadBall : public Thread {
public:

	ThreadBall(std::string name, int color, int radius, int life) : Thread(name, DEF_PRI) {
		this->radius = radius;
		this->life = life;
		bmp = create_bitmap(radius*2+1, radius*2+1);
		clear_to_color(bmp, bitmap_mask_color(bmp));
		
		circlefill(bmp, radius, radius, radius, color);
		circlefill(bmp, radius*1/2, radius*1/2, radius/2, makecol(255,255,255));
		circlefill(bmp, radius*1/2, radius*1/2, radius/4, makecol(0,0,0));
		circlefill(bmp, radius*3/2, radius*1/2, radius/2, makecol(255,255,255));
		circlefill(bmp, radius*3/2, radius*1/2, radius/4, makecol(0,0,0));					
				
		int ix = SCREEN_W/2;
		int iy = SCREEN_H/2;
		dx = ((rand() % 41) - 20) / 10.0;
		dy = ((rand() % 101) - 50) / 10.0;				
		
		x = ix;
		y = iy;
		
		NEWCOUNT++;
	}

	~ThreadBall() {		
		
		int ix = (int) (x + 0.5);
		int iy = (int) (y + 0.5);			
		int ir = (int) (radius + 0.5);
						
		rectfill(screen, ix - ir*2 , iy - ir*2, ix + ir*2, iy + ir*2,
			makecol(255, 0, 0));		
		destroy_bitmap(bmp);	
		DELETECOUNT++;	
	}

	void run() { 
		
		int ix;
		int iy;
		int ir = (int) (radius + 0.5);
		
		STARTCOUNT++;
		
		while(life > 0) {
			sleep(10);
			life -= 10;
			
			ix = (int) (x + 0.5);
			iy = (int) (y + 0.5);						
			
			rectfill(screen, ix - ir , iy - ir, ix + ir, iy + ir,
				makecol(50, 50, 50));	
													
			if( x + dx - radius > 0 && x + dx + radius < SCREEN_W-1 )			
				x += dx;
			else
				dx = - dx;
				
			if( y + dy - radius > 0 && y + dy + radius < SCREEN_H-1 )			
				y += dy;
			else
				dy = - dy;
				
			ix = (int) (x + 0.5);
			iy = (int) (y + 0.5);		
				
			draw_sprite(screen, bmp, ix - ir, iy - ir);
							
		}
						
		hline(screen, ix - ir*2, iy, ix + ir*2, makecol(255, 0, 0));
		vline(screen, ix, iy - ir*2, iy + ir*2, makecol(255, 0, 0));
				
		textprintf_centre(screen, font, ix, iy + ir, makecol(255,0,0), "R I P");
			
		sleep(1000);
		
		rectfill(screen, ix - ir*2 , iy - ir*2, ix + ir*2, iy + ir*2,
			makecol(0, 100, 0));		
			
		STOPCOUNT++;			
				
	}
private:
	BITMAP *bmp;
	float radius;
	float x, y;
	float dx, dy;
	float life;
};

int main(void) {

	AllegroSample::init(500,500);

	GNE::init(NL_IP, atexit);
	
	// Keep a list of all the ThreadBalls.
	ThreadBall *ball[BALLSNUM];
			
	// Start some ThreadBalls.
  int n;
	for(n = 0; n < BALLSNUM; n++) {							
		ball[n] = new ThreadBall("ThreadBall",			// name them all the same
			makecol(rand(), rand(), rand()),				// random color
			10+(rand()%10),									// random size
			2000+(rand()%(1000+(10000*n/BALLSNUM))));	// random life-time
		ball[n]->start();										// start it
	}
	
	// Wait until all of them have died, and delete dead ones.
	for(n = 0; n < BALLSNUM; n++) {
		ball[n]->join();
		delete ball[n];
	}
		
	// Wait a second.
	Thread::sleep(1000);
			
	assert(NEWCOUNT == BALLSNUM);		
	assert(STARTCOUNT == NEWCOUNT);		
	assert(STOPCOUNT == STARTCOUNT);		
	assert(DELETECOUNT == STOPCOUNT);		
										
	return 0;
} END_OF_MAIN();

// example modified for Allegro by networm                                    //
// GNE by Gillius                                                             //