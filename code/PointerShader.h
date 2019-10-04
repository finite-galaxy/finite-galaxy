// PointerShader.h

#ifndef POINTER_SHADER_H_
#define POINTER_SHADER_H_

class Colour;
class Point;



// Functions for drawing triangular "pointers," e.g. for target crosshairs.
class PointerShader {
public:
  static void Init();

  static void Draw(const Point &centre, const Point &angle, float width, float height, float offset, const Colour &colour);

  static void Bind();
  static void Add(const Point &centre, const Point &angle, float width, float height, float offset, const Colour &colour);
  static void Unbind();
};



#endif
