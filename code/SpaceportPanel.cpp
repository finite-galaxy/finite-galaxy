// SpaceportPanel.cpp

#include "SpaceportPanel.h"

#include "Colour.h"
#include "FontSet.h"
#include "GameData.h"
#include "Interface.h"
#include "News.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "Point.h"
#include "UI.h"

using namespace std;



SpaceportPanel::SpaceportPanel(PlayerInfo &player)
  : player(player)
{
  SetTrapAllEvents(false);
  
  text.SetFont(FontSet::Get(18));
  text.SetAlignment(Font::JUSTIFIED);
  text.SetWrapWidth(480);
  text.Wrap(player.GetPlanet()->SpaceportDescription());
  
  // Query the news interface to find out the wrap width.
  // TODO: Allow Interface to handle wrapped text directly.
  const Interface *interface = GameData::Interfaces().Get("news");
  newsMessage.SetWrapWidth(interface->GetBox("message").Width());
  newsMessage.SetFont(FontSet::Get(18));
}



void SpaceportPanel::UpdateNews()
{
  const News *news = GameData::PickNews(player.GetPlanet());
  if(!news)
    return;
  
  // Randomly pick a name, portrait, and message. These must be cached here
  // because every time the functions in News are called, they return a new
  // random element.
  hasNews = true;
  newsInfo.SetString("name", news->Name() + ':');
  newsInfo.SetSprite("portrait", news->Portrait());
  newsMessage.Wrap('"' + news->Message() + '"');
}



void SpaceportPanel::Step()
{
  if(GetUI()->IsTop(this))
  {
    Mission *mission = player.MissionToOffer(Mission::SPACEPORT);
    // Special case: if the player somehow got to the spaceport before all
    // landing missions were offered, they can still be offered here:
    if(!mission)
      mission = player.MissionToOffer(Mission::LANDING);
    if(mission)
      mission->Do(Mission::OFFER, player, GetUI());
    else
      player.HandleBlockedMissions(Mission::SPACEPORT, GetUI());
  }
}



void SpaceportPanel::Draw()
{
  if(player.IsDead())
    return;
  
  text.Draw(Point(-300., 80.), *GameData::Colours().Get("bright"));
  
  if(hasNews)
  {
    const Interface *interface = GameData::Interfaces().Get("news");
    interface->Draw(newsInfo);
    newsMessage.Draw(interface->GetBox("message").TopLeft(), *GameData::Colours().Get("medium"));
  }
}
