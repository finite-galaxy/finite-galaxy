// BoardingPanel.h

#ifndef BOARDING_PANEL_H_
#define BOARDING_PANEL_H_

#include "Panel.h"

#include "CaptureOdds.h"

#include <memory>
#include <string>
#include <vector>

class Outfit;
class PlayerInfo;
class Ship;



// This panel is displayed whenever your flagship boards another ship, to give
// you a choice of what to plunder or whether to attempt to capture it. The
// items you can plunder are shown in a list sorted by value per ton. Ship
// capture is "turn-based" combat where each "turn" one or both ships lose crew.
class BoardingPanel : public Panel {
public:
  BoardingPanel(PlayerInfo &player, const std::shared_ptr<Ship> &victim);

  virtual void Draw() override;


protected:
  // Overrides from Panel.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;
  virtual bool Drag(double dx, double dy) override;
  virtual bool Scroll(double dx, double dy) override;


private:
  // You can't exit this dialogue if you are in the middle of combat.
  bool CanExit() const;
  // Check if you can take the outfit at the given position in the list.
  bool CanTake() const;
  // Check if you can initiate hand to hand combat.
  bool CanCapture() const;
  // Check if you are in the midst of hand to hand combat.
  bool CanAttack() const;


private:
  // This class represents one item in the list of outfits you can plunder.
  class Plunder {
  public:
    // Plunder can be either outfits or commodities.
    Plunder(const std::string &commodity, int count, int unitValue);
    Plunder(const Outfit *outfit, int count);

    // Sort by value per ton of mass.
    bool operator<(const Plunder &other) const;

    // Check how many of this item are left un-plundered. Once this is zero,
    // the item can be removed from the list.
    int Count() const;
    // Get the value of each unit of this plunder item.
    int64_t UnitValue() const;

    // Get the name of this item. If it is a commodity, this is its name.
    const std::string &Name() const;
    // Get the mass, in the format "<count> x <unit mass>". If this is a
    // commodity, no unit mass is given (because it is 1). If the count is
    // 1, only the unit mass is reported.
    const std::string &Size() const;
    // Get the total value (unit value times count) as a string.
    const std::string &Value() const;

    // If this is an outfit, get the outfit. Otherwise, this returns null.
    const Outfit *GetOutfit() const;
    // Find out how many of these I can take if I have this amount of cargo
    // space free.
    bool CanTake(const Ship &ship) const;
    // Take some or all of this plunder item.
    void Take(int count);

  private:
    void UpdateStrings();
    double UnitMass() const;

  private:
    std::string name;
    const Outfit *outfit;
    int count;
    int64_t unitValue;
    std::string size;
    std::string value;
  };

private:
  PlayerInfo &player;
  std::shared_ptr<Ship> you;
  std::shared_ptr<Ship> victim;

  // List of items you can plunder.
  std::vector<Plunder> plunder;
  int selected = 0;
  double scroll = 0.;

  bool playerDied = false;
  bool isCapturing = false;
  bool isFirstCaptureAction = true;
  // Calculating the odds of combat success, and the expected casualties, is
  // non-trivial. So, cache the results for all crew amounts up to full.
  CaptureOdds attackOdds;
  CaptureOdds defenceOdds;
  // These messages are shown to report the results of hand to hand combat.
  std::vector<std::string> messages;
};



#endif
