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

  void RefreshUI();


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;


private:
  PlayerInfo &player;
  const System &system;

  // Data about the fuel that might be needed in the refuel-panel.
  double fuel;
  double refuelPrice;
  // Determines wether the the fleet isn't fully refueled.
  bool empty;
};



#endif
