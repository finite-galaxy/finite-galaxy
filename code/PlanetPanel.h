// PlanetPanel.h

#ifndef PLANET_PANEL_H_
#define PLANET_PANEL_H_

#include "Panel.h"

#include "WrappedText.h"

#include <functional>
#include <memory>

class Interface;
class Planet;
class PlayerInfo;
class RefuelPanel;
class SpaceportPanel;
class System;



// Dialogue that pops up when you land on a planet. The shipyard and outfitter are
// shown in full-screen panels that pop up above this one, but the remaining views
// (trading, jobs, bank, port, and crew) are displayed within this dialogue.
class PlanetPanel : public Panel {
public:
  PlanetPanel(PlayerInfo &player, std::function<void()> callback);

  virtual void Step() override;
  virtual void Draw() override;


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;


private:
  void TakeOffIfReady();
  void TakeOff();


private:
  PlayerInfo &player;
  std::function<void()> callback = nullptr;
  bool requestedLaunch = false;

  const Planet &planet;
  const System &system;
  const Interface &ui;

  std::shared_ptr<Panel> trading;
  std::shared_ptr<Panel> bank;
  std::shared_ptr<SpaceportPanel> spaceport;
  std::shared_ptr<Panel> hiring;
  std::shared_ptr<RefuelPanel> refuel;
  Panel *selectedPanel = nullptr;

  WrappedText text;
};



#endif
