// MenuPanel.cpp

#include "MenuPanel.h"

#include "Command.h"
#include "ConversationPanel.h"
#include "Files.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "GameData.h"
#include "Interface.h"
#include "Information.h"
#include "LoadPanel.h"
#include "MainPanel.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "Point.h"
#include "PointerShader.h"
#include "PreferencesPanel.h"
#include "Ship.h"
#include "ShipyardPanel.h"
#include "Sprite.h"
#include "SpriteShader.h"
#include "StarField.h"
#include "System.h"
#include "UI.h"

#include "gl_header.h"

#include <algorithm>

using namespace std;

namespace {
  bool isReady = false;
  float alpha = 1.f;
  const int scrollSpeed = 2;
}



MenuPanel::MenuPanel(PlayerInfo &player, UI &gamePanels)
  : player(player), gamePanels(gamePanels), scroll(0)
{
  SetIsFullScreen(true);
  
  credits = Format::Split(Files::Read(Files::Resources() + "credits.txt"), "\n");
}



void MenuPanel::Step()
{
  if(GetUI()->IsTop(this) && alpha < 1.f)
  {
    ++scroll;
    if(scroll >= (20 * credits.size() + 300) * scrollSpeed)
      scroll = 0;
  }
  progress = static_cast<int>(GameData::Progress() * 60.);
  if(progress == 60 && !isReady)
  {
    if(gamePanels.IsEmpty())
    {
      gamePanels.Push(new MainPanel(player));
      // It takes one step to figure out the planet panel should be created, and
      // another step to actually place it. So, take two steps to avoid a flicker.
      gamePanels.StepAll();
      gamePanels.StepAll();
    }
    isReady = true;
  }
}



void MenuPanel::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT);
  GameData::Background().Draw(Point(), Point());
  const Font &font = FontSet::Get(18);
  
  Information info;
  const Font::Layout layout(Font::TRUNC_MIDDLE, 165);
  if(player.IsLoaded() && !player.IsDead())
  {
    info.SetCondition("pilot loaded");
    info.SetString("pilot", player.FirstName() + " " + player.LastName(), layout);
    if(player.Flagship())
    {
      const Ship &flagship = *player.Flagship();
      info.SetSprite("ship sprite", flagship.GetSprite());
      info.SetString("ship", flagship.Name(), layout);
    }
    if(player.GetSystem())
      info.SetString("system", player.GetSystem()->Name());
    if(player.GetPlanet())
      info.SetString("planet", player.GetPlanet()->Name());
    info.SetString("credits", Format::Credits(player.Accounts().Credits()));
    info.SetString("date", player.GetDate().ToString());
  }
  else if(player.IsLoaded())
  {
    info.SetCondition("no pilot loaded");
    info.SetString("pilot", player.FirstName() + " " + player.LastName(), layout);
    info.SetString("ship", "You have died.");
  }
  else
  {
    info.SetCondition("no pilot loaded");
    info.SetString("pilot", "No Pilot Loaded");
  }
  
  GameData::Interfaces().Get("menu background")->Draw(info, this);
  GameData::Interfaces().Get("main menu")->Draw(info, this);
  GameData::Interfaces().Get("menu player info")->Draw(info, this);
  
  if(progress == 60)
    alpha -= .02f;
  if(alpha > 0.f)
  {
    Angle da(6.);
    Angle a(0.);
    for(int i = 0; i < progress; ++i)
    {
      Colour colour(.5f * alpha, 0.f);
      PointerShader::Draw(Point(), a.Unit(), 8.f, 20.f, 140.f * alpha, colour);
      a += da;
    }
  }
  
  int y = 120 - scroll / scrollSpeed;
  for(const string &line : credits)
  {
    float fade = 1.f;
    if(y < -145)
      fade = max(0.f, (y + 165) / 20.f);
    else if(y > 95)
      fade = max(0.f, (115 - y) / 20.f);
    if(fade)
    {
      Colour colour(((line.empty() || line[0] == ' ') ? .2f : .4f) * fade, 0.f);
      font.Draw(line, Point(-470., y), colour);
    }
    y += 20;
  }
}



// New player "conversation" callback.
void MenuPanel::OnCallback(int)
{
  GetUI()->Pop(this);
  gamePanels.Reset();
  gamePanels.Push(new MainPanel(player));
   gamePanels.CanSave(true);
  // Tell the main panel to re-draw itself (and pop up the planet panel).
  gamePanels.StepAll();
  // If the starting conditions don't specify any ships, let the player buy one.
  if(player.Ships().empty())
  {
    gamePanels.Push(new ShipyardPanel(player));
    gamePanels.StepAll();
  }
}



bool MenuPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command)
{
  if(!isReady)
    return false;
  
  if(player.IsLoaded() && (key == 'e' || command.Has(Command::MENU)))
  {
    gamePanels.CanSave(true);
    GetUI()->Pop(this);
  }
  else if(key == 'p')
    GetUI()->Push(new PreferencesPanel());
  else if(key == 'l')
    GetUI()->Push(new LoadPanel(player, gamePanels));
  else if(key == 'n' && (!player.IsLoaded() || player.IsDead()))
  {
    // If no player is loaded, the "Enter Ship" button becomes "New Pilot."
    player.New();
    
    ConversationPanel *panel = new ConversationPanel(
      player, *GameData::Conversations().Get("intro"));
    GetUI()->Push(panel);
    panel->SetCallback(this, &MenuPanel::OnCallback);
  }
  else if(key == 'q')
    GetUI()->Quit();
  else
    return false;
  
  return true;
}
