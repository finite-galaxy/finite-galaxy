// BatchShader.h

#ifndef BATCH_SHADER_H_
#define BATCH_SHADER_H_

class Sprite;

#include <vector>



// Class for drawing sprites in a batch. The input to each draw command is a
// sprite, whether it should be drawn high DPI, and the vertex data.
class BatchShader {
public:
  // Initialize the shaders.
  static void Init();

  static void Bind();
  static void Add(const Sprite *sprite, bool isHighDPI, const std::vector<float> &data);
  static void Unbind();
};



#endif
