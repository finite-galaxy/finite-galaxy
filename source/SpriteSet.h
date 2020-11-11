// SpriteSet.h

#ifndef SPRITE_SET_H_
#define SPRITE_SET_H_

#include <string>

class Sprite;



// Class for storing sprites, and for getting the sprite associated with a given
// name. If a sprite has not been loaded yet, this will still return an object
// but with no OpenGL textures associated with it (so it will draw nothing).
class SpriteSet {
public:
  static const Sprite *Get(const std::string &name);


private:
  // Only SpriteQueue is allowed to modify the sprites.
  friend class SpriteQueue;
  static Sprite *Modify(const std::string &name);
};



#endif
