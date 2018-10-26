// FillShader.h

#ifndef FILL_SHADER_H_
#define FILL_SHADER_H_

class Point;
class Color;



// Class holding a function to fill a rectangular region of the screen with a
// given color. This can be used with translucent colors to darken or lighten a
// part of the screen, or with additive colors (alpha = 0) as well.
class FillShader {
public:
  static void Init();
  
  static void Fill(const Point &centre, const Point &size, const Color &color);
};



#endif
