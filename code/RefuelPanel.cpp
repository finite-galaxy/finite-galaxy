// RefuelPanel.cpp

#include "RefuelPanel.h"

#include "Audio.h"
#include "Colour.h"
#include "Command.h"
#include "FillShader.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "GameData.h"
#include "Government.h"
#include "Information.h"
#include "Interface.h"
#include "MapDetailPanel.h"
#include "Messages.h"
#include "Outfit.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "Preferences.h"
#include "System.h"
#include "UI.h"

#include <algorithm>
#include <string>

using namespace std;

namespace {
  const int MIN_X = -310;
  
  const int FIRST_Y = 80;
}



RefuelPanel::RefuelPanel(PlayerInfo &player)
  : player(player), system(*player.GetSystem())
{
  SetTrapAllEvents(false);
}


  
void RefuelPanel::Step()
{
  DoHelp("refuel");
}



void RefuelPanel::Draw()
{
  double ramscoop = Preferences::GetMaxRamscoopFactor();
  if(ramscoop < 0)
  	ramscoop = 1;
  double price = Preferences::GetMaxPrice();
  string priceText;
  if(price == 10)
    priceText = "Refuel independently of the price.";
  else if(price == 0)
    priceText = "Only refuel if it is for free.";
  else
    priceText = "Only refuel if price is less than " + Format::Number(price) + " credits per unit.";
  price /= 10;
  string localPrice = "The fuel price on this planet is " + Format::Number(player.GetPlanet()->GetFuelPrice()) + " credits per unit of fuel.";
  string ramscoopText;
  if(ramscoop == 1)
    ramscoopText = "Refuel independently of ramscoop value of the ship";
  else if(ramscoop == 0)
    ramscoopText = "Only refuel if it is for free.";
  else
    ramscoopText = "Only refuel ships where ramscoop per jump fuel is less than " + Format::Number(ramscoop) + ".";
  
  
  const Font &font = FontSet::Get(14);
  const Colour &colour = *GameData::Colours().Get("bright");
  
  font.Draw(localPrice, Point(MIN_X+10, FIRST_Y), colour);
  font.Draw(priceText, Point(MIN_X+10, FIRST_Y+80), colour);
  font.Draw(ramscoopText, Point(MIN_X+10, FIRST_Y+160), colour);
  
  const Interface *interface = GameData::Interfaces().Get("refuel");
  Information info;
  info.SetBar("refuel behaviour", price);
  info.SetBar("ramscoop behaviour", ramscoop);
  interface->Draw(info, this);
}



// Only override the ones you need; the default action is to return false.
bool RefuelPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command)
{
  // Refuels all ships.
  if(key == 'r')
  {
    player.Refuel(true);
    return true;
  }
  return false;
}



bool RefuelPanel::Click(int x, int y, int clicks)
{
  // Checks if one of the bars is clicked on.
  if(x >= -280 && x < -75 && y >= 135 && y < 165)
  {
    double max = (x+280)/20.;
    if(max > 10)
      max = 10;
    Preferences::SetMaxPrice(max);
    Audio::Play(Audio::Get("warder"));
    return true;
  }
  if(x >= -280 && x < -75 && y >= 215 && y < 245)
  {
    double max = (x+280)/200.;
    if(max > 1)
      max = 1;
    if(max == 1)
      Preferences::SetMaxRamscoopFactor(-1);
    else
      Preferences::SetMaxRamscoopFactor(max);
    Audio::Play(Audio::Get("warder"));
    return true;
  }
  
  return false;
}
