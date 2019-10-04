// Galaxy.h

#ifndef GALAXY_H_
#define GALAXY_H_

#include "Point.h"

class DataNode;
class Sprite;



// This is any object that should be drawn as a backdrop to the map. Multiple
// galaxies can be handled by just spacing them out so widely that the player
// will never accidentally scroll the view from one to the other.
class Galaxy {
public:
  void Load(const DataNode &node);

  const Point &Position() const;
  const Sprite *GetSprite() const;


private:
  Point position;
  const Sprite *sprite = nullptr;
};

#endif
