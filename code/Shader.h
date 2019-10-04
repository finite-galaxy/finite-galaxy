// Shader.h

#ifndef SHADER_H_
#define SHADER_H_

#include "gl_header.h"



// Class representing a shader, i.e. a compiled GLSL program that the GPU uses
// in order to draw something. In modern GPL, everything is drawn with shaders.
// In general, rather than using this class directly, drawing code will use one
// of the classes representing a particular shader.
class Shader {
public:
  Shader() = default;
  Shader(const char *vertex, const char *fragment);

  GLuint Object() const;
  GLint Attrib(const char *name) const;
  GLint Uniform(const char *name) const;


private:
  GLuint Compile(const char *str, GLenum type);


private:
  GLuint program;
};



#endif
