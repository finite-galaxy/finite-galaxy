// MenuPanel.h

#ifndef MENU_PANEL_H_
#define MENU_PANEL_H_

#include "Panel.h"

#include <string>
#include <vector>

class PlayerInfo;
class UI;



// Class representing the main menu, which is shown before you enter a game or
// when you hit "escape" to return here. This includes a scrolling list of
// credits and basic information on the currently loaded player.
class MenuPanel : public Panel {
public:
  MenuPanel(PlayerInfo &player, UI &gamePanels);

  virtual void Step() override;
  virtual void Draw() override;

  // New player "conversation" callback.
  void OnCallback(int value);


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;


private:
  PlayerInfo &player;
  UI &gamePanels;

  std::vector<std::string> credits;
  unsigned scroll;
  int progress = 0;
};



#endif
