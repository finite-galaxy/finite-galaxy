// PreferencesPanel.cpp

#include "PreferencesPanel.h"

#include "Audio.h"
#include "Colour.h"
#include "Dialogue.h"
#include "Files.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "GameData.h"
#include "Information.h"
#include "Interface.h"
#include "Preferences.h"
#include "Screen.h"
#include "Sprite.h"
#include "SpriteSet.h"
#include "SpriteShader.h"
#include "StarField.h"
#include "Table.h"
#include "UI.h"
#include "WrappedText.h"

#include "gl_header.h"
#include <SDL2/SDL.h>

#include <algorithm>

using namespace std;

namespace {
  // Settings that require special handling.
  const string ZOOM_FACTOR = "Main zoom factor";
  const int ZOOM_FACTOR_MIN = 100;
  const int ZOOM_FACTOR_MAX = 200;
  const int ZOOM_FACTOR_INCREMENT = 10;
  const string VIEW_ZOOM_FACTOR = "View zoom factor";
  const string EXPEND_AMMO = "Escorts expend ammo";
  const string FIGHTER_REPAIR = "Repair fighters in";
  const string TURRET_TRACKING = "Turret tracking";
  const string FOCUS_PREFERENCE = "Turrets focus fire";
  const string FRUGAL_ESCORTS = "Escorts use ammo frugally";
  const string REACTIVATE_HELP = "Reactivate first-time help";
  const string SCROLL_SPEED = "Scroll speed";
}



PreferencesPanel::PreferencesPanel()
  : editing(-1), selected(0), hover(-1)
{
  if(!GameData::PluginAboutText().empty())
    selectedPlugin = GameData::PluginAboutText().begin()->first;

  SetIsFullScreen(true);
}



// Draw this panel.
void PreferencesPanel::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT);
  GameData::Background().Draw(Point(), Point());

  Information info;
  info.SetBar("volume", Audio::Volume());
  GameData::Interfaces().Get("menu background")->Draw(info, this);
  string pageName = (page == 'c' ? "controls" : page == 's' ? "settings" : "plugins");
  if(page == 's')
    info.SetBar("refuel behaviour", Preferences::GetMaxPrice()/10);
  GameData::Interfaces().Get(pageName)->Draw(info, this);
  GameData::Interfaces().Get("preferences")->Draw(info, this);

  zones.clear();
  prefZones.clear();
  pluginZones.clear();
  if(page == 'c')
    DrawControls();
  else if(page == 's')
    DrawSettings();
  else if(page == 'p')
    DrawPlugins();
}



bool PreferencesPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress)
{
  if(static_cast<unsigned>(editing) < zones.size())
  {
    Command::SetKey(zones[editing].Value(), key);
    EndEditing();
    return true;
  }

  if(key == SDLK_DOWN && static_cast<unsigned>(selected + 1) < zones.size())
    ++selected;
  else if(key == SDLK_UP && selected > 0)
    --selected;
  else if(key == SDLK_RETURN)
    editing = selected;
  else if(key == 'b' || command.Has(Command::MENU) || (key == 'w' && (mod & (KMOD_CTRL | KMOD_GUI))))
    Exit();
  else if(key == 'c' || key == 's' || key == 'p')
    page = key;
  else
    return false;

  return true;
}



bool PreferencesPanel::Click(int x, int y, int clicks)
{
  EndEditing();

  if(x >= 265 && x < 295 && y >= -220 && y < 70)
  {
    Audio::SetVolume((20 - y) / 200.);
    Audio::Play(Audio::Get("warder"));
    return true;
  }

  if(page == 's' && x >= 25 && x < 235 && y >= 95 && y < 125)
  {
    double max = (x-30)/20.;
    if(max > 10)
      max = 10;
    if(max < 0)
      max = 0;
    Preferences::SetMaxPrice(max);
    Audio::SetVolume((20 - y) / 200.);
    Audio::Play(Audio::Get("warder"));
    return true;
  }

  Point point(x, y);
  for(unsigned index = 0; index < zones.size(); ++index)
    if(zones[index].Contains(point))
      editing = selected = index;

  for(const auto &zone : prefZones)
    if(zone.Contains(point))
    {
      if(zone.Value() == ZOOM_FACTOR)
      {
        int newZoom = Screen::Zoom() + ZOOM_FACTOR_INCREMENT;
        Screen::SetZoom(newZoom);
        if(newZoom > ZOOM_FACTOR_MAX || Screen::Zoom() != newZoom)
        {
          // Notify the user why setting the zoom any higher isn't permitted.
          // Only show this if it's not possible to zoom the view at all, as
          // otherwise the dialogue will show every time, which is annoying.
          if(newZoom == ZOOM_FACTOR_MIN + ZOOM_FACTOR_INCREMENT)
            GetUI()->Push(new Dialogue(
              "Your screen resolution is too low to support a zoom level above 100%."));
          Screen::SetZoom(ZOOM_FACTOR_MIN);
        }
        // Convert to raw window coordinates, at the new zoom level.
        point *= Screen::Zoom() / 100.;
        point += .5 * Point(Screen::RawWidth(), Screen::RawHeight());
        SDL_WarpMouseInWindow(nullptr, point.X(), point.Y());
      }
      else if(zone.Value() == VIEW_ZOOM_FACTOR)
      {
        // Increase the zoom factor unless it is at the maximum. In that
        // case, cycle around to the lowest zoom factor.
        if(!Preferences::ZoomViewIn())
          while(Preferences::ZoomViewOut()) {}
      }

      // Update saved preferences.
      if(zone.Value() == EXPEND_AMMO)
        Preferences::ToggleAmmoUsage();
      else if(zone.Value() == TURRET_TRACKING)
        Preferences::Set(FOCUS_PREFERENCE, !Preferences::Has(FOCUS_PREFERENCE));
      else if(zone.Value() == REACTIVATE_HELP)
      {
        for(const auto &it : GameData::HelpTemplates())
          Preferences::Set("help: " + it.first, false);
      }
      else if(zone.Value() == SCROLL_SPEED)
      {
        // Toogle between three different speeds.
        int speed = Preferences::ScrollSpeed() + 20;
        if(speed > 60)
          speed = 20;
        Preferences::SetScrollSpeed(speed);
      }
      else
        Preferences::Set(zone.Value(), !Preferences::Has(zone.Value()));
      break;
    }

  for(const auto &zone : pluginZones)
    if(zone.Contains(point))
    {
      selectedPlugin = zone.Value();
      break;
    }

  return true;
}



bool PreferencesPanel::Hover(int x, int y)
{
  hoverPoint = Point(x, y);

  hover = -1;
  for(unsigned index = 0; index < zones.size(); ++index)
    if(zones[index].Contains(hoverPoint))
      hover = index;

  hoverPreference.clear();
  for(const auto &zone : prefZones)
    if(zone.Contains(hoverPoint))
      hoverPreference = zone.Value();

  hoverPlugin.clear();
  for(const auto &zone : pluginZones)
    if(zone.Contains(hoverPoint))
      hoverPlugin = zone.Value();

  return true;
}



// Change the value being hovered over in the direction of the scroll.
bool PreferencesPanel::Scroll(double dx, double dy)
{
  if(!dy || hoverPreference.empty())
    return false;

  if(hoverPreference == ZOOM_FACTOR)
  {
    int zoom = Screen::UserZoom();
    if(dy < 0. && zoom > ZOOM_FACTOR_MIN)
      zoom -= ZOOM_FACTOR_INCREMENT;
    if(dy > 0. && zoom < ZOOM_FACTOR_MAX)
      zoom += ZOOM_FACTOR_INCREMENT;

    Screen::SetZoom(zoom);
    if(Screen::Zoom() != zoom)
      Screen::SetZoom(Screen::Zoom());

    // Convert to raw window coordinates, at the new zoom level.
    Point point = hoverPoint * (Screen::Zoom() / 100.);
    point += .5 * Point(Screen::RawWidth(), Screen::RawHeight());
    SDL_WarpMouseInWindow(nullptr, point.X(), point.Y());
  }
  else if(hoverPreference == VIEW_ZOOM_FACTOR)
  {
    if(dy < 0.)
      Preferences::ZoomViewOut();
    else
      Preferences::ZoomViewIn();
  }
  else if(hoverPreference == SCROLL_SPEED)
  {
    int speed = Preferences::ScrollSpeed();
    if(dy < 0.)
      speed = max(20, speed - 20);
    else
      speed = min(60, speed + 20);
    Preferences::SetScrollSpeed(speed);
  }
  return true;
}



void PreferencesPanel::EndEditing()
{
  editing = -1;
}



void PreferencesPanel::DrawControls()
{
  const Colour &back = *GameData::Colours().Get("faint");
  const Colour &dim = *GameData::Colours().Get("dim");
  const Colour &medium = *GameData::Colours().Get("medium");
  const Colour &bright = *GameData::Colours().Get("bright");

  // Check for conflicts.
  Colour red(.3f, 0.f, 0.f, .3f);

  Table table;
  table.AddColumn(-115, Table::LEFT);
  table.AddColumn(115, Table::RIGHT);
  table.SetUnderline(-120, 120);

  int firstY = -248;
  table.DrawAt(Point(-130, firstY));

  static const string CATEGORIES[] = {
    "Navigation",
    "Weapons",
    "Targeting",
    "Menus",
    "Fleet"
  };
  const string *category = CATEGORIES;
  static const Command COMMANDS[] = {
    Command::NONE,
    Command::FORWARD,
    Command::LEFT,
    Command::RIGHT,
    Command::BACK,
    Command::AFTERBURNER,
    Command::LAND,
    Command::JUMP,
    Command::NONE,
    Command::PRIMARY,
    Command::SELECT,
    Command::SECONDARY,
    Command::CLOAK,
    Command::NONE,
    Command::NEAREST,
    Command::TARGET,
    Command::HAIL,
    Command::BOARD,
    Command::SCAN,
    Command::NONE,
    Command::MENU,
    Command::MAP,
    Command::INFO,
    Command::FULLSCREEN,
    Command::NONE,
    Command::DEPLOY,
    Command::FIGHT,
    Command::GATHER,
    Command::HOLD,
    Command::AMMO
  };
  static const Command *BREAK = &COMMANDS[19];
  for(const Command &command : COMMANDS)
  {
    // The "BREAK" line is where to go to the next column.
    if(&command == BREAK)
      table.DrawAt(Point(130, firstY));

    if(!command)
    {
      table.DrawGap(10);
      table.DrawUnderline(medium);
      if(category != end(CATEGORIES))
        table.Draw(*category++, bright);
      else
        table.Advance();
      table.Draw("Key", bright);
      table.DrawGap(5);
    }
    else
    {
      int index = zones.size();
      // Mark conflicts.
      bool isConflicted = command.HasConflict();
      bool isEditing = (index == editing);
      if(isConflicted || isEditing)
      {
        table.SetHighlight(66, 120);
        table.DrawHighlight(isEditing ? dim: red);
      }

      // Mark the selected row.
      bool isHovering = (index == hover && !isEditing);
      if(!isHovering && index == selected)
      {
        table.SetHighlight(-120, 64);
        table.DrawHighlight(back);
      }

      // Highlight whichever row the mouse hovers over.
      table.SetHighlight(-120, 120);
      if(isHovering)
        table.DrawHighlight(back);

      zones.emplace_back(table.GetCentrePoint(), table.GetRowSize(), command);

      table.Draw(command.Description(), medium);
      table.Draw(command.KeyName(), isEditing ? bright : medium);
    }
  }

  Table shiftTable;
  shiftTable.AddColumn(125, Table::RIGHT);
  shiftTable.SetUnderline(0, 130);
  shiftTable.DrawAt(Point(-400, 52));

  shiftTable.DrawUnderline(medium);
  shiftTable.Draw("With &lt;shift&gt; key", bright);
  shiftTable.DrawGap(5);
  shiftTable.Draw("Select nearest ship", medium);
  shiftTable.Draw("Select next escort", medium);
  shiftTable.Draw("Talk to planet", medium);
  shiftTable.Draw("Board disabled escort", medium);
}



void PreferencesPanel::DrawSettings()
{
  const Colour &back = *GameData::Colours().Get("faint");
  const Colour &dim = *GameData::Colours().Get("dim");
  const Colour &medium = *GameData::Colours().Get("medium");
  const Colour &bright = *GameData::Colours().Get("bright");

  Table table;
  table.AddColumn(-115, Table::LEFT);
  table.AddColumn(115, Table::RIGHT);
  table.SetUnderline(-120, 120);

  int firstY = -248;
  table.DrawAt(Point(-130, firstY));

  static const string SETTINGS[] = {
    "Display",
    ZOOM_FACTOR,
    VIEW_ZOOM_FACTOR,
    "Show status overlays",
    "Highlight player's flagship",
    "Rotate flagship in HUD",
    "Show planet labels",
    "Show mini-map",
    "",
    "AI",
    "Automatic aiming",
    "Automatic firing",
    EXPEND_AMMO,
    FIGHTER_REPAIR,
    TURRET_TRACKING,
    "\n",
    "Performance",
    "Show CPU / GPU load",
    "Render motion blur",
    "Reduce large graphics",
    "Draw background haze",
    "Show hyperspace flash",
    "",
    "Other",
    "Clickable radar display",
    "Hide unexplored map regions",
    REACTIVATE_HELP,
    "Rehire extra crew when lost",
    SCROLL_SPEED,
    "Show escort systems on map",
    "Warning siren"
    // If you add anything in here be careful not to conflict with the "refuel behaviour"-bar.
  };
  bool isCategory = true;
  for(const string &setting : SETTINGS)
  {
    // Check if this is a category break or column break.
    if(setting.empty() || setting == "\n")
    {
      isCategory = true;
      if(!setting.empty())
        table.DrawAt(Point(130, firstY));
      continue;
    }

    if(isCategory)
    {
      isCategory = false;
      table.DrawGap(10);
      table.DrawUnderline(medium);
      table.Draw(setting, bright);
      table.Advance();
      table.DrawGap(5);
      continue;
    }

    // Record where this setting is displayed, so the user can click on it.
    prefZones.emplace_back(table.GetCentrePoint(), table.GetRowSize(), setting);

    // Get the "on / off" text for this setting. Setting "isOn"
    // draws the setting "bright" (i.e. the setting is active).
    bool isOn = Preferences::Has(setting);
    string text;
    if(setting == ZOOM_FACTOR)
    {
      isOn = Screen::UserZoom() == Screen::Zoom();
      text = to_string(Screen::UserZoom());
    }
    else if(setting == VIEW_ZOOM_FACTOR)
    {
      isOn = true;
      text = to_string(static_cast<int>(100. * Preferences::ViewZoom()));
    }
    else if(setting == EXPEND_AMMO)
      text = Preferences::AmmoUsage();
    else if(setting == FIGHTER_REPAIR)
    {
      isOn = true;
      text = Preferences::Has(FIGHTER_REPAIR) ? "parallel" : "series";
    }
    else if(setting == TURRET_TRACKING)
    {
      isOn = true;
      text = Preferences::Has(FOCUS_PREFERENCE) ? "focused" : "opportunistic";
    }
    else if(setting == REACTIVATE_HELP)
    {
      // Check how many help messages have been displayed.
      const map<string, string> &help = GameData::HelpTemplates();
      int shown = 0;
      int total = 0;
      for(const auto &it : help)
      {
        // Don't count certain special help messages that are always
        // active for new players.
        bool special = false;
        const string SPECIAL_HELP[] = {"basics", "lost"};
        for(const string &str : SPECIAL_HELP)
          if(it.first.find(str) == 0)
            special = true;

        if(!special)
        {
          ++total;
          shown += Preferences::Has("help: " + it.first);
        }
      }

      if(shown)
        text = to_string(shown) + " / " + to_string(total);
      else
      {
        isOn = true;
        text = "done";
      }
    }
    else if(setting == SCROLL_SPEED)
    {
      isOn = true;
      text = to_string(Preferences::ScrollSpeed());
    }
    else
      text = isOn ? "on" : "off";

    if(setting == hoverPreference)
      table.DrawHighlight(back);
    table.Draw(setting, isOn ? medium : dim);
    table.Draw(text, isOn ? bright : medium);
  }

  // Draw the text under the refuel bar.
  string refuelText;
  string refuelText2 = "";
  double maxPrice = Preferences::GetMaxPrice();
  if(maxPrice == 10)
    refuelText = "Always refuel";
  else if(maxPrice)
  {
    refuelText = "Only refuel if the price is less than ";
    refuelText2 = Format::Number(maxPrice)+" credits per unit.";
  }
  else
    refuelText = "Only refuel if it is for free.";
  table.DrawAt(Point(130, 80));
  table.Draw("Automatic refuel behaviour", medium);
  table.DrawAt(Point(130, 120));
  table.Draw(refuelText, medium);
  table.DrawAt(Point(130, 140));
  table.Draw(refuelText2, medium);
}



void PreferencesPanel::DrawPlugins()
{
  const Colour &back = *GameData::Colours().Get("faint");
  const Colour &medium = *GameData::Colours().Get("medium");
  const Colour &bright = *GameData::Colours().Get("bright");

  Table table;
  table.AddColumn(-115, Table::LEFT);
  table.SetUnderline(-120, 120);

  int firstY = -238;
  table.DrawAt(Point(-130, firstY));
  table.DrawUnderline(medium);
  table.Draw("Installed plugins:", bright);
  table.DrawGap(5);

  const int MAX_TEXT_WIDTH = 230;
  const Font &font = FontSet::Get(18);
  for(const pair<string, string> &plugin : GameData::PluginAboutText())
  {
    pluginZones.emplace_back(table.GetCentrePoint(), table.GetRowSize(), plugin.first);

    bool isSelected = (plugin.first == selectedPlugin);
    if(isSelected || plugin.first == hoverPlugin)
      table.DrawHighlight(back);
    const Font::Layout layout(Font::TRUNC_MIDDLE, MAX_TEXT_WIDTH);
    table.Draw(plugin.first, isSelected ? bright : medium, &layout);

    if(isSelected)
    {
      const Sprite *sprite = SpriteSet::Get(plugin.first);
      Point top(15., firstY);
      if(sprite)
      {
        Point centre(130., top.Y() + .5 * sprite->Height());
        SpriteShader::Draw(sprite, centre);
        top.Y() += sprite->Height() + 10.;
      }

      WrappedText wrap(font);
      wrap.SetWrapWidth(MAX_TEXT_WIDTH);
      static const string EMPTY = "(No description given.)";
      wrap.Wrap(plugin.second.empty() ? EMPTY : plugin.second);
      wrap.Draw(top, medium);
    }
  }
}



void PreferencesPanel::Exit()
{
  Command::SaveSettings(Files::Config() + "keys.txt");

  GetUI()->Pop(this);
}
