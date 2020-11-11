// SpriteShader.h

#ifndef SPRITE_SHADER_H_
#define SPRITE_SHADER_H_

class Sprite;
class Point;

#include <cstdint>



// Class for drawing sprites. You can optionally draw a sprite with a custom
// zoom level or colour swizzle. A more complicated function is also provided for
// adjusting the scale, rotation, clipping, fading, etc. of a sprite; this is
// most often just for use by the DrawList class, which calculates those input
// parameters based on an object's rotation, animation frame, etc.
class SpriteShader {
public:
  class Item {
  public:
    uint32_t texture = 0;
    uint32_t swizzle = 0;
    float frame = 0.f;
    float frameCount = 1.f;
    float position[2] = {0.f, 0.f};
    float transform[4] = {0.f, 0.f, 0.f, 0.f};
    float blur[2] = {0.f, 0.f};
    float clip = 1.f;
    float alpha = 1.f;
  };


public:
  // Initialize the shaders.
  static void Init();

  // Draw a sprite.
  static void Draw(const Sprite *sprite, const Point &position, float zoom = 1.f, int swizzle = 0, float frame = 0.f);

  static void Bind();
  static void Add(const Item &item, bool withBlur = false);
  static void Unbind();
};



#endif
