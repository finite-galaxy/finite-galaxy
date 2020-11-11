// LineShader.h

#ifndef LINE_SHADER_H_
#define LINE_SHADER_H_

class Colour;
class Point;



// Class to be used for drawing lines. The sides of a line are anti-aliased, but
// the start and end of the line are not.
class LineShader {
public:
  static void Init();

  static void Draw(const Point &from, const Point &to, float width, const Colour &colour);
};



#endif
