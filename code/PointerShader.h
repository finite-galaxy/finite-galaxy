// PointerShader.h

#ifndef POINTER_SHADER_H_
#define POINTER_SHADER_H_

class Color;
class Point;



// Functions for drawing triangular "pointers," e.g. for target crosshairs.
class PointerShader {
public:
  static void Init();
  
  static void Draw(const Point &center, const Point &angle, float width, float height, float offset, const Color &color);
  
  static void Bind();
  static void Add(const Point &center, const Point &angle, float width, float height, float offset, const Color &color);
  static void Unbind();
};



#endif
