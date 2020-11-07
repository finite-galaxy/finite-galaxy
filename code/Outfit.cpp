// Outfit.cpp

#include "Outfit.h"

#include "Audio.h"
#include "Body.h"
#include "DataNode.h"
#include "Effect.h"
#include "GameData.h"
#include "SpriteSet.h"

#include <cmath>

using namespace std;

namespace {
  const double EPS = 0.000'000'0001;

  // A list of attributes which are allowed to have a minimum value other than 0.
  // The key is the attribute name and the value is the minimum value
  // that the ship's attribute is allowed to have. A value of 0 means that the
  // attribute can have any value. Non-zero values mean that the ship's attributes
  // are not allowed to go below that value when installing or selling outfits.
  const map<string, double> ALLOWLIST = {
    {"hull energy", 0.},
    {"hull fuel", 0.},
    {"hull heat", 0.},
    {"shield energy", 0.},
    {"shield fuel", 0.},
    {"shield heat", 0.},
    // resistances
    {"disruption resistance", 0.},
    {"force resistance", 0.},
    {"fuel resistance", 0.},
    {"heat resistance", 0.},
    {"hit force resistance", 0.},
    {"hull resistance", 0.},
    {"ion resistance", 0.},
    {"pierce resistance", 0.},
    {"shield resistance", 0.},
    {"slowing resistance", 0.}
  };
}



void Outfit::Load(const DataNode &node)
{
  if(node.Size() >= 2)
  {
    name = node.Token(1);
    pluralName = name + 's';
  }

  for(const DataNode &child : node)
  {
    if(child.Token(0) == "category" && child.Size() >= 2)
      category = child.Token(1);
    else if(child.Token(0) == "plural" && child.Size() >= 2)
      pluralName = child.Token(1);
    else if(child.Token(0) == "flare sprite" && child.Size() >= 2)
    {
      flareSprites.emplace_back(Body(), 1);
      flareSprites.back().first.LoadSprite(child);
    }
    else if(child.Token(0) == "flare sound" && child.Size() >= 2)
      ++flareSounds[Audio::Get(child.Token(1))];
    else if(child.Token(0) == "afterburner effect" && child.Size() >= 2)
      ++afterburnerEffects[GameData::Effects().Get(child.Token(1))];
    else if(child.Token(0) == "flotsam sprite" && child.Size() >= 2)
      flotsamSprite = SpriteSet::Get(child.Token(1));
    else if(child.Token(0) == "thumbnail" && child.Size() >= 2)
      thumbnail = SpriteSet::Get(child.Token(1));
    else if(child.Token(0) == "weapon")
      LoadWeapon(child);
    else if(child.Token(0) == "ammo" && child.Size() >= 2)
    {
      // Non-weapon outfits can have ammunition so that storage outfits
      // properly remove excess ammunition when the storage is sold, instead
      // of blocking the sale of the outfit until the ammo is sold first.
      ammo = make_pair(GameData::Outfits().Get(child.Token(1)), 0);
    }
    else if(child.Token(0) == "description" && child.Size() >= 2)
    {
      description += child.Token(1);
      description += '\n';
    }
    else if(child.Token(0) == "cost" && child.Size() >= 2)
      cost = child.Value(1);
    else if(child.Token(0) == "mass" && child.Size() >= 2)
      mass = child.Value(1);
    else if(child.Token(0) == "licences")
    {
      for(const DataNode &grand : child)
        licences.push_back(grand.Token(0));
    }
    else if(child.Size() >= 2)
      attributes[child.Token(0)] = child.Value(1);
    else
      child.PrintTrace("Skipping unrecognized attribute:");
  }

  // Legacy support for turrets that don't specify a turn rate:
  if(IsWeapon() && attributes.Get("turret mounts") && !TurretTurn() && !AntiMissile())
  {
    SetTurretTurn(4.);
    node.PrintTrace("Warning: Deprecated use of a turret without specified \"turret turn\":");
  }
  // Convert any legacy cargo / outfit scan definitions into power & speed,
  // so no runtime code has to check for both.
  auto convertScan = [&](string &&kind) -> void
  {
    const string label = kind + " scan";
    double initial = attributes.Get(label);
    if(initial)
    {
      attributes[label] = 0.;
      node.PrintTrace("Warning: Deprecated use of \"" + label + "\" instead of \""
          + label + " power\" and \"" + label + " speed\":");

      // A scan value of 300 is equivalent to a scan power of 9.
      attributes[label + " power"] += initial * initial * .0001;
      // The default scan speed of 1 is unrelated to the magnitude of the scan value.
      // It may have been already specified, and if so, should not be increased.
      if(!attributes.Get(label + " speed"))
        attributes[label + " speed"] = 1.;
    }
  };
  convertScan("outfit");
  convertScan("cargo");
}



const string &Outfit::Name() const
{
  return name;
}



const string &Outfit::PluralName() const
{
  return pluralName;
}



const string &Outfit::Category() const
{
  return category;
}



const string &Outfit::Description() const
{
  return description;
}



// Get the licences needed to purchase this outfit.
const vector<string> &Outfit::Licences() const
{
  return licences;
}



// Get the image to display in the outfitter when buying this item.
const Sprite *Outfit::Thumbnail() const
{
  return thumbnail;
}



double Outfit::Get(const char *attribute) const
{
  return attributes.Get(attribute);
}



double Outfit::Get(const string &attribute) const
{
  return Get(attribute.c_str());
}



const Dictionary &Outfit::Attributes() const
{
  return attributes;
}



// Determine whether the given number of instances of the given outfit can
// be added to a ship with the attributes represented by this instance. If
// not, return the maximum number that can be added.
int Outfit::CanAdd(const Outfit &other, int count) const
{
  for(const auto &at : other.attributes)
  {
    // The minimum allowed value of most attributes is 0. Some attributes
    // have special functionality when negative, though, and are therefore
    // allowed to have values less than 0.
    double minimum = 0.;
    auto it = ALLOWLIST.find(at.first);
    if(it != ALLOWLIST.end())
    {
      minimum = it->second;
      // Attributes listed with a minimum value of 0 can have any value.
      if(!minimum)
        continue;
    }
    double value = Get(at.first);
    // Allow for rounding errors:
    if(value + at.second * count < minimum - EPS)
      count = (value - minimum) / -at.second + EPS;
  }

  return count;
}



// For tracking a combination of outfits in a ship: add the given number of
// instances of the given outfit to this outfit.
void Outfit::Add(const Outfit &other, int count)
{
  cost += other.cost * count;
  mass += other.mass * count;
  for(const auto &at : other.attributes)
  {
    attributes[at.first] += at.second * count;
    if(fabs(attributes[at.first]) < EPS)
      attributes[at.first] = 0.;
  }

  for(const auto &it : other.flareSprites)
  {
    auto oit = flareSprites.begin();
    for( ; oit != flareSprites.end(); ++oit)
      if(oit->first.GetSprite() == it.first.GetSprite())
        break;

    if(oit == flareSprites.end())
      flareSprites.emplace_back(it.first, count * it.second);
    else
      oit->second += count * it.second;
  }
  for(const auto &it : other.flareSounds)
    flareSounds[it.first] += count * it.second;
  for(const auto &it : other.afterburnerEffects)
    afterburnerEffects[it.first] += count * it.second;
}



// Modify this outfit's attributes.
void Outfit::Set(const char *attribute, double value)
{
  attributes[attribute] = value;
}



// Get this outfit's engine flare sprite, if any.
const vector<pair<Body, int>> &Outfit::FlareSprites() const
{
  return flareSprites;
}



const map<const Sound *, int> &Outfit::FlareSounds() const
{
  return flareSounds;
}



// Get the afterburner effect, if any.
const map<const Effect *, int> &Outfit::AfterburnerEffects() const
{
  return afterburnerEffects;
}



// Get the sprite this outfit uses when dumped into space.
const Sprite *Outfit::FlotsamSprite() const
{
  return flotsamSprite;
}
