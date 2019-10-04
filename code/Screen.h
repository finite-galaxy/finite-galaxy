// Screen.h

#ifndef SCREEN_H_
#define SCREEN_H_

#include "Point.h"



// Class that simply holds the screen dimensions. This is really just a wrapper
// around some global variables, which means that no one but the drawing thread
// is allowed to use it.
class Screen {
public:
  static void SetRaw(int width, int height);

  // Zoom level as specified by the user.
  static int UserZoom();
  // Effective zoom level, as restricted by the current resolution / window size.
  static int Zoom();
  static void SetZoom(int percent);

  // Specify that this is a high-DPI window.
  static void SetHighDPI(bool isHighDPI = true);
  // This is true if the screen is high DPI, or if the zoom is above 100%.
  static bool IsHighResolution();

  static Point Dimensions();
  static int Width();
  static int Height();

  static int RawWidth();
  static int RawHeight();

  // Get the positions of the edges and corners of the viewport.
  static int Left();
  static int Top();
  static int Right();
  static int Bottom();

  static Point TopLeft();
  static Point TopRight();
  static Point BottomLeft();
  static Point BottomRight();
};



#endif
