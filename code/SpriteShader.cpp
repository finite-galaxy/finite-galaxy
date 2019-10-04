// SpriteShader.cpp

#include "SpriteShader.h"

#include "Point.h"
#include "Screen.h"
#include "Shader.h"
#include "Sprite.h"

#include <vector>

using namespace std;

namespace {
  Shader shader;
  GLint scaleI;
  GLint frameI;
  GLint frameCountI;
  GLint positionI;
  GLint transformI;
  GLint blurI;
  GLint clipI;
  GLint alphaI;

  GLuint vao;
  GLuint vbo;

  const vector<vector<GLint>> SWIZZLE = {
    // three different colours
    {GL_RED,   GL_GREEN, GL_BLUE,  GL_ALPHA}, //  0: red + yellow (republic)
    {GL_RED,   GL_BLUE,  GL_GREEN, GL_ALPHA}, //  1: red + magenta (deep)
    {GL_GREEN, GL_RED,   GL_BLUE,  GL_ALPHA}, //  2: green + yellow (free worlds)
    {GL_BLUE,  GL_RED,   GL_GREEN, GL_ALPHA}, //  3: green + cyan (militia)
    {GL_GREEN, GL_BLUE,  GL_RED,   GL_ALPHA}, //  4: blue + magenta (syndicate)
    {GL_BLUE,  GL_GREEN, GL_RED,   GL_ALPHA}, //  5: blue + cyan (merchant)
    // two red
    {GL_RED,   GL_RED,   GL_GREEN, GL_ALPHA}, //  6:
    {GL_RED,   GL_RED,   GL_BLUE,  GL_ALPHA}, //  7:
    {GL_RED,   GL_GREEN, GL_RED,   GL_ALPHA}, //  8:
    {GL_RED,   GL_BLUE,  GL_RED,   GL_ALPHA}, //  9:
    {GL_GREEN, GL_RED,   GL_RED,   GL_ALPHA}, // 10:
    {GL_BLUE,  GL_RED,   GL_RED,   GL_ALPHA}, // 11:
    // two green
    {GL_GREEN, GL_GREEN, GL_RED,   GL_ALPHA}, // 12:
    {GL_GREEN, GL_GREEN, GL_BLUE,  GL_ALPHA}, // 13:
    {GL_GREEN, GL_RED,   GL_GREEN, GL_ALPHA}, // 14:
    {GL_GREEN, GL_BLUE,  GL_GREEN, GL_ALPHA}, // 15:
    {GL_RED,   GL_GREEN, GL_GREEN, GL_ALPHA}, // 16:
    {GL_BLUE,  GL_GREEN, GL_GREEN, GL_ALPHA}, // 17:
    // two blue
    {GL_BLUE,  GL_BLUE,  GL_RED,   GL_ALPHA}, // 18:
    {GL_BLUE,  GL_BLUE,  GL_GREEN, GL_ALPHA}, // 19:
    {GL_BLUE,  GL_RED,   GL_BLUE,  GL_ALPHA}, // 20:
    {GL_BLUE,  GL_GREEN, GL_BLUE,  GL_ALPHA}, // 21:
    {GL_RED,   GL_BLUE,  GL_BLUE,  GL_ALPHA}, // 22: (alpha)
    {GL_GREEN, GL_BLUE,  GL_BLUE,  GL_ALPHA}, // 23: red and black (pirate)
    // three identical
    {GL_RED,   GL_RED,   GL_RED,   GL_ALPHA}, // 24:
    {GL_GREEN, GL_GREEN, GL_GREEN, GL_ALPHA}, // 25:
    {GL_BLUE,  GL_BLUE,  GL_BLUE,  GL_ALPHA}, // 26:

    // third is zero
    {GL_RED,   GL_RED,   GL_ZERO,  GL_ALPHA}, // 27:
    {GL_RED,   GL_GREEN, GL_ZERO,  GL_ALPHA}, // 28:
    {GL_RED,   GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 29:
    {GL_GREEN, GL_RED,   GL_ZERO,  GL_ALPHA}, // 30:
    {GL_GREEN, GL_GREEN, GL_ZERO,  GL_ALPHA}, // 31:
    {GL_GREEN, GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 32:
    {GL_BLUE,  GL_RED,   GL_ZERO,  GL_ALPHA}, // 33:
    {GL_BLUE,  GL_GREEN, GL_ZERO,  GL_ALPHA}, // 34:
    {GL_BLUE,  GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 35:
    // second is zero
    {GL_RED,   GL_ZERO,  GL_RED,   GL_ALPHA}, // 36:
    {GL_RED,   GL_ZERO,  GL_GREEN, GL_ALPHA}, // 37:
    {GL_RED,   GL_ZERO,  GL_BLUE,  GL_ALPHA}, // 38:
    {GL_GREEN, GL_ZERO,  GL_RED,   GL_ALPHA}, // 39:
    {GL_GREEN, GL_ZERO,  GL_GREEN, GL_ALPHA}, // 40:
    {GL_GREEN, GL_ZERO,  GL_BLUE,  GL_ALPHA}, // 41:
    {GL_BLUE,  GL_ZERO,  GL_RED,   GL_ALPHA}, // 42:
    {GL_BLUE,  GL_ZERO,  GL_GREEN, GL_ALPHA}, // 43:
    {GL_BLUE,  GL_ZERO,  GL_BLUE,  GL_ALPHA}, // 44:
    // first is zero
    {GL_ZERO,  GL_RED,   GL_RED,   GL_ALPHA}, // 45:
    {GL_ZERO,  GL_RED,   GL_GREEN, GL_ALPHA}, // 46:
    {GL_ZERO,  GL_RED,   GL_BLUE,  GL_ALPHA}, // 47:
    {GL_ZERO,  GL_GREEN, GL_RED,   GL_ALPHA}, // 48:
    {GL_ZERO,  GL_GREEN, GL_GREEN, GL_ALPHA}, // 49:
    {GL_ZERO,  GL_GREEN, GL_BLUE,  GL_ALPHA}, // 50:
    {GL_ZERO,  GL_BLUE,  GL_RED,   GL_ALPHA}, // 51:
    {GL_ZERO,  GL_BLUE,  GL_GREEN, GL_ALPHA}, // 52:
    {GL_ZERO,  GL_BLUE,  GL_BLUE,  GL_ALPHA}, // 53:

    // two are zero
    {GL_RED,   GL_ZERO,  GL_ZERO,  GL_ALPHA}, // 54:
    {GL_GREEN, GL_ZERO,  GL_ZERO,  GL_ALPHA}, // 55:
    {GL_BLUE,  GL_ZERO,  GL_ZERO,  GL_ALPHA}, // 56: red only (cloaked)
    {GL_ZERO,  GL_RED,   GL_ZERO,  GL_ALPHA}, // 57:
    {GL_ZERO,  GL_GREEN, GL_ZERO,  GL_ALPHA}, // 58:
    {GL_ZERO,  GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 59:
    {GL_ZERO,  GL_ZERO,  GL_RED,   GL_ALPHA}, // 60:
    {GL_ZERO,  GL_ZERO,  GL_GREEN, GL_ALPHA}, // 61:
    {GL_ZERO,  GL_ZERO,  GL_BLUE,  GL_ALPHA}, // 62:

    // three are zero
    {GL_ZERO,  GL_ZERO,  GL_ZERO,  GL_ALPHA}  // 63: black only (outline)

/*
    {GL_ZERO,  GL_ZERO,  GL_ZERO,  GL_ALPHA}, //  0: black only (outline)
    {GL_ZERO,  GL_ZERO,  GL_RED,   GL_ALPHA}, //  1:
    {GL_ZERO,  GL_ZERO,  GL_GREEN, GL_ALPHA}, //  2:
    {GL_ZERO,  GL_ZERO,  GL_BLUE,  GL_ALPHA}, //  3:

    {GL_ZERO,  GL_RED,   GL_ZERO,  GL_ALPHA}, //  4:
    {GL_ZERO,  GL_RED,   GL_RED,   GL_ALPHA}, //  5:
    {GL_ZERO,  GL_RED,   GL_GREEN, GL_ALPHA}, //  6:
    {GL_ZERO,  GL_RED,   GL_BLUE,  GL_ALPHA}, //  7:

    {GL_ZERO,  GL_GREEN, GL_ZERO,  GL_ALPHA}, //  8:
    {GL_ZERO,  GL_GREEN, GL_RED,   GL_ALPHA}, //  9:
    {GL_ZERO,  GL_GREEN, GL_GREEN, GL_ALPHA}, // 10:
    {GL_ZERO,  GL_GREEN, GL_BLUE,  GL_ALPHA}, // 11:

    {GL_ZERO,  GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 12:
    {GL_ZERO,  GL_BLUE,  GL_RED,   GL_ALPHA}, // 13:
    {GL_ZERO,  GL_BLUE,  GL_GREEN, GL_ALPHA}, // 14:
    {GL_ZERO,  GL_BLUE,  GL_BLUE,  GL_ALPHA}, // 15:


    {GL_RED,   GL_ZERO,  GL_ZERO,  GL_ALPHA}, // 16:
    {GL_RED,   GL_ZERO,  GL_RED,   GL_ALPHA}, // 17:
    {GL_RED,   GL_ZERO,  GL_GREEN, GL_ALPHA}, // 18:
    {GL_RED,   GL_ZERO,  GL_BLUE,  GL_ALPHA}, // 19:

    {GL_RED,   GL_RED,   GL_ZERO,  GL_ALPHA}, // 20:
    {GL_RED,   GL_RED,   GL_RED,   GL_ALPHA}, // 21:
    {GL_RED,   GL_RED,   GL_GREEN, GL_ALPHA}, // 22:
    {GL_RED,   GL_RED,   GL_BLUE,  GL_ALPHA}, // 23:

    {GL_RED,   GL_GREEN, GL_ZERO,  GL_ALPHA}, // 24:
    {GL_RED,   GL_GREEN, GL_RED,   GL_ALPHA}, // 25:
    {GL_RED,   GL_GREEN, GL_GREEN, GL_ALPHA}, // 26:
    {GL_RED,   GL_GREEN, GL_BLUE,  GL_ALPHA}, // 27: red + yellow markings (republic)

    {GL_RED,   GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 28:
    {GL_RED,   GL_BLUE,  GL_RED,   GL_ALPHA}, // 29:
    {GL_RED,   GL_BLUE,  GL_GREEN, GL_ALPHA}, // 30: red + magenta markings
    {GL_RED,   GL_BLUE,  GL_BLUE,  GL_ALPHA}, // 31:


    {GL_GREEN, GL_ZERO,  GL_ZERO,  GL_ALPHA}, // 32:
    {GL_GREEN, GL_ZERO,  GL_RED,   GL_ALPHA}, // 33:
    {GL_GREEN, GL_ZERO,  GL_GREEN, GL_ALPHA}, // 34:
    {GL_GREEN, GL_ZERO,  GL_BLUE,  GL_ALPHA}, // 35:

    {GL_GREEN, GL_RED,   GL_ZERO,  GL_ALPHA}, // 36:
    {GL_GREEN, GL_RED,   GL_RED,   GL_ALPHA}, // 37:
    {GL_GREEN, GL_RED,   GL_GREEN, GL_ALPHA}, // 38:
    {GL_GREEN, GL_RED,   GL_BLUE,  GL_ALPHA}, // 39: green + yellow (free worlds)

    {GL_GREEN, GL_GREEN, GL_ZERO,  GL_ALPHA}, // 40:
    {GL_GREEN, GL_GREEN, GL_RED,   GL_ALPHA}, // 41:
    {GL_GREEN, GL_GREEN, GL_GREEN, GL_ALPHA}, // 42:
    {GL_GREEN, GL_GREEN, GL_BLUE,  GL_ALPHA}, // 43:

    {GL_GREEN, GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 44:
    {GL_GREEN, GL_BLUE,  GL_RED,   GL_ALPHA}, // 45: blue + magenta (syndicate)
    {GL_GREEN, GL_BLUE,  GL_GREEN, GL_ALPHA}, // 46:
    {GL_GREEN, GL_BLUE,  GL_BLUE,  GL_ALPHA}, // 47: red and black (pirate)


    {GL_BLUE,  GL_ZERO,  GL_ZERO,  GL_ALPHA}, // 48: red only (cloaked)
    {GL_BLUE,  GL_ZERO,  GL_RED,   GL_ALPHA}, // 49:
    {GL_BLUE,  GL_ZERO,  GL_GREEN, GL_ALPHA}, // 50:
    {GL_BLUE,  GL_ZERO,  GL_BLUE,  GL_ALPHA}, // 51:

    {GL_BLUE,  GL_RED,   GL_ZERO,  GL_ALPHA}, // 52:
    {GL_BLUE,  GL_RED,   GL_RED,   GL_ALPHA}, // 53:
    {GL_BLUE,  GL_RED,   GL_GREEN, GL_ALPHA}, // 54: green + cyan
    {GL_BLUE,  GL_RED,   GL_BLUE,  GL_ALPHA}, // 55:

    {GL_BLUE,  GL_GREEN, GL_ZERO,  GL_ALPHA}, // 56:
    {GL_BLUE,  GL_GREEN, GL_RED,   GL_ALPHA}, // 57: blue + cyan (merchant)
    {GL_BLUE,  GL_GREEN, GL_GREEN, GL_ALPHA}, // 58:
    {GL_BLUE,  GL_GREEN, GL_BLUE,  GL_ALPHA}, // 59:

    {GL_BLUE,  GL_BLUE,  GL_ZERO,  GL_ALPHA}, // 60:
    {GL_BLUE,  GL_BLUE,  GL_RED,   GL_ALPHA}, // 61:
    {GL_BLUE,  GL_BLUE,  GL_GREEN, GL_ALPHA}, // 62:
    {GL_BLUE,  GL_BLUE,  GL_BLUE,  GL_ALPHA}  // 63:
*/
  };
}



// Initialize the shaders.
void SpriteShader::Init()
{
  static const char *vertexCode =
    "uniform vec2 scale;\n"
    "uniform vec2 position;\n"
    "uniform mat2 transform;\n"
    "uniform vec2 blur;\n"
    "uniform float clip;\n"

    "in vec2 vert;\n"
    "out vec2 fragTexCoord;\n"

    "void main() {\n"
    "  vec2 blurOff = 2 * vec2(vert.x * abs(blur.x), vert.y * abs(blur.y));\n"
    "  gl_Position = vec4((transform * (vert + blurOff) + position) * scale, 0, 1);\n"
    "  vec2 texCoord = vert + vec2(.5, .5);\n"
    "  fragTexCoord = vec2(texCoord.x, max(clip, texCoord.y)) + blurOff;\n"
    "}\n";

  static const char *fragmentCode =
    "uniform sampler2DArray tex;\n"
    "uniform float frame;\n"
    "uniform float frameCount;\n"
    "uniform vec2 blur;\n"
    "uniform float alpha;\n"
    "const int range = 5;\n"

    "in vec2 fragTexCoord;\n"

    "out vec4 finalColour;\n"

    "void main() {\n"
    "  float first = floor(frame);\n"
    "  float second = mod(ceil(frame), frameCount);\n"
    "  float fade = frame - first;\n"
    "  vec4 colour;\n"
    "  if(blur.x == 0 && blur.y == 0)\n"
    "  {\n"
    "    if(fade != 0)\n"
    "      colour = mix(\n"
    "        texture(tex, vec3(fragTexCoord, first)),\n"
    "        texture(tex, vec3(fragTexCoord, second)), fade);\n"
    "    else\n"
    "      colour = texture(tex, vec3(fragTexCoord, first));\n"
    "  }\n"
    "  else\n"
    "  {\n"
    "    colour = vec4(0., 0., 0., 0.);\n"
    "    const float divisor = range * (range + 2) + 1;\n"
    "    for(int i = -range; i <= range; ++i)\n"
    "    {\n"
    "      float scale = (range + 1 - abs(i)) / divisor;\n"
    "      vec2 coord = fragTexCoord + (blur * i) / range;\n"
    "      if(fade != 0)\n"
    "        colour += scale * mix(\n"
    "          texture(tex, vec3(coord, first)),\n"
    "          texture(tex, vec3(coord, second)), fade);\n"
    "      else\n"
    "        colour += scale * texture(tex, vec3(coord, first));\n"
    "    }\n"
    "  }\n"
    "  finalColour = colour * alpha;\n"
    "}\n";

  shader = Shader(vertexCode, fragmentCode);
  scaleI = shader.Uniform("scale");
  frameI = shader.Uniform("frame");
  frameCountI = shader.Uniform("frameCount");
  positionI = shader.Uniform("position");
  transformI = shader.Uniform("transform");
  blurI = shader.Uniform("blur");
  clipI = shader.Uniform("clip");
  alphaI = shader.Uniform("alpha");

  glUseProgram(shader.Object());
  glUniform1i(shader.Uniform("tex"), 0);
  glUseProgram(0);

  // Generate the vertex data for drawing sprites.
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLfloat vertexData[] = {
    -.5f, -.5f,
    -.5f,  .5f,
     .5f, -.5f,
     .5f,  .5f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

  glEnableVertexAttribArray(shader.Attrib("vert"));
  glVertexAttribPointer(shader.Attrib("vert"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

  // unbind the VBO and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}



void SpriteShader::Draw(const Sprite *sprite, const Point &position, float zoom, int swizzle, float frame)
{
  if(!sprite)
    return;

  Item item;
  item.texture = sprite->Texture();
  item.frame = frame;
  item.frameCount = sprite->Frames();
  // Position.
  item.position[0] = static_cast<float>(position.X());
  item.position[1] = static_cast<float>(position.Y());
  // Rotation (none) and scale.
  item.transform[0] = sprite->Width() * zoom;
  item.transform[3] = sprite->Height() * zoom;
  // Swizzle.
  item.swizzle = swizzle;

  Bind();
  Add(item);
  Unbind();
}



void SpriteShader::Bind()
{
  glUseProgram(shader.Object());
  glBindVertexArray(vao);

  GLfloat scale[2] = {2.f / Screen::Width(), -2.f / Screen::Height()};
  glUniform2fv(scaleI, 1, scale);
}



void SpriteShader::Add(const Item &item, bool withBlur)
{
  glBindTexture(GL_TEXTURE_2D_ARRAY, item.texture);

  glUniform1f(frameI, item.frame);
  glUniform1f(frameCountI, item.frameCount);
  glUniform2fv(positionI, 1, item.position);
  glUniformMatrix2fv(transformI, 1, false, item.transform);
  // Special case: check if the blur should be applied or not.
  static const float UNBLURRED[2] = {0.f, 0.f};
  glUniform2fv(blurI, 1, withBlur ? item.blur : UNBLURRED);
  // Clipping has the opposite sense in the shader.
  glUniform1f(clipI, 1.f - item.clip);
  glUniform1f(alphaI, item.alpha);

  // Bounds check for the swizzle value:
  int swizzle = (static_cast<size_t>(item.swizzle) >= SWIZZLE.size() ? 0 : item.swizzle);
  // Set the colour swizzle.
  glTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_RGBA, SWIZZLE[swizzle].data());

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



void SpriteShader::Unbind()
{
  glBindVertexArray(0);
  glUseProgram(0);

  // Reset the swizzle.
  glTexParameteriv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_RGBA, SWIZZLE[0].data());
}
