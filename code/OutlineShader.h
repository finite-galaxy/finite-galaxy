// OutlineShader.h

#ifndef OUTLINE_SHADER_H_
#define OUTLINE_SHADER_H_

#include "Point.h"

class Colour;
class Sprite;



// Functions for drawing the "outline" of a sprite, i.e. a Sobel filter of its
// alpha channel.
class OutlineShader {
public:
  static void Init();

  static void Draw(const Sprite *sprite, const Point &pos, const Point &size, const Colour &colour, const Point &unit = Point(0., -1.), float frame = 0.f);
};



#endif
