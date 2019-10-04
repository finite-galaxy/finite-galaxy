// OutfitInfoDisplay.cpp

#include "OutfitInfoDisplay.h"

#include "Depreciation.h"
#include "Format.h"
#include "Outfit.h"
#include "PlayerInfo.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <sstream>

using namespace std;

namespace {
  const map<string, string> BOOLEAN_ATTRIBUTES = {
    {"installable", "This is not an installable item."},
    {"plunderable", "Can be plundered when installed."},
    {"unplunderable", "Can never be plundered."},
    {"hyperdrive", "Allows you to make hyperjumps."},
    {"jump drive", "Lets you jump to any nearby system."}
  };

  const map<string, double> PERCENT_ATTRIBUTES = {
    {"jump heat", 1.}
  };

  const map<string, double> SCALE = {
    {"active cooling", 60.},
    {"afterburner energy", 60.},
    {"afterburner fuel", 60.},
    {"afterburner heat", 60.},
    {"afterburner thrust", 60. * 60.},
    {"cloak", 60.},
    {"cloaking energy", 60.},
    {"cloaking fuel", 60.},
    {"cloaking heat", 60.},
    {"cooling", 60.},
    {"cooling energy", 60.},
    {"energy consumption", 60.},
    {"energy generation", 60.},
    {"fuel consumption", 60.},
    {"fuel energy", 60.},
    {"fuel generation", 60.},
    {"fuel heat", 60.},
    {"heat generation", 60.},
    {"hull repair rate", 60.},
    {"hull energy", 60.},
    {"hull fuel", 60.},
    {"hull heat", 60.},
    {"jump speed", 60.},
    {"reverse thrust", 60. * 60.},
    {"reverse thrusting energy", 60.},
    {"reverse thrusting heat", 60.},
    {"shield energy", 60.},
    {"shield fuel", 60.},
    {"shield generation", 60.},
    {"shield heat", 60.},
    {"solar collection", 60.},
    {"thrust", 60. * 60.},
    {"thrusting energy", 60.},
    {"thrusting heat", 60.},
    {"turn", 60.},
    {"turning energy", 60.},
    {"turning heat", 60.},

    {"disruption resistance", 60. * 100.},
    {"ion resistance", 60. * 100.},
    {"slowing resistance", 60. * 100.}
  };
}



OutfitInfoDisplay::OutfitInfoDisplay(const Outfit &outfit, const PlayerInfo &player, bool canSell)
{
  Update(outfit, player, canSell);
}



// Call this every time the ship changes.
void OutfitInfoDisplay::Update(const Outfit &outfit, const PlayerInfo &player, bool canSell)
{
  UpdateDescription(outfit.Description(), outfit.Licences(), false);
  UpdateRequirements(outfit, player, canSell);
  UpdateAttributes(outfit);

  maximumHeight = max(descriptionHeight, max(requirementsHeight, attributesHeight));
}



int OutfitInfoDisplay::RequirementsHeight() const
{
  return requirementsHeight;
}



void OutfitInfoDisplay::DrawRequirements(const Point &topLeft) const
{
  Draw(topLeft, requirementLabels, requirementValues);
}



void OutfitInfoDisplay::UpdateRequirements(const Outfit &outfit, const PlayerInfo &player, bool canSell)
{
  requirementLabels.clear();
  requirementValues.clear();
  requirementsHeight = 20;

  int day = player.GetDate().DaysSinceEpoch();
  int64_t cost = outfit.Cost();
  int64_t buyValue = player.StockDepreciation().Value(&outfit, day);
  int64_t sellValue = player.FleetDepreciation().Value(&outfit, day);

  if(buyValue == cost)
    requirementLabels.push_back("cost:");
  else
  {
    ostringstream out;
    out << "cost (" << (100 * buyValue) / cost << "%):";
    requirementLabels.push_back(out.str());
  }
  requirementValues.push_back(Format::Credits(buyValue));
  requirementsHeight += 20;

  if(canSell && sellValue != buyValue)
  {
    if(sellValue == cost)
      requirementLabels.push_back("sells for:");
    else
    {
      ostringstream out;
      out << "sells for (" << (100 * sellValue) / cost << "%):";
      requirementLabels.push_back(out.str());
    }
    requirementValues.push_back(Format::Credits(sellValue));
    requirementsHeight += 20;
  }

  if(outfit.Mass())
  {
    requirementLabels.emplace_back("mass:");
    requirementValues.emplace_back(Format::Number(outfit.Mass()));
    requirementsHeight += 20;
  }

  bool hasContent = true;
  static const vector<string> NAMES = {
    "", "",
    "outfit space needed:", "outfit space",
    "core space needed:", "core space",
    "engine space needed:", "engine space",
    "weapon space needed:", "weapon space",
    "", "",
    "gun ports needed:", "gun ports",
    "turret mounts needed:", "turret mounts"
  };
  for(unsigned i = 0; i + 1 < NAMES.size(); i += 2)
  {
    if(NAMES[i].empty() && hasContent)
    {
      requirementLabels.emplace_back();
      requirementValues.emplace_back();
      requirementsHeight += 10;
      hasContent = false;
    }
    else if(outfit.Get(NAMES[i + 1]))
    {
      requirementLabels.push_back(NAMES[i]);
      requirementValues.push_back(Format::Number(-outfit.Get(NAMES[i + 1])));
      requirementsHeight += 20;
      hasContent = true;
    }
  }
}



void OutfitInfoDisplay::UpdateAttributes(const Outfit &outfit)
{
  attributeLabels.clear();
  attributeValues.clear();
  attributesHeight = 20;

  bool hasNormalAttributes = false;
  for(const pair<const char *, double> &it : outfit.Attributes())
  {
    static const set<string> SKIP = {
      "outfit space",
      "core space",
      "engine space",
      "weapon space",
      "gun ports",
      "turret mounts"
    };
    if(SKIP.count(it.first))
      continue;

    auto bit = BOOLEAN_ATTRIBUTES.find(it.first);
    auto pit = PERCENT_ATTRIBUTES.find(it.first);
    auto sit = SCALE.find(it.first);
    double scale = (sit == SCALE.end() ? 1. : sit->second);

    if(bit != BOOLEAN_ATTRIBUTES.end()) 
    {
      attributeLabels.emplace_back(bit->second);
      attributeValues.emplace_back(" ");
      attributesHeight += 20;
    }
    else if(pit != PERCENT_ATTRIBUTES.end())
    {
      attributeLabels.emplace_back(static_cast<string>(it.first) + ":");
      attributeValues.emplace_back(Format::Number(it.second * 100) + "%");
      attributesHeight += 20;
    }
    else
    {
      attributeLabels.emplace_back(static_cast<string>(it.first) + ":");
      attributeValues.emplace_back(Format::Number(it.second * scale));
      attributesHeight += 20;
    }
    hasNormalAttributes = true;
  }

  if(!outfit.IsWeapon())
    return;

  // Insert padding if any normal attributes were listed above.
  if(hasNormalAttributes)
  {
    attributeLabels.emplace_back();
    attributeValues.emplace_back();
    attributesHeight += 10;
  }

  static const vector<string> VALUE_NAMES = {
    "shield damage",
    "hull damage",
    "disruption damage",
    "fuel damage",
    "ion damage",
    "heat damage",
    "slowing damage",
    "hit force",
    "firing fuel",
    "firing energy",
    "firing heat",
    "firing force"
  };

  vector<double> values = {
    outfit.ShieldDamage(),
    outfit.HullDamage(),
    outfit.DisruptionDamage() * 100.,
    outfit.FuelDamage(),
    outfit.IonDamage() * 100.,
    outfit.HeatDamage(),
    outfit.SlowingDamage() * 100.,
    outfit.HitForce(),
    outfit.FiringFuel(),
    outfit.FiringEnergy(),
    outfit.FiringHeat(),
    outfit.FiringForce()
  };

  vector<double> randomValues = {
    outfit.RandomShieldDamage(),
    outfit.RandomHullDamage(),
    outfit.RandomDisruptionDamage() * 100.,
    outfit.RandomFuelDamage(),
    outfit.RandomIonDamage() * 100.,
    outfit.RandomHeatDamage(),
    outfit.RandomSlowingDamage() * 100.,
    outfit.RandomHitForce(),
    0,
    0,
    0,
    0
  };

  double reload = outfit.Reload();
  bool isContinuous = (reload <= 1);
  // Add any per shot and per second values to the table.
  if(reload)
  {
    if(isContinuous)
    {
      attributeLabels.emplace_back("per second:");
      attributeValues.emplace_back(" ");
      attributesHeight += 10;
      for(unsigned i = 0; i < values.size(); ++i)
      {
        if(randomValues[i] && values[i])
        {
          attributeLabels.emplace_back("    " + VALUE_NAMES[i] + ":");
          attributeValues.emplace_back(Format::Number(60. * (values[i] + 0.5 * randomValues[i]) / reload));
          attributesHeight += 20;
        }
        else if(randomValues[i])
        {
          attributeLabels.emplace_back("    " + VALUE_NAMES[i] + ":");
          attributeValues.emplace_back(Format::Number(60. * 0.5 * randomValues[i] / reload));
          attributesHeight += 20;
        }
        else if(values[i])
        {
          attributeLabels.emplace_back("    " + VALUE_NAMES[i] + ":");
          attributeValues.emplace_back(Format::Number(60. * values[i] / reload));
          attributesHeight += 20;
        }
      }
    }
    else
    {
      attributeLabels.emplace_back("per shot / per second:");
      attributeValues.emplace_back(" ");
      attributesHeight += 10;
      for(unsigned i = 0; i < values.size(); ++i)
      {
        if(randomValues[i] && values[i])
        {
          attributeLabels.emplace_back("    " + VALUE_NAMES[i] + ":");
          attributeValues.emplace_back(Format::Number(values[i]) 
            + "–" + Format::Number(values[i] + randomValues[i]) 
            + " / " + Format::Number(60. * (values[i] + 0.5 * randomValues[i]) / reload));
          attributesHeight += 20;
        }
        else if(randomValues[i])
        {
          attributeLabels.emplace_back("    " + VALUE_NAMES[i] + ":");
          attributeValues.emplace_back(Format::Number(0) 
            + "–" + Format::Number(randomValues[i]) 
            + " / " + Format::Number(60. * 0.5 * randomValues[i] / reload));
          attributesHeight += 20;
        }
        else if(values[i])
        {
          attributeLabels.emplace_back("    " + VALUE_NAMES[i] + ":");
          attributeValues.emplace_back(Format::Number(values[i]) 
            + " / " + Format::Number(60. * values[i] / reload));
          attributesHeight += 20;
        }
      }
    }
  }

  // Pad the table.
  attributeLabels.emplace_back();
  attributeValues.emplace_back();
  attributesHeight += 10;

  if(outfit.Ammo())
  {
    attributeLabels.emplace_back("ammunition:");
    attributeValues.emplace_back(outfit.Ammo()->Name());
    attributesHeight += 20;
  }

  attributeLabels.emplace_back("shots per second:");
  if(isContinuous)
    attributeValues.emplace_back("continuous");
  else
    attributeValues.emplace_back(Format::Number(60. / reload));
  attributesHeight += 20;

  if(outfit.Lifetime() > 1)
  {
    attributeLabels.emplace_back("velocity / range:");
    attributeValues.emplace_back(Format::Number(60. * outfit.Velocity())
      + " / " + Format::Number(outfit.Range()));
    attributesHeight += 20;
  }
  else
  {
    attributeLabels.emplace_back("range:");
    attributeValues.emplace_back(Format::Number(outfit.Range()));
    attributesHeight += 20;
  }

  double turretTurn = outfit.TurretTurn() * 60.;
  if(turretTurn)
  {
    attributeLabels.emplace_back("turret turn rate:");
    attributeValues.emplace_back(Format::Number(turretTurn) + "°");
    attributesHeight += 20;
  }
  int homing = outfit.Homing();
  if(homing)
  {
    static const string skill[] = {
      "none",
      "poor",
      "fair",
      "good",
      "excellent"
    };
    attributeLabels.emplace_back("homing:");
    attributeValues.push_back(skill[max(0, min(4, homing))]);
    attributesHeight += 20;
  }
  static const vector<string> PERCENT_NAMES = {
    "tracking:",
    "optical tracking:",
    "infrared tracking:",
    "radar tracking:",
    "piercing:"
  };
  vector<double> percentValues = {
    outfit.Tracking(),
    outfit.OpticalTracking(),
    outfit.InfraredTracking(),
    outfit.RadarTracking(),
    outfit.Piercing()
  };
  for(unsigned i = 0; i < PERCENT_NAMES.size(); ++i)
    if(percentValues[i])
    {
      int percent = lround(100. * percentValues[i]);
      attributeLabels.push_back(PERCENT_NAMES[i]);
      attributeValues.push_back(Format::Number(percent) + "%");
      attributesHeight += 20;
    }

  static const vector<string> OTHER_NAMES = {
    "inaccuracy:",
    "blast radius:",
    "missile strength:",
    "anti-missile:"
  };
  vector<double> otherValues = {
    outfit.Inaccuracy(),
    outfit.BlastRadius(),
    static_cast<double>(outfit.MissileStrength()),
    static_cast<double>(outfit.AntiMissile())
  };

  for(unsigned i = 0; i < OTHER_NAMES.size(); ++i)
    if(otherValues[i])
    {
      attributeLabels.emplace_back(OTHER_NAMES[i]);
      attributeValues.emplace_back(Format::Number(otherValues[i]));
      attributesHeight += 20;
    }
}

