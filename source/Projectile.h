// Projectile.h

#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "Body.h"

#include "Angle.h"
#include "Point.h"

#include <memory>
#include <vector>

class Government;
class Ship;
class Visual;
class Weapon;



// Class representing a projectile (a moving object which can hit ships or
// asteroids and can potentially be hit by antimissile systems). A projectile
// may either move at a constant heading and velocity, or may accelerate or
// change course to track its target. Also, when they hit their target or reach
// the end of their lifetime, some projectiles split into "sub-munitions," new
// projectiles that may look different or travel in a new direction.
class Projectile : public Body {
public:
  Projectile(const Ship &parent, Point position, Angle angle, const Weapon *weapon);
  Projectile(const Projectile &parent, const Weapon *weapon);
  // Ship explosion.
  Projectile(Point position, const Weapon *weapon);

  /* Functions provided by the Body base class:
  Frame GetFrame(int step = -1) const;
  const Point &Position() const;
  const Point &Velocity() const;
  const Angle &Facing() const;
  Point Unit() const;
  const Government *GetGovernment() const;
  */

  // Move the projectile. It may create effects or submunitions.
  void Move(std::vector<Visual> &visuals, std::vector<Projectile> &projectiles);
  // This projectile hit something. Create the explosion, if any. This also
  // marks the projectile as needing deletion.
  void Explode(std::vector<Visual> &visuals, double intersection, Point hitVelocity = Point());
  // Get the amount of clipping that should be applied when drawing this projectile.
  double Clip() const;
  // This projectile was killed, e.g. by an antimissile system.
  void Kill();

  // Find out if this is a missile, and if so, how strong it is (i.e. what
  // chance an antimissile shot has of destroying it).
  int MissileStrength() const;
  // Get information on the weapon that fired this projectile.
  const Weapon &GetWeapon() const;

  // Find out which ship this projectile is targeting. Note: this pointer is
  // not guaranteed to be dereferenceable, so only use it for comparing.
  const Ship *Target() const;
  // This function is much more costly, so use it only if you need to get a
  // non-const shared pointer to the target.
  std::shared_ptr<Ship> TargetPtr() const;


private:
  void CheckLock(const Ship &target);


private:
  const Weapon *weapon = nullptr;

  std::weak_ptr<Ship> targetShip;
  const Ship *cachedTarget = nullptr;
  const Government *targetGovernment = nullptr;

  double clip = 1.;
  int lifetime = 0;
  bool hasLock = true;
};



#endif
