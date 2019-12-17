// Small demo that shows how to use the functions in display.h for drawing the beautiful Mandebrot Set.
#include "display.h"
#include <float.h>
#include <time.h>
#include <assert.h>

#define ZOOM 1.03 // zoom-in speed factor applied at navigational adjustment
#define ACC 64 // computational depth, slower and more accurate if larger
#define STEPS 2 // stride, use 1 for full resolution, higher numbers for approximations

// Data structure to describe how a small part of the complex plane maps to an image.
// (User of associated functions does not need to know that this structure even exists!)
// Details: s is a scaling factor or scalar target, x is the shift in x direction, y is the shift in y direction.
typedef struct params {double s, x, y;} params;

// Creates a state and initialises it with an interesting starting position
// for surfing through the Mandelbrot Set.
void *newFractalState() {
  srand(time(0)); //seed the random number generator with current time
  params *p = malloc(sizeof(params)); //allocate memory for the state
  *p = (params) {2.0, - 3.0E-3 * ((rand()) % 1000 - 5.0E2) - 1.5, -1.0}; //initialise state
  return (void*)p; //return generic pointer to the state
}

// Clears all allocated memory of data.
void freeFractalState(void *data) {
  if (data) free(data);
}

// Tilo's implementation for calculating the Mandelbrot Set, an example of a fractal.
// Given a state p that describes how the complex plane maps to an
// image of (w,h) resolution, the function calculates for a pixel (x,y)
// if the pixel is estimated to be part of the Mandelbrot Set. If so it returns
// accuracy, otherwise it returns the iteration at which the recurring calculation
// escaped a theshold.
int calculateMandelbrot(const int accuracy, const int x, const int y,
                               const int w, const int h, const params *p) {
  int i;
  double a[4] = {0.0};
  for (i = 0; (i < accuracy) && ((a[2] + a[3]) < accuracy / 4); i++) {
    a[0] = 2.0 * a[0] * a[1] + y * p->s / h + p->y;
    a[1] = a[2] - a[3] + x * p->s / w + p->x;
    a[2] = a[1] * a[1];
    a[3] = a[0] * a[0];
  }
  return i;
}

// Tilo's implementation to draw and navigate the Mandelbrot Set.
// Given state data (assumed to be of type params*) that describes how
// the complex plane maps to a display d, the function draws and shows the
// Mandelbrot Set, and then updates data to a nearby interesting state
// in the complex plane considering some user input via pressedKey.
// Thus, this function can be called repeatedly to calculate a fly-through
// of the Mandelbrot Set. Arrow keys are used to navigate, 'z' to zoom in fast,
// 'x' to zoom out fast, ESC makes the function return true,
// If no pressedKey is provided (i.e. =0), a slow zoom-in is performed.
// The function returns true, iff the navigation should come to an end.
bool navigateMandelbrot(display *d, void *data, const char pressedKey) {

  //casting generic data to params type as assumed by this function
  params *p = (params*) data;

  //checks and initialisations
  if ((d == NULL) || (p == NULL)) return true; //stop if an input is NULL
  int w = getWidth(d), h = getHeight(d), tx = w / 4, ty = h / 4; //image dimensions and target position for interesting pixels
  params c = (params) {DBL_MAX, 0.0, 0.0}; //buffer to store most interesting visible location

  //go through the pixels of the image with stride STEPS
  for(int y = 0; y < h; y += STEPS) {
    bool last = false;
    for(int x = 0; x < w; x += STEPS) {
      //analyse the Mandelbrot Set for a particular pixel
      int i = calculateMandelbrot(ACC, x, y, w, h, p);
      //if pixel is part of the Mandelbrot Set then set black colour
      if (i == ACC) {
        //if pixel before was not in the Set and it is close to target, then make it the most interesting visible location
        if (!last) {
          int target = (x - tx) * (x - tx) + (y - ty) * (y - ty);
          if (target < c.s) c = (params) {target, x - tx, y - ty};
        }
        colour(d, 0xFF);
        last = true;
      }
      else {
        //set colour blue-ish according to i
        colour(d, 0xFFFF * (1 + 16 * (i % 16)));
        last = false;
      }
      //draw a rectangle of current colour at current pixel and stride size
      block(d, x, y, STEPS, STEPS);
    }
  }
  //show everything drawn on the display window
  show(d);
  //auto zoom-out if zoomed in too far
  if (p->s < 1.0E-6) p->s = 1.0E-3;
  //use pressedKey input and/or interesting target to update the state p for future drawing
  *p = (params) {(pressedKey == 120) ? (p->s / (ZOOM / 1.15)) : ((pressedKey == 122) ? (p->s / (ZOOM * 1.15)) : (p->s / ZOOM)),
                 ((pressedKey == 79) || (pressedKey == 80)) ? (p->x + p->s * (w / 4 - ((pressedKey - 78) * 2 - 3) * (w / 20) - tx) / w) : (p->x + p->s * c.x / w),
                 ((pressedKey == 81) || (pressedKey == 82)) ? (p->y + p->s * (h / 4 - ((pressedKey - 80) * 2 - 3) * (h / 20) - ty) / h) : (p->y + p->s * c.y / h)};
  //indicate that this should be the last time this function is called if ESC is pressed
  return (pressedKey == 27);
}

//Run the Demo
int main() {
  display *d = newDisplay("Mandelbrot Flight", 256, 256);
  void *data = newFractalState();
  run(d, data, navigateMandelbrot);
  freeFractalState(data);
  freeDisplay(d);
  return 0;
}
