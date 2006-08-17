#include <allegro.h>
#include <assert.h>
#include "../include/gnelib.h"

class AllegroSample {
public:  
  static void init(int w, int h) {
    valassert(allegro_init(), 0);
    set_color_depth(desktop_color_depth());
    valassert(set_gfx_mode(GFX_DIRECTX_WIN, w, h, 0, 0), 0);
    valassert(install_keyboard(), 0);
    srand(time(0));
  }  
};