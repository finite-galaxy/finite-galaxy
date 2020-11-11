// BankPanel.h

#ifndef BANK_PANEL_H_
#define BANK_PANEL_H_

#include "Panel.h"

#include <string>

class PlayerInfo;



// This is an overlay drawn on top of the PlanetPanel when the player clicks on
// the "bank" button. It shows the player's mortgages and other expenses, and
// allows them to apply for new mortgages or pay extra on existing debts.
class BankPanel : public Panel {
public:
  explicit BankPanel(PlayerInfo &player);

  virtual void Step() override;
  virtual void Draw() override;


protected:
  // Overrides from Panel.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;


private:
  // Callback for the dialogues asking you to enter an amount to pay extra on an
  // existing loan or the total amount for a new loan.
  void PayExtra(const std::string &str);
  void NewMortgage(const std::string &str);


private:
  PlayerInfo &player;
  // Loan amount you're prequalified for.
  int64_t qualify;
  int selectedRow = 0;

  bool mergedMortgages = false;
  int mortgageRows = 0;
};



#endif
