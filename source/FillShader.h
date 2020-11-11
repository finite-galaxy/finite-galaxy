// FillShader.h

#ifndef FILL_SHADER_H_
#define FILL_SHADER_H_

class Point;
class Colour;



// Class holding a function to fill a rectangular region of the screen with a
// given colour. This can be used with translucent colours to darken or lighten a
// part of the screen, or with additive colours (alpha = 0) as well.
class FillShader {
public:
  static void Init();

  static void Fill(const Point &centre, const Point &size, const Colour &colour);
};



#endif
