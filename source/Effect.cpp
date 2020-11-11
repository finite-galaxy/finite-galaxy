// Effect.cpp

#include "Effect.h"

#include "Audio.h"
#include "DataNode.h"

using namespace std;



const string &Effect::Name() const
{
  return name;
}



void Effect::Load(const DataNode &node)
{
  if(node.Size() > 1)
    name = node.Token(1);

  for(const DataNode &child : node)
  {
    if(child.Token(0) == "sprite")
      LoadSprite(child);
    else if(child.Token(0) == "sound" && child.Size() >= 2)
      sound = Audio::Get(child.Token(1));
    else if(child.Token(0) == "lifetime" && child.Size() >= 2)
      lifetime = child.Value(1);
    else if(child.Token(0) == "velocity scale" && child.Size() >= 2)
      velocityScale = child.Value(1);
    else if(child.Token(0) == "random velocity" && child.Size() >= 2)
      randomVelocity = child.Value(1);
    else if(child.Token(0) == "random angle" && child.Size() >= 2)
      randomAngle = child.Value(1);
    else if(child.Token(0) == "random spin" && child.Size() >= 2)
      randomSpin = child.Value(1);
    else if(child.Token(0) == "random frame rate" && child.Size() >= 2)
      randomFrameRate = child.Value(1);
    else
      child.PrintTrace("Skipping unrecognized attribute:");
  }
}
