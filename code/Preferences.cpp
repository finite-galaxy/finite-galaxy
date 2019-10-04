// Preferences.cpp

#include "Preferences.h"

#include "Audio.h"
#include "DataFile.h"
#include "DataNode.h"
#include "DataWriter.h"
#include "Files.h"
#include "Screen.h"

#include <algorithm>
#include <map>

using namespace std;

namespace {
  map<string, bool> settings;
  int scrollSpeed = 60;

  // Strings for ammo expenditure:
  const string EXPEND_AMMO = "Escorts expend ammo";
  const string FRUGAL_ESCORTS = "Escorts use ammo frugally";

  const vector<double> ZOOMS = {.06, .09, .13, .18, .25, .35, .50, .71, 1.00, 1.41, 2.00};
  int zoomIndex = 4;
  const double VOLUME_SCALE = .25;
  // refuel preferences, standard value: always refuel(-1)
  double maxRefuelPrice = 10;
}



void Preferences::Load()
{
  // These settings should be on by default. There is no need to specify
  // values for settings that are off by default.
  settings["Automatic aiming"] = true;
  settings["Render motion blur"] = true;
  settings[EXPEND_AMMO] = true;
  settings[FRUGAL_ESCORTS] = true;
  settings["Damaged fighters retreat"] = true;
  settings["Warning siren"] = true;
  settings["Show escort systems on map"] = true;
  settings["Show mini-map"] = true;
  settings["Show planet labels"] = true;
  settings["Show hyperspace flash"] = true;
  settings["Draw background haze"] = true;
  settings["Hide unexplored map regions"] = true;
  settings["Turrets focus fire"] = true;

  DataFile prefs(Files::Config() + "preferences.txt");
  for(const DataNode &node : prefs)
  {
    if(node.Token(0) == "window size" && node.Size() >= 3)
      Screen::SetRaw(node.Value(1), node.Value(2));
    else if(node.Token(0) == "zoom" && node.Size() >= 2)
      Screen::SetZoom(node.Value(1));
    else if(node.Token(0) == "volume" && node.Size() >= 2)
      Audio::SetVolume(node.Value(1) * VOLUME_SCALE);
    else if(node.Token(0) == "scroll speed" && node.Size() >= 2)
      scrollSpeed = node.Value(1);
    else if(node.Token(0) == "view zoom")
      zoomIndex = node.Value(1);
    else if(node.Token(0) == "max refuel price" && node.Size() >= 2)
      maxRefuelPrice = node.Value(1);
    else
      settings[node.Token(0)] = (node.Size() == 1 || node.Value(1));
  }
}



void Preferences::Save()
{
  DataWriter out(Files::Config() + "preferences.txt");

  out.Write("volume", Audio::Volume() / VOLUME_SCALE);
  out.Write("window size", Screen::RawWidth(), Screen::RawHeight());
  out.Write("zoom", Screen::UserZoom());
  out.Write("scroll speed", scrollSpeed);
  out.Write("view zoom", zoomIndex);
  out.Write("max refuel price", maxRefuelPrice);

  for(const auto &it : settings)
    out.Write(it.first, it.second);
}



bool Preferences::Has(const string &name)
{
  auto it = settings.find(name);
  return (it != settings.end() && it->second);
}



void Preferences::Set(const string &name, bool on)
{
  settings[name] = on;
}



void Preferences::ToggleAmmoUsage()
{
  bool expend = Has(EXPEND_AMMO);
  bool frugal = Has(FRUGAL_ESCORTS);
  Preferences::Set(EXPEND_AMMO, !(expend && !frugal));
  Preferences::Set(FRUGAL_ESCORTS, !expend);
}



string Preferences::AmmoUsage()
{
  return Has(EXPEND_AMMO) ? Has(FRUGAL_ESCORTS) ? "frugally" : "always" : "never";
}



// Scroll speed preference.
int Preferences::ScrollSpeed()
{
  return scrollSpeed;
}



void Preferences::SetScrollSpeed(int speed)
{
  scrollSpeed = speed;
}



// View zoom.
double Preferences::ViewZoom()
{
  return ZOOMS[zoomIndex];
}



bool Preferences::ZoomViewIn()
{
  if(zoomIndex == static_cast<int>(ZOOMS.size() - 1))
    return false;

  ++zoomIndex;
  return true;
}



bool Preferences::ZoomViewOut()
{
  if(zoomIndex == 0)
    return false;

  --zoomIndex;
  return true;
}



double Preferences::GetMaxPrice()
{
  return maxRefuelPrice;
}



void Preferences::SetMaxPrice(double price)
{
  maxRefuelPrice = price;
}
