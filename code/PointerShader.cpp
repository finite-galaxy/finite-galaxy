// PointerShader.cpp

#include "PointerShader.h"

#include "Colour.h"
#include "Point.h"
#include "Screen.h"
#include "Shader.h"

#include <stdexcept>

using namespace std;

namespace {
  Shader shader;
  GLint scaleI;
  GLint centreI;
  GLint angleI;
  GLint sizeI;
  GLint offsetI;
  GLint colourI;

  GLuint vao;
  GLuint vbo;
}



void PointerShader::Init()
{
  static const char *vertexCode =
    "uniform vec2 scale;\n"
    "uniform vec2 centre;\n"
    "uniform vec2 angle;\n"
    "uniform vec2 size;\n"
    "uniform float offset;\n"

    "in vec2 vert;\n"
    "out vec2 coord;\n"

    "void main() {\n"
    "  coord = vert * size.x;\n"
    "  vec2 base = centre + angle * (offset - size.y * (vert.x + vert.y));\n"
    "  vec2 wing = vec2(angle.y, -angle.x) * (size.x * .5 * (vert.x - vert.y));\n"
    "  gl_Position = vec4((base + wing) * scale, 0, 1);\n"
    "}\n";

  static const char *fragmentCode =
    "uniform vec4 colour = vec4(1, 1, 1, 1);\n"
    "uniform vec2 size;\n"

    "in vec2 coord;\n"
    "out vec4 finalColour;\n"

    "void main() {\n"
    "  float height = (coord.x + coord.y) / size.x;\n"
    "  float taper = height * height * height;\n"
    "  taper *= taper * .5 * size.x;\n"
    "  float alpha = clamp(.8 * min(coord.x, coord.y) - taper, 0, 1);\n"
    "  alpha *= clamp(1.8 * (1. - height), 0, 1);\n"
    "  finalColour = colour * alpha;\n"
    "}\n";

  shader = Shader(vertexCode, fragmentCode);
  scaleI = shader.Uniform("scale");
  centreI = shader.Uniform("centre");
  angleI = shader.Uniform("angle");
  sizeI = shader.Uniform("size");
  offsetI = shader.Uniform("offset");
  colourI = shader.Uniform("colour");

  // Generate the vertex data for drawing sprites.
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLfloat vertexData[] = {
    0.f, 0.f,
    0.f, 1.f,
    1.f, 0.f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

  glEnableVertexAttribArray(shader.Attrib("vert"));
  glVertexAttribPointer(shader.Attrib("vert"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

  // unbind the VBO and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}



void PointerShader::Draw(const Point &centre, const Point &angle, float width, float height, float offset, const Colour &colour)
{
  Bind();

  Add(centre, angle, width, height, offset, colour);

  Unbind();
}



void PointerShader::Bind()
{
  if(!shader.Object())
    throw runtime_error("PointerShader: Bind() called before Init().");

  glUseProgram(shader.Object());
  glBindVertexArray(vao);

  GLfloat scale[2] = {2.f / Screen::Width(), -2.f / Screen::Height()};
  glUniform2fv(scaleI, 1, scale);
}



void PointerShader::Add(const Point &centre, const Point &angle, float width, float height, float offset, const Colour &colour)
{
  GLfloat c[2] = {static_cast<float>(centre.X()), static_cast<float>(centre.Y())};
  glUniform2fv(centreI, 1, c);

  GLfloat a[2] = {static_cast<float>(angle.X()), static_cast<float>(angle.Y())};
  glUniform2fv(angleI, 1, a);

  GLfloat size[2] = {width, height};
  glUniform2fv(sizeI, 1, size);

  glUniform1f(offsetI, offset);

  glUniform4fv(colourI, 1, colour.Get());

  glDrawArrays(GL_TRIANGLES, 0, 3);
}



void PointerShader::Unbind()
{
  glBindVertexArray(0);
  glUseProgram(0);
}
