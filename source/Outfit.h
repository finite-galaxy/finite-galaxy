// Outfit.h

#ifndef OUTFIT_H_
#define OUTFIT_H_

#include "Weapon.h"

#include "Dictionary.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

class Body;
class DataNode;
class Effect;
class Sound;
class Sprite;



// Class representing an outfit that can be installed in a ship. A ship's
// "attributes" are simply stored as a series of key-value pairs, and an outfit
// can add to or subtract from any of those values. Weapons also have another
// set of attributes unique to them, and outfits can also specify additional
// information like the sprite to use in the outfitter panel for selling them,
// or the sprite or sound to be used for an engine flare.
class Outfit : public Weapon {
public:
  // An "outfit" can be loaded from an "outfit" node or from a ship's
  // "attributes" node.
  void Load(const DataNode &node);

  const std::string &Name() const;
  const std::string &PluralName() const;
  const std::string &Category() const;
  const std::string &Description() const;
  int64_t Cost() const;
  double Mass() const;
  // Get the licences needed to buy or operate this ship.
  const std::vector<std::string> &Licences() const;
  // Get the image to display in the outfitter when buying this item.
  const Sprite *Thumbnail() const;

  double Get(const char *attribute) const;
  double Get(const std::string &attribute) const;
  const Dictionary &Attributes() const;

  // Determine whether the given number of instances of the given outfit can
  // be added to a ship with the attributes represented by this instance. If
  // not, return the maximum number that can be added.
  int CanAdd(const Outfit &other, int count = 1) const;
  // For tracking a combination of outfits in a ship: add the given number of
  // instances of the given outfit to this outfit.
  void Add(const Outfit &other, int count = 1);
  // Modify this outfit's attributes. Note that this cannot be used to change
  // special attributes, like cost and mass.
  void Set(const char *attribute, double value);

  // Get this outfit's engine flare sprites, if any.
  const std::vector<std::pair<Body, int>> &FlareSprites() const;
  const std::map<const Sound *, int> &FlareSounds() const;
  // Get the afterburner effect, if any.
  const std::map<const Effect *, int> &AfterburnerEffects() const;
  // Get the sprite this outfit uses when dumped into space.
  const Sprite *FlotsamSprite() const;


private:
  std::string name;
  std::string pluralName;
  std::string category;
  std::string description;
  const Sprite *thumbnail = nullptr;
  int64_t cost = 0;
  double mass = 0.;
  // Licences needed to purchase this item.
  std::vector<std::string> licences;

  Dictionary attributes;

  std::vector<std::pair<Body, int>> flareSprites;
  std::map<const Sound *, int> flareSounds;
  std::map<const Effect *, int> afterburnerEffects;
  const Sprite *flotsamSprite = nullptr;
};



// These get called a lot, so inline them for speed.
inline int64_t Outfit::Cost() const { return cost; }
inline double Outfit::Mass() const { return mass; }



#endif
