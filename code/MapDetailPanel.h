// MapDetailPanel.h

#ifndef MAP_DETAIL_PANEL_H_
#define MAP_DETAIL_PANEL_H_

#include "MapPanel.h"

#include "Point.h"

#include <map>

class Planet;
class PlayerInfo;
class System;



// A panel that displays the galaxy star map, with options for colour-coding the
// stars based on attitude towards the player, government, or commodity price.
// This panel also lets you view what planets are in each system, and you can
// click on a planet to view its description.
class MapDetailPanel : public MapPanel {
public:
  explicit MapDetailPanel(PlayerInfo &player, const System *system = nullptr);
  explicit MapDetailPanel(const MapPanel &panel);

  virtual void Step() override;
  virtual void Draw() override;


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;


private:
  void DrawKey();
  void DrawInfo();
  void DrawOrbits();

  // Set the commodity colouring, and update the player info as well.
  void SetCommodity(int index);


private:
  int governmentY = 0;
  int tradeY = 0;

  std::map<const Planet *, int> planetY;
  std::map<const Planet *, Point> planets;
};



#endif
