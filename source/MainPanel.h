// MainPanel.h

#ifndef MAIN_PANEL_H_
#define MAIN_PANEL_H_

#include "Panel.h"

#include "Command.h"
#include "Engine.h"

#include <list>

class PlayerInfo;
class ShipEvent;



// Class representing the main panel (i.e. the view of your ship moving around).
// The goal is that the Engine class will not need to know about displaying
// panels or handling key presses; it instead focuses just on the calculations
// needed to move the ships around and to figure out where they should be drawn.
class MainPanel : public Panel {
public:
  explicit MainPanel(PlayerInfo &player);

  virtual void Step() override;
  virtual void Draw() override;

  // The planet panel calls this when it closes.
  void OnCallback();


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;
  virtual bool RClick(int x, int y) override;
  virtual bool Drag(double dx, double dy) override;
  virtual bool Release(int x, int y) override;
  virtual bool Scroll(double dx, double dy) override;


private:
  void ShowScanDialogue(const ShipEvent &event);
  bool ShowHailPanel();
  void StepEvents(bool &isActive);


private:
  PlayerInfo &player;

  Engine engine;

  // These are the pending ShipEvents that have yet to be processed.
  std::list<ShipEvent> eventQueue;
  bool handledFront = false;

  Command show;

  // For displaying the GPU load.
  double load = 0.;
  double loadSum = 0.;
  int loadCount = 0;

  // Keep track of how long a starting player has spent drifting in deep space.
  int lostness = 0;
  int lostCount = 0;

  Point dragSource;
  Point dragPoint;
  bool isDragging = false;
  bool hasShift = false;
  bool canClick = false;
  bool canDrag = false;
};



#endif
