// OutfitterPanel.cpp

#include "OutfitterPanel.h"

#include "Colour.h"
#include "Dialogue.h"
#include "DistanceMap.h"
#include "FillShader.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "GameData.h"
#include "Hardpoint.h"
#include "Outfit.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "Point.h"
#include "Screen.h"
#include "Ship.h"
#include "Sprite.h"
#include "SpriteSet.h"
#include "SpriteShader.h"
#include "UI.h"

#include <algorithm>
#include <limits>
#include <memory>

using namespace std;

namespace {
  string Tons(int tons)
  {
    return to_string(tons) + (tons == 1 ? " ton" : " tons");
  }
}



OutfitterPanel::OutfitterPanel(PlayerInfo &player)
  : ShopPanel(player, true)
{
  for(const pair<const string, Outfit> &it : GameData::Outfits())
    catalogue[it.second.Category()].insert(it.first);

  // Add owned licences
  const string PREFIX = "licence: ";
  for(auto &it : player.Conditions())
    if(it.first.compare(0, PREFIX.length(), PREFIX) == 0 && it.second > 0)
    {
      const string name = it.first.substr(PREFIX.length()) + " Licence";
      const Outfit *outfit = GameData::Outfits().Get(name);
      if(outfit)
        catalogue[outfit->Category()].insert(name);
    }

  if(player.GetPlanet())
    outfitter = player.GetPlanet()->Outfitter();
}



void OutfitterPanel::Step()
{
  CheckRefill();
  DoHelp("outfitter");
  ShopPanel::Step();
}



int OutfitterPanel::TileSize() const
{
  return OUTFIT_SIZE;
}



int OutfitterPanel::DrawPlayerShipInfo(const Point &point)
{
  shipInfo.Update(*playerShip, player.FleetDepreciation(), day);
  shipInfo.DrawAttributes(point);

  return shipInfo.AttributesHeight();
}



bool OutfitterPanel::HasItem(const string &name) const
{
  const Outfit *outfit = GameData::Outfits().Get(name);
  if((outfitter.Has(outfit) || player.Stock(outfit) > 0) && showForSale)
    return true;

  if(player.Cargo().Get(outfit) && (!playerShip || showForSale))
    return true;

  for(const Ship *ship : playerShips)
    if(ship->OutfitCount(outfit))
      return true;

  if(showForSale && HasLicence(name))
    return true;

  return false;
}



void OutfitterPanel::DrawItem(const string &name, const Point &point, int scrollY)
{
  const Outfit *outfit = GameData::Outfits().Get(name);
  zones.emplace_back(point, Point(OUTFIT_SIZE, OUTFIT_SIZE), outfit, scrollY);
  if(point.Y() + OUTFIT_SIZE / 2 < Screen::Top() || point.Y() - OUTFIT_SIZE / 2 > Screen::Bottom())
    return;

  bool isSelected = (outfit == selectedOutfit);
  bool isOwned = playerShip && playerShip->OutfitCount(outfit);
  DrawOutfit(*outfit, point, isSelected, isOwned);

  // Check if this outfit is a "licence".
  bool isLicence = IsLicence(name);
  int mapSize = outfit->Get("map");

  const Font &font = FontSet::Get(18);
  const Colour &bright = *GameData::Colours().Get("bright");
  if(playerShip || isLicence || mapSize)
  {
    int minCount = numeric_limits<int>::max();
    int maxCount = 0;
    if(isLicence)
      minCount = maxCount = player.GetCondition(LicenceName(name));
    else if(mapSize)
      minCount = maxCount = HasMapped(mapSize);
    else
    {
      for(const Ship *ship : playerShips)
      {
        int count = ship->OutfitCount(outfit);
        minCount = min(minCount, count);
        maxCount = max(maxCount, count);
      }
    }

    if(maxCount)
    {
      string label = "installed: " + to_string(minCount);
      if(maxCount > minCount)
        label += " - " + to_string(maxCount);

      Point labelPos = point + Point(-OUTFIT_SIZE / 2 + 20, OUTFIT_SIZE / 2 - 38);
      font.Draw(label, labelPos, bright);
    }
  }
  // Don't show the "in stock" amount if the outfit has an unlimited stock or
  // if it is not something that you can buy.
  int stock = 0;
  if(!outfitter.Has(outfit) && outfit->Get("installable") >= 0.)
    stock = max(0, player.Stock(outfit));
  int cargo = player.Cargo().Get(outfit);

  string message;
  if(cargo && stock)
    message = "in cargo: " + to_string(cargo) + ", in stock: " + to_string(stock);
  else if(cargo)
    message = "in cargo: " + to_string(cargo);
  else if(stock)
    message = "in stock: " + to_string(stock);
  else if(!outfitter.Has(outfit))
    message = "(not sold here)";
  if(!message.empty())
  {
    Point pos = point + Point(
      OUTFIT_SIZE / 2 - 20 - font.Width(message),
      OUTFIT_SIZE / 2 - 24);
    font.Draw(message, pos, bright);
  }
}



int OutfitterPanel::DividerOffset() const
{
  return 80;
}



int OutfitterPanel::DetailWidth() const
{
  return 3 * outfitInfo.PanelWidth();
}



int OutfitterPanel::DrawDetails(const Point &centre)
{
  if(!selectedOutfit)
    return 0;

  outfitInfo.Update(*selectedOutfit, player, CanSell());
  Point offset(outfitInfo.PanelWidth(), 0.);

  outfitInfo.DrawDescription(centre - offset * 1.5 - Point(0., 10.));
  outfitInfo.DrawRequirements(centre - offset * .5 - Point(0., 10.));
  outfitInfo.DrawAttributes(centre + offset * .5 - Point(0., 10.));

  return outfitInfo.MaximumHeight();
}



bool OutfitterPanel::CanBuy() const
{
  if(!planet || !selectedOutfit)
    return false;

  bool isInCargo = player.Cargo().Get(selectedOutfit) && playerShip;
  if(!(outfitter.Has(selectedOutfit) || player.Stock(selectedOutfit) > 0 || isInCargo))
    return false;

  int mapSize = selectedOutfit->Get("map");
  if(mapSize > 0 && HasMapped(mapSize))
    return false;

  // Determine what you will have to pay to buy this outfit.
  int64_t cost = player.StockDepreciation().Value(selectedOutfit, day);
  // Check that the player has any necessary licences.
  int64_t licenceCost = LicenceCost(selectedOutfit);
  if(licenceCost < 0)
    return false;
  cost += licenceCost;
  // If you have this in your cargo hold, installing it is free.
  if(cost > player.Accounts().Credits() && !isInCargo)
    return false;

  if(HasLicence(selectedOutfit->Name()))
    return false;

  if(!playerShip)
  {
    double mass = selectedOutfit->Mass();
    return (!mass || player.Cargo().Free() >= mass);
  }

  for(const Ship *ship : playerShips)
    if(ShipCanBuy(ship, selectedOutfit))
      return true;

  return false;
}



void OutfitterPanel::Buy(bool fromCargo)
{
  int64_t licenceCost = LicenceCost(selectedOutfit);
  if(licenceCost)
  {
    player.Accounts().AddCredits(-licenceCost);
    for(const string &licenceName : selectedOutfit->Licences())
      if(!player.GetCondition("licence: " + licenceName))
        player.Conditions()["licence: " + licenceName] = true;
  }

  int modifier = Modifier();
  for(int i = 0; i < modifier && CanBuy(); ++i)
  {
    // Special case: maps.
    int mapSize = selectedOutfit->Get("map");
    if(mapSize > 0)
    {
      if(!HasMapped(mapSize))
      {
        DistanceMap distance(player.GetSystem(), mapSize);
        for(const System *system : distance.Systems())
          if(!player.HasVisited(system))
            player.Visit(system);
        int64_t price = player.StockDepreciation().Value(selectedOutfit, day);
        player.Accounts().AddCredits(-price);
      }
      return;
    }

    // Special case: licences.
    if(IsLicence(selectedOutfit->Name()))
    {
      auto &entry = player.Conditions()[LicenceName(selectedOutfit->Name())];
      if(entry <= 0)
      {
        entry = true;
        int64_t price = player.StockDepreciation().Value(selectedOutfit, day);
        player.Accounts().AddCredits(-price);
      }
      return;
    }

    if(!playerShip)
    {
      player.Cargo().Add(selectedOutfit);
      int64_t price = player.StockDepreciation().Value(selectedOutfit, day);
      player.Accounts().AddCredits(-price);
      player.AddStock(selectedOutfit, -1);
      continue;
    }

    // Find the ships with the fewest number of these outfits.
    const vector<Ship *> shipsToOutfit = GetShipsToOutfit(true);

    for(Ship *ship : shipsToOutfit)
    {
      if(!CanBuy())
        return;

      if(player.Cargo().Get(selectedOutfit))
        player.Cargo().Remove(selectedOutfit);
      else if(fromCargo || !(player.Stock(selectedOutfit) > 0 || outfitter.Has(selectedOutfit)))
        break;
      else
      {
        int64_t price = player.StockDepreciation().Value(selectedOutfit, day);
        player.Accounts().AddCredits(-price);
        player.AddStock(selectedOutfit, -1);
      }
      ship->AddOutfit(selectedOutfit, 1);
      int required = selectedOutfit->Get("required crew");
      if(required && ship->Crew() + required <= static_cast<int>(ship->Attributes().Get("bunks")))
        ship->AddCrew(required);
      ship->Recharge();
    }
  }
}



void OutfitterPanel::FailBuy() const
{
  if(!selectedOutfit)
    return;

  int64_t cost = player.StockDepreciation().Value(selectedOutfit, day);
  int64_t credits = player.Accounts().Credits();
  bool isInCargo = player.Cargo().Get(selectedOutfit);
  if(!isInCargo && cost > credits)
  {
    GetUI()->Push(new Dialogue("You cannot buy this outfit, because it costs "
      + Format::Credits(cost) + " credits, and you only have "
      + Format::Credits(credits) + "."));
    return;
  }
  // Check that the player has any necessary licences.
  int64_t licenceCost = LicenceCost(selectedOutfit);
  if(licenceCost < 0)
  {
    GetUI()->Push(new Dialogue(
      "You cannot buy this outfit, because it requires a licence that you don't have."));
    return;
  }
  if(!isInCargo && cost + licenceCost > credits)
  {
    GetUI()->Push(new Dialogue(
      "You don't have enough money to buy this outfit, because it will cost you an extra "
      + Format::Credits(licenceCost) + " credits to buy the necessary licences."));
    return;
  }

  if(!(outfitter.Has(selectedOutfit) || player.Stock(selectedOutfit) > 0 || isInCargo))
  {
    GetUI()->Push(new Dialogue("You cannot buy this outfit here. "
      "It is being shown in the list because you have one installed in your ship, "
      "but this " + planet->Noun() + " does not sell them."));
    return;
  }

  if(selectedOutfit->Get("map"))
  {
    GetUI()->Push(new Dialogue("You have already mapped all the systems shown by this map, "
      "so there is no reason to buy another."));
    return;
  }

  if(HasLicence(selectedOutfit->Name()))
  {
    GetUI()->Push(new Dialogue("You already have one of these licences, "
      "so there is no reason to buy another."));
    return;
  }

  if(!playerShip)
    return;

  double coreNeeded = -selectedOutfit->Get("core space");
  double coreSpace = playerShip->Attributes().Get("core space");
  if(coreNeeded > coreSpace)
  {
    GetUI()->Push(new Dialogue("Only part of your ship's outfit capacity is usable for core systems. "
      "You cannot install this outfit, because it takes up "
      + Tons(coreNeeded) + " of core space, and this ship has "
      + Tons(coreSpace) + " free."));
    return;
  }

  double engineNeeded = -selectedOutfit->Get("engine space");
  double engineSpace = playerShip->Attributes().Get("engine space");
  if(engineNeeded > engineSpace)
  {
    GetUI()->Push(new Dialogue("Only part of your ship's outfit capacity is usable for engines. "
      "You cannot install this outfit, because it takes up "
      + Tons(engineNeeded) + " of engine space, and this ship has "
      + Tons(engineSpace) + " free."));
    return;
  }

  double weaponNeeded = -selectedOutfit->Get("weapon space");
  double weaponSpace = playerShip->Attributes().Get("weapon space");
  if(weaponNeeded > weaponSpace)
  {
    GetUI()->Push(new Dialogue("Only part of your ship's outfit capacity is usable for weapons. "
      "You cannot install this outfit, because it takes up "
      + Tons(weaponNeeded) + " of weapon space, and this ship has "
      + Tons(weaponSpace) + " free."));
    return;
  }

  double outfitNeeded = -selectedOutfit->Get("outfit space");
  double outfitSpace = coreSpace + engineSpace + weaponSpace;
  if(outfitNeeded > outfitSpace)
  {
    string need =  to_string(outfitNeeded) + (outfitNeeded != 1. ? "tons" : "ton");
    GetUI()->Push(new Dialogue("You cannot install this outfit, because it takes up "
      + Tons(outfitNeeded) + " of outfit space, and this ship has "
      + Tons(outfitSpace) + " free."));
    return;
  }

  if(selectedOutfit->Category() == "Ammunition")
  {
    if(!playerShip->OutfitCount(selectedOutfit))
      GetUI()->Push(new Dialogue("This outfit is ammunition for a weapon. "
        "You cannot install it without first installing the appropriate weapon."));
    else
      GetUI()->Push(new Dialogue("You already have the maximum amount of ammunition for this weapon. "
        "If you want to install more ammunition, you must first install another of these weapons."));
    return;
  }

  int mountsNeeded = -selectedOutfit->Get("turret mounts");
  int mountsFree = playerShip->Attributes().Get("turret mounts");
  if(mountsNeeded && !mountsFree)
  {
    GetUI()->Push(new Dialogue("This weapon is designed to be installed on a turret mount, "
      "but your ship does not have any unused turret mounts available."));
    return;
  }

  int gunsNeeded = -selectedOutfit->Get("gun ports");
  int gunsFree = playerShip->Attributes().Get("gun ports");
  if(gunsNeeded && !gunsFree)
  {
    GetUI()->Push(new Dialogue("This weapon is designed to be installed in a gun port, "
      "but your ship does not have any unused gun ports available."));
    return;
  }

  if(selectedOutfit->Get("installable") < 0.)
  {
    GetUI()->Push(new Dialogue("This item is not an outfit that can be installed in a ship."));
    return;
  }

  if(!playerShip->Attributes().CanAdd(*selectedOutfit, 1))
  {
    GetUI()->Push(new Dialogue("You cannot install this outfit in your ship, "
      "because it would reduce one of your ship's attributes to a negative amount. "
      "For example, it may use up more cargo space than you have left."));
    return;
  }
}



bool OutfitterPanel::CanSell(bool toCargo) const
{
  if(!planet || !selectedOutfit)
    return false;

  if(!toCargo && player.Cargo().Get(selectedOutfit))
    return true;

  for(const Ship *ship : playerShips)
    if(ShipCanSell(ship, selectedOutfit))
      return true;

  return false;
}



void OutfitterPanel::Sell(bool toCargo)
{
  if(!toCargo && player.Cargo().Get(selectedOutfit))
  {
    player.Cargo().Remove(selectedOutfit);
    int64_t price = player.FleetDepreciation().Value(selectedOutfit, day);
    player.Accounts().AddCredits(price);
    player.AddStock(selectedOutfit, 1);
  }
  else
  {
    // Get the ships that have the most of this outfit installed.
    const vector<Ship *> shipsToOutfit = GetShipsToOutfit();

    for(Ship *ship : shipsToOutfit)
    {
      ship->AddOutfit(selectedOutfit, -1);
      if(selectedOutfit->Get("required crew"))
        ship->AddCrew(-selectedOutfit->Get("required crew"));
      ship->Recharge();
      if(toCargo && player.Cargo().Add(selectedOutfit))
      {
        // Transfer to cargo completed.
      }
      else
      {
        int64_t price = player.FleetDepreciation().Value(selectedOutfit, day);
        player.Accounts().AddCredits(price);
        player.AddStock(selectedOutfit, 1);
      }

      const Outfit *ammo = selectedOutfit->Ammo();
      if(ammo && ship->OutfitCount(ammo))
      {
        // Determine how many of this ammo I must sell to also sell the launcher.
        int mustSell = 0;
        for(const pair<const char *, double> &it : ship->Attributes().Attributes())
          if(it.second < 0.)
            mustSell = max<int>(mustSell, it.second / ammo->Get(it.first));

        if(mustSell)
        {
          ship->AddOutfit(ammo, -mustSell);
          if(toCargo)
            mustSell -= player.Cargo().Add(ammo, mustSell);
          if(mustSell)
          {
            int64_t price = player.FleetDepreciation().Value(ammo, day, mustSell);
            player.Accounts().AddCredits(price);
            player.AddStock(ammo, mustSell);
          }
        }
      }
    }
  }
}



void OutfitterPanel::FailSell(bool toCargo) const
{
  const string &verb = toCargo ? "uninstall" : "sell";
  if(!planet || !selectedOutfit)
    return;
  else if(selectedOutfit->Get("map"))
    GetUI()->Push(new Dialogue("You cannot " + verb + " maps. Once you buy one, it is yours permanently."));
  else if(HasLicence(selectedOutfit->Name()))
    GetUI()->Push(new Dialogue("You cannot " + verb + " licences. Once you obtain one, it is yours permanently."));
  else
  {
    bool hasOutfit = !toCargo && player.Cargo().Get(selectedOutfit);
    for(const Ship *ship : playerShips)
      if(ship->OutfitCount(selectedOutfit))
      {
        hasOutfit = true;
        break;
      }
    if(!hasOutfit)
      GetUI()->Push(new Dialogue("You do not have any of these outfits to " + verb + "."));
    else
    {
      for(const Ship *ship : playerShips)
        for(const pair<const char *, double> &it : selectedOutfit->Attributes())
          if(ship->Attributes().Get(it.first) < it.second)
          {
            for(const auto &sit : ship->Outfits())
              if(sit.first->Get(it.first) < 0.)
              {
                GetUI()->Push(new Dialogue("You cannot " + verb + " this outfit, "
                  "because that would cause your ship's \"" + it.first +
                  "\" value to be reduced to less than zero. "
                  "To " + verb + " this outfit, you must " + verb + " the " +
                  sit.first->Name() + " outfit first."));
                return;
              }
            GetUI()->Push(new Dialogue("You cannot " + verb + " this outfit, "
              "because that would cause your ship's \"" + it.first +
              "\" value to be reduced to less than zero."));
            return;
          }
      GetUI()->Push(new Dialogue("You cannot " + verb + " this outfit, "
        "because something else in your ship depends on it."));
    }
  }
}



bool OutfitterPanel::ShouldHighlight(const Ship *ship)
{
  if(!selectedOutfit)
    return false;

  if(hoverButton == 'b')
    return CanBuy() && ShipCanBuy(ship, selectedOutfit);
  else if(hoverButton == 's')
    return CanSell() && ShipCanSell(ship, selectedOutfit);

  return false;
}



void OutfitterPanel::DrawKey()
{
  const Font &font = FontSet::Get(18);
  Colour colour[2] = {*GameData::Colours().Get("medium"), *GameData::Colours().Get("bright")};
  const Sprite *box[2] = {SpriteSet::Get("interface/button/check"), SpriteSet::Get("interface/button/uncheck")};

  // 250 is the width of the shop panel, which displays the ship and fleet.
  Point pos = Screen::TopRight() + Point(-180. - 250., 10.);
  Point off = Point(10., -.5 * font.Height());
  SpriteShader::Draw(box[showForSale], pos);
  font.Draw("Show outfits for sale", pos + off, colour[showForSale]);
  AddZone(Rectangle(pos + Point(80., 0.), Point(180., 20.)), [this](){ ToggleForSale(); });

  bool showCargo = !playerShip;
  pos.Y() += 20.;
  SpriteShader::Draw(box[showCargo], pos);
  font.Draw("Show outfits in cargo", pos + off, colour[showCargo]);
  AddZone(Rectangle(pos + Point(80., 0.), Point(180., 20.)), [this](){ ToggleCargo(); });
}



void OutfitterPanel::ToggleForSale()
{
  showForSale = !showForSale;

  ShopPanel::ToggleForSale();
}



void OutfitterPanel::ToggleCargo()
{
  if(playerShip)
  {
    previousShip = playerShip;
    playerShip = nullptr;
    previousShips = playerShips;
    playerShips.clear();
  }
  else if(previousShip)
  {
    playerShip = previousShip;
    playerShips = previousShips;
  }
  else
  {
    playerShip = player.Flagship();
    if(playerShip)
      playerShips.insert(playerShip);
  }

  ShopPanel::ToggleCargo();
}



bool OutfitterPanel::ShipCanBuy(const Ship *ship, const Outfit *outfit)
{
  return (ship->Attributes().CanAdd(*outfit, 1) > 0);
}



bool OutfitterPanel::ShipCanSell(const Ship *ship, const Outfit *outfit)
{
  if(!ship->OutfitCount(outfit))
    return false;

  // If this outfit requires ammo, check if we could sell it if we sold all
  // the ammo for it first.
  const Outfit *ammo = outfit->Ammo();
  if(ammo && ship->OutfitCount(ammo))
  {
    Outfit attributes = ship->Attributes();
    attributes.Add(*ammo, -ship->OutfitCount(ammo));
    return attributes.CanAdd(*outfit, -1);
  }

  // Now, check whether this ship can sell this outfit.
  return ship->Attributes().CanAdd(*outfit, -1);
}



void OutfitterPanel::DrawOutfit(const Outfit &outfit, const Point &centre, bool isSelected, bool isOwned)
{
  const Sprite *thumbnail = outfit.Thumbnail();
  const Sprite *back = SpriteSet::Get(
    isSelected ? "ui/outfitter selected" : "ui/outfitter unselected");
  SpriteShader::Draw(back, centre);
  SpriteShader::Draw(thumbnail, centre);

  // Draw the outfit name.
  const string &name = outfit.Name();
  const Font &font = FontSet::Get(18);
  Point offset(-.5f * font.Width(name), -.5f * OUTFIT_SIZE + 10.f);
  font.Draw(name, centre + offset, Colour((isSelected | isOwned) ? .8 : .5, 0.));
}



bool OutfitterPanel::HasMapped(int mapSize) const
{
  DistanceMap distance(player.GetSystem(), mapSize);
  for(const System *system : distance.Systems())
    if(!player.HasVisited(system))
      return false;

  return true;
}



bool OutfitterPanel::IsLicence(const string &name) const
{
  static const string &LICENCE = " Licence";
  if(name.length() < LICENCE.length())
    return false;
  if(name.compare(name.length() - LICENCE.length(), LICENCE.length(), LICENCE))
    return false;

  return true;
}



bool OutfitterPanel::HasLicence(const string &name) const
{
  return (IsLicence(name) && player.GetCondition(LicenceName(name)) > 0);
}



string OutfitterPanel::LicenceName(const string &name) const
{
  static const string &LICENCE = " Licence";
  return "licence: " + name.substr(0, name.length() - LICENCE.length());
}



void OutfitterPanel::CheckRefill()
{
  if(checkedRefill)
    return;
  checkedRefill = true;

  int count = 0;
  map<const Outfit *, int> needed;
  for(const shared_ptr<Ship> &ship : player.Ships())
  {
    if(ship->GetSystem() != player.GetSystem() || ship->IsDisabled())
      continue;

    ++count;
    set<const Outfit *> toRefill;
    for(const Hardpoint &it : ship->Weapons())
      if(it.GetOutfit() && it.GetOutfit()->Ammo())
        toRefill.insert(it.GetOutfit()->Ammo());

    for(const Outfit *outfit : toRefill)
    {
      int amount = ship->Attributes().CanAdd(*outfit, numeric_limits<int>::max());
      if(amount > 0 && (outfitter.Has(outfit) || player.Stock(outfit) > 0 || player.Cargo().Get(outfit)))
        needed[outfit] += amount;
    }
  }

  int64_t cost = 0;
  for(auto &it : needed)
  {
    // Don't count cost of anything installed from cargo.
    it.second = max(0, it.second - player.Cargo().Get(it.first));
    if(!outfitter.Has(it.first))
      it.second = min(it.second, max(0, player.Stock(it.first)));
    cost += player.StockDepreciation().Value(it.first, day, it.second);
  }
  if(!needed.empty() && cost < player.Accounts().Credits())
  {
    string message = "Do you want to reload all the ammunition for your ship";
    message += (count == 1) ? "?" : "s?";
    if(cost)
      message += " It will cost " + Format::Credits(cost) + " credits.";
    GetUI()->Push(new Dialogue(this, &OutfitterPanel::Refill, message));
  }
}



void OutfitterPanel::Refill()
{
  for(const shared_ptr<Ship> &ship : player.Ships())
  {
    if(ship->GetSystem() != player.GetSystem() || ship->IsDisabled())
      continue;

    set<const Outfit *> toRefill;
    for(const Hardpoint &it : ship->Weapons())
      if(it.GetOutfit() && it.GetOutfit()->Ammo())
        toRefill.insert(it.GetOutfit()->Ammo());

    for(const Outfit *outfit : toRefill)
    {
      int neededAmmo = ship->Attributes().CanAdd(*outfit, numeric_limits<int>::max());
      if(neededAmmo > 0)
      {
        // Fill first from any stockpiles in cargo.
        int fromCargo = player.Cargo().Remove(outfit, neededAmmo);
        neededAmmo -= fromCargo;
        // Then, buy at reduced (or full) price.
        int available = outfitter.Has(outfit) ? neededAmmo : min<int>(neededAmmo, max<int>(0, player.Stock(outfit)));
        if(neededAmmo && available > 0)
        {
          int64_t price = player.StockDepreciation().Value(outfit, day, available);
          player.Accounts().AddCredits(-price);
          player.AddStock(outfit, -available);
        }
        ship->AddOutfit(outfit, available + fromCargo);
      }
    }
  }
}



// Determine which ships of the selected ships should be referenced in this
// iteration of Buy / Sell.
const vector<Ship *> OutfitterPanel::GetShipsToOutfit(bool isBuy) const
{
  vector<Ship *> shipsToOutfit;
  int compareValue = isBuy ? numeric_limits<int>::max() : 0;
  int compareMod = 2 * isBuy - 1;
  for(Ship *ship : playerShips)
  {
    if((isBuy && !ShipCanBuy(ship, selectedOutfit))
        || (!isBuy && !ShipCanSell(ship, selectedOutfit)))
      continue;

    int count = ship->OutfitCount(selectedOutfit);
    if(compareMod * count < compareMod * compareValue)
    {
      shipsToOutfit.clear();
      compareValue = count;
    }
    if(count == compareValue)
      shipsToOutfit.push_back(ship);
  }

  return shipsToOutfit;
}
