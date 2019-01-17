// RefuelPanel.h

#ifndef REFUEL_PANEL_H_
#define REFUEL_PANEL_H_

#include "Panel.h"

class PlayerInfo;
class System;



// Overlay on the PlanetPanel showing fuel price and current refuel settings.
// Also the player can buy and sell fuel here.
class RefuelPanel : public Panel {
public:
  explicit RefuelPanel(PlayerInfo &player);
  
  virtual void Step() override;
  virtual void Draw() override;
  
  
protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command) override;
  virtual bool Click(int x, int y, int clicks) override;
  
  
private:
  PlayerInfo &player;
  const System &system;
};



#endif