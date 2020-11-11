// EscortDisplay.cpp

#include "EscortDisplay.h"

#include "Colour.h"
#include "Font.h"
#include "FontSet.h"
#include "GameData.h"
#include "Government.h"
#include "LineShader.h"
#include "OutlineShader.h"
#include "Point.h"
#include "Rectangle.h"
#include "Ship.h"
#include "Sprite.h"
#include "System.h"

#include <algorithm>
#include <set>

using namespace std;

namespace {
  //  Horizontal layout of each escort icon:
  // (PAD) ICON (BAR_PAD) BARS (BAR_PAD) (PAD)
  const double PAD = 10.;
  const double ICON_SIZE = 20.;
  const double BAR_PAD = 5.;
  const double WIDTH = 120.;
  const double BAR_WIDTH = WIDTH - ICON_SIZE - 2. * PAD - 2. * BAR_PAD;
}



void EscortDisplay::Clear()
{
  icons.clear();
}



void EscortDisplay::Add(const Ship &ship, bool isHere, bool fleetIsJumping, bool isSelected)
{
  icons.emplace_back(ship, isHere, fleetIsJumping, isSelected);
}



// Draw as many escort icons as will fit in the given bounding box.
void EscortDisplay::Draw(const Rectangle &bounds) const
{
  // Figure out how much space there is for the icons.
  int maxColumns = max(1., bounds.Width() / WIDTH);
  MergeStacks(maxColumns * bounds.Height());
  icons.sort();
  stacks.clear();
  zones.clear();
  static const Set<Colour> &colours = GameData::Colours();

  // Draw escort status.
  const Font &font = FontSet::Get(18);
  // Top left corner of the current escort icon.
  Point corner = Point(bounds.Left(), bounds.Bottom());
  const Colour &elsewhereColour = *colours.Get("escort elsewhere");
  const Colour &cannotJumpColour = *colours.Get("escort blocked");
  const Colour &notReadyToJumpColour = *colours.Get("escort not ready");
  const Colour &selectedColour = *colours.Get("escort selected");
  const Colour &hereColour = *colours.Get("escort present");
  const Colour &hostileColour = *colours.Get("escort hostile");
  for(const Icon &escort : icons)
  {
    if(!escort.sprite)
      continue;

    corner.Y() -= escort.Height();
    // Show only as many escorts as we have room for on screen.
    if(corner.Y() <= bounds.Top())
    {
      corner.X() += WIDTH;
      if(corner.X() + WIDTH > bounds.Right())
        break;
      corner.Y() = bounds.Bottom() - escort.Height();
    }
    Point pos = corner + Point(PAD + .5 * ICON_SIZE, .5 * ICON_SIZE);

    // Draw the system name for any escort not in the current system.
    if(!escort.system.empty())
      font.Draw(escort.system, pos + Point(-10., 10.), elsewhereColour);

    Colour colour;
    if(escort.isHostile)
      colour = hostileColour;
    else if(!escort.isHere)
      colour = elsewhereColour;
    else if(escort.cannotJump)
      colour = cannotJumpColour;
    else if(escort.notReadyToJump)
      colour = notReadyToJumpColour;
    else if(escort.isSelected)
      colour = selectedColour;
    else
      colour = hereColour;

    // Figure out what scale should be applied to the ship sprite.
    float scale = min(ICON_SIZE / escort.sprite->Width(), ICON_SIZE / escort.sprite->Height());
    Point size(escort.sprite->Width() * scale, escort.sprite->Height() * scale);
    OutlineShader::Draw(escort.sprite, pos, size, colour);
    zones.push_back(pos);
    stacks.push_back(escort.ships);
    // Draw the number of ships in this stack.
    double width = BAR_WIDTH;
    if(escort.ships.size() > 1)
    {
      string number = to_string(escort.ships.size());

      Point numberPos = pos;
      numberPos.X() += 15. + width - font.Width(number);
      numberPos.Y() -= .5 * font.Height();
      font.Draw(number, numberPos, elsewhereColour);
      width -= 20.;
    }

    // Draw the status bars.
    static const Colour fullColour[5] = {
      colours.Get("shields")->Additive(1.), colours.Get("hull")->Additive(1.),
      colours.Get("fuel")->Additive(1.), colours.Get("energy")->Additive(1.), colours.Get("heat")->Additive(1.)
    };
    static const Colour halfColour[5] = {
      fullColour[0].Additive(.5), fullColour[1].Additive(.5),
      fullColour[2].Additive(.5), fullColour[3].Additive(.5), fullColour[4].Additive(.5),
    };
    Point from(pos.X() + .5 * ICON_SIZE + BAR_PAD, pos.Y() - 8.5);
    for(int i = 0; i < 5; ++i)
    {
      // If the low and high levels are different, draw a fully opaque bar up
      // to the low limit, and half-transparent up to the high limit.
      if(escort.high[i] > 0.)
      {
        bool isSplit = (escort.low[i] != escort.high[i]);
        const Colour &colour = (isSplit ? halfColour : fullColour)[i];

        Point to = from + Point(width * min(1., escort.high[i]), 0.);
        LineShader::Draw(from, to, 1.5f, colour);

        if(isSplit)
        {
          Point to = from + Point(width * max(0., escort.low[i]), 0.);
          LineShader::Draw(from, to, 1.5f, colour);
        }
      }
      from.Y() += 4.;
      if(i == 1)
      {
        from.X() += 5.;
        width -= 5.;
      }
    }
  }
}



// Check if the given point is a click on an escort icon. If so, return the
// stack of ships represented by the icon. Otherwise, return an empty stack.
const vector<const Ship *> &EscortDisplay::Click(const Point &point) const
{
  for(unsigned i = 0; i < zones.size(); ++i)
    if(point.Distance(zones[i]) < 15.)
      return stacks[i];

  static const vector<const Ship *> empty;
  return empty;
}



EscortDisplay::Icon::Icon(const Ship &ship, bool isHere, bool fleetIsJumping, bool isSelected)
  : sprite(ship.GetSprite()),
  isHere(isHere && !ship.IsDisabled()),
  isHostile(ship.GetGovernment() && ship.GetGovernment()->IsEnemy()),
  notReadyToJump(fleetIsJumping && !ship.IsHyperspacing() && !ship.IsReadyToJump(true)),
  cannotJump(fleetIsJumping && !ship.IsHyperspacing() && !ship.JumpsRemaining()),
  isSelected(isSelected),
  cost(ship.Cost()),
  system((!isHere && ship.GetSystem()) ? ship.GetSystem()->Name() : ""),
  low{ship.Shields(), ship.Hull(), ship.Fuel(), ship.Energy(), ship.Heat()},
  high(low),
  ships(1, &ship)
{
}



// Sorting operator. It comes sooner if it costs more.
bool EscortDisplay::Icon::operator<(const Icon &other) const
{
  return (cost > other.cost);
}



int EscortDisplay::Icon::Height() const
{
  return 30 + 15 * !system.empty();
}



void EscortDisplay::Icon::Merge(const Icon &other)
{
  isHere &= other.isHere;
  isHostile |= other.isHostile;
  notReadyToJump |= other.notReadyToJump;
  cannotJump |= other.cannotJump;
  isSelected |= other.isSelected;
  if(system.empty() && !other.system.empty())
    system = other.system;

  for(unsigned i = 0; i < low.size(); ++i)
  {
    low[i] = min(low[i], other.low[i]);
    high[i] = max(high[i], other.high[i]);
  }
  ships.insert(ships.end(), other.ships.begin(), other.ships.end());
}



void EscortDisplay::MergeStacks(int maxHeight) const
{
  if(icons.empty())
    return;

  set<const Sprite *> unstackable;
  while(true)
  {
    Icon *cheapest = nullptr;

    int height = 0;
    for(Icon &icon : icons)
    {
      if(!unstackable.count(icon.sprite) && (!cheapest || *cheapest < icon))
        cheapest = &icon;

      height += icon.Height();
    }

    if(height < maxHeight || !cheapest)
      break;

    // Merge together each group of escorts that have this icon and are in
    // the same system and have the same attitude towards the player.
    map<const bool, map<string, Icon *>> merged;

    // The "cheapest" element in the list may be removed to merge it with an
    // earlier ship of the same type, so store a copy of its sprite pointer:
    const Sprite *sprite = cheapest->sprite;
    list<Icon>::iterator it = icons.begin();
    while(it != icons.end())
    {
      if(it->sprite != sprite)
      {
        ++it;
        continue;
      }

      // If this is the first escort we've seen so far in its system, it
      // is the one we will merge all others in this system into.
      auto mit = merged[it->isHostile].find(it->system);
      if(mit == merged[it->isHostile].end())
      {
        merged[it->isHostile][it->system] = &*it;
        ++it;
      }
      else
      {
        mit->second->Merge(*it);
        it = icons.erase(it);
      }
    }
    unstackable.insert(sprite);
  }
}
