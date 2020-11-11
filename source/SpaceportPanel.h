// SpaceportPanel.h

#ifndef SPACEPORT_PANEL_H_
#define SPACEPORT_PANEL_H_

#include "Panel.h"

#include "Information.h"
#include "WrappedText.h"

#include <map>

class PlayerInfo;
class Sprite;



// GUI panel to be shown when you are in a spaceport. This just draws the port
// description, but can also pop up conversation panels or dialogues offering
// missions that are marked as originating in the spaceport.
class SpaceportPanel : public Panel {
public:
  explicit SpaceportPanel(PlayerInfo &player);

  void UpdateNews();

  virtual void Step() override;
  virtual void Draw() override;


private:
  PlayerInfo &player;
  WrappedText text;

  // Current news item (if any):
  bool hasNews = false;
  Information newsInfo;
  WrappedText newsMessage;
  // After displaying a portrait for a particular profession,
  // only show it for that same profession.
  std::map<const Sprite *, std::string> displayedProfessions;
};



#endif
