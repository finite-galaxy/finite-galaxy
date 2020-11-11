// PreferencesPanel.h

#ifndef PREFERENCES_PANEL_H_
#define PREFERENCES_PANEL_H_

#include "Panel.h"

#include "ClickZone.h"
#include "Command.h"
#include "Point.h"

#include <string>
#include <vector>



// UI panel for editing preferences, especially the key mappings.
class PreferencesPanel : public Panel {
public:
  PreferencesPanel();

  // Draw this panel.
  virtual void Draw() override;


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;
  virtual bool Hover(int x, int y) override;
  virtual bool Scroll(double dx, double dy) override;

  virtual void EndEditing() override;


private:
  void DrawControls();
  void DrawSettings();
  void DrawPlugins();

  void Exit();


private:
  int editing;
  int selected;
  int hover;
  Point hoverPoint;
  // Which page of the preferences we're on.
  char page = 'c';
  std::string hoverPreference;

  std::string selectedPlugin;
  std::string hoverPlugin;

  std::vector<ClickZone<Command>> zones;
  std::vector<ClickZone<std::string>> prefZones;
  std::vector<ClickZone<std::string>> pluginZones;
};



#endif
