// RefuelPanel.cpp

#include "RefuelPanel.h"

#include "Audio.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "GameData.h"
#include "Information.h"
#include "Interface.h"
#include "Planet.h"
#include "PlayerInfo.h"

using namespace std;

namespace {
  const int MIN_X = -310;

  const int FIRST_Y = 80;

  // Determines what ratio is used to refuel ships.
  double ratio = 1;
}



RefuelPanel::RefuelPanel(PlayerInfo &player)
  : player(player), system(*player.GetSystem())
{
  SetTrapAllEvents(false);
  fuel = 0;
  RefreshUI();
}



void RefuelPanel::Step()
{
  DoHelp("refuel");
}



void RefuelPanel::Draw()
{

  string localPrice = "The fuel price on this planet is " + Format::Number(player.GetPlanet()->GetFuelPrice()) + " credits per unit of fuel.";

  const Font &font = FontSet::Get(18);
  const Colour &colour = *GameData::Colours().Get("medium");
  Information info;

  if(fuel && empty)
  {
    string price1 = "To refuel all your ships to " + Format::Round(ratio*100) + "% which will take " + Format::Round(fuel) + " units of fuel";
    string price2 = "you would need to pay " + Format::Round(refuelPrice) + " credits.";
    font.Draw(price1, Point(MIN_X+10, FIRST_Y+120), colour);
    font.Draw(price2, Point(MIN_X+10, FIRST_Y+140), colour);
    info.SetCondition("can refuel");
    info.SetCondition("empty");
  }
  else if(!empty)
  {
    string message1 = "Your fleet is fully refueled.";
    font.Draw(message1, Point(MIN_X+10, FIRST_Y+100), colour);
  }
  else
  {
    string message1 = "Your fleet is at least " + Format::Round(ratio*100) + "% filled.";
    font.Draw(message1, Point(MIN_X+10, FIRST_Y+100), colour);
    info.SetCondition("empty");
  }




  font.Draw(localPrice, Point(MIN_X+10, FIRST_Y), colour);

  info.SetBar("ratio", ratio);
  const Interface *interface = GameData::Interfaces().Get("refuel");
  interface->Draw(info, this);
}



void RefuelPanel::RefreshUI()
{
  empty = player.FuelNeeded(1);
  if(empty)
  {
    fuel = player.FuelNeeded(ratio);
    double price = player.GetPlanet()->GetFuelPrice();
    refuelPrice = fuel*price;
  }
}



// Only override the ones you need; the default action is to return false.
bool RefuelPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress)
{
  // Refuel the defined ratios.
  if(key == 'r')
  {
    player.RefuelRatio(ratio);
    RefreshUI();
    return true;
  }
  return false;
}



bool RefuelPanel::Click(int x, int y, int clicks)
{
  // Checks if the bar is clicked on.
  if(x >= -285 && x < -75 && y >= 135 && y < 165)
  {
    ratio = (x+280)/200.;
    if(ratio > 1)
      ratio = 1;
    if(ratio < 0)
      ratio = 0;
    RefreshUI();
    Audio::Play(Audio::Get("warder"));
    return true;
  }

  return false;
}
