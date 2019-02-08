// ShipyardPanel.cpp

#include "ShipyardPanel.h"

#include "Colour.h"
#include "Dialogue.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "GameData.h"
#include "Phrase.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "Point.h"
#include "Screen.h"
#include "Ship.h"
#include "SpriteSet.h"
#include "SpriteShader.h"
#include "UI.h"

class System;

using namespace std;

namespace {
  // The name entry dialogue should include a "Random" button to choose a random
  // name using the civilian ship name generator.
  class NameDialogue : public Dialogue {
  public:
    NameDialogue(ShipyardPanel *panel, void (ShipyardPanel::*fun)(const string &), const string &message)
      : Dialogue(panel, fun, message) {}
    
    virtual void Draw() override
    {
      Dialogue::Draw();
      
      randomPos = cancelPos - Point(80., 0.);
      SpriteShader::Draw(SpriteSet::Get("ui/dialogue cancel"), randomPos);

      const Font &font = FontSet::Get(18);
      static const string label = "Random";
      Point labelPos = randomPos - .5 * Point(font.Width(label), font.Height());
      font.Draw(label, labelPos, *GameData::Colours().Get("medium"));
    }
    
  protected:
    virtual bool Click(int x, int y, int clicks) override
    {
      Point off = Point(x, y) - randomPos;
      if(fabs(off.X()) < 40. && fabs(off.Y()) < 20.)
      {
        input = GameData::Phrases().Get("civilian")->Get();
        return true;
      }
      return Dialogue::Click(x, y, clicks);
    }
    
  private:
    Point randomPos;
  };
}



ShipyardPanel::ShipyardPanel(PlayerInfo &player)
  : ShopPanel(player, false), modifier(0)
{
  for(const auto &it : GameData::Ships())
    catalogue[it.second.Attributes().Category()].insert(it.first);
  
  if(player.GetPlanet())
    shipyard = player.GetPlanet()->Shipyard();
}



int ShipyardPanel::TileSize() const
{
  return SHIP_SIZE;
}



int ShipyardPanel::DrawPlayerShipInfo(const Point &point)
{
  shipInfo.Update(*playerShip, player.FleetDepreciation(), player.GetDate().DaysSinceEpoch());
  shipInfo.DrawSale(point);
  shipInfo.DrawAttributes(point + Point(0, shipInfo.SaleHeight()));
  
  return shipInfo.SaleHeight() + shipInfo.AttributesHeight();
}



bool ShipyardPanel::HasItem(const string &name) const
{
  const Ship *ship = GameData::Ships().Get(name);
  return shipyard.Has(ship);
}



void ShipyardPanel::DrawItem(const string &name, const Point &point, int scrollY)
{
  const Ship *ship = GameData::Ships().Get(name);
  zones.emplace_back(point, Point(SHIP_SIZE, SHIP_SIZE), ship, scrollY);
  if(point.Y() + SHIP_SIZE / 2 < Screen::Top() || point.Y() - SHIP_SIZE / 2 > Screen::Bottom())
    return;
  
  DrawShip(*ship, point, ship == selectedShip);
}



int ShipyardPanel::DividerOffset() const
{
  return 121;
}



int ShipyardPanel::DetailWidth() const
{
  return 3 * shipInfo.PanelWidth();
}



int ShipyardPanel::DrawDetails(const Point &centre)
{
  shipInfo.Update(*selectedShip, player.StockDepreciation(), player.GetDate().DaysSinceEpoch());
  Point offset(shipInfo.PanelWidth(), 0.);
  
  shipInfo.DrawDescription(centre - offset * 1.5);
  shipInfo.DrawAttributes(centre - offset * .5);
  shipInfo.DrawOutfits(centre + offset * .5);
  
  return shipInfo.MaximumHeight();
}



bool ShipyardPanel::CanBuy() const
{
  if(!selectedShip)
    return false;
  
  int64_t cost = player.StockDepreciation().Value(*selectedShip, day);
  
  // Check that the player has any necessary licences.
  int64_t licenceCost = LicenceCost(&selectedShip->Attributes());
  if(licenceCost < 0)
    return false;
  cost += licenceCost;
  
  return (player.Accounts().Credits() >= cost);
}



void ShipyardPanel::Buy(bool fromCargo)
{
  int64_t licenceCost = LicenceCost(&selectedShip->Attributes());
  if(licenceCost < 0)
    return;
  
  modifier = Modifier();
  string message;
  if(licenceCost)
    message = "Note: you will need to pay " + Format::Credits(licenceCost)
      + " credits for the licences required to operate this ship, in addition to its cost."
      " If that is okay with you, go ahead and enter a name for your brand new ";
  else
    message = "Enter a name for your brand new ";
  
  if(modifier == 1)
    message += selectedShip->ModelName() + "! (Or leave it blank to use a randomly chosen name.)";
  else
    message += selectedShip->PluralModelName() + "! (Or leave it blank to use randomly chosen names.)";
  
  GetUI()->Push(new NameDialogue(this, &ShipyardPanel::BuyShip, message));
}



void ShipyardPanel::FailBuy() const
{
  if(!selectedShip)
    return;
  
  int64_t cost = player.StockDepreciation().Value(*selectedShip, day);
  
  // Check that the player has any necessary licences.
  int64_t licenceCost = LicenceCost(&selectedShip->Attributes());
  if(licenceCost < 0)
  {
    GetUI()->Push(new Dialogue("Buying this ship requires a special licence. "
      "You will probably need to complete some sort of mission to get one."));
    return;
  }
  
  cost += licenceCost;
  if(player.Accounts().Credits() < cost)
  {
    for(const auto &it : player.Ships())
      cost -= player.FleetDepreciation().Value(*it, day);
    if(player.Accounts().Credits() < cost)
      GetUI()->Push(new Dialogue("You do not have enough credits to buy this ship. "
        "Consider checking if the bank will offer you a loan."));
    else
    {
      string ship = (player.Ships().size() == 1) ? "your current ship" : "one of your ships";
      GetUI()->Push(new Dialogue("You do not have enough credits to buy this ship. "
        "If you want to buy it, you must sell " + ship + " first."));
    }
    return;
  }
}



bool ShipyardPanel::CanSell(bool toCargo) const
{
  return playerShip;
}



void ShipyardPanel::Sell(bool toCargo)
{
  static const int MAX_LIST = 20;
  
  int count = playerShips.size();
  int initialCount = count;
  string message = "Sell ";
  if(count == 1)
    message += playerShip->Name();
  else if(count <= MAX_LIST)
  {
    auto it = playerShips.begin();
    message += (*it++)->Name();
    --count;
    
    if(count == 1)
      message += " and ";
    else
    {
      while(count-- > 1)
        message += ",\n" + (*it++)->Name();
      message += ",\nand ";
    }
    message += (*it)->Name();
  }
  else
  {
    auto it = playerShips.begin();
    message += (*it++)->Name() + ",\n";
    for(int i = 1; i < MAX_LIST - 1; ++i)
      message += (*it++)->Name() + ",\n";
    
    message += "and " + to_string(count - (MAX_LIST - 1)) + " other ships";
  }
  // To allow calculating the sale price of all the ships in the list,
  // temporarily copy into a shared_ptr vector:
  vector<shared_ptr<Ship>> toSell;
  for(const auto &it : playerShips)
    toSell.push_back(it->shared_from_this());
  int64_t total = player.FleetDepreciation().Value(toSell, day);
  
  message += ((initialCount > 2) ? "\nfor " : " for ") + Format::Credits(total) + " credits?";
  GetUI()->Push(new Dialogue(this, &ShipyardPanel::SellShip, message, Font::TRUNC_MIDDLE));
}



bool ShipyardPanel::CanSellMultiple() const
{
  return false;
}



void ShipyardPanel::BuyShip(const string &name)
{
  int64_t licenceCost = LicenceCost(&selectedShip->Attributes());
  if(licenceCost)
  {
    player.Accounts().AddCredits(-licenceCost);
    for(const string &licenceName : selectedShip->Attributes().Licences())
      if(player.GetCondition("licence: " + licenceName) <= 0)
        player.Conditions()["licence: " + licenceName] = true;
  }
  
  for(int i = 1; i <= modifier; ++i)
  {
    // If no name is given, choose a random name. Otherwise, if buying
    // multiple ships, append a number to the given ship name.
    string shipName = name;
    if(name.empty())
      shipName = GameData::Phrases().Get("civilian")->Get();
    else if(modifier > 1)
      shipName += " " + to_string(i);
    
    player.BuyShip(selectedShip, shipName);
  }
  
  playerShip = &*player.Ships().back();
  playerShips.clear();
  playerShips.insert(playerShip);
}



void ShipyardPanel::SellShip()
{
  for(Ship *ship : playerShips)
    player.SellShip(ship);
  playerShips.clear();
  playerShip = nullptr;
  for(const shared_ptr<Ship> &ship : player.Ships())
    if(ship->GetSystem() == player.GetSystem() && !ship->IsDisabled())
    {
      playerShip = ship.get();
      break;
    }
  if(playerShip)
    playerShips.insert(playerShip);
  player.UpdateCargoCapacities();
}
