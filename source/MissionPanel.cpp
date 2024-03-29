// MissionPanel.cpp

#include "MissionPanel.h"

#include "Command.h"
#include "Dialogue.h"
#include "FillShader.h"
#include "Font.h"
#include "FontSet.h"
#include "GameData.h"
#include "Information.h"
#include "Interface.h"
#include "LineShader.h"
#include "Mission.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "PointerShader.h"
#include "Preferences.h"
#include "RingShader.h"
#include "Screen.h"
#include "Ship.h"
#include "Sprite.h"
#include "SpriteSet.h"
#include "SpriteShader.h"
#include "System.h"
#include "UI.h"

#include <algorithm>
#include <cmath>
#include <sstream>

using namespace std;

namespace {
  constexpr int SIDE_WIDTH = 280;

  // Check if the mission involves the given system,
  bool Involves(const Mission &mission, const System *system)
  {
    if(!system)
      return false;

    if(mission.Destination()->IsInSystem(system))
      return true;

    for(const System *waypoint : mission.Waypoints())
      if(waypoint == system)
        return true;

    for(const Planet *stopover : mission.Stopovers())
      if(stopover->IsInSystem(system))
        return true;

    return false;
  }

  size_t MaxDisplayedMissions(bool onRight)
  {
    return static_cast<unsigned>(max(0, static_cast<int>(floor((Screen::Height() - (onRight ? 160. : 190.)) / 20.))));
  }

  // Compute the required scroll amount for the given list of jobs/missions.
  void DoScroll(const list<Mission> &missionList, const list<Mission>::const_iterator &it, double &sideScroll, bool checkVisibility)
  {
    // We don't need to scroll at all if the selection must be within the viewport. The current
    // scroll could be non-zero if missions were added/aborted, so return the delta that will reset it.
    const auto maxViewable = MaxDisplayedMissions(checkVisibility);
    const auto missionCount = missionList.size();
    if(missionCount < maxViewable)
      sideScroll = 0;
    else
    {
      const auto countBefore = static_cast<size_t>(checkVisibility
          ? count_if(missionList.begin(), it, [](const Mission &m) { return m.IsVisible(); })
          : distance(missionList.begin(), it));

      const auto maximumScroll = (missionCount - maxViewable) * 20.;
      const auto pageScroll = maxViewable * 20.;
      const auto desiredScroll = countBefore * 20.;
      const auto bottomOfPage = sideScroll + pageScroll;
      if(desiredScroll < sideScroll)
      {
        // Scroll upwards.
        sideScroll = desiredScroll;
      }
      else if(desiredScroll > bottomOfPage)
      {
        // Scroll downwards (but not so far that the list's bottom sprite comes upwards further than needed).
        sideScroll = min(maximumScroll, sideScroll + (desiredScroll - bottomOfPage));
      }
    }
  }
}



// Open the missions panel directly.
MissionPanel::MissionPanel(PlayerInfo &player)
  : MapPanel(player),
  available(player.AvailableJobs()),
  accepted(player.Missions()),
  availableIt(player.AvailableJobs().begin()),
  acceptedIt(player.AvailableJobs().empty() ? accepted.begin() : accepted.end())
{
  while(acceptedIt != accepted.end() && !acceptedIt->IsVisible())
    ++acceptedIt;

  wrap.SetWrapWidth(380);
  wrap.SetFont(FontSet::Get(18));
  wrap.SetAlignment(Font::JUSTIFIED);

  // Select the first available or accepted mission in the currently selected
  // system, or along the travel plan.
  if(!FindMissionForSystem(selectedSystem) && player.HasTravelPlan())
  {
    const auto &tp = player.TravelPlan();
    for(auto it = tp.crbegin(); it != tp.crend(); ++it)
      if(FindMissionForSystem(*it))
        break;
  }

  // Auto select the destination system for the current mission.
    if(availableIt != available.end())
  {
    selectedSystem = availableIt->Destination()->GetSystem();
    DoScroll(available, availableIt, availableScroll, false);
  }
  else if(acceptedIt != accepted.end())
  {
    selectedSystem = acceptedIt->Destination()->GetSystem();
    DoScroll(accepted, acceptedIt, acceptedScroll, true);
  }

  // Centre on the selected system.
  CentreOnSystem(selectedSystem, true);
}



// Switch to the missions panel from another map panel.
MissionPanel::MissionPanel(const MapPanel &panel)
  : MapPanel(panel),
  available(player.AvailableJobs()),
  accepted(player.Missions()),
  availableIt(player.AvailableJobs().begin()),
  acceptedIt(player.AvailableJobs().empty() ? accepted.begin() : accepted.end()),
  availableScroll(0), acceptedScroll(0), dragSide(0)
{
  // In this view, always colour systems based on player reputation.
  commodity = SHOW_REPUTATION;

  while(acceptedIt != accepted.end() && !acceptedIt->IsVisible())
    ++acceptedIt;

  wrap.SetWrapWidth(380);
  wrap.SetFont(FontSet::Get(18));
  wrap.SetAlignment(Font::JUSTIFIED);

  // Select the first available or accepted mission in the currently selected
  // system, or along the travel plan.
  if(!FindMissionForSystem(selectedSystem)
    && !(player.GetSystem() != selectedSystem && FindMissionForSystem(player.GetSystem()))
    && player.HasTravelPlan())
  {
    const auto &tp = player.TravelPlan();
    for(auto it = tp.crbegin(); it != tp.crend(); ++it)
      if(FindMissionForSystem(*it))
        break;
  }
}



void MissionPanel::Step()
{
  MapPanel::Step();
  DoHelp("jobs");
}



void MissionPanel::Draw()
{
  MapPanel::Draw();

  const Colour &routeColour = *GameData::Colours().Get("map mission route");
  const System *system = selectedSystem;
  while(distance.Days(system) > 0)
  {
    const System *next = distance.Route(system);

    Point from = Zoom() * (next->Position() + centre);
    Point to = Zoom() * (system->Position() + centre);
    Point unit = (from - to).Unit() * 7.;
    from -= unit;
    to += unit;

    LineShader::Draw(from, to, 5.f, routeColour);

    system = next;
  }

  const Set<Colour> &colours = GameData::Colours();
  const Colour &availableColour = *colours.Get("available back");
  const Colour &unavailableColour = *colours.Get("unavailable back");
  const Colour &currentColour = *colours.Get("active back");
  const Colour &blockedColour = *colours.Get("blocked back");
  if(availableIt != available.end() && availableIt->Destination())
    DrawMissionSystem(*availableIt, CanAccept() ? availableColour : unavailableColour);
  if(acceptedIt != accepted.end() && acceptedIt->Destination())
    DrawMissionSystem(*acceptedIt, IsSatisfied(*acceptedIt) ? currentColour : blockedColour);

  Point pos = DrawPanel(
    Screen::TopLeft() + Point(0., -availableScroll),
    "Missions available here:",
    available.size());
  DrawList(available, pos);

  pos = DrawPanel(
    Screen::TopRight() + Point(-SIDE_WIDTH, -acceptedScroll),
    "Your current missions:",
    AcceptedVisible());
  DrawList(accepted, pos);

  // Now that the mission lists and map elements are drawn, draw the top-most UI elements.
  DrawKey();
  DrawSelectedSystem();
  DrawMissionInfo();
  DrawButtons("is missions");
}



// Only override the ones you need; the default action is to return false.
bool MissionPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress)
{
  if(key == 'a' && CanAccept())
  {
    Accept();
    return true;
  }
  else if(key == 'A' || (key == 'a' && (mod & KMOD_SHIFT)))
  {
    if(acceptedIt != accepted.end() && acceptedIt->IsVisible())
      GetUI()->Push(new Dialogue(this, &MissionPanel::AbortMission,
        "Abort mission \"" + acceptedIt->Name() + "\"?"));
    return true;
  }
  else if(key == SDLK_LEFT && availableIt == available.end())
  {
    // Switch to the first mission in the "available missions" list.
    acceptedIt = accepted.end();
    availableIt = available.begin();
  }
  else if(key == SDLK_RIGHT && acceptedIt == accepted.end() && AcceptedVisible())
  {
    // Switch to the first mission in the "accepted missions" list.
    availableIt = available.end();
    acceptedIt = accepted.begin();
    while(acceptedIt != accepted.end() && !acceptedIt->IsVisible())
      ++acceptedIt;
  }
  else if(key == SDLK_UP)
  {
    SelectAnyMission();
    if(availableIt != available.end())
    {
      // All available missions are, by definition, visible.
      if(availableIt == available.begin())
        availableIt = available.end();
      --availableIt;
    }
    else if(acceptedIt != accepted.end())
    {
      // Skip over any invisible, accepted missions.
      do {
        if(acceptedIt == accepted.begin())
          acceptedIt = accepted.end();
        --acceptedIt;
      } while(!acceptedIt->IsVisible());
    }
  }
  else if(key == SDLK_DOWN && !SelectAnyMission())
  {
    // Keyed "Down," and didn't auto-select the first mission on a side,
    // so update the existing selected mission.
    if(availableIt != available.end())
    {
      ++availableIt;
      if(availableIt == available.end())
        availableIt = available.begin();
    }
    else if(acceptedIt != accepted.end())
    {
      do {
        ++acceptedIt;
        if(acceptedIt == accepted.end())
          acceptedIt = accepted.begin();
      } while(!acceptedIt->IsVisible());
    }
  }
  else
    return MapPanel::KeyDown(key, mod, command, isNewPress);

  // To reach here, we changed the selected mission. Scroll the active
  // mission list, update the selected system, and pan the map.
  if(availableIt != available.end())
  {
    selectedSystem = availableIt->Destination()->GetSystem();
    DoScroll(available, availableIt, availableScroll, false);
  }
  else if(acceptedIt != accepted.end())
  {
    selectedSystem = acceptedIt->Destination()->GetSystem();
    DoScroll(accepted, acceptedIt, acceptedScroll, true);
  }

  if(selectedSystem)
    CentreOnSystem(selectedSystem);

  return true;
}



bool MissionPanel::Click(int x, int y, int clicks)
{
  dragSide = 0;

  if(x > Screen::Right() - 80 && y > Screen::Bottom() - 50)
    return DoKey('p');

  if(x < Screen::Left() + SIDE_WIDTH)
  {
    unsigned index = max(0, (y + static_cast<int>(availableScroll) - 36 - Screen::Top()) / 20);
    if(index < available.size())
    {
      availableIt = available.begin();
      while(index--)
        ++availableIt;
      acceptedIt = accepted.end();
      dragSide = -1;
      selectedSystem = availableIt->Destination()->GetSystem();
      DoScroll(available, availableIt, availableScroll, false);
      CentreOnSystem(selectedSystem);
      return true;
    }
  }
  else if(x >= Screen::Right() - SIDE_WIDTH)
  {
    int index = max(0, (y + static_cast<int>(acceptedScroll) - 36 - Screen::Top()) / 20);
    if(index < AcceptedVisible())
    {
      acceptedIt = accepted.begin();
      while(index || !acceptedIt->IsVisible())
      {
        index -= acceptedIt->IsVisible();
        ++acceptedIt;
      }
      availableIt = available.end();
      dragSide = 1;
      selectedSystem = acceptedIt->Destination()->GetSystem();
      DoScroll(accepted, acceptedIt, acceptedScroll, true);
      CentreOnSystem(selectedSystem);
      return true;
    }
  }

  // Figure out if a system was clicked on.
  Point click = Point(x, y) / Zoom() - centre;
  const System *system = nullptr;
  for(const auto &it : GameData::Systems())
    if(click.Distance(it.second.Position()) < 10.
        && (player.HasSeen(&it.second) || &it.second == specialSystem))
    {
      system = &it.second;
      break;
    }
  if(system)
  {
    Select(system);
    int options = available.size() + accepted.size();
    // If you just aborted your last mission, it is possible that neither
    // iterator is valid. In that case, start over from the beginning.
    if(availableIt == available.end() && acceptedIt == accepted.end())
    {
      if(!available.empty())
        availableIt = available.begin();
      else
        acceptedIt = accepted.begin();
    }
    while(options--)
    {
      if(availableIt != available.end())
      {
        ++availableIt;
        if(availableIt == available.end())
        {
          if(!accepted.empty())
            acceptedIt = accepted.begin();
          else
            availableIt = available.begin();
        }
      }
      else if(acceptedIt != accepted.end())
      {
        ++acceptedIt;
        if(acceptedIt == accepted.end())
        {
          if(!available.empty())
            availableIt = available.begin();
          else
            acceptedIt = accepted.begin();
        }
      }
      if(acceptedIt != accepted.end() && !acceptedIt->IsVisible())
        continue;

      if(availableIt != available.end() && Involves(*availableIt, system))
        break;
      if(acceptedIt != accepted.end() && Involves(*acceptedIt, system))
        break;
    }
    // Make sure invisible missions are never selected, even if there were
    // no other missions in this system.
    if(acceptedIt != accepted.end() && !acceptedIt->IsVisible())
      acceptedIt = accepted.end();
  }

  return true;
}



bool MissionPanel::Drag(double dx, double dy)
{
  if(dragSide < 0)
  {
    availableScroll = max(0.,
      min(available.size() * 20. + 190. - Screen::Height(),
        availableScroll - dy));
  }
  else if(dragSide > 0)
  {
    acceptedScroll = max(0.,
      min(accepted.size() * 20. + 160. - Screen::Height(),
        acceptedScroll - dy));
  }
  else
    MapPanel::Drag(dx, dy);

  return true;
}



// Check to see if the mouse is over either of the mission lists.
bool MissionPanel::Hover(int x, int y)
{
  dragSide = 0;
  unsigned index = max(0, (y + static_cast<int>(availableScroll) - 36 - Screen::Top()) / 20);
  if(x < Screen::Left() + SIDE_WIDTH)
  {
    if(index < available.size())
      dragSide = -1;
  }
  else if(x >= Screen::Right() - SIDE_WIDTH)
  {
    if(static_cast<int>(index) < AcceptedVisible())
      dragSide = 1;
  }
  return dragSide ? true : MapPanel::Hover(x, y);
}



bool MissionPanel::Scroll(double dx, double dy)
{
  if(dragSide)
    return Drag(0., dy * Preferences::ScrollSpeed());

  return MapPanel::Scroll(dx, dy);
}



void MissionPanel::DrawKey() const
{
  const Sprite *back = SpriteSet::Get("interface/panel/map_missions_key");
  SpriteShader::Draw(back, Screen::BottomLeft() + .5 * Point(back->Width(), -back->Height()));

  const Font &font = FontSet::Get(18);
  Point angle = Point(1., 1.).Unit();

  const int ROWS = 5;
  Point pos(Screen::Left() + 10., Screen::Bottom() - ROWS * 20. + 5.);
  Point pointerOff(5., 5.);
  Point textOff(8., -.5 * font.Height());

  const Set<Colour> &colours = GameData::Colours();
  const Colour &bright = *colours.Get("bright");
  const Colour &medium = *colours.Get("medium");
  const Colour COLOUR[ROWS] = {
    *colours.Get("available job"),
    *colours.Get("unavailable job"),
    *colours.Get("active mission"),
    *colours.Get("blocked mission"),
    *colours.Get("waypoint")
  };
  static const string LABEL[ROWS] = {
    "Available job; can accept",
    "Too little space to accept",
    "Active job; go here to complete",
    "Has unfinished requirements",
    "Waypoint you must visit"
  };
  int selected = -1;
  if(availableIt != available.end())
    selected = 0 + !CanAccept();
  if(acceptedIt != accepted.end() && acceptedIt->Destination())
    selected = 2 + !IsSatisfied(*acceptedIt);

  for(int i = 0; i < ROWS; ++i)
  {
    PointerShader::Draw(pos + pointerOff, angle, 10.f, 18.f, 0.f, COLOUR[i]);
    font.Draw(LABEL[i], pos + textOff, i == selected ? bright : medium);
    pos.Y() += 20.;
  }
}



// Fill in the top-middle header bar that names the selected system, and indicates its distance.
void MissionPanel::DrawSelectedSystem() const
{
  const Sprite *sprite = SpriteSet::Get("interface/panel/selected_system");
  SpriteShader::Draw(sprite, Point(0., Screen::Top() + .5f * sprite->Height()));

  string text;
  if(!selectedSystem)
    text = "Selected system: none";
  else if(!player.KnowsName(selectedSystem))
    text = "Selected system: unexplored system";
  else
    text = "Selected system: " + selectedSystem->Name();

  int jumps = 0;
  const vector<const System *> &plan = player.TravelPlan();
  auto it = find(plan.begin(), plan.end(), selectedSystem);
  if(it != plan.end())
    jumps = plan.end() - it;
  else if(distance.HasRoute(selectedSystem))
    jumps = distance.Days(selectedSystem);

  if(jumps == 1)
    text += " (1 jump away)";
  else if(jumps > 0)
    text += " (" + to_string(jumps) + " jumps away)";

  const Font &font = FontSet::Get(18);
  Point pos(-.5 * font.Width(text), Screen::Top() + .5 * (30. - font.Height()));
  font.Draw(text, pos, *GameData::Colours().Get("bright"));
}



// Highlight the systems associated with the given mission (i.e. destination and
// waypoints) by drawing coloured rings around them.
void MissionPanel::DrawMissionSystem(const Mission &mission, const Colour &colour) const
{
  const Colour &waypoint = *GameData::Colours().Get("waypoint back");
  const Colour &visited = *GameData::Colours().Get("faint");
  const float MISSION_OUTER = 22.f;
  const float MISSION_INNER = 20.5f;

  double zoom = Zoom();
  // Draw a coloured ring around the destination system.
  Point pos = zoom * (mission.Destination()->GetSystem()->Position() + centre);
  RingShader::Draw(pos, MISSION_OUTER, MISSION_INNER, colour);

  // Draw bright rings around systems that still need to be visited.
  for(const System *system : mission.Waypoints())
    RingShader::Draw(zoom * (system->Position() + centre), MISSION_OUTER, MISSION_INNER, waypoint);
  for(const Planet *planet : mission.Stopovers())
    RingShader::Draw(zoom * (planet->GetSystem()->Position() + centre), MISSION_OUTER, MISSION_INNER, waypoint);

  // Draw faint rings around systems already visited for this mission.
  for(const System *system : mission.VisitedWaypoints())
    RingShader::Draw(zoom * (system->Position() + centre), MISSION_OUTER, MISSION_INNER, visited);
  for(const Planet *planet : mission.VisitedStopovers())
    RingShader::Draw(zoom * (planet->GetSystem()->Position() + centre), MISSION_OUTER, MISSION_INNER, visited);
}



// Draw the background for the lists of available and accepted missions (based on pos).
Point MissionPanel::DrawPanel(Point pos, const string &label, int entries) const
{
  const Colour &backgroundColour = *GameData::Colours().Get("map mission panel background");
  const Colour &lineColour = *GameData::Colours().Get("map mission panel border");
  const Colour &unselected = *GameData::Colours().Get("medium");
  const Colour &selected = *GameData::Colours().Get("bright");

  // Draw the panel.
  Point size(SIDE_WIDTH, 20 * entries + 40);
  FillShader::Fill(pos + .5 * size, size, backgroundColour);
  // Draw the left border.
  FillShader::Fill(
    Point((pos.X() - .5), pos.Y() + 0.5 * size.Y()),
    Point(1., size.Y()),
    lineColour);
  // Draw the right border.
  FillShader::Fill(
    Point((pos.X() + 0.5 + size.X()), pos.Y() + 0.5 * size.Y()),
    Point(1., size.Y()),
    lineColour);

  // Add the bottom.
  const Sprite *bottom = SpriteSet::Get("interface/panel/edge_bottom");
  Point bottomPos = pos + Point(0., size.Y() + .5 * bottom->Height());
  if(pos.X() < 0.)
    bottomPos += Point(size.X() + 3. - .5 * bottom->Width(), 0.);
  else
    bottomPos += Point(-3. + .5 * bottom->Width(), 0.);
  SpriteShader::Draw(bottom, bottomPos);

  const Font &font = FontSet::Get(18);
  pos += Point(10., 10. + (20. - font.Height()) * .5);
  font.Draw(label, pos, selected);
  FillShader::Fill(
    pos + Point(.5 * size.X() - 5., 20.),
    Point(size.X() - 10., 1.),
    unselected);
  pos.Y() += 5.;

  return pos;
}



Point MissionPanel::DrawList(const list<Mission> &list, Point pos) const
{
  const Font &font = FontSet::Get(18);
  const Colour &highlightColour = *GameData::Colours().Get("map mission panel highlight");
  const Colour &unselected = *GameData::Colours().Get("medium");
  const Colour &selected = *GameData::Colours().Get("bright");
  const Colour &dim = *GameData::Colours().Get("dim");

  for(auto it = list.begin(); it != list.end(); ++it)
  {
    if(!it->IsVisible())
      continue;

    pos.Y() += 20.;

    bool isSelected = (it == availableIt || it == acceptedIt);
    if(isSelected)
      FillShader::Fill(
        pos + Point(.5 * SIDE_WIDTH - 10., 12.),
        Point(SIDE_WIDTH + 10., 20.),
        highlightColour);

    bool canAccept = (&list == &available ? it->HasSpace(player) : IsSatisfied(*it));
    font.Draw(it->Name(), pos,
      (!canAccept ? dim : isSelected ? selected : unselected));
  }

  return pos;
}



void MissionPanel::DrawMissionInfo()
{
  Information info;

  // The "accept / abort" button text and activation depends on what mission,
  // if any, is selected, and whether missions are available.
  if(CanAccept())
    info.SetCondition("can accept");
  else if(acceptedIt != accepted.end())
    info.SetCondition("can abort");

  info.SetString("cargo free", to_string(player.Cargo().Free()) + " tons");
  info.SetString("bunks free", to_string(player.Cargo().BunksFree()) + " bunks");

  info.SetString("today", player.GetDate().ToString());

  const Interface *interface = GameData::Interfaces().Get("mission");
  interface->Draw(info, this);

  // If a mission is selected, draw its descriptive text.
  if(availableIt != available.end())
    wrap.Wrap(availableIt->Description());
  else if(acceptedIt != accepted.end())
    wrap.Wrap(acceptedIt->Description());
  else
    return;
  wrap.Draw(Point(-190., Screen::Bottom() - 213.), *GameData::Colours().Get("bright"));
}



bool MissionPanel::CanAccept() const
{
  if(availableIt == available.end())
    return false;

  return availableIt->HasSpace(player);
}



void MissionPanel::Accept()
{
  const Mission &toAccept = *availableIt;
  int cargoToSell = 0;
  if(toAccept.CargoSize())
    cargoToSell = toAccept.CargoSize() - player.Cargo().Free();
  int crewToFire = 0;
  if(toAccept.Passengers())
    crewToFire = toAccept.Passengers() - player.Cargo().BunksFree();
  if(cargoToSell > 0 || crewToFire > 0)
  {
    ostringstream out;
    if(cargoToSell > 0 && crewToFire > 0)
      out << "You must fire " << crewToFire << " of your flagship's non-essential crew members and sell "
        << cargoToSell << " tons of ordinary commodities to make room for this mission. Continue?";
    else if(crewToFire > 0)
      out << "You must fire " << crewToFire
        << " of your flagship's non-essential crew members to make room for this mission. Continue?";
    else
      out << "You must sell " << cargoToSell
        << " tons of ordinary commodities to make room for this mission. Continue?";
    GetUI()->Push(new Dialogue(this, &MissionPanel::MakeSpaceAndAccept, out.str()));
    return;
  }

  ++availableIt;
  player.AcceptJob(toAccept, GetUI());
  if(availableIt == available.end() && !available.empty())
    --availableIt;

  // Check if any other jobs are available with the same destination. Prefer
  // jobs that also have the same destination planet.
  if(toAccept.Destination())
  {
    const Planet *planet = toAccept.Destination();
    const System *system = planet->GetSystem();
    for(auto it = available.begin(); it != available.end(); ++it)
      if(it->Destination() && it->Destination()->IsInSystem(system))
      {
        availableIt = it;
        if(it->Destination() == planet)
          break;
      }
  }
}



void MissionPanel::MakeSpaceAndAccept()
{
  const Mission &toAccept = *availableIt;
  int cargoToSell = toAccept.CargoSize() - player.Cargo().Free();
  int crewToFire = toAccept.Passengers() - player.Cargo().BunksFree();

  if(crewToFire > 0)
    player.Flagship()->AddCrew(-crewToFire);

  for(const auto &it : player.Cargo().Commodities())
  {
    if(cargoToSell <= 0)
      break;

    int toSell = min(cargoToSell, it.second);
    int64_t price = player.GetSystem()->Trade(it.first);

    int64_t basis = player.GetBasis(it.first, toSell);
    player.AdjustBasis(it.first, -basis);
    player.Cargo().Remove(it.first, toSell);
    player.Accounts().AddCredits(toSell * price);
  }

  player.UpdateCargoCapacities();
  Accept();
}



void MissionPanel::AbortMission()
{
  if(acceptedIt != accepted.end())
  {
    const Mission &toAbort = *acceptedIt;
    ++acceptedIt;
    player.RemoveMission(Mission::FAIL, toAbort, GetUI());
    if(acceptedIt == accepted.end() && !accepted.empty())
      --acceptedIt;
    if(acceptedIt != accepted.end() && !acceptedIt->IsVisible())
      acceptedIt = accepted.end();
  }
}



int MissionPanel::AcceptedVisible() const
{
  int count = 0;
  for(const Mission &mission : accepted)
    count += mission.IsVisible();
  return count;
}



bool MissionPanel::FindMissionForSystem(const System *system)
{
  if(!system)
    return false;

  acceptedIt = accepted.end();

  for(availableIt = available.begin(); availableIt != available.end(); ++availableIt)
    if(availableIt->Destination()->IsInSystem(system))
      return true;
  for(acceptedIt = accepted.begin(); acceptedIt != accepted.end(); ++acceptedIt)
    if(acceptedIt->IsVisible() && acceptedIt->Destination()->IsInSystem(system))
      return true;

  return false;
}



bool MissionPanel::SelectAnyMission()
{
  if(availableIt == available.end() && acceptedIt == accepted.end())
  {
    // No previous selected mission, so select the first job/mission for any system.
    if(!available.empty())
      availableIt = available.begin();
    else
    {
      acceptedIt = accepted.begin();
      while(acceptedIt != accepted.end() && !acceptedIt->IsVisible())
        ++acceptedIt;
    }
    return availableIt != available.end() || acceptedIt != accepted.end();
  }
  return false;
}
