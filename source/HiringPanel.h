// HiringPanel.h

#ifndef HIRING_PANEL_H_
#define HIRING_PANEL_H_

#include "Panel.h"

class PlayerInfo;



// This panel is drawn as an overlay on top of the PlanetPanel. It shows your
// current crew and passengers and allows you to hire extra crew if you are
// hunting other ships to capture.
class HiringPanel : public Panel {
public:
  explicit HiringPanel(PlayerInfo &player);

  virtual void Step() override;
  virtual void Draw() override;


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;


private:
  PlayerInfo &player;

  int maxHire;
  int maxFire;
};



#endif
