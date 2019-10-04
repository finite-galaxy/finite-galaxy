// ShipInfoDisplay.cpp

#include "ShipInfoDisplay.h"

#include "Colour.h"
#include "Depreciation.h"
#include "FillShader.h"
#include "Format.h"
#include "GameData.h"
#include "Outfit.h"
#include "Ship.h"
#include "Table.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>

using namespace std;



ShipInfoDisplay::ShipInfoDisplay(const Ship &ship, const Depreciation &depreciation, int day)
{
  Update(ship, depreciation, day);
}



// Call this every time the ship changes.
void ShipInfoDisplay::Update(const Ship &ship, const Depreciation &depreciation, int day)
{
  UpdateDescription(ship.Description(), ship.Attributes().Licences(), true);
  UpdateAttributes(ship, depreciation, day);
  UpdateOutfits(ship, depreciation, day);

  maximumHeight = max(descriptionHeight, max(attributesHeight, outfitsHeight));
}



int ShipInfoDisplay::OutfitsHeight() const
{
  return outfitsHeight;
}



int ShipInfoDisplay::SaleHeight() const
{
  return saleHeight;
}



// Draw each of the panels.
void ShipInfoDisplay::DrawAttributes(const Point &topLeft) const
{
  Point point = Draw(topLeft, attributeLabels, attributeValues);

  // Get standard colours to draw with.
  const Colour &labelColour = *GameData::Colours().Get("medium");
  const Colour &valueColour = *GameData::Colours().Get("bright");

  Table table;
  table.AddColumn(10, Table::LEFT);
  table.AddColumn(WIDTH - 120, Table::RIGHT);
  table.AddColumn(WIDTH - 60, Table::RIGHT);
  table.AddColumn(WIDTH - 10, Table::RIGHT);
  table.SetHighlight(0, WIDTH);
  table.DrawAt(point);
  table.DrawGap(10.);

  table.Advance();
  table.Draw("fuel", labelColour);
  table.Draw("energy", labelColour);
  table.Draw("heat", labelColour);

  for(unsigned i = 0; i < tableLabels.size(); ++i)
  {
    CheckHover(table, tableLabels[i]);
    table.Draw(tableLabels[i], labelColour);
    table.Draw(fuelTable[i], valueColour);
    table.Draw(energyTable[i], valueColour);
    table.Draw(heatTable[i], valueColour);
  }
}



void ShipInfoDisplay::DrawOutfits(const Point &topLeft) const
{
  Draw(topLeft, outfitLabels, outfitValues);
}



void ShipInfoDisplay::DrawSale(const Point &topLeft) const
{
  Draw(topLeft, saleLabels, saleValues);

  const Colour &colour = *GameData::Colours().Get("medium");
  FillShader::Fill(topLeft + Point(.5 * WIDTH, saleHeight + 8.), Point(WIDTH - 20., 1.), colour);
}



void ShipInfoDisplay::UpdateAttributes(const Ship &ship, const Depreciation &depreciation, int day)
{
  bool isGeneric = ship.Name().empty() || ship.GetPlanet();

  attributeLabels.clear();
  attributeValues.clear();
  attributesHeight = 20;

  const Outfit &attributes = ship.Attributes();

  int64_t fullCost = ship.Cost();
  int64_t depreciated = depreciation.Value(ship, day);
  if(depreciated == fullCost)
    attributeLabels.push_back("cost:");
  else
  {
    ostringstream out;
    out << "value (" << (100 * depreciated) / fullCost << "%):";
    attributeLabels.push_back(out.str());
  }
  attributeValues.push_back(Format::Credits(depreciated));
  attributesHeight += 20;

  attributeLabels.push_back(string());
  attributeValues.push_back(string());
  attributesHeight += 10;

  int maximumShields = attributes.Get("shields");
  int maximumHull = attributes.Get("hull");
  int maximumFuel = attributes.Get("fuel capacity");
  int maximumEnergy = attributes.Get("energy capacity");
  int maximumHeat = 60. * ship.HeatDissipation() * ship.MaximumHeat();
/*
  if(!isGeneric)
  {
    ostringstream shields;
    shields << "shields (" << round(100. * ship.Shields()) << "%):";
    attributeLabels.push_back(shields.str());
    attributeValues.push_back(Format::Round(ship.Shields() * maximumShields)
      + " / " + Format::Round(maximumShields));
    attributesHeight += 20;
    ostringstream hull;
    hull << "hull (" << round(100. * ship.Shields()) << "%):";
    attributeLabels.push_back(hull.str());
    attributeValues.push_back(Format::Round(ship.Hull() * maximumHull)
      + " / " + Format::Round(maximumHull));
    attributesHeight += 20;
    ostringstream fuel;
    fuel << "fuel (" << round(100. * ship.Fuel()) << "%):";
    attributeLabels.push_back(fuel.str());
    attributeValues.push_back(Format::Round(ship.Fuel() * maximumFuel)
      + " / " + Format::Round(maximumFuel));
    attributesHeight += 20;
    ostringstream energy;
    energy << "energy (" << round(100. * ship.Energy()) << "%):";
    attributeLabels.push_back(energy.str());
    attributeValues.push_back(Format::Round(ship.Energy() * maximumEnergy)
      + " / " + Format::Round(maximumEnergy));
    attributesHeight += 20;
    ostringstream heat;
    heat << "heat (" << round(100. * ship.Heat()) << "%):";
    attributeLabels.push_back(heat.str());
    attributeValues.push_back(Format::Round(ship.Heat() * maximumHeat)
      + " / " + Format::Round(maximumHeat));
    attributesHeight += 20;
  }
  else
*/
  {
    if(attributes.Get("shield generation"))
    {
      attributeLabels.push_back("shields charge / max:");
      attributeValues.push_back(Format::Number(60. * attributes.Get("shield generation"))
        + " / " + Format::Number(maximumShields));
    }
    else
    {
      attributeLabels.push_back("shields:");
      attributeValues.push_back(Format::Number(maximumShields));
    }
    attributesHeight += 20;
    if(attributes.Get("hull repair rate"))
    {
      attributeLabels.push_back("hull repair / max:");
      attributeValues.push_back(Format::Number(60. * attributes.Get("hull repair rate"))
        + " / " + Format::Number(maximumHull));
    }
    else
    {
      attributeLabels.push_back("hull:");
      attributeValues.push_back(Format::Number(maximumHull));
    }
    attributesHeight += 20;
  }

  attributeLabels.push_back(string());
  attributeValues.push_back(string());
  attributesHeight += 10;

  attributeLabels.push_back("required crew / bunks:");
  attributeValues.push_back(Format::Number(ship.RequiredCrew())
    + " / " + Format::Number(attributes.Get("bunks")));
  attributesHeight += 20;
  attributeLabels.push_back(isGeneric ? "cargo space:" : "cargo:");
  if(isGeneric)
    attributeValues.push_back(Format::Number(attributes.Get("cargo space")));
  else
    attributeValues.push_back(Format::Number(ship.Cargo().Used())
      + " / " + Format::Number(attributes.Get("cargo space")));
  attributesHeight += 20;

  // Find out how much outfit, core, engine, and weapon space the chassis has.
  map<string, double> chassis;
  static const vector<string> NAMES = {
    "outfit space",
    "core space",
    "engine space",
    "weapon space",
    "gun ports",
    "turret mounts"
  };
  for(unsigned i = 0; i < NAMES.size(); i++)
    chassis[NAMES[i]] = attributes.Get(NAMES[i]);
  for(const auto &it : ship.Outfits())
    for(auto &cit : chassis)
      cit.second -= it.second * it.first->Get(cit.first);

  attributeLabels.push_back("free outfit space:");
  attributeValues.push_back(Format::Number(attributes.Get(NAMES[0]))
      + " / " + Format::Number(chassis[NAMES[0]]));
  attributesHeight += 20;
  attributeLabels.push_back("    core space:");
  attributeValues.push_back(Format::Number(attributes.Get(NAMES[1]))
      + " / " + Format::Number(chassis[NAMES[1]]));
  attributesHeight += 20;
  attributeLabels.push_back("    engine space:");
  attributeValues.push_back(Format::Number(attributes.Get(NAMES[2]))
      + " / " + Format::Number(chassis[NAMES[2]]));
  attributesHeight += 20;
  attributeLabels.push_back("    weapon space:");
  attributeValues.push_back(Format::Number(attributes.Get(NAMES[3]))
      + " / " + Format::Number(chassis[NAMES[3]]));
  attributesHeight += 20;
  if(chassis[NAMES[4]])
  {
    attributeLabels.push_back("gun ports free:");
    attributeValues.push_back(Format::Number(attributes.Get(NAMES[4]))
        + " / " + Format::Number(chassis[NAMES[4]]));
    attributesHeight += 20;
  }
  if(chassis[NAMES[5]])
  {
    attributeLabels.push_back("turret mounts free:");
    attributeValues.push_back(Format::Number(attributes.Get(NAMES[5]))
        + " / " + Format::Number(chassis[NAMES[5]]));
    attributesHeight += 20;
  }

  // Print the number of bays for each bay-type we have
  for(auto bayType : Ship::BAY_TYPES())
    if(ship.BaysFree(bayType))
    {
      // make sure the label is printed in lower case
      string bayLabel = bayType;
      transform(bayLabel.begin(), bayLabel.end(), bayLabel.begin(), ::tolower);

      attributeLabels.push_back(bayLabel + " bays:");
      attributeValues.push_back(to_string(ship.BaysFree(bayType)));
      attributesHeight += 20;
    }

  // Miscellaneous
  {
    attributeLabels.push_back(string());
    attributeValues.push_back(string());
    attributesHeight += 10;
    if(attributes.Get("radar jamming"))
    {
      attributeLabels.push_back("radar jamming:");
      attributeValues.push_back(Format::Number(attributes.Get("radar jamming")));
      attributesHeight += 20;
    }
    if(attributes.Get("ramscoop"))
    {
      attributeLabels.push_back("ramscoop:");
      attributeValues.push_back(Format::Number(attributes.Get("ramscoop")));
      attributesHeight += 20;
    }
    if(attributes.Get("scan interference"))
    {
      attributeLabels.push_back("chance to block scan:");
      attributeValues.push_back(Format::Number(100.
        - 100. / (1. + attributes.Get("scan interference"))) + "%");
      attributesHeight += 20;
    }
    if(attributes.Get("asteroid scan power"))
    {
      attributeLabels.push_back("asteroid scan range:");
      attributeValues.push_back(Format::Round(100. * sqrt(attributes.Get("asteroid scan power"))));
      attributesHeight += 20;
    }
    if(attributes.Get("cargo scan power"))
    {
      attributeLabels.push_back("cargo scan range:");
      attributeValues.push_back(Format::Round(100. * sqrt(attributes.Get("cargo scan power"))));
      attributesHeight += 20;
    }
    if(attributes.Get("outfit scan power"))
    {
      attributeLabels.push_back("outfit scan range:");
      attributeValues.push_back(Format::Round(100. * sqrt(attributes.Get("outfit scan power"))));
      attributesHeight += 20;
    }
    if(attributes.Get("tactical scan power"))
    {
      attributeLabels.push_back("tactical scan range:");
      attributeValues.push_back(Format::Round(100. * sqrt(attributes.Get("tactical scan power"))));
      attributesHeight += 20;
    }
  }

  // Movement
  double emptyMass = ship.Mass();
  double fullMass = emptyMass + (isGeneric ? attributes.Get("cargo space") : ship.Cargo().Used());
  isGeneric &= (fullMass != emptyMass);
  double thruster = attributes.Get("thrust");
  double afterburner = attributes.Get("afterburner thrust");
  double reverse = -attributes.Get("reverse thrust");
  double turning = attributes.Get("turn");
  double forwardThrust = reverse && !thruster ? reverse : thruster;
  attributeLabels.push_back(string());
  attributeValues.push_back(string());
  attributesHeight += 10;

  if(!isGeneric)
  {
    attributeLabels.push_back("mass:");
    attributeValues.push_back(Format::Round(emptyMass));
  }
  else
  {
    attributeLabels.push_back("mass full / no cargo:");
    attributeValues.push_back(Format::Round(fullMass) + " / " + Format::Round(emptyMass));
  }
  attributesHeight += 20;


  attributeLabels.push_back("    maximum velocity:");
  if(afterburner)
    attributeValues.push_back(Format::Round(60. * forwardThrust / attributes.Get("drag"))
      + " + " + Format::Round(60. * afterburner / attributes.Get("drag")));
  else
    attributeValues.push_back(Format::Round(60. * forwardThrust / attributes.Get("drag")));
  attributesHeight += 20;

  attributeLabels.push_back("    acceleration:");
  if(!isGeneric)
    attributeValues.push_back(Format::Round(3600. * thruster / fullMass));
  else
    attributeValues.push_back(Format::Round(3600. * thruster / fullMass)
      + " / " + Format::Round(3600. * thruster / emptyMass));
  attributesHeight += 20;

  if(afterburner)
  {
    attributeLabels.push_back("    afterburner:");
    if(!isGeneric)
      attributeValues.push_back(Format::Round(3600. * afterburner / fullMass));
    else
      attributeValues.push_back(Format::Round(3600. * afterburner / fullMass)
        + " / " + Format::Round(3600. * afterburner / emptyMass));
    attributesHeight += 20;
  }

  if(reverse)
  {
    attributeLabels.push_back("    reverse:");
    if(!isGeneric)
      attributeValues.push_back(Format::Round(3600. * reverse / fullMass));
    else
      attributeValues.push_back(Format::Round(3600. * reverse / fullMass)
        + " / " + Format::Round(3600. * reverse / emptyMass));
    attributesHeight += 20;
  }

  attributeLabels.push_back("    turning:");
  if(!isGeneric)
    attributeValues.push_back(Format::Round(60. * turning / fullMass));
  else
    attributeValues.push_back(Format::Round(60. * turning / fullMass)
      + "° / " + Format::Round(60. * turning / emptyMass) + "°");
  attributesHeight += 20;
  if(!ship.CanBeCarried() && (attributes.Get("hyperdrive") || attributes.Get("jump drive")))
  {
    if(attributes.Get("scram drive"))
    {
      attributeLabels.push_back("scram drive:");
      attributeValues.push_back(Format::Number(attributes.Get("scram drive")));
      attributesHeight += 20;
    }
    else
    {
      attributeLabels.push_back("jump speed:");
      attributeValues.push_back(Format::Number(60 * attributes.Get("jump speed")));
      attributesHeight += 20;
    }
  }

  // fuel/energy/heat table
  tableLabels.clear();
  fuelTable.clear();
  energyTable.clear();
  heatTable.clear();
  // Skip a spacer and the table header.
  attributesHeight += 30;

  tableLabels.push_back("idle:");
  fuelTable.push_back(Format::Round(
    60. * (attributes.Get("fuel generation")
      - attributes.Get("fuel consumption"))));
  energyTable.push_back(Format::Round(
    60. * (attributes.Get("energy generation")
      + attributes.Get("solar collection")
      + attributes.Get("fuel energy")
      - attributes.Get("energy consumption")
      - (attributes.Get("bunks") / 60.))));
  double efficiency = ship.CoolingEfficiency();
  heatTable.push_back(Format::Round(
    60. * (attributes.Get("heat generation")
      + attributes.Get("fuel heat")
      - efficiency * attributes.Get("cooling"))));
  attributesHeight += 20;

  if(attributes.Get("cloak"))
  {
    tableLabels.push_back("cloaking:");
    fuelTable.push_back(Format::Round(-60. * attributes.Get("cloaking fuel")));
    energyTable.push_back(Format::Round(-60. * attributes.Get("cloaking energy")));
    heatTable.push_back(Format::Round(60. * attributes.Get("cloaking heat")));
    attributesHeight += 20;
  }

  double firingFuel = 0.;
  double firingEnergy = 0.;
  double firingHeat = 0.;
  for(const auto &it : ship.Outfits())
    if(it.first->IsWeapon() && it.first->Reload())
    {
      firingFuel += it.second * it.first->FiringFuel() / it.first->Reload();
      firingEnergy += it.second * it.first->FiringEnergy() / it.first->Reload();
      firingHeat += it.second * it.first->FiringHeat() / it.first->Reload();
    }
  tableLabels.push_back("firing:");
  fuelTable.push_back(Format::Round(60. * firingFuel));
  energyTable.push_back(Format::Round(-60. * firingEnergy));
  heatTable.push_back(Format::Round(60. * firingHeat));
  attributesHeight += 20;

  tableLabels.push_back("moving:");
  fuelTable.push_back(Format::Round(
    -60. * (max(attributes.Get("thrusting fuel"), attributes.Get("reverse thrusting fuel"))
      + attributes.Get("turning fuel")
      + attributes.Get("afterburner fuel"))));
  energyTable.push_back(Format::Round(
    -60. * (max(attributes.Get("thrusting energy"), attributes.Get("reverse thrusting energy"))
      + attributes.Get("turning energy")
      + attributes.Get("afterburner energy"))));
  heatTable.push_back(Format::Round(
    60. * (max(attributes.Get("thrusting heat"), attributes.Get("reverse thrusting heat"))
      + attributes.Get("turning heat")
      + attributes.Get("afterburner heat"))));
  attributesHeight += 20;

  tableLabels.push_back("recovering:");
  fuelTable.push_back(Format::Round(
    -60. * (attributes.Get("shield fuel")
      + attributes.Get("hull fuel"))));
  energyTable.push_back(Format::Round(
    -60. * (attributes.Get("shield energy")
      + attributes.Get("hull energy")
      + attributes.Get("cooling energy"))));
  heatTable.push_back(Format::Round(
    60. * (attributes.Get("shield heat")
      + attributes.Get("hull heat")
      - ship.CoolingEfficiency() * attributes.Get("active cooling"))));
  attributesHeight += 20;

  tableLabels.push_back("capacity:");
  fuelTable.push_back(Format::Round(maximumFuel));
  energyTable.push_back(Format::Round(maximumEnergy));
  heatTable.push_back(Format::Round(maximumHeat));
  // Pad by 10 pixels on the top and bottom.
  attributesHeight += 30;
}



void ShipInfoDisplay::UpdateOutfits(const Ship &ship, const Depreciation &depreciation, int day)
{
  outfitLabels.clear();
  outfitValues.clear();
  outfitsHeight = 20;

  map<string, map<string, int>> listing;
  for(const auto &it : ship.Outfits())
    listing[it.first->Category()][it.first->Name()] += it.second;

  for(const auto &cit : listing)
  {
    // Pad by 10 pixels before each category.
    if(&cit != &*listing.begin())
    {
      outfitLabels.push_back(string());
      outfitValues.push_back(string());
      outfitsHeight += 10;
    }
  
    outfitLabels.push_back(cit.first + ':');
    outfitValues.push_back(string());
    outfitsHeight += 20;

    for(const auto &it : cit.second)
    {
      outfitLabels.push_back(it.first);
      outfitValues.push_back(to_string(it.second));
      outfitsHeight += 20;
    }
  }


  int64_t totalCost = depreciation.Value(ship, day);
  int64_t chassisCost = depreciation.Value(GameData::Ships().Get(ship.ModelName()), day);
  saleLabels.clear();
  saleValues.clear();
  saleHeight = 20;

  saleLabels.push_back("This ship will sell for:");
  saleValues.push_back(string());
  saleHeight += 20;
  saleLabels.push_back("empty hull:");
  saleValues.push_back(Format::Credits(chassisCost));
  saleHeight += 20;
  saleLabels.push_back("    + outfits:");
  saleValues.push_back(Format::Credits(totalCost - chassisCost));
  saleHeight += 5;
}

