// OutfitInfoDisplay.h

#ifndef OUTFIT_INFO_DISPLAY_H_
#define OUTFIT_INFO_DISPLAY_H_

#include "ItemInfoDisplay.h"

#include <string>
#include <vector>

class PlayerInfo;
class Point;
class Outfit;



// Class representing three panels of information about a given outfit. One
// shows the outfit's description, one shows the required space and cost to
// install it, and one shows other attributes of the outfit.
class OutfitInfoDisplay : public ItemInfoDisplay {
public:
  OutfitInfoDisplay() = default;
  OutfitInfoDisplay(const Outfit &outfit, const PlayerInfo &player, bool canSell = false);

  // Call this every time the ship changes.
  void Update(const Outfit &outfit, const PlayerInfo &player, bool canSell = false);

  // Provided by ItemInfoDisplay:
  // int PanelWidth();
  // int MaximumHeight() const;
  // int DescriptionHeight() const;
  // int AttributesHeight() const;
  int RequirementsHeight() const;

  // Provided by ItemInfoDisplay:
  // void DrawDescription(const Point &topLeft) const;
  // void DrawAttributes(const Point &topLeft) const;
  void DrawRequirements(const Point &topLeft) const;


private:
  void UpdateRequirements(const Outfit &outfit, const PlayerInfo &player, bool canSell);
  void UpdateAttributes(const Outfit &outfit);


private:
  std::vector<std::string> requirementLabels;
  std::vector<std::string> requirementValues;
  int requirementsHeight = 0;
};



#endif
