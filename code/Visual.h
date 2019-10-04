// Visual.h

#ifndef VISUAL_H_
#define VISUAL_H_

#include "Body.h"

#include "Angle.h"
#include "Point.h"

class Effect;



// A Visual is the object created by an Effect. This is a separate class from
// Effect to allow it to be much more lightweight.
class Visual : public Body {
public:
  Visual() = default;
  Visual(const Effect &effect, Point pos, Point vel, Angle facing, Point hitVelocity = Point());

  /* Functions provided by the Body base class:
  Frame GetFrame(int step = -1) const;
  const Point &Position() const;
  const Point &Velocity() const;
  const Angle &Facing() const;
  Point Unit() const;
  double Zoom() const;
  */

  // Step the effect forward.
  void Move();


private:
  Angle spin;
  int lifetime = 0;
};



#endif
