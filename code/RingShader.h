// RingShader.h

#ifndef RING_SHADER_H_
#define RING_SHADER_H_

class Color;
class Point;



// Class representing a shader that draws round "dots," either filled in or with
// transparent centers (i.e. circles or rings).
class RingShader {
public:
  static void Init();
  
  static void Draw(const Point &pos, float out, float in, const Color &color);
  static void Draw(const Point &pos, float radius, float width, float fraction, const Color &color, float dash = 0.f, float startAngle = 0.f);
  
  static void Bind();
  static void Add(const Point &pos, float out, float in, const Color &color);
  static void Add(const Point &pos, float radius, float width, float fraction, const Color &color, float dash = 0.f, float startAngle = 0.f);
  static void Unbind();
};



#endif
