// LogbookPanel.h

#ifndef LOGBOOK_PANEL_H_
#define LOGBOOK_PANEL_H_

#include "Panel.h"

#include "Date.h"

#include <map>
#include <string>
#include <vector>

class PlayerInfo;



// User interface panel that displays a conversation, allowing you to make
// choices. If a callback function is given, that function will be called when
// the panel closes, to report the outcome of the conversation.
class LogbookPanel : public Panel {
public:
  LogbookPanel(PlayerInfo &player);

  // Draw this panel.
  virtual void Draw() override;


protected:
  // Event handlers.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;
  virtual bool Drag(double dx, double dy) override;
  virtual bool Scroll(double dx, double dy) override;


private:
  void Update(bool selectLast = true);


private:
  // Reference to the player, to apply any changes to them.
  PlayerInfo &player;

  // Current month being displayed:
  Date selectedDate;
  std::string selectedName;
  std::multimap<Date, std::string>::const_iterator begin;
  std::multimap<Date, std::string>::const_iterator end;
  // Other months available for display:
  std::vector<std::string> contents;
  std::vector<Date> dates;

  // Current scroll:
  double scroll = 0.;
  mutable double maxScroll = 0.;
};



#endif
