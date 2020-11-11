// RingShader.h

#ifndef RING_SHADER_H_
#define RING_SHADER_H_

class Colour;
class Point;



// Class representing a shader that draws round "dots," either filled in or with
// transparent centres (i.e. circles or rings).
class RingShader {
public:
  static void Init();

  static void Draw(const Point &pos, float out, float in, const Colour &colour);
  static void Draw(const Point &pos, float radius, float width, float fraction, const Colour &colour, float dash = 0.f, float startAngle = 0.f);

  static void Bind();
  static void Add(const Point &pos, float out, float in, const Colour &colour);
  static void Add(const Point &pos, float radius, float width, float fraction, const Colour &colour, float dash = 0.f, float startAngle = 0.f);
  static void Unbind();
};



#endif
