// Visual.cpp

#include "Visual.h"

#include "Audio.h"
#include "Effect.h"
#include "Random.h"

using namespace std;



// Generate a visual based on the given Effect.
Visual::Visual(const Effect &effect, Point pos, Point vel, Angle facing, Point hitVelocity)
  : Body(effect, pos, vel, facing), lifetime(effect.lifetime)
{
  angle += Angle::Random(effect.randomAngle) - Angle::Random(effect.randomAngle);
  spin = Angle::Random(effect.randomSpin) - Angle::Random(effect.randomSpin);

  velocity *= effect.velocityScale;
  velocity += hitVelocity * (1. - effect.velocityScale);
  if(effect.randomVelocity)
    velocity += angle.Unit() * Random::Real() * effect.randomVelocity;

  if(effect.sound)
    Audio::Play(effect.sound, position);

  if(effect.randomFrameRate)
    AddFrameRate(effect.randomFrameRate * Random::Real());
}



// Step the effect forward.
void Visual::Move()
{
  if(lifetime-- <= 0)
    MarkForRemoval();
  else
  {
    position += velocity;
    angle += spin;
  }
}
