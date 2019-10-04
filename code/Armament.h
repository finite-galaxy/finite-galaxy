// Armament.h

#ifndef ARMAMENT_H_
#define ARMAMENT_H_

#include "Hardpoint.h"

#include <map>
#include <vector>

class Command;
class Outfit;
class Point;
class Projectile;
class Ship;
class Visual;



// This class handles the logic for a ship's set of weapons. All weapons of the
// same type coordinate their fire with each other, either firing in clusters
// (if the projectiles are vulnerable to anti-missile) or in a "stream" where
// the guns take turns firing. Instead of firing straight, guns (that is, non-
// turreted weapons) fire aimed slightly inward in a convergence pattern so
// that even if the guns are spaced out horizontally on the ship, their
// projectiles will nearly meet at the end of their range. This class also
// handles turrets, which aim automatically and take into account the target's
// distance away and velocity relative to the ship that is firing.
class Armament {
public:
  // Add a gun or turret hard-point.
  void AddGunPort(const Point &point, const Outfit *outfit = nullptr);
  void AddTurret(const Point &point, const Outfit *outfit = nullptr);
  // This must be called after all the outfit data is loaded. If you add more
  // of a given weapon than there are slots for it, the extras will not fire.
  // But, the "gun ports" attribute should keep that from happening. To
  // remove a weapon, just pass a negative value here.
  void Add(const Outfit *outfit, int count = 1);
  // Call this once all the outfits have been loaded to make sure they are all
  // set up properly (even the ones that were pre-assigned to a hardpoint).
  void FinishLoading();
  // Reload all weapons (because a day passed in-game).
  void ReloadAll();

  // Swap the weapons in the given two hardpoints.
  void Swap(int first, int second);

  // Access the array of weapon hardpoints.
  const std::vector<Hardpoint> &Get() const;
  int GunCount() const;
  int TurretCount() const;

  // Adjust the aim of the turrets.
  void Aim(const Command &command);
  // Fire the given weapon, if it is ready. If it did not fire because it is
  // not ready, return false.
  void Fire(int index, Ship &ship, std::vector<Projectile> &projectiles, std::vector<Visual> &visuals);
  // Fire the given anti-missile system.
  bool FireAntiMissile(int index, Ship &ship, const Projectile &projectile, std::vector<Visual> &visuals);

  // Update the reload counters.
  void Step(const Ship &ship);


private:
  // Note: the Armament must be copied when an instance of a Ship is made, so
  // it should not hold any pointers specific to one ship (including to
  // elements of this Armament itself).
  std::map<const Outfit *, int> streamReload;
  std::vector<Hardpoint> hardpoints;
};



#endif
