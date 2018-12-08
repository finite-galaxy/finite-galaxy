// MapDetailPanel.cpp

#include "MapDetailPanel.h"

#include "Colour.h"
#include "Command.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "GameData.h"
#include "Government.h"
#include "MapOutfitterPanel.h"
#include "MapShipyardPanel.h"
#include "pi.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "PointerShader.h"
#include "Politics.h"
#include "Radar.h"
#include "RingShader.h"
#include "Screen.h"
#include "Ship.h"
#include "Sprite.h"
#include "SpriteSet.h"
#include "SpriteShader.h"
#include "StellarObject.h"
#include "System.h"
#include "Trade.h"
#include "UI.h"
#include "WrappedText.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <utility>
#include <vector>

using namespace std;

namespace {
  // Convert the angle between two vectors into a sortable angle, i.e an angle
  // plus a length that is used as a tie-breaker.
  pair<double, double> SortAngle(const Point &reference, const Point &point)
  {
    // Rotate the given point by the reference amount.
    Point rotated(reference.Dot(point), reference.Cross(point));
    
    // This will be the tiebreaker value: the length, squared.
    double length = rotated.Dot(rotated);
    // Calculate the angle, but rotated 180 degrees so that the discontinuity
    // comes at the reference angle rather than directly opposite it.
    double angle = atan2(-rotated.Y(), -rotated.X());
    
    // Special case: collinear with the reference vector. If the point is
    // a longer vector than the reference, it's the very best angle.
    // Otherwise, it is the very worst angle. (Note: this also is applied if
    // the angle is opposite (angle == 0) but then it's a no-op.)
    if(!rotated.Y())
      angle = copysign(angle, rotated.X() - reference.Dot(reference));
    
    // Return the angle, plus the length as a tie-breaker.
    return make_pair(angle, length);
  }
}



MapDetailPanel::MapDetailPanel(PlayerInfo &player, const System *system)
  : MapPanel(player, system ? MapPanel::SHOW_REPUTATION : player.MapColouring(), system)
{
}



MapDetailPanel::MapDetailPanel(const MapPanel &panel)
  : MapPanel(panel)
{
  // Use whatever map colouring is specified in the PlayerInfo.
  commodity = player.MapColouring();
}



void MapDetailPanel::Step()
{
  MapPanel::Step();
  if(!player.GetPlanet())
    DoHelp("map");
}



void MapDetailPanel::Draw()
{
  MapPanel::Draw();
  
  DrawInfo();
  DrawOrbits();
  DrawKey();
}



// Only override the ones you need; the default action is to return false.
bool MapDetailPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command)
{
  if((key == SDLK_TAB || command.Has(Command::JUMP)) && player.Flagship())
  {
    // Clear the selected planet, if any.
    selectedPlanet = nullptr;
    // Toggle to the next link connected to the "source" system. If the
    // shift key is down, the source is the end of the travel plan; otherwise
    // it is one step before the end.
    vector<const System *> &plan = player.TravelPlan();
    const System *source = plan.empty() ? player.GetSystem() : plan.front();
    const System *next = nullptr;
    Point previousUnit = Point(0., -1.);
    if(!plan.empty() && !(mod & KMOD_SHIFT))
    {
      previousUnit = plan.front()->Position();
      plan.erase(plan.begin());
      next = source;
      source = plan.empty() ? player.GetSystem() : plan.front();
      previousUnit = (previousUnit - source->Position()).Unit();
    }
    Point here = source->Position();
    const System *original = next;
    
    // Depending on whether the flagship has a jump drive, the possible links
    // we can travel along are different:
    bool hasJumpDrive = player.Flagship()->Attributes().Get("jump drive");
    const set<const System *> &links = hasJumpDrive ? source->Neighbours() : source->Links();
    
    // For each link we can travel from this system, check whether the link
    // is closer to the current angle (while still being larger) than any
    // link we have seen so far.
    auto bestAngle = make_pair(4., 0.);
    for(const System *it : links)
    {
      // Skip the currently selected link, if any. Also skip links to
      // systems the player has not seen, and skip hyperspace links if the
      // player has not visited either end of them.
      if(it == original)
        continue;
      if(!player.HasSeen(it))
        continue;
      if(!(hasJumpDrive || player.HasVisited(it) || player.HasVisited(source)))
        continue;
      
      // Generate a sortable angle with vector length as a tiebreaker.
      // Otherwise if two systems are in exactly the same direction it is
      // not well defined which one comes first.
      auto angle = SortAngle(previousUnit, it->Position() - here);
      if(angle < bestAngle)
      {
        next = it;
        bestAngle = angle;
      }
    }
    if(next)
    {
      plan.insert(plan.begin(), next);
      Select(next);
    }
  }
  else if((key == SDLK_DELETE || key == SDLK_BACKSPACE) && player.HasTravelPlan())
  {
    vector<const System *> &plan = player.TravelPlan();
    plan.erase(plan.begin());
    Select(plan.empty() ? player.GetSystem() : plan.front());
  }
  else if(key == SDLK_DOWN)
  {
    if(commodity < 0 || commodity == 9)
      SetCommodity(0);
    else
      SetCommodity(commodity + 1);
  }
  else if(key == SDLK_UP)
  {
    if(commodity <= 0)
      SetCommodity(9);
    else
      SetCommodity(commodity - 1);
  }
  else
    return MapPanel::KeyDown(key, mod, command);
  
  return true;
}



bool MapDetailPanel::Click(int x, int y, int clicks)
{
  if(x < Screen::Left() + 160)
  {
    if(y >= tradeY && y < tradeY + 200)
    {
      SetCommodity((y - tradeY) / 20);
      return true;
    }
    else if(y < governmentY)
      SetCommodity(SHOW_REPUTATION);
    else if(y >= governmentY && y < governmentY + 20)
      SetCommodity(SHOW_GOVERNMENT);
    else
    {
      for(const auto &it : planetY)
        if(y >= it.second && y < it.second + 110)
        {
          selectedPlanet = it.first;
          if(y >= it.second + 30 && y < it.second + 110)
          {
            // Figure out what row of the planet info was clicked.
            int row = (y - (it.second + 30)) / 20;
            static const int SHOW[4] = {
              SHOW_REPUTATION, SHOW_SHIPYARD, SHOW_OUTFITTER, SHOW_VISITED};
            SetCommodity(SHOW[row]);
            
            if(clicks > 1 && SHOW[row] == SHOW_SHIPYARD)
            {
              GetUI()->Pop(this);
              GetUI()->Push(new MapShipyardPanel(*this, true));
            }
            if(clicks > 1 && SHOW[row] == SHOW_OUTFITTER)
            {
              GetUI()->Pop(this);
              GetUI()->Push(new MapOutfitterPanel(*this, true));
            }
          }
          return true;
        }
    }
  }
  else if(x >= Screen::Right() - 240 && y <= Screen::Top() + 270)
  {
    Point click = Point(x, y);
    selectedPlanet = nullptr;
    double distance = numeric_limits<double>::infinity();
    for(const auto &it : planets)
    {
      double d = click.Distance(it.second);
      if(d < distance)
      {
        distance = d;
        selectedPlanet = it.first;
      }
    }
    if(selectedPlanet && player.Flagship())
      player.SetTravelDestination(selectedPlanet);
    
    return true;
  }
  else if(y >= Screen::Bottom() - 40 && x >= Screen::Right() - 335 && x < Screen::Right() - 265)
  {
    // The user clicked the "done" button.
    return DoKey(SDLK_d);
  }
  else if(y >= Screen::Bottom() - 40 && x >= Screen::Right() - 415 && x < Screen::Right() - 345)
  {
    // The user clicked the "missions" button.
    return DoKey(SDLK_PAGEDOWN);
  }
  
  MapPanel::Click(x, y, clicks);
  if(selectedPlanet && !selectedPlanet->IsInSystem(selectedSystem))
    selectedPlanet = nullptr;
  return true;
}



// Draw the legend, correlating between a system's colour and the value of the
// selected "commodity," which may be reputation level, outfitter size, etc.
void MapDetailPanel::DrawKey()
{
  const Colour &dim = *GameData::Colours().Get("dim");
  const Colour &medium = *GameData::Colours().Get("medium");
  const Font &font = FontSet::Get(14);
  
  Point pos = Screen::TopRight() + Point(-110., 310.);
  Point headerOff(-5., -.5 * font.Height());
  Point textOff(10., -.5 * font.Height());
  
  static const string HEADER[] = {
    "Trade prices:",
    "Ships for sale:",
    "Outfits for sale:",
    "You have visited:",
    "", // Special should never be active in this mode.
    "Government:",
    "System:"
  };
  const string &header = HEADER[-min(0, max(-6, commodity))];
  font.Draw(header, pos + headerOff, medium);
  pos.Y() += 20.;
  
  if(commodity >= 0)
  {
    // Each system is coloured by the selected commodity's price. Draw
    // four distinct colours and the price each colour represents.
    const vector<Trade::Commodity> &commodities = GameData::Commodities();
    const auto &range = commodities[commodity];
    if(static_cast<unsigned>(commodity) >= commodities.size())
      return;
    
    for(int i = 0; i <= 3; ++i)
    {
      RingShader::Draw(pos, OUTER, INNER, MapColour(i * (2. / 3.) - 1.));
      int price = range.low + ((range.high - range.low) * i) / 3;
      font.Draw(to_string(price), pos + textOff, dim);
      pos.Y() += 20.;
    }
  }
  else if(commodity >= SHOW_OUTFITTER)
  {
    // Each system is coloured by the number of outfits for sale.
    static const string LABEL[2][4] = {
      {"None", "1", "5", "10+"},
      {"None", "1", "30", "60+"}};
    static const double VALUE[4] = {-1., 0., .5, 1.};
    
    for(int i = 0; i < 4; ++i)
    {
      RingShader::Draw(pos, OUTER, INNER, MapColour(VALUE[i]));
      font.Draw(LABEL[commodity == SHOW_OUTFITTER][i], pos + textOff, dim);
      pos.Y() += 20.;
    }
  }
  else if(commodity == SHOW_VISITED)
  {
    static const string LABEL[3] = {
      "All planets",
      "Some",
      "None"
    };
    for(int i = 0; i < 3; ++i)
    {
      RingShader::Draw(pos, OUTER, INNER, MapColour(1 - i));
      font.Draw(LABEL[i], pos + textOff, dim);
      pos.Y() += 20.;
    }
  }
  else if(commodity == SHOW_GOVERNMENT)
  {
    // Each system is coloured by the government of the system. Only the
    // four largest visible governments are labeled in the legend.
    vector<pair<double, const Government *>> distances;
    for(const auto &it : closeGovernments)
      distances.emplace_back(it.second, it.first);
    sort(distances.begin(), distances.end());
    for(unsigned i = 0; i < 4 && i < distances.size(); ++i)
    {
      RingShader::Draw(pos, OUTER, INNER, GovernmentColour(distances[i].second));
      font.Draw(distances[i].second->GetName(), pos + textOff, dim);
      pos.Y() += 20.;
    }
  }
  else if(commodity == SHOW_REPUTATION)
  {
    // Each system is coloured in accordance with the player's reputation
    // with its owning government. The specific colours associated with a
    // given reputation (0.1, 100, and 10000) are shown for each sign.
    RingShader::Draw(pos, OUTER, INNER, ReputationColour(1e-1, true, false));
    RingShader::Draw(pos + Point(12., 0.), OUTER, INNER, ReputationColour(1e2, true, false));
    RingShader::Draw(pos + Point(24., 0.), OUTER, INNER, ReputationColour(1e4, true, false));
    font.Draw("Friendly", pos + textOff + Point(24., 0.), dim);
    pos.Y() += 20.;
    
    RingShader::Draw(pos, OUTER, INNER, ReputationColour(-1e-1, false, false));
    RingShader::Draw(pos + Point(12., 0.), OUTER, INNER, ReputationColour(-1e2, false, false));
    RingShader::Draw(pos + Point(24., 0.), OUTER, INNER, ReputationColour(-1e4, false, false));
    font.Draw("Hostile", pos + textOff + Point(24., 0.), dim);
    pos.Y() += 20.;
    
    RingShader::Draw(pos, OUTER, INNER, ReputationColour(0., false, false));
    font.Draw("Restricted", pos + textOff, dim);
    pos.Y() += 20.;
    
    RingShader::Draw(pos, OUTER, INNER, ReputationColour(0., false, true));
    font.Draw("Dominated", pos + textOff, dim);
    pos.Y() += 20.;
  }
  
  RingShader::Draw(pos, OUTER, INNER, UninhabitedColour());
  font.Draw("Uninhabited", pos + textOff, dim);
  pos.Y() += 20.;
  
  RingShader::Draw(pos, OUTER, INNER, UnexploredColour());
  font.Draw("Unexplored", pos + textOff, dim);
}



// Draw the various information displays: system name & government, planetary
// details, trade prices, and details about the selected object.
void MapDetailPanel::DrawInfo()
{
  const Colour &faint = *GameData::Colours().Get("faint");
  const Colour &dim = *GameData::Colours().Get("dim");
  const Colour &medium = *GameData::Colours().Get("medium");
  
  Point uiPoint(Screen::Left() + 100., Screen::Top() + 45.);
  
  // System sprite goes from 0 to 90.
  const Sprite *systemSprite = SpriteSet::Get("ui/map system");
  SpriteShader::Draw(systemSprite, uiPoint);
  
  const Font &font = FontSet::Get(14);
  string systemName = player.KnowsName(selectedSystem) ?
    selectedSystem->Name() : "Unexplored System";
  font.Draw(systemName, uiPoint + Point(-90., -7.), medium);
  
  governmentY = uiPoint.Y() + 10.;
  string gov = player.HasVisited(selectedSystem) ?
    selectedSystem->GetGovernment()->GetName() : "Unknown Government";
  font.Draw(gov, uiPoint + Point(-90., 13.), (commodity == SHOW_GOVERNMENT) ? medium : dim);
  if(commodity == SHOW_GOVERNMENT)
    PointerShader::Draw(uiPoint + Point(-90., 23.), Point(1., 0.),
      10., 10., 0., medium);
  
  uiPoint.Y() += 115.;
  
  planetY.clear();
  // Draw the basic information for visitable planets in this system.
  if(player.HasVisited(selectedSystem))
  {
    set<const Planet *> shown;
    const Sprite *planetSprite = SpriteSet::Get("ui/map planet");
    for(const StellarObject &object : selectedSystem->Objects())
      if(object.GetPlanet())
      {
        // The same "planet" may appear multiple times in one system,
        // providing multiple landing and departure points (e.g. ringworlds).
        const Planet *planet = object.GetPlanet();
        if(planet->IsWormhole() || !planet->IsAccessible(player.Flagship()) || shown.count(planet))
          continue;
        shown.insert(planet);
        
        SpriteShader::Draw(planetSprite, uiPoint);
        planetY[planet] = uiPoint.Y() - 60;

//        font.Draw(planet->IsInhabited() ? object.Name() + " (" + planet->GetGovernment()->GetName() + ")" : object.Name() + " (Uninhabited)",
        font.Draw(object.Name(),
          uiPoint + Point(-70., -55.),
          planet == selectedPlanet ? medium : dim);
        
        string reputationLabel = !planet->IsInhabited() ? "Uninhabited" :
          GameData::GetPolitics().HasDominated(planet) ? "Dominated" :
          planet->GetGovernment()->IsEnemy() ? "Hostile" :
          planet->CanLand() ? "Friendly" : "Restricted";
        font.Draw(reputationLabel,
          uiPoint + Point(-60., -35.),
          planet->IsInhabited() ? medium : faint);
        if(commodity == SHOW_REPUTATION)
          PointerShader::Draw(uiPoint + Point(-60., -25.), Point(1., 0.),
            10., 10., 0., medium);
        
        font.Draw("Shipyard",
          uiPoint + Point(-60., -15.),
          planet->HasShipyard() ? medium : faint);
        if(commodity == SHOW_SHIPYARD)
          PointerShader::Draw(uiPoint + Point(-60., -5.), Point(1., 0.),
            10., 10., 0., medium);
        
        font.Draw("Outfitter",
          uiPoint + Point(-60., 5.),
          planet->HasOutfitter() ? medium : faint);
        if(commodity == SHOW_OUTFITTER)
          PointerShader::Draw(uiPoint + Point(-60., 15.), Point(1., 0.),
            10., 10., 0., medium);
        
        bool hasVisited = player.HasVisited(planet);
        font.Draw(hasVisited ? "(has been visited)" : "(not yet visited)",
          uiPoint + Point(-70., 25.),
          dim);
        if(commodity == SHOW_VISITED)
          PointerShader::Draw(uiPoint + Point(-70., 35.), Point(1., 0.),
            10., 10., 0., medium);
        
        uiPoint.Y() += 130.;
      }
  }
  
  uiPoint.Y() += 45.;
  tradeY = uiPoint.Y() - 95.;
  
  // Trade sprite goes from 310 to 540.
  const Sprite *tradeSprite = SpriteSet::Get("ui/map trade");
  SpriteShader::Draw(tradeSprite, uiPoint);
  
  uiPoint.X() -= 90.;
  uiPoint.Y() -= 97.;
  for(const Trade::Commodity &commodity : GameData::Commodities())
  {
    bool isSelected = false;
    if(static_cast<unsigned>(this->commodity) < GameData::Commodities().size())
      isSelected = (&commodity == &GameData::Commodities()[this->commodity]);
    const Colour &colour = isSelected ? medium : dim;
    
    font.Draw(commodity.name, uiPoint, colour);
    
    string price;
    
    bool hasVisited = player.HasVisited(selectedSystem);
    if(hasVisited && selectedSystem->IsInhabited(player.Flagship()))
    {
      int value = selectedSystem->Trade(commodity.name);
      int localValue = (player.GetSystem() ? player.GetSystem()->Trade(commodity.name) : 0);
      // Don't "compare" prices if the current system is uninhabited and
      // thus has no prices to compare to.
      bool noCompare = (!player.GetSystem() || !player.GetSystem()->IsInhabited(player.Flagship()));
      if(!value)
        price = "----";
      else if(noCompare || player.GetSystem() == selectedSystem || !localValue)
        price = to_string(value);
      else
      {
        value -= localValue;
        price += "(";
        if(value > 0)
          price += '+';
        price += to_string(value);
        price += ")";
      }
    }
    else
      price = (hasVisited ? "n/a" : "?");
    
    Point pos = uiPoint + Point(140. - font.Width(price), 0.);
    font.Draw(price, pos, colour);
    
    if(isSelected)
      PointerShader::Draw(uiPoint + Point(0., 10.), Point(1., 0.), 10., 10., 0., colour);
    
    uiPoint.Y() += 20.;
  }
  
  if(selectedPlanet && !selectedPlanet->Description().empty()
      && player.HasVisited(selectedPlanet) && !selectedPlanet->IsWormhole())
  {
    static const int X_OFFSET = 240;
    static const int WIDTH = 500;
    const Sprite *panelSprite = SpriteSet::Get("ui/description panel");
    Point pos(Screen::Right() - X_OFFSET - .5 * panelSprite->Width(),
      Screen::Top() + .5 * panelSprite->Height());
    SpriteShader::Draw(panelSprite, pos);
    
    WrappedText text(font);
    text.SetAlignment(WrappedText::JUSTIFIED);
    text.SetWrapWidth(WIDTH - 20);
    text.Wrap(selectedPlanet->Description());
    text.Draw(Point(Screen::Right() - X_OFFSET - WIDTH, Screen::Top() + 20), medium);
  }
  
  DrawButtons("is ports");
}



// Draw the planet orbits in the currently selected system, on the current day.
void MapDetailPanel::DrawOrbits()
{
  const Sprite *orbitSprite = SpriteSet::Get("ui/orbits and key");
  SpriteShader::Draw(orbitSprite, Screen::TopRight() + .5 * Point(-orbitSprite->Width(), orbitSprite->Height()));
  Point orbitCentre = Screen::TopRight() + Point(-120., 160.);
  
  if(!selectedSystem || !player.HasVisited(selectedSystem))
    return;
  
  const Font &font = FontSet::Get(14);
  
  // Figure out what the largest orbit in this system is.
  double maxDistance = 0.;
  for(const StellarObject &object : selectedSystem->Objects())
    maxDistance = max(maxDistance, object.Position().Length() + object.Radius());
  
  // 2400 -> 120.
  double scale = .03;
  maxDistance *= scale;
  
  if(maxDistance > 115.)
    scale *= 115. / maxDistance;
  
  // Draw the orbits.
  static const Colour habitColour[7] = {
    Colour(.4, .2, .2, 1.),
    Colour(.3, .3, 0., 1.),
    Colour(0., .4, 0., 1.),
    Colour(0., .3, .4, 1.),
    Colour(.1, .2, .5, 1.),
    Colour(.2, .2, .2, 1.),
    Colour(1., 1., 1., 1.)
  };
  for(const StellarObject &object : selectedSystem->Objects())
  {
    if(object.Radius() <= 0.)
      continue;
    
    Point parentPos;
    int habit = 5;
    if(object.Parent() >= 0)
      parentPos = selectedSystem->Objects()[object.Parent()].Position();
    else
    {
      double warmth = object.Distance() / selectedSystem->HabitableZone();
      habit = (warmth > .5) + (warmth > .8) + (warmth > 1.2) + (warmth > 2.0);
    }
    
    double radius = object.Distance() * scale;
    RingShader::Draw(orbitCentre + parentPos * scale,
      radius + .7, radius - .7,
      habitColour[habit]);
  }
  
  // Draw the planets themselves.
  planets.clear();
  for(const StellarObject &object : selectedSystem->Objects())
  {
    if(object.Radius() <= 0.)
      continue;
    
    Point pos = orbitCentre + object.Position() * scale;
    if(object.GetPlanet() && object.GetPlanet()->IsAccessible(player.Flagship()))
      planets[object.GetPlanet()] = pos;
    
    const float *rgb = Radar::GetColour(object.RadarType(player.Flagship())).Get();
    // Darken and saturate the colour, and make it opaque.
    Colour colour(max(0., rgb[0] * 1.2 - .2), max(0., rgb[1] * 1.2 - .2), max(0., rgb[2] * 1.2 - .2), 1.);
    RingShader::Draw(pos, object.Radius() * scale + 1., 0., colour);
  }
  
  // Draw the selection ring on top of everything else.
  for(const StellarObject &object : selectedSystem->Objects())
    if(selectedPlanet && object.GetPlanet() == selectedPlanet)
      RingShader::Draw(orbitCentre + object.Position() * scale,
        object.Radius() * scale + 5., object.Radius() * scale + 4.,
        habitColour[6]);
  
  // Draw the name of the selected planet.
  const string &name = selectedPlanet ? selectedPlanet->Name() : selectedSystem->Name();
  Point namePos(Screen::Right() - .5 * font.Width(name) - 100., Screen::Top() + 7.);
  font.Draw(name, namePos, *GameData::Colours().Get("medium"));
}



// Set the commodity colouring, and update the player info as well.
void MapDetailPanel::SetCommodity(int index)
{
  commodity = index;
  player.SetMapColouring(commodity);
}
