// Galaxy.cpp

#include "Galaxy.h"

#include "DataNode.h"
#include "SpriteSet.h"

using namespace std;



void Galaxy::Load(const DataNode &node)
{
  for(const DataNode &child : node)
  {
    if(child.Token(0) == "pos" && child.Size() >= 3)
      position = Point(child.Value(1), child.Value(2));
    else if(child.Token(0) == "sprite" && child.Size() >= 2)
      sprite = SpriteSet::Get(child.Token(1));
    else
      child.PrintTrace("Skipping unrecognized attribute:");
  }
}



const Point &Galaxy::Position() const
{
  return position;
}



const Sprite *Galaxy::GetSprite() const
{
  return sprite;
}
