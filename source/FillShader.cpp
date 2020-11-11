// FillShader.cpp

#include "FillShader.h"

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
  GLint sizeI;
  GLint colourI;

  GLuint vao;
  GLuint vbo;
}



void FillShader::Init()
{
  static const char *vertexCode =
    "uniform vec2 scale;\n"
    "uniform vec2 centre;\n"
    "uniform vec2 size;\n"

    "in vec2 vert;\n"

    "void main() {\n"
    "  gl_Position = vec4((centre + vert * size) * scale, 0, 1);\n"
    "}\n";

  static const char *fragmentCode =
    "uniform vec4 colour = vec4(1, 1, 1, 1);\n"

    "out vec4 finalColour;\n"

    "void main() {\n"
    "  finalColour = colour;\n"
    "}\n";

  shader = Shader(vertexCode, fragmentCode);
  scaleI = shader.Uniform("scale");
  centreI = shader.Uniform("centre");
  sizeI = shader.Uniform("size");
  colourI = shader.Uniform("colour");

  // Generate the vertex data for drawing sprites.
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLfloat vertexData[] = {
    -.5f, -.5f,
     .5f, -.5f,
    -.5f,  .5f,
     .5f,  .5f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

  glEnableVertexAttribArray(shader.Attrib("vert"));
  glVertexAttribPointer(shader.Attrib("vert"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

  // unbind the VBO and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}



void FillShader::Fill(const Point &centre, const Point &size, const Colour &colour)
{
  if(!shader.Object())
    throw runtime_error("FillShader: Draw() called before Init().");

  glUseProgram(shader.Object());
  glBindVertexArray(vao);

  GLfloat scale[2] = {2.f / Screen::Width(), -2.f / Screen::Height()};
  glUniform2fv(scaleI, 1, scale);

  GLfloat centreV[2] = {static_cast<float>(centre.X()), static_cast<float>(centre.Y())};
  glUniform2fv(centreI, 1, centreV);

  GLfloat sizeV[2] = {static_cast<float>(size.X()), static_cast<float>(size.Y())};
  glUniform2fv(sizeI, 1, sizeV);

  glUniform4fv(colourI, 1, colour.Get());

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray(0);
  glUseProgram(0);
}
