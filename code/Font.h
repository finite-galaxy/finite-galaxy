// Font.h

#ifndef FONT_H_
#define FONT_H_

#include "Shader.h"

#include "gl_header.h"

#include <string>

class Color;
class ImageBuffer;
class Point;



// Class for drawing text in OpenGL. Each font is based on a single image with
// glyphs for each character in ASCII order (not counting control characters).
// The kerning between characters is automatically adjusted to look good. At the
// moment only plain ASCII characters are supported, not Unicode.
class Font {
public:
  Font();
  explicit Font(const std::string &imagePath);
  
  void Load(const std::string &imagePath);
  
  void Draw(const std::string &str, const Point &point, const Color &color) const;
  void DrawAliased(const std::string &str, double x, double y, const Color &color) const;
  
  int Width(const std::string &str, char after = ' ') const;
  int Width(const char *str, char after = ' ') const;
  std::string Truncate(const std::string &str, int width) const;
  std::string TruncateFront(const std::string &str, int width) const;
  std::string TruncateMiddle(const std::string &str, int width) const;
  
  int Height() const;
  
  int Space() const;
  
  static void ShowUnderlines(bool show);
  
  
private:
  static int Glyph(char c, bool isAfterSpace);
  void LoadTexture(ImageBuffer &image);
  void CalculateAdvances(ImageBuffer &image);
  void SetUpShader(float glyphW, float glyphH);
  
  
private:
  Shader shader;
  GLuint texture;
  GLuint vao;
  GLuint vbo;
  
  GLint colorI;
  GLint scaleI;
  GLint glyphI;
  GLint aspectI;
  GLint positionI;
  
  int height;
  int space;
  mutable int screenWidth;
  mutable int screenHeight;
  
  static const int GLYPHS = 98;
  int advance[GLYPHS * GLYPHS];
};



#endif
